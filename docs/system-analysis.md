# System analysis

```
root@rhino:~# uname -a
Linux rhino 3.14.43+ #137 Wed Oct 25 21:06:53 EDT 2017 armv7l GNU/Linux
root@rhino:~# cat /proc/cpuinfo 
processor	: 0
model name	: ARMv7 Processor rev 2 (v7l)
BogoMIPS	: 597.60
Features	: swp half thumb fastmult vfp edsp thumbee neon vfpv3 tls vfpd32 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x3
CPU part	: 0xc08
CPU revision	: 2

Hardware	: Generic AM33XX (Flattened Device Tree)
Revision	: 0000
Serial		: 0000000000000000
```

- CPU: ARMv7 (32-bit), Cortex-A8 (Sitara AM33XX-Familie from Texas Instruments) → typical SoC for Embedded Linux, also on the BeagleBone Black
- Kernel: Linux 3.14.43+, old (2015-2017).
- Internal codename: rhino
- Rootfs: UBIFS on UBI (NAND Flash), partially read-only (ubi0:rootfs), temp files in tmpfs
- probably minimal Embedded Linux (Buildroot, OpenEmbedded/Yocto or vendor-specific)

Full list of running processed:
```
root@rhino:~# ps auxww
PID   USER     TIME   COMMAND
    1 root       0:01 init [5]
    2 root       0:00 [kthreadd]
    3 root       0:18 [ksoftirqd/0]
    5 root       0:00 [kworker/0:0H]
    7 root       0:00 [khelper]
    8 root       0:00 [kdevtmpfs]
    9 root       0:00 [netns]
   44 root       0:04 [kworker/0:1]
  222 root       0:00 [writeback]
  225 root       0:00 [bioset]
  226 root       0:00 [crypto]
  228 root       0:00 [kblockd]
  315 root       0:00 [khubd]
  421 root       0:00 [rpciod]
  429 root       0:01 [kswapd0]
  430 root       0:00 [fsnotify_mark]
  431 root       0:00 [nfsiod]
  452 root       0:00 [ocf_0]
  453 root       0:00 [ocf_ret_0]
  555 root       0:00 [spi1]
  625 root       0:00 [deferwq]
  683 root       0:00 [irq/87-4802a000]
  698 root       0:00 [kworker/0:2]
  701 root       0:00 [ubi_bgt0d]
  722 root       0:00 [gpio_led_tick_t]
  769 root       0:00 [ubi_bgt1d]
  772 root       0:00 [ubifs_bgt1_0]
  786 root       0:00 [ubi_bgt2d]
  789 root       0:00 [ubifs_bgt2_0]
  820 root       0:00 /lib/udev/udevd -d
  844 root       0:00 [IRDecodeThread]
  848 root       0:00 [debounce_thread]
 1059 root       0:00 /lib/udev/udevd -d
 1060 root       0:00 /lib/udev/udevd -d
 1348 root       0:00 [cfg80211]
 1351 root       0:00 [wl12xx_wq]
 1409 root       0:21 [irq/175-wl18xx]
 1596 messageb   0:00 /usr/bin/dbus-daemon --system
 1618 root       0:49 /sbin/syslogd -n -C512
 1620 root       0:00 /sbin/klogd -n
 1642 root       0:00 /usr/local/bin/SS1BTPM
 1662 root       0:00 udhcpd -S /etc/udhcpd.conf
 1696 root       0:00 ./shepherdd --recovery /var/run/shepherd
 1698 root       0:00 /sbin/getty -l /usr/bin/spawn_telnet_longsleep.sh -n 115200 ttyGS0
 1705 root       0:03 avahi-daemon: running [Bose-SM2-a810873e97cf.local]
 1706 root       0:00 Bluetooth
 1707 root       0:01 BTLESetup
 1708 root       0:01 NetManager --autoswitching=true
 1709 root       0:00 PtsServer /opt/Bose/pts-handler 80
 1710 root       0:00 SoftwareUpdate
 1711 root       0:00 scm_finalize
 1712 root       0:00 CLIServer
 1713 root       0:05 scmmond --ir-sleep --amp-faults
 1714 root       1:55 STSCertified
 1716 root       0:03 IoT
 1717 root       2:41 TPDA -c /opt/Bose/etc/Voice.xml
 1718 root       0:01 Rhino
 1720 root       0:00 Firewall -4i wlan0 /etc/init.d/Firewalls/update_iptables
 1721 root       2:57 APServer -c /opt/Bose/etc/APConfig-rhino.xml
 1722 root       3:53 UpnpSource -c/opt/Bose/etc/UpnpSource-rhino.xml
 1723 root       0:02 WebServer -c/opt/Bose/etc/WebServer-rhino.xml
 1724 root       3:07 BoseApp -c/opt/Bose/etc/BoseApp-rhino.xml
 1732 root       0:00 [kworker/0:1H]
 1775 root       0:00 telnet localhost 17000
 1992 root       0:00 /usr/sbin/sshd
 1999 root       0:00 /usr/sbin/telnetd
 2109 root       0:01 wpa_supplicant -i wlan0 -s -c /etc/wpa_supplicant.conf -D nl80211
 2256 root       0:00 udhcpc --release --arping --syslog --background --timeout 3 --tryagain 3 --pidfile /var/run/udhcpc.wlan0.pid --interface wlan0 -x hostname SoundTouch-Bad --script /op
 2550 root       0:09 {sshd} sshd: root@pts/0
 2553 root       0:01 -sh
 2745 root       0:00 [scsi_eh_1]
 2746 root       0:00 [scsi_tmf_1]
 2747 root       0:00 [usb-storage]
 2960 root       0:00 sh
 3857 root       0:00 [kworker/u2:2]
 3962 root       0:00 [kworker/u2:0]
 4002 root       0:00 [kworker/u2:1]
 4032 root       0:00 ps auxww
```

