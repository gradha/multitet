#! /usr/bin/perl -w
#
#  Generates djgpp/Unix/Mingw32 makefiles, Watcom and MSVC batch files,
#  and an MSVC version 6 project file (.dsp) for building portable
#  programs, reading info about them from the builder.in file.
#
#  By Shawn Hargreaves, December 1999.
#
#  Modified by George Foot, August 2000, to support Mingw32, provide
#  fixup scripts, and support packages.


# builder file directory
$build = "build";

# read project information from builder.in into %param
$in = "$build/builder.in";

unless (-f $in) {
   $in = $0;
   $in =~ s/\.[^\/\\]*$//;
   $in .= ".in";
}

print "Reading $in\n";

open FILE, "$in" or die "Error: can't open $in";

while (<FILE>) {
   s/#.*//;

   if (/\s*([^=]*\S)\s*=\s*(.*\S)/) {
      $param{$1} = $2;
   }
}

close FILE;

die "Error: NAME not set in $in" unless ($param{"NAME"});
die "Error: OBJDIR not set in $in" unless ($param{"OBJDIR"});
die "Error: BINDIR not set in $in" unless ($param{"BINDIR"});


# import package settings
foreach $package (split (" ", $param{"PACKAGES"})) {
   $in = "$build/$package.pkg";
   print "Reading $in\n";
   open FILE, "$in" or die "Error: can't open $in";

   while (<FILE>) {
      s/#.*//;

      if (/\s*([^=]*\S)\s*\+=\s*(.*\S)/) {
         $param{$1} .= " $2";
      }
      elsif (/\s*([^=]*\S)\s*=\s*(.*\S)/) {
         $param{$1} = $2;
      }
   }
   close FILE;
}


# convert obj and bin paths to pretty formats
$dosobj = $unixobj = $param{"OBJDIR"};
$dosbin = $unixbin = $param{"BINDIR"};

# get slashes the right way around
$dosobj =~ s/\//\\/g;
$dosbin =~ s/\//\\/g;

$unixobj =~ s/\\/\//g;
$unixbin =~ s/\\/\//g;

# strip trailing slashes
$dosobj =~ s/\\$//;
$dosbin =~ s/\\$//;

$unixobj =~ s/\/$//;
$unixbin =~ s/\/$//;



# scans a source file for dependency information
sub get_dependencies
{
   my $filename = shift;

   return if ($depends{$filename});

   # scan the file for include statements
   open FILE, $filename or die "Error opening $filename\n";

   my @deps = ();

   while (<FILE>) {
      # strip comments
      s/\/\/.*//;
      s/\/\*.*\*\///g;

      if (/^\s*#include\s*[<"](.*)[>"]/) {
	 push @deps, $1;
      }
   }

   close FILE;

   # build the dependency list
   $depends{$filename}{$filename} = 1;

   for my $header (@deps) {
      my $head = $header;
      $head =~ s/\\/\//g;

      if ($filename =~ /(.*[\\\/])/) {
	 $head = $1 . $head;
      }

      if (-f $head) {
	 # recurse into a header
	 $headers{$head} = 1;
	 $depends{$filename}{$head} = 1;

	 get_dependencies($head);

	 for (keys %{$depends{$head}}) {
	    $depends{$filename}{$_} = 1;
	 }
      }
      else {
	 $unresolved{$header} = 1;
      }
   }
}



# scan for source files
sub scan_directory
{
   my $dir = shift;

   for my $filename (<$dir*>) {
      if (-d $filename) {
	 # recurse into a directory
	 scan_directory("$filename/");
      }
      elsif ($filename =~ /\.(c|cpp)$/) {
	 # got a source file
	 print "Found $filename\n";
	 $cplusplus = 1 if ($1 eq "cpp");
	 $sources{$filename} = 1;
	 get_dependencies($filename);
      }
   }
}



# initiate the recursive directory scan
print "Looking for source files\n";

scan_directory("");



