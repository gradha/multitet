@echo off
rem generated by builder.pl: configures for building with djgpp
if exist clean.bat del clean.bat
if exist makefile.* del makefile.*
if exist watmake.bat del watmake.bat
if exist msvcmake.bat del msvcmake.bat
if exist multitet.dsp del multitet.dsp
copy build\makefile.dj makefile
copy build\makefile.all
copy build\clean.bat
utod .../*.c .../*.h
echo Ready for building with djgpp -- type:
echo   'make' for a release build
echo   'make debug' for a debugging build
echo   'make clean' or just 'clean' to tidy up
