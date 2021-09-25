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

#define FILE_POWER "/etc/handle_power.sh"
#define FILE_VOLUP "/etc/handle_volup.sh"
#define FILE_VOLDN "/etc/handle_voldn.sh"
#define FILE_MUTE  "/etc/handle_mute.sh"

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
        "ID        : Bus=%04x Vendor=%04x Product=%04x Version=%04x\n"
        "----------\n"
        ,
        name,

        version >> 16,
        (version >> 8) & 0xff,
        version & 0xff,

        id[ID_BUS],
        id[ID_VENDOR],
        id[ID_PRODUCT],
        id[ID_VERSION]
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
            
            //Handle J720 R/M/L/Power Keys, for each the existence 
            // of a handler file is tested and if found executed
	    if (ev[i].type == EV_KEY && ev[i].code == KEY_POWER) {
		if( access(FILE_POWER, F_OK ) == 0 ) {
		    int status = system(FILE_POWER);
		}
	    }
	    else if (ev->type == EV_KEY && ev->code == KEY_VOLUMEDOWN) {
		if( access(FILE_VOLDN, F_OK ) == 0 ) {
		    int status = system(FILE_VOLDN);
		}	    
	    }
	    else if (ev->type == EV_KEY && ev->code == KEY_VOLUMEUP) {
	        if( access(FILE_VOLUP, F_OK ) == 0 ) {
		    int status = system(FILE_VOLUP);
		}
	    }
	    else if (ev->type == EV_KEY && ev->code == KEY_MUTE) {
	        if( access(FILE_MUTE, F_OK ) == 0 ) {
		    int status = system(FILE_MUTE);
		}
	    }
	    
        }
    }

fine:
    close(fd);
    return errno;
}