# warn about any unresolved headers
%standard_headers = (
   "assert.h" => 1,
   "ctype.h" => 1,
   "errno.h" => 1,
   "fcntl.h" => 1,
   "limits.h" => 1,
   "locale.h" => 1,
   "malloc.h" => 1,
   "math.h" => 1,
   "memory.h" => 1,
   "search.h" => 1,
   "setjmp.h" => 1,
   "signal.h" => 1,
   "stdarg.h" => 1,
   "stdio.h" => 1,
   "stdlib.h" => 1,
   "string.h" => 1,
   "sys/stat.h" => 1,
   "sys/timeb.h" => 1,
   "sys/types.h" => 1,
   "time.h" => 1,
);

for (sort keys %unresolved) {
   print "Warning: program uses non-standard header $_\n" unless ($standard_headers{$_}) or ($param{"STDHDRS"} =~ /$_/);
}



# check if we have info for the djgpp output
if (!$param{"DJGPP_RELEASE_CFLAGS"}) {
   print "DJGPP_RELEASE_CFLAGS parameter not set: skipping djgpp output files\n";
}
elsif (!$param{"DJGPP_DEBUG_CFLAGS"}) {
   print "DJGPP_DEBUG_CFLAGS parameter not set: skipping djgpp output files\n";
}
elsif (!$param{"DJGPP_RELEASE_LFLAGS"}) {
   print "DJGPP_RELEASE_LFLAGS parameter not set: skipping djgpp output files\n";
}
elsif (!$param{"DJGPP_DEBUG_LFLAGS"}) {
   print "DJGPP_DEBUG_LFLAGS parameter not set: skipping djgpp output files\n";
}
else {
   $got_djgpp = 1;
}



# check if we have info for the Unix output
if (!$param{"UNIX_RELEASE_CFLAGS"}) {
   print "UNIX_RELEASE_CFLAGS parameter not set: skipping Unix output files\n";
}
elsif (!$param{"UNIX_DEBUG_CFLAGS"}) {
   print "UNIX_DEBUG_CFLAGS parameter not set: skipping Unix output files\n";
}
elsif (!$param{"UNIX_RELEASE_LFLAGS"}) {
   print "UNIX_RELEASE_LFLAGS parameter not set: skipping Unix output files\n";
}
elsif (!$param{"UNIX_DEBUG_LFLAGS"}) {
   print "UNIX_DEBUG_LFLAGS parameter not set: skipping Unix output files\n";
}
else {
   $got_unix = 1;
}



# check if we have info for the Mingw32 output
if (!$param{"MINGW_RELEASE_CFLAGS"}) {
   print "MINGW_RELEASE_CFLAGS parameter not set: skipping Mingw32 output files\n";
}
elsif (!$param{"MINGW_DEBUG_CFLAGS"}) {
   print "MINGW_DEBUG_CFLAGS parameter not set: skipping Mingw32 output files\n";
}
elsif (!$param{"MINGW_RELEASE_LFLAGS"}) {
   print "MINGW_RELEASE_LFLAGS parameter not set: skipping Mingw32 output files\n";
}
elsif (!$param{"MINGW_DEBUG_LFLAGS"}) {
   print "MINGW_DEBUG_LFLAGS parameter not set: skipping Mingw32 output files\n";
}
else {
   $got_mingw = 1;
}



