@echo off
rem generated by builder.pl: cleans out temporary files

if exist multitet.dsw del multitet.dsw
if exist multitet.ncb del multitet.ncb
if exist multitet.opt del multitet.opt
if exist multitet.plg del multitet.plg
if exist *.pch del *.pch
if exist *.idb del *.idb
if exist *.err del *.err
if exist obj\*.pch del obj\*.pch
if exist obj\*.idb del obj\*.idb
if exist obj\*.pdb del obj\*.pdb
if exist obj\*.obj del obj\*.obj
if exist obj\*.o del obj\*.o
if exist obj\*.a del obj\*.a
if exist .\multitet del .\multitet
if exist .\multitet.exe del .\multitet.exe
if exist .\multitet.ilk del .\multitet.ilk
if exist .\multitet.pdb del .\multitet.pdb
if exist allegro.log del allegro.log
if exist core del core
