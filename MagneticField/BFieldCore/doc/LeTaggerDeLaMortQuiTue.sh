#!/bin/bash -f
#
 TagVersion=${1}
#
 echo "-->"
 echo "--> *************************************************** "
 echo "-->            NEW VERSION NUMBER "${TagVersion}
 echo "--> *************************************************** "
 echo "-->"
 echo "-->"
#
 read -p  "-->   Continue (y/n)? " DoContinue
 if [ "${DoContinue}" != "y"  ] 
 then
   echo "--> Give up!"
 else
   DirPasGloopList=( cmt CVS doc Amdbsimrec BFieldAgeH BFieldAthH BFieldCoreH BFieldStandH ChangeLog MagneticField MagneticFieldAge MagneticFieldAgeHeader MagneticFieldAthena MagneticFieldAthenaHeader MagneticFieldAthenaTest MagneticFieldCore MagneticFieldCoreHeader MagneticFieldStand MagneticFieldStandHeader src test Reconstruction)
   for DirCur in `ls`
   do
     DoTag="yes"
     for DirPasGloop in ${DirPasGloopList[@]}
     do
       if [ "${DirCur}"  == "${DirPasGloop}" ] 
       then
         DoTag="no"
       fi
     done
     if [ "${DoTag}"  == "yes" ] 
     then
       TheString=" ${DirCur}-${TagVersion}        offline/MagneticField/${DirCur}"
       echo "${TheString}"
       cvs rtag ${TheString}
     fi
   done
   TheString=" MagneticField-"${TagVersion}" offline/MagneticField/"
   echo "${TheString}"
   cvs rtag  ${TheString}
#
 fi
