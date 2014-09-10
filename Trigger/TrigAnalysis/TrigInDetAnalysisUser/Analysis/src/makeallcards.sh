#!/bin/csh

foreach git ( 1421{65,66,74,91,93,95} ) 
   set name = data-$git.dat
   set dir = `grep $git data.txt | awk '{print $2}'`
   makecards.sh $dir > $name
end
