`builder.pl' Build System -- Brief Documentation
================================================

This Perl script was designed to greatly simplify the process of making
a portable distribution of your game.  Most of it was written by Shawn 
Hargreaves, and George Foot extended it to create this version.


Configuration
~~~~~~~~~~~~~
Before you can use this system, you must configure it.  This is pretty 
simple -- open `build/builder.in' in your text editor and modify it to 
suit your needs.  Some notes:

    NAME:
        This is the basename of the executable, of the MSVC .dsp file,
        and the name of the project in MSVC.

    PACKAGES:
        This allows you to select predefined packages which your project 
        uses.  They modify the settings after your file is read.  See 
        below for more information on packages.

    WINICON:
        This is optional; if set, the Mingw32 build will attach the icon
        to your executable.  It's not supported in the MSVC build; I 
        don't know how to do it.

    OBJDIR, BINDIR:
        Directories for object files and your executable.  Make sure 
        they exist in the package you send to users -- remember to put
        dummy files into empty directories, because some unzip programs
        won't create them otherwise.

The remainder of the file gives flags to pass various compilers for 
compiling and linking the release or debug version of your program.  The
examples should work; you may want to tweak the options for compilers 
you understand.


Usage
~~~~~
When you've written the config file, you can run `build/builder.pl'.
Make sure you're in the directory above its directory when you do this.
It will generate some build scripts (makefiles, batch files, and a .dsp)
in the `build' directory, and some fixup scripts in the top directory.
When your users run the appropriate fixup script for their systems, the
relevant build scripts will be copied into the top directory, ready for
use.


Packages
~~~~~~~~
The most obvious use for this is to provide linker options for extra 
libraries, which is what the `allegro' and `libnet' packages do, as 
examples.  Packages are defined by the `.pkg' files in the `build' 
directory.  The same options are valid here as in the main config file,
with an added syntax: you can write "OPTION += value" to add `value' to
the settings for `OPTION'.  This is usually the syntax you want to use 
in package files.

In addition, there is an option which is most useful in package files,
though still valid in builder.in files:

    STDHDRS:
        Lists headers which are to be assumed standard.  The builder
        gives warnings when it can't locate a dependency, unless it is
        a standard header file.  The builder already knows about most
        ANSI header files, but addon packages can use this option to
        suppress warnings about their own header files.


Credits and Contributions
~~~~~~~~~~~~~~~~~~~~~~~~~
builder.pl was first written by Shawn Hargreaves in December 1999, and 
was extended by George Foot in August 2000.  Unless he reads this, 
Shawn won't know much about those extensions, so any queries or 
contributions are probably best directed to George (me).

    George Foot:      gfoot@users.sourceforge.net
    Shawn Hargreaves: shawn@talula.demon.co.uk


