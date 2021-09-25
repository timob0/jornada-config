#include <stdio.h>
#include <stdlib.h>

#include <string.h>     /* strerror() */
#include <errno.h>      /* errno */

#include <signal.h>

#include <fcntl.h>      /* open() */
#include <unistd.h>     /* close() */
#include <sys/ioctl.h>  /* ioctl() */
#include <linux/input.h>    /* EVIOCGVERSION ++ */
#include <libevdev/libevdev.h>  /* sudo apt-get install -y libevdev-dev */
#include <libevdev/libevdev-uinput.h>  /* sudo apt-get install -y libevdev-dev */


#define EV_BUF_SIZE 16

#define FILE_POWER "/etc/j720b_power.sh"
#define FILE_VOLUP "/etc/j720b_volup.sh"
#define FILE_VOLDN "/etc/j720b_voldn.sh"
#define FILE_MUTE  "/etc/j720b_mute.sh"

// Handle Ctrl-C gracefully
static volatile int running = 1;

void intHandler(int dummy) {
    running = 0;
}

int main(int argc, char *argv[])
{
    int fd, sz;
    unsigned i;

    /* A few examples of information to gather */
    unsigned version;
    unsigned short id[4];                   /* or use struct input_id */
    char name[256] = "N/A";

    struct input_event ev[EV_BUF_SIZE]; /* Read up to N events ata time */

    unsigned short key = 0;
    signed int value = -1;
    unsigned short lastkey = 0;
    signed int lastvalue = -1;

    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s /dev/input/eventN\n"
            "Where X = input device number\n",
            argv[0]
        );
        return EINVAL;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr,
            "ERR %d:\n"
            "Unable to open `%s'\n"
            "%s\n",
            errno, argv[1], strerror(errno)
        );
    }
    /* Error check here as well. */
    ioctl(fd, EVIOCGVERSION, &version);
    ioctl(fd, EVIOCGID, id); 
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);

    fprintf(stderr,
        "Name      : %s\n"
        "Version   : %d.%d.%d\n"
        ,
        name,
        version >> 16,
        (version >> 8) & 0xff,
        version & 0xff
    );

   // Setup CTRL-C Handler
   signal(SIGINT, intHandler);

    /* Loop. Read event file and parse result. */
    while(running) {
        sz = read(fd, ev, sizeof(struct input_event) * EV_BUF_SIZE);

        if (sz < (int) sizeof(struct input_event)) {
            fprintf(stderr,
                "ERR %d:\n"
                "Reading of `%s' failed\n"
                "%s\n",
                errno, argv[1], strerror(errno)
            );
            goto fine;
        }

        /* Implement code to translate type, code and value */
        for (i = 0; i < sz / sizeof(struct input_event); ++i) {
            #ifdef DEBUG
            fprintf(stderr,
                "%ld.%06ld: "
                "type=%02x "
                "code=%02x "
                "value=%02x\n",
                ev[i].time.tv_sec,
                ev[i].time.tv_usec,
                ev[i].type,
                ev[i].code,
                ev[i].value
            );
            #endif

            // Key and either pressed or released
            if (ev[i].type == EV_KEY && (ev[i].value==1 || ev[i].value==2)) {
                key = ev[i].code;
                value = ev[i].value;

                // I want key transitions
                if (key==lastkey && value!=lastvalue) {
                    //Handle J720 R/M/L/Power Keys, for each the existence 
                    // of a handler file is tested and if found executed
                    if (ev[i].code == 0x5b) {
                        printf("power key\n");
                        if( access(FILE_POWER, F_OK ) == 0 ) {
                            printf("file found, running\n");
                            int status = system(FILE_POWER);
                        }
                    }
                    else if (ev[i].code == KEY_VOLUMEDOWN) {
                        printf("volume- key\n");
                        if( access(FILE_VOLDN, F_OK ) == 0 ) {
                            printf("file found, running\n");
                            int status = system(FILE_VOLDN);
                        }
                    }
                    else if (ev[i].code == KEY_VOLUMEUP) {
                        printf("volume+ key\n");
                        if( access(FILE_VOLUP, F_OK ) == 0 ) {
                            printf("file found, running\n");
                            int status = system(FILE_VOLUP);
                        }
                    }
                    else if (ev[i].code == KEY_MUTE) {
                        printf("mute key\n");
                        if( access(FILE_MUTE, F_OK ) == 0 ) {
                            printf("file found, running\n");
                            int status = system(FILE_MUTE);
                        }
                    }
                    else if (ev[i].code == KEY_F12) {
                        printf("f12 key\n");
                        if( access(FILE_MUTE, F_OK ) == 0 ) {
                            printf("file found, running mute file\n");
                            int status = system(FILE_MUTE);
                        }
                    }
                }
                lastkey = key;
                lastvalue = value;
            }

        }
    }

fine:
    close(fd);
    return errno;
}