# write out GNU makefiles for djgpp, Unix, and Mingw32
if ($got_djgpp or $got_unix or $got_mingw) {
   print "Writing makefiles\n";

   if ($got_djgpp) {
      open FILE, "> $build/makefile.dj" or die "Error creating makefile.dj\n";
      print FILE "# generated by builder.pl: makefile for djgpp\n\n";

      print FILE "CC = " . ($cplusplus ? "gxx" : "gcc") . "\n";

      print FILE <<EOF;
EXE = .exe
RELEASE_CFLAGS = $param{DJGPP_RELEASE_CFLAGS}
DEBUG_CFLAGS = $param{DJGPP_DEBUG_CFLAGS}
RELEASE_LFLAGS = $param{DJGPP_RELEASE_LFLAGS}
DEBUG_LFLAGS = $param{DJGPP_DEBUG_LFLAGS}

include makefile.all
EOF
      close FILE;
   }

   if ($got_unix) {
      open FILE, "> $build/makefile.uni" or die "Error creating makefile.uni\n";
      print FILE "# generated by builder.pl: makefile for Unix\n\n";

      print FILE "CC = " . ($cplusplus ? "g++" : "gcc") . "\n";

      print FILE <<EOF;
EXE =
RELEASE_CFLAGS = $param{UNIX_RELEASE_CFLAGS}
DEBUG_CFLAGS = $param{UNIX_DEBUG_CFLAGS}
RELEASE_LFLAGS = $param{UNIX_RELEASE_LFLAGS}
DEBUG_LFLAGS = $param{UNIX_DEBUG_LFLAGS}

include makefile.all
EOF
      close FILE;
   }

   if ($got_mingw) {
      open FILE, "> $build/makefile.mgw" or die "Error creating makefile.mgw\n";
      print FILE "# generated by builder.pl: makefile for Mingw32\n\n";

      print FILE "CC = " . ($cplusplus ? "g++" : "gcc") . "\n";

      print FILE <<EOF;
EXE = .exe
RELEASE_CFLAGS = $param{MINGW_RELEASE_CFLAGS}
DEBUG_CFLAGS = $param{MINGW_DEBUG_CFLAGS}
RELEASE_LFLAGS = $param{MINGW_RELEASE_LFLAGS}
DEBUG_LFLAGS = $param{MINGW_DEBUG_LFLAGS}

EOF

      if ($param{WINICON}) {
         $winicon = $param{WINICON};
         $winicon =~ s/\\/\//g;
         print FILE "OBJS += $unixobj/icon.a\n\n";
      }
      print FILE <<EOF;
include makefile.all

$unixobj/icon.a: $winicon
	echo MYICON ICON \$^ | windres -o \$@
EOF
      close FILE;
   }

   open FILE, "> $build/makefile.all" or die "Error creating makefile.all\n";

   print FILE <<EOF;
# generated by builder.pl: shared between djgpp, Unix and Mingw32

ifdef DEBUGMODE

# debug build
CFLAGS = \$(DEBUG_CFLAGS)
LFLAGS = \$(DEBUG_LFLAGS)

else

# release build
CFLAGS = \$(RELEASE_CFLAGS)
LFLAGS = \$(RELEASE_LFLAGS)

endif

.PHONY: all clean debug release

all: $unixbin/$param{NAME}\$(EXE)

debug:
\t\$(MAKE) DEBUGMODE=1

release:
\t\$(MAKE) DEBUGMODE=

OBJS += \\
EOF

   $first = 1;

   for (sort keys %sources) {
      $filename = $_;
      $filename =~ /([^\/.]+)\./;
      $objname = "$unixobj/$1.o";

      print FILE " \\\n" unless ($first);
      print FILE "\t$objname";

      $first = 0;
   }

   print FILE <<EOF;
\n
$unixbin/$param{NAME}\$(EXE): \$(OBJS)
\t\$(CC) -o \$@ \$^ \$(LFLAGS)

EOF

   for (sort keys %sources) {
      $filename = $_;
      $filename =~ /([^\/.]+)\./;
      $objname = "$unixobj/$1.o";

      print FILE "$objname: " . (join ' ', sort keys %{$depends{$_}}) . "\n";
      print FILE "\t\$(CC) \$(CFLAGS) -c $filename -o \$@\n\n";
   }

   print FILE <<EOF;
CLEAN_FILES = \\
\t$param{"NAME"}.dsw \\
\t$param{"NAME"}.ncb \\
\t$param{"NAME"}.opt \\
\t$param{"NAME"}.plg \\
\t*.pch \\
\t*.idb \\
\t*.err \\
\t$unixobj/*.pch \\
\t$unixobj/*.idb \\
\t$unixobj/*.pdb \\
\t$unixobj/*.obj \\
\t$unixobj/*.o \\
\t$unixobj/*.a \\
\t$unixbin/$param{"NAME"} \\
\t$unixbin/$param{"NAME"}.exe \\
\t$unixbin/$param{"NAME"}.ilk \\
\t$unixbin/$param{"NAME"}.pdb \\
\tallegro.log \\
\tcore

clean:
\trm -f \$(CLEAN_FILES)
EOF

   close FILE;
}



# check if we have info for the Watcom output
if (!$param{"WATCOM_LFLAGS"}) {
   print "WATCOM_LFLAGS parameter not set: skipping Watcom output files\n";
}
elsif (!$param{"WATCOM_CFLAGS"}) {
   print "WATCOM_CFLAGS parameter not set: skipping Watcom output files\n";
}
else {
   $got_watcom = 1;

   # write a batch file for building with Watcom
   print "Writing watmake.bat\n";

   open FILE, "> $build/watmake.bat" or die "Error creating watmake.bat\n";
   binmode FILE;

   print FILE <<EOF;
\@echo off\r
rem generated by builder.pl: compiles using Watcom\r
\r
echo option quiet > _ld.arg\r
echo name $dosbin\\$param{NAME}.exe >> _ld.arg\r
EOF

   while ($param{"WATCOM_LFLAGS"} =~ /"([^"]+)"/g) {
      print FILE "echo $1 >> _ld.arg\r\n";
   }

   print FILE "\r\n";

   for (sort keys %sources) {
      $filename = $_;
      $filename =~ s/\//\\/g;

      $filename =~ /([^\\.]+)\./;
      $objname = "$dosobj\\$1.obj";

      print FILE "echo $filename\r\n"; 
      print FILE "wcl386.exe $param{WATCOM_CFLAGS} -c $filename /fo=$objname\r\n";
      print FILE "echo file $objname >> _ld.arg\r\n\r\n";
   }

   print FILE <<EOF;
echo Linking\r
wlink \@_ld.arg\r
del _ld.arg > nul\r
\r
echo Done!\r
EOF

   close FILE;
}



# check if we have info for the MSVC output
if (!$param{"MSVC_RELEASE_LFLAGS"}) {
   print "MSVC_RELEASE_LFLAGS parameter not set: skipping MSVC output files\n";
}
elsif (!$param{"MSVC_DEBUG_LFLAGS"}) {
   print "MSVC_DEBUG_LFLAGS parameter not set: skipping MSVC output files\n";
}
elsif (!$param{"MSVC_RELEASE_CFLAGS"}) {
   print "MSVC_RELEASE_CFLAGS parameter not set: skipping MSVC output files\n";
}
elsif (!$param{"MSVC_DEBUG_CFLAGS"}) {
   print "MSVC_DEBUG_CFLAGS parameter not set: skipping MSVC output files\n";
}
else {
   $got_msvc = 1;

   # write a MSVC 6.0 .dsp format project
   print "Writing $param{NAME}.dsp\n";

   open FILE, "> $build/$param{NAME}.dsp" or die "Error creating $param{NAME}.dsp\n";

   binmode FILE;
   binmode DATA;

   while (<DATA>) {
      s/\r//g;
      chomp;

      if (/_SOURCES_/) {
	 # write list of source files
	 for (sort keys %sources) {
	    $filename = $_;
	    $filename =~ s/\//\\/g;
	    print FILE "# Begin Source File\r\n\r\nSOURCE=.\\$filename\r\n# End Source File\r\n";
	 }
      }
      elsif (/_HEADERS_/) {
	 # write list of header files
	 for (sort keys %headers) {
	    $filename = $_;
	    $filename =~ s/\//\\/g;
	    print FILE "# Begin Source File\r\n\r\nSOURCE=.\\$filename\r\n# End Source File\r\n";
	 }
      }
      else {
	 # change 'untitled' to our project name
	 s/untitled/$param{"NAME"}/g;

	 # change Intermediate_Dir to our obj directory
	 s/Intermediate_Dir ".*"/Intermediate_Dir "$dosobj"/g;

	 # change Output_Dir to our bin directory
	 s/Output_Dir ".*"/Output_Dir "$dosbin"/g;

	 # change _RLIBS_ and _DLIBS_
	 s/_RLIBS_/$param{"MSVC_RELEASE_LFLAGS"}/;
	 s/_DLIBS_/$param{"MSVC_DEBUG_LFLAGS"}/;

	 # change _RFLAGS_ and _DFLAGS_
	 s/_RFLAGS_/$param{"MSVC_RELEASE_CFLAGS"}/;
	 s/_DFLAGS_/$param{"MSVC_DEBUG_CFLAGS"}/;

	 print FILE "$_\r\n";
      }
   }

   close FILE;

   # write a batch file for building with MSVC
   print "Writing msvcmake.bat\n";

   open FILE, "> $build/msvcmake.bat" or die "Error creating msvcmake.bat\n";
   binmode FILE;

   print FILE <<EOF;
\@echo off\r
rem generated by builder.pl: compiles using MSVC\r
\r
if not "%MSVCDIR%" == "" goto got_msvc\r
if not "%MSDEVDIR%" == "" goto got_msvc\r
\r
echo MSVC not installed: you need to run vcvars32.bat\r
goto end\r
\r
:got_msvc\r
\r
EOF

   $first = 1;

   for (sort keys %sources) {
      $filename = $_;
      $filename =~ s/\//\\/g;

      $filename =~ /([^\\.]+)\./;
      $objname = "$dosobj\\$1.obj";

      print FILE "cl.exe /nologo $param{MSVC_RELEASE_CFLAGS} /c $filename /Fo$objname\r\n";
      print FILE "echo $objname " . ($first ? ">" : ">>") . " _ld.arg\r\n\r\n";

      $first = 0;
   }

   print FILE <<EOF;
echo Linking\r
cl.exe /nologo /Fe$dosbin\\$param{NAME}.exe $param{MSVC_RELEASE_LFLAGS} \@_ld.arg\r
del _ld.arg > nul\r
\r
echo Done!\r
:end\r
EOF

   close FILE;
}



# write a 'make clean' batch file
print "Writing clean.bat\n";

open FILE, "> $build/clean.bat" or die "Error creating clean.bat\n";
binmode FILE;

print FILE <<EOF;
\@echo off\r
rem generated by builder.pl: cleans out temporary files\r
\r
if exist $param{"NAME"}.dsw del $param{"NAME"}.dsw\r
if exist $param{"NAME"}.ncb del $param{"NAME"}.ncb\r
if exist $param{"NAME"}.opt del $param{"NAME"}.opt\r
if exist $param{"NAME"}.plg del $param{"NAME"}.plg\r
if exist *.pch del *.pch\r
if exist *.idb del *.idb\r
if exist *.err del *.err\r
if exist $dosobj\\*.pch del $dosobj\\*.pch\r
if exist $dosobj\\*.idb del $dosobj\\*.idb\r
if exist $dosobj\\*.pdb del $dosobj\\*.pdb\r
if exist $dosobj\\*.obj del $dosobj\\*.obj\r
if exist $dosobj\\*.o del $dosobj\\*.o\r
if exist $dosobj\\*.a del $dosobj\\*.a\r
if exist $dosbin\\$param{"NAME"} del $dosbin\\$param{"NAME"}\r
if exist $dosbin\\$param{"NAME"}.exe del $dosbin\\$param{"NAME"}.exe\r
if exist $dosbin\\$param{"NAME"}.ilk del $dosbin\\$param{"NAME"}.ilk\r
if exist $dosbin\\$param{"NAME"}.pdb del $dosbin\\$param{"NAME"}.pdb\r
if exist allegro.log del allegro.log\r
if exist core del core\r
EOF

close FILE;


print "Generating fixup scripts:\n";

if ($got_djgpp) {
   print "   fixdjgpp.bat\n";
   open FILE, "> fixdjgpp.bat" or die "Error creating fixdjgpp.bat\n";
   binmode FILE;
   print FILE <<EOF;
\@echo off\r
rem generated by builder.pl: configures for building with djgpp\r
if exist clean.bat del clean.bat\r
if exist makefile.* del makefile.*\r
if exist watmake.bat del watmake.bat\r
if exist msvcmake.bat del msvcmake.bat\r
if exist $param{"NAME"}.dsp del $param{"NAME"}.dsp\r
copy build\\makefile.dj makefile\r
copy build\\makefile.all\r
copy build\\clean.bat\r
utod .../*.c .../*.h\r
echo Ready for building with djgpp -- type:\r
echo   'make' for a release build\r
echo   'make debug' for a debugging build\r
echo   'make clean' or just 'clean' to tidy up\r
EOF
   close FILE;
}

if ($got_unix) {
   print "   fixunix.sh\n";
   open FILE, "> fixunix.sh" or die "Error creating fixunix.sh\n";
   print FILE <<EOF;
#!/bin/sh
# generated by builder.pl: configures for building in Unix
rm -f makefile makefile.* clean.bat watmake.bat msvcmake.bat *.dsp
cp -f $build/makefile.uni makefile
cp -f $build/makefile.all .
find -name \*.c -o -name \*.h -exec sh -c 'tmpfile=/tmp/.dtou.\$\$;
	tr -d \\\\r < {} > \$tmpfile &&
	touch -r {} \$tmpfile &&
	mv \$tmpfile {}' \\;
echo "Ready for building in Unix -- type:"
echo "  'make' for a release build"
echo "  'make debug' for a debugging build"
echo "  'make clean' to tidy up"
EOF
   close FILE;
   chmod 0755, "fixunix.sh";
}

if ($got_mingw) {
   print "   fixmingw.bat\n";
   open FILE, "> fixmingw.bat" or die "Error creating fixmingw.bat\n";
   binmode FILE;
   print FILE <<EOF;
\@echo off\r
rem generated by builder.pl: configures for building with Mingw32\r
if exist clean.bat del clean.bat\r
if exist makefile.* del makefile.*\r
if exist watmake.bat del watmake.bat\r
if exist msvcmake.bat del msvcmake.bat\r
if exist $param{"NAME"}.dsp del $param{"NAME"}.dsp\r
copy build\\makefile.mgw makefile\r
copy build\\makefile.all\r
copy build\\clean.bat\r
utod .../*.c .../*.h\r
echo Ready for building with Mingw32 -- type:\r
echo   'make' for a release build\r
echo   'make debug' for a debugging build\r
echo   'make clean' or just 'clean' to tidy up\r
EOF
   close FILE;
}

if ($got_watcom) {
   print "   fixwat.bat\n";
   open FILE, "> fixwat.bat" or die "Error creating fixwat.bat\n";
   binmode FILE;
   print FILE <<EOF;
\@echo off\r
rem generated by builder.pl: configures for building with Watcom\r
if exist clean.bat del clean.bat\r
if exist makefile.* del makefile.*\r
if exist watmake.bat del watmake.bat\r
if exist msvcmake.bat del msvcmake.bat\r
if exist $param{"NAME"}.dsp del $param{"NAME"}.dsp\r
copy build\\watmake.bat\r
copy build\\clean.bat\r
utod .../*.c .../*.h\r
echo Ready for building with Watcom -- type:\r
echo   'watmake' for a release build\r
echo   'clean' to tidy up\r
EOF
   close FILE;
}

if ($got_msvc) {
   print "   fixmsvc.bat\n";
   open FILE, "> fixmsvc.bat" or die "Error creating fixmsvc.bat\n";
   binmode FILE;
   print FILE <<EOF;
\@echo off\r
rem generated by builder.pl: configures for building with MSVC\r
if exist clean.bat del clean.bat\r
if exist makefile.* del makefile.*\r
if exist watmake.bat del watmake.bat\r
if exist msvcmake.bat del msvcmake.bat\r
if exist $param{"NAME"}.dsp del $param{"NAME"}.dsp\r
copy build\\msvcmake.bat\r
copy build\\clean.bat\r
copy build\\$param{"NAME"}.dsp\r
utod .../*.c .../*.h\r
echo Ready for building with MSVC -- type:\r
echo   'msvcmake' for a release build\r
echo   'clean' to tidy up\r
echo or just run the project file $param{"NAME"}.dsp\r
EOF
   close FILE;
}


print "All done, now tell your users to run the fixup scripts to configure for\n";
print "their platforms.  Have a nice day!\n";



# Everything from here on is a Microsoft Visual C++ 6.0 format .dsp file,
# as generated by MSVC 6, with a few minor hand edits replacing some data
# with markers. This script generates our custom files by replacing those
# markers with other data as required.

__DATA__
# Microsoft Developer Studio Project File - Name="untitled" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=untitled - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "untitled.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "untitled.mak" CFG="untitled - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "untitled - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "untitled - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "untitled - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD CPP /nologo _RFLAGS_ /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 _RLIBS_ /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "untitled - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD CPP /nologo _DFLAGS_ /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 _DLIBS_ /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "untitled - Win32 Release"
# Name "untitled - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
_SOURCES_
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
_HEADERS_
# End Group
# End Target
# End Project
