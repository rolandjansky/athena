#!/bin/bash -f

 for package in AmdcAth AmdcCore AmdcStand AmdcMGM AmdcData AmdcOracle AmdcDb ; do
   if [ ! -d ${package} ] 
   then
     svn co   $SVNROOT/MuonSpectrometer/Amdcsimrec/${package}/trunk  ${package} 
   fi
 done

 TagVersion=${1}

 echo "-->"
 echo "--> *************************************************** "
 echo "-->            NEW VERSION NUMBER "${TagVersion}
 echo "--> *************************************************** "
 echo "-->"
 echo "-->"

 read -p  "-->   Continue (y/n)? " DoContinue
 if [ "${DoContinue}" != "y"  ] 
 then
   echo "--> Give up!"
 else
   for package in AmdcAth AmdcCore AmdcStand AmdcMGM AmdcData AmdcOracle AmdcDb ; do
     if [ -d ${package} ] 
     then
       echo " ... start tagging " ${package}-${TagVersion} " ... "
       cd ${package} 
       svn cp . $SVNROOT/MuonSpectrometer/Amdcsimrec/${package}/tags/${package}-${TagVersion} -m "Updating ${TagVersion}"
       cd ..
     fi
   done
 fi
