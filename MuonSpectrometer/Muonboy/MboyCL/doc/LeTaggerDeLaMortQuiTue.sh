#!/bin/bash -f

 for package in MboyAthEvt MboyAthTool MboyCL MboyCoreEvt MboyEDMTool MboyEDMToolInterfaces MboyPerformance MboyStatistics MboySvc MboyView MuonboyAth MuonboyCore MboyAthToolInterfaces MboySvcInterfaces ; do
  if [ ! -d ${package} ] 
  then
    svn co   $SVNROOT/MuonSpectrometer/Muonboy/${package}/trunk  ${package} 
  fi
 done

 TagVersion=${1}

 more MuonboyCore/mb_rec/get_muonboy_version.F90

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
   for package in MboyAthEvt MboyAthTool MboyCL MboyCoreEvt MboyEDMTool MboyEDMToolInterfaces MboyPerformance MboyStatistics MboySvc MboyView MuonboyAth MuonboyCore MboyAthToolInterfaces MboySvcInterfaces ; do
    if [ -d ${package} ] 
    then
      echo
      echo " ... start tagging " ${package}-${TagVersion} " ... "
      cd ${package} 
      svn cp . $SVNROOT/MuonSpectrometer/Muonboy/${package}/tags/${package}-${TagVersion} -m "Updating ${TagVersion}"
      cd ..
    fi
   done
 fi
