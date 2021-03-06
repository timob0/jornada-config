jornada-config
================

Jornada 7xx Devuan-Linux configuration scripts and localized keyboard / button handler services

How to use:
-----------
1. Burn your ARM Devuan Distribution for the Jornada 720 to a storage card
  - Download the latest image from here: [Jovals Sugarsync site](https://app.sugarsync.com/iris/wf/D4898663_09915533_66697#cGFnZUlkPXdlYmxpbmtzJmlzV2VibGlua3NGb2xkZXI9dHJ1ZSZpc0l0ZW1SZWZyZXNoQWxsb3dlZD10cnVlJnVzZXJJZD0tMSZjdXJyZW50T3duZXJJZD05MzQzMTE4JmN1cnJlbnRGb2xkZXJJZD01NDQ2MDA4OF8xMTE0MiZjdXJyZW50Rm9sZGVyTmFtZT1hLURldkpvcm5hZGE3MjAmd2VibGlua0lkPUQ0ODk4NjYzXzA5OTE1NTMzXzY2Njk3JnRva2VuVGltZT0xNjMzODg4NTI3NTk3JmNzcmZ0b2tlbj00MDI0MGQ3Ni1kMTI1LTQ3OTItYTA0Zi1hZjUwMjc1MTE5ZmImbW9kZT0=)
  - Then insert your memory card and find its device, under Linux do `lsblk` to find it
  - Burn the image, under Linux: `unzip -p ~/Downloads/DevJ7_31Gb_AutoIceWM_WPA_VNC_RS.zip | sudo dd of=/dev/sdd bs=1M status=progress` where /dev/sdd is your memory card device
2. Mount the storage card on your desktop Linux-PC under a QEMU container:
  - Install supporting packages if you don't have yet: 
  - `sudo apt install qemu binfmt-support qemu-user-static systemd-container`
  - Mount the card:
  - `sudo systemd-nspawn -D /media/user/cardname`
    - where _user_ is your username and _cardname_ is the particular mountpoint of the storage card 
3. Then, install this package in the QEMU environment by runnig the below script (save it as a file to your card to run conveniently):

```
mv /etc/resolv.conf /etc/resolv.bak

# replace 192.168.178.1 with your routers IP
echo "nameserver 192.168.178.1" > /etc/resolv.conf    

apt-get install whiptail gparted git kbd pcmciautils busybox build-essential manpages-dev libevdev-dev rcconf
git clone https://github.com/timob0/jornada-config.git /opt/jornada-config

cp -f /opt/jornada-config/scripts/inittab.noauto /etc/inittab

/opt/jornada-config/jornada-config --install

rm /etc/resolv.conf
mv /etc/resolv.bak /etc/resolv.conf
```
4. Restart and login as root
You will automatically be presented the `jornada-config` program to do the baseline setup of your Jornada under Linux:
![Jornada Config Screen](https://raw.githubusercontent.com/timob0/jornada-config/main/img/jc_root.png "System view")

Start with resizing the disk so you can use its full capacity, then set localization options (language, timezone and keyboard), then continue with creating a new user for regular work (it will get sudo access) and setup the WiFi.
_note:_ locale and timezone take quite some time due to background package configuration, the machine is not stuck!

When you finish, select the option to disable the config program after next reboot, otherwise it will show up on each login. 

You can run it later at any time with `/opt/jornada-config/jornada-config`

### User mode ###
The program has a user mode too which exposes functions to turn on / off the WiFi, connect to different networks, configure startup behavior (automatic console login, automatic X11 startup) and a bit more. 
![Jornada Config Screen](https://raw.githubusercontent.com/timob0/jornada-config/main/img/jc_user.png "User view")

It does not run automatically, to call it use: `jconf` (this is an alias to `sudo /opt/jornada-config/jornada-config`).

(Optional) Install the button handler
-------------------------------------
Included is a background program which runs as a services and listens for certain keystrokes (Power, Mute, Vol+, Vol-). For each of these keystrokes, actions can be configured by placing a shell script with a certain name in the /etc folder:
  - j720b_power.sh  --  Runs when the power key is pressed
  - 720b_mute.sh  -- Runs when the mute button is pressed
  - j720_volup.sh -- Runs when the volume + button is pressed
  - j720b_voldn.sh -- you guess it, right? 

The program along with configuration examples is in the /opt/jornada-config/buttons folder. You can install it by running:
`./install.sh` in that folder. The script will compile the .c file, move the binary to /usr/bin/jbuttons and create an init.d/j720-buttons.sh init script. 
Also it will setup the Vol- button to "Sleep" the jornada (power off the Wifi Card and the display) and Vol+ to "unsleep"

_Note: This program is not 100% tested yet and it still has the issue of spewing log output to the console and passing through the keys as keystrokes (i.e. tildes on the commandline)._ 
