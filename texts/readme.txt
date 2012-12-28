This directory contains the text resource files used by
Multitet. Each supported language has an ??text.cfg file, build.sh
is used to rebuild the real ??text.cfg file used by the game,
which is a mixture of Allegro's message translations and Multitet's
message translations.

If you are interested in translating Multitet, ignore the allegro
subdirectory (unless you want to translate those messages too)
and translate entext.cfg to your language, then send it to me at
gradha@users.sourceforge.net for inclusion in the game, after you
have verified the translation with your own copy of Multitet.

To test your translation, copy the ??text.cfg you have written to
the upper directory (where the Multitet binary is found), and add
to the top of it something like (don't copy the dotted lines):

.....................
language_name = your_language_name_written_in_native_form

[language]
.....................

With that little header Multitet will be able to understand
your translation file and hopefully will show it in the language
selection dialog. Nearly... the files you find here use the Latin1
codepage because it's easier for me to maintain them. If you take
a look at build.sh you will see that the final .cfg files used by
Multitet require them to be in Unicode (UTF8) format.

This means that probably you won't be able to see your native
characters properly unless you have an utf8 capable editor. In any
case, send me the file and I will answer you with the correct
final version.
