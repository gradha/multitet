#!/bin/sh

for file in ??text.cfg; do
   echo "Regenerating ../$file"
   cat allegro/$file > ../$file
   cat $file | textconv -ia -ou >> ../$file
done