All Bose related binaries seem to be located under `/opt/Bose/` as the output of `ls -l /proc/*/exe 2>/dev/null | awk '{print $9, $10, $11}'` relates these running processes with their path. The content is:
```
root@rhino:~# ls -lah /opt/Bose/
total 65016
drwxr-xr-x    9 root     root        7.5K Aug  4  2022 .
drwxr-xr-x    3 root     root         224 Aug  4  2022 ..
-rwxr-xr-x    1 root     root        2.1M Aug  4  2022 A4VServer_linus
-rwxr-xr-x    1 root     root        3.5M Aug  4  2022 APServer
-rwxr-xr-x    1 root     root      490.0K Aug  4  2022 APTestApplication
-rwxr-xr-x    1 root     root      180.9K Aug  4  2022 ARMPatch
-rwxr-xr-x    1 root     root        1.4M Aug  4  2022 ARMUpdate
-rw-r--r--    1 root     root      138.0K Aug  4  2022 Ar_WGL__.ttf
-rw-r--r--    1 root     root      329.5K Aug  4  2022 Arial_Japanese.ltt
-rw-r--r--    1 root     root      333.7K Aug  4  2022 Arial_Korean.ltt
-rw-r--r--    1 root     root      210.9K Aug  4  2022 Arial_SimplifiedChinese.ltt
-rw-r--r--    1 root     root      316.8K Aug  4  2022 Arial_TraditionalChinese.ltt
-rwxr-xr-x    1 root     root        2.2M Aug  4  2022 BT
-rwxr-xr-x    1 root     root        1.5M Aug  4  2022 BTLESetup
-rwxr-xr-x    1 root     root      409.5K Aug  4  2022 BatteryMonitor
-rwxr-xr-x    1 root     root        3.4M Aug  4  2022 Bluetooth
-rwxr-xr-x    1 root     root        2.4M Aug  4  2022 BluetoothManager
-rwxr-xr-x    1 root     root        7.0M Aug  4  2022 BoseApp
-rwxr-xr-x    1 root     root      186.6K Aug  4  2022 CLIServer
-rwxr-xr-x    1 root     root      396.3K Aug  4  2022 ClockSync
-rw-r--r--    1 root     root        5.6K Aug  4  2022 CzechStrings.txt
-rw-r--r--    1 root     root        5.4K Aug  4  2022 DanishStrings.txt
-rw-r--r--    1 root     root        5.6K Aug  4  2022 DutchStrings.txt
-rw-r--r--    1 root     root        4.6K Aug  4  2022 EnglishStrings.txt
-rw-r--r--    1 root     root        5.5K Aug  4  2022 FinnishStrings.txt
-rwxr-xr-x    1 root     root        8.4K Aug  4  2022 Firewall
-rw-r--r--    1 root     root        6.3K Aug  4  2022 FrenchStrings.txt
-rw-r--r--    1 root     root        6.2K Aug  4  2022 GermanStrings.txt
-rw-r--r--    1 root     root       10.7K Aug  4  2022 GreekStrings.txt
-rw-r--r--    1 root     root        6.2K Aug  4  2022 HungarianStrings.txt
-rwxr-xr-x    1 root     root        1.6M Aug  4  2022 IoT
-rw-r--r--    1 root     root        5.9K Aug  4  2022 ItalianStrings.txt
-rw-r--r--    1 root     root        7.2K Aug  4  2022 JapaneseStrings.txt
-rw-r--r--    1 root     root      577.8K Aug  4  2022 Japanese_SC0_TC0_JP1_KR0_BOSE_053013.ttf
-rw-r--r--    1 root     root      386.6K Aug  4  2022 Japanese_SC0_TC0_JP2_KR3_BOSE_053013.ttf
-rw-r--r--    1 root     root       24.6K Aug  4  2022 Japanese_SC5_TC6_JP3_KR4_BOSE_053013.ttf
-rw-r--r--    1 root     root        6.2K Aug  4  2022 KoreanStrings.txt
-rw-r--r--    1 root     root      610.5K Aug  4  2022 Korean_SC0_TC0_JP0_KR2_BOSE_053013.ttf
-rw-r--r--    1 root     root      574.9K Aug  4  2022 Korean_SC6_TC7_JP7_KR1_BOSE_053013.ttf
drwxr-xr-x    4 root     root         368 Aug  4  2022 LightswitchAssets
-rw-r--r--    1 root     root       29.5K Aug  4  2022 MLO
-rwxr-xr-x    1 root     root     1007.0K Aug  4  2022 NetManager
-rw-r--r--    1 root     root        5.3K Aug  4  2022 NorwegianStrings.txt
-rwxr-xr-x    1 root     root      165.4K Aug  4  2022 PlatformReset
-rwxr-xr-x    1 root     root      307.4K Aug  4  2022 PlatformServices
-rw-r--r--    1 root     root        5.9K Aug  4  2022 PolishStrings.txt
-rw-r--r--    1 root     root        5.9K Aug  4  2022 PortugueseStrings.txt
-rwxr-xr-x    1 root     root        6.7K Aug  4  2022 PtsServer
-rwxr-xr-x    1 root     root        5.1M Aug  4  2022 Rhino
-rw-r--r--    1 root     root        5.8K Aug  4  2022 RomanianStrings.txt
-rw-r--r--    1 root     root        9.6K Aug  4  2022 RussianStrings.txt
-rwxr-xr-x    1 root     root        1.3M Aug  4  2022 SCMReset_Update
-rwxr-xr-x    1 root     root      239.6K Aug  4  2022 SMSCControlApp
-rwxr-xr-x    1 root     root        6.1M Aug  4  2022 STSCertified
-rw-r--r--    1 root     root        4.5K Aug  4  2022 SimpChineseStrings.txt
-rw-r--r--    1 root     root        1.2M Aug  4  2022 SimpChinese_SC1_TC0_JP0_KR0_BOSE_053013.ttf
-rw-r--r--    1 root     root      965.8K Aug  4  2022 SimpChinese_SC2_TC4_JP6_KR7_BOSE_053013.ttf
-rw-r--r--    1 root     root      125.5K Aug  4  2022 SimpChinese_SC3_TC5_JP0_KR0_BOSE_053013.ttf
-rw-r--r--    1 root     root        5.6K Aug  4  2022 SlovenianStrings.txt
-rwxr-xr-x    1 root     root      955.1K Aug  4  2022 SoftwareUpdate
-rwxr-xr-x    1 root     root        1.8M Aug  4  2022 SoftwareUpdateInstaller
-rw-r--r--    1 root     root        5.9K Aug  4  2022 SpanishStrings.txt
-rw-r--r--    1 root     root        5.4K Aug  4  2022 SwedishStrings.txt
-rwxr-xr-x    1 root     root        1.7M Aug  4  2022 TPDA
drwxr-xr-x    3 root     root         544 Aug  4  2022 Telemetry
-rw-r--r--    1 root     root       12.0K Aug  4  2022 ThaiStrings.txt
-rw-r--r--    1 root     root        4.5K Aug  4  2022 TradChineseStrings.txt
-rw-r--r--    1 root     root      741.7K Aug  4  2022 TradChinese_SC0_TC1_JP0_KR0_BOSE_053013.ttf
-rw-r--r--    1 root     root      932.7K Aug  4  2022 TradChinese_SC0_TC2_JP4_KR5_BOSE_053013.ttf
-rw-r--r--    1 root     root      313.5K Aug  4  2022 TradChinese_SC4_TC3_JP5_KR6_BOSE_053013.ttf
-rw-r--r--    1 root     root        5.5K Aug  4  2022 TurkishStrings.txt
-rwxr-xr-x    1 root     root        1.4M Aug  4  2022 UbootUpdate
-rw-r--r--    1 root     root      182.3K Aug  4  2022 UniversOTSThai.ttf
-rwxr-xr-x    1 root     root        1.1M Aug  4  2022 Upnp
-rwxr-xr-x    1 root     root        1.6M Aug  4  2022 UpnpSource
-rwxr-xr-x    1 root     root        1.2M Aug  4  2022 WebServer
drwxr-xr-x    2 root     root         552 Aug  4  2022 Xml
-rwxr-xr-x    1 root     root      395.1K Aug  4  2022 bt_mfg
drwxr-xr-x    2 root     root         552 Aug  4  2022 chimes
-rwxr-xr-x    1 root     root        5.2K Aug  4  2022 cmd_adc
-rwxr-xr-x    1 root     root        3.9K Aug  4  2022 cmd_ir
-rwxr-xr-x    1 root     root        1.1K Aug  4  2022 encrypt-logs
drwxr-xr-x    2 root     root        1.6K Jan 14 01:10 etc
-rwxr-xr-x    1 root     root       12.4K Aug  4  2022 fbshot
drwxr-xr-x    4 root     root         576 Aug  4  2022 html
-rwxr-xr-x    1 root     root       46.9K Aug  4  2022 led-splash-write
-rw-r--r--    1 root     root        3.1M Aug  4  2022 linuxPatched
-rwxr-xr-x    1 root     root      181.4K Aug  4  2022 mfgdatatool
-rwxr-xr-x    1 root     root        6.6K Aug  4  2022 microbswitch
-rwxr-xr-x    1 root     root        4.4K Aug  4  2022 pts-handler
-rwxr-xr-x    1 root     root         365 Aug  4  2022 pts-info
-rwxr-xr-x    1 root     root       39.9K Aug  4  2022 scm_finalize
-rwxr-xr-x    1 root     root      418.2K Aug  4  2022 scmmond
-rwxr-xr-x    1 root     root      153.9K Aug  4  2022 shepherdd
-rwxr-xr-x    1 root     root       22.2K Aug  4  2022 sizes
-rwxr-xr-x    1 root     root        4.1K Aug  4  2022 splash-write
-rwxr-xr-x    1 root     root         883 Aug  4  2022 start-avahi
-rw-r--r--    1 root     root      415.6K Aug  4  2022 u-boot.img
-rwxr-xr-x    1 root     root        2.0K Aug  4  2022 udhcpc.script
drwxr-xr-x    2 root     root         376 Aug  4  2022 wpa_supplicant
```

Unclear whether it uses hardware floats or software emulation:
```
root@rhino:~# cat /proc/$(pgrep STSCertified)/maps | grep "libc-"
b633d000-b6418000 r-xp 00000000 00:0d 2388       /lib/libc-2.15.so
b6418000-b6420000 ---p 000db000 00:0d 2388       /lib/libc-2.15.so
b6420000-b6422000 r-xp 000db000 00:0d 2388       /lib/libc-2.15.so
b6422000-b6423000 rwxp 000dd000 00:0d 2388       /lib/libc-2.15.so
root@rhino:~# LD_SHOW_AUXV=1 /bin/true | grep AT_PLATFORM
AT_PLATFORM:     v7l
root@rhino:~# LD_SHOW_AUXV=1 /bin/true | grep AT_HWCAP
AT_HWCAP:    swp half thumb fastmult vfp edsp thumbee neon vfpv3
```
Looks more like `armel` (software)d due to missing `hf` in `/lib/libc-2.15.so`.

File system (read-only, use `rw` for write access but be sure what you are doing!):
```
root@rhino:~# mount | grep " / "
rootfs on / type rootfs (rw)
ubi0:rootfs on / type ubifs (ro,relatime)
```