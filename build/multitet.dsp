# Microsoft Developer Studio Project File - Name="multitet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=multitet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "multitet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "multitet.mak" CFG="multitet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "multitet - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "multitet - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "multitet - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "obj"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD CPP /nologo /W3 /GX /O2 /YX /FD /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32  alleg.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "multitet - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "obj"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /YX /FD /GZ /D "DEBUGMODE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32  alld.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "multitet - Win32 Release"
# Name "multitet - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\error.c
# End Source File
# Begin Source File

SOURCE=.\src\gamecore.c
# End Source File
# Begin Source File

SOURCE=.\src\gui.c
# End Source File
# Begin Source File

SOURCE=.\src\init.c
# End Source File
# Begin Source File

SOURCE=.\src\intro.c
# End Source File
# Begin Source File

SOURCE=.\src\langsel.c
# End Source File
# Begin Source File

SOURCE=.\src\misc.c
# End Source File
# Begin Source File

SOURCE=.\src\multitet.c
# End Source File
# Begin Source File

SOURCE=.\src\score.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\data.h
# End Source File
# Begin Source File

SOURCE=.\src\error.h
# End Source File
# Begin Source File

SOURCE=.\src\gamecore.h
# End Source File
# Begin Source File

SOURCE=.\src\gui.h
# End Source File
# Begin Source File

SOURCE=.\src\init.h
# End Source File
# Begin Source File

SOURCE=.\src\intro.h
# End Source File
# Begin Source File

SOURCE=.\src\langsel.h
# End Source File
# Begin Source File

SOURCE=.\src\misc.h
# End Source File
# Begin Source File

SOURCE=.\src\multitet.h
# End Source File
# Begin Source File

SOURCE=.\src\score.h
# End Source File
# End Group
# End Target
# End Project
