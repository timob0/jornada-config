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
3. Then, install this package in the QEMU environment:

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
