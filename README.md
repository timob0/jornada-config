jornada-config
================

Jornada 7xx Devuan-Linux configuration scripts and localized keyboard / button handler services

How to use:
-----------
1. Burn your ARM Devuan Distribution for the Jornada 720 to a storage card
2. Mount the storage card on the desktop PC under a QEMU container:
  - Install packages if you don't have yet: 
  - `sudo apt install qemu binfmt-support qemu-user-static systemd-container`
  - Mount the card:
  - `sudo systemd-nspawn -D /media/user/cardname`
3. Then, install this package in the QEMU environment by runnig the below script (save it as a file to your card to run conveniently):

```
mv /etc/resolv.conf /etc/resolv.bak
echo "nameserver 192.168.178.1" > /etc/resolv.conf 
echo "search fritz.box" >> /etc/resolv.conf
 
apt-get install whiptail gparted git kbd pcmciautils libevdev-dev screen
# optional apt-get install build-essential manpages-dev awesome ratpoison sc wordgrinder
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

When you finish, select the option to disable the config program after next reboot, otherwise it will show up on each login. 

(Optional) Install the button handler
-------------------------------------
- Included is a background program which runs as a services and listens for certain keystrokes (Power, Mute, Vol+, Vol-). For each of these keystrokes, actions can be configured by placing a shell script with a certain name in the /etc folder:
    - j720b_power.sh  --  Runs when the power key is pressed
    - 720b_mute.sh  -- Runs when the mute button is pressed
    - j720_volup.sh -- Runs when the volume + button is pressed
    - j720b_voldn.sh -- you guess it, right? 
