@echo off
rem generated by builder.pl: configures for building with Watcom
if exist clean.bat del clean.bat
if exist makefile.* del makefile.*
if exist watmake.bat del watmake.bat
if exist msvcmake.bat del msvcmake.bat
if exist multitet.dsp del multitet.dsp
copy build\watmake.bat
copy build\clean.bat
utod .../*.c .../*.h
echo Ready for building with Watcom -- type:
echo   'watmake' for a release build
echo   'clean' to tidy up
