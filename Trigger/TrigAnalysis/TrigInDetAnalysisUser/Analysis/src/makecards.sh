#!/bin/csh

echo "DataFiles = { "

set first = ""
set firstset = 0

foreach dir ( $1 ) 

 cd $dir

 set datadir = $cwd



 foreach rfile ( *.root* )
   if ( "$firstset" == 0 ) then 
     set firstset = 1
     set first = $rfile
   else
     echo "   "\"$cwd/$rfile\"","
   endif 
 end


end

 echo "   "\"$cwd/$first\" 
 echo "};"

