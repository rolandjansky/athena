#!/usr/bin/env zsh

#export NICOS_ATLAS_RELEASE=14.5.0
#release=$NICOS_ATLAS_RELEASE
release=$1

echo "******** Release: $release ********"

jo=TriggerMenuPython/testAthenaModernRDO_forXMLgeneration.py
jo_onlyLVL1=TriggerMenuPython/testAthenaModernRDO_forXMLgeneration_onlyLVL1.py
get_files -jo -symlink runHLT_standalone.py

jo_cosmics=runHLT_standalone.py
sqliteFile=/tmp/TriggerMenuSQLiteFileWithSettings.sqlite

#this is needed to get an input data sample
source RecExCommon_links.sh

menu_names=(
    ## physics pp v1
    MC_pp_v1_no_prescale
    MC_pp_v1_loose_mc_prescale
    MC_pp_v1_tight_mc_prescale
    ## physics pp v2
    MC_pp_v2_no_prescale
    MC_pp_v2_loose_mc_prescale
    MC_pp_v2_tight_mc_prescale
)

#remove old files
if [[ -e TriggerTool.jar ]]; then
    echo "Removing old TriggerTool.jar"
    rm -f TriggerTool.jar
fi
if [[ -e $sqliteFile ]]; then
    echo "Removing old $sqliteFile "
    rm -f $sqliteFile
fi
if [[ -e MenusKeys.txt ]]; then
    echo "Removing old MenusKeys.txt "
    rm -f MenusKeys.txt
fi

source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
echo "Getting TriggerTool"
get_files -data -symlink TriggerTool.jar
export _JAVA_OPTIONS="-Xms256m -Xmx1048m"	    

for menu in $menu_names; do

    echo "*** Running menu: $menu for $release ***"
    logfile=${menu}.log
    if [[ -e $logfile ]]; then
	rm -f $logfile
    fi
    athena.py -c "EvtMax=1;doDBConfig=True;TriggerMenuSetup='$menu'" $jo >& $logfile
    echo "Converting the settings txt file to xml "
    ConvertHLTSetup_txt2xml.py L2EF_setup.txt

    #temporarily upload only lumi1E31 to the SQLite file (takes too much time)
    if [[ ${menu} == "MC_pp_v2_no_prescale" ]]; then
	if [[ -s outputHLTconfig.xml ]]; then
	
	    echo "Uploading the xml files to the DB"
	    java -jar TriggerTool.jar -up -release $release -l1_menu outputLVL1config.xml -hlt_menu outputHLTconfig.xml -hlt_l2_setup L2EF_setup.xml -hlt_ef_setup L2EF_setup.xml -name $menu -offlineconfig -dbConn "sqlite_file:$sqliteFile"
	    
	fi
    fi

    echo "Check whether output exists"
    if [[ -e outputLVL1config.xml ]]; then
	echo "Copying output"
	mv outputLVL1config.xml LVL1config_${menu}_${release}.xml
	mv outputHLTconfig.xml HLTconfig_${menu}_${release}.xml
	mv L2EF_setup.xml HLTsetup_${menu}_${release}.xml
    else
	echo "Error : HLT failed, try only LVL1"

	logfile_onlyLVL1=${menu}_onlyLVL1.log
	if [[ -e $logfile_onlyLVL1 ]]; then
	    rm -f $logfile_onlyLVL1
	fi
	athena.py -c "exitAfterConfig=True;TriggerMenuSetup='$menu'" $jo_onlyLVL1 >& $logfile_onlyLVL1

	mv outputLVL1config.xml LVL1config_${menu}_${release}.xml
	rm outputHLTconfig.xml
    fi

    #this gives some more sensitivity to hiden problems
    #grep ERROR $logfile #This gives us too many errors
    grep -A 100 "Shortened traceback" $logfile
    
done

if [[ -s $sqliteFile ]]; then
    echo "Copying sqlite"
    chmod 777 $sqliteFile #hopefully this will give us rights to read the file directly from afs
    mv $sqliteFile TriggerMenuSQLiteFileWithSettings_${release}.sqlite
    mv MenusKeys.txt MenusKeys_${release}.txt
else
    echo 'ERROR SQLite file has not been produced. Most likely this nightly is broken!!!'
    exit 1
fi
