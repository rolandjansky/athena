#!/usr/bin/env zsh
# Script to generate generate the trigger menu XML files
#

if [ $# -ne 1 ]; then
    echo "Syntax: XMLDumperFromAthena.sh MENU"
    exit 1
fi

menu=$1
jo=TriggerMenuXML/runHLT_forXMLgeneration.py
sqliteFile=TriggerMenuSQLiteFile.sqlite
uploadLog=TriggerMenuUpload.log
doDBUpload="false"

if [[ "$NICOS_ATLAS_RELEASE" != "" ]]; then
    release=$NICOS_ATLAS_RELEASE
    if [[ "$AtlasProject" != "AtlasP1HLT" && "$AtlasProject" != "AtlasCAFHLT" && "$AtlasProject" != "AtlasTestHLT" ]]; then
        #doDBUpload="true"     # Temporarily disabled due to sqlite slowness (Savannah #102038)
        # we upload the default menu
	menuForUpload=`python -c "from TriggerJobOpts import TriggerFlags;print TriggerFlags.triggerMenuSetup().get_Value()" | tail -1`
    fi
else
    release=`python -c "from TriggerJobOpts import TriggerFlags; print(TriggerFlags.menuVersion().get_Value())" | tail -1`
fi

if [[ -e MenusKeys.txt ]]; then
    echo "XMLDumperFromAthena: Removing old MenusKeys.txt "
    rm -f MenusKeys.txt
fi

# Setup TriggerTool
if [ ${doDBUpload} = "true" ]; then
    source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
    get_files -data -remove -symlink TriggerTool.jar
    export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
fi

# Temporary run directroy and cleanup traps in case of termination
rundir=`mktemp -t -d tmxml.${menu}.XXXXXXXXXX`
TRAPINT() {
    rm -rf $rundir
    return 130 # 128+SIGINT
}
TRAPTERM() {
    rm -rf $rundir
    return 143 # 128+SIGTERM
}


## menu generation starts here
pkgdir=`readlink -f $PWD/../`
echo "XMLDumperFromAthena: *** Building menu: $menu for $release ***"
logfiletopo=topo${menu}.log
logfile=${menu}.log
cd $rundir

# L1Topo config file
generateL1TopoMenu.py $menu >&! $logfiletopo
cp L1Topoconfig_*.xml ${pkgdir}/data


# L1 + HLT config file
athena.py -c "TriggerMenuSetup='$menu'" $jo >&! $logfile
athena_exit=$?
cp $logfile $logfiletopo ${pkgdir}/${CMTCONFIG}
if [ $athena_exit -eq 0 ]; then
    echo "XMLDumperFromAthena: *** $menu DONE | Exit code: $athena_exit | Log: $pkgdir/$CMTCONFIG/$logfile ***"
else
    echo "XMLDumperFromAthena: *** $menu FAILED | Exit code: $athena_exit | Log: $pkgdir/$CMTCONFIG/$logfile ***"
fi




#upload default menu into to the SQLite file
#doDBUpload is True if we are in NICOS and not in the P1HLT cache
if [[ ( ${doDBUpload} = "true" ) && ( ${menu} = ${menuForUpload} ) && ( -s outputHLTconfig.xml ) ]]; then
    echo "XMLDumperFromAthena: Uploading the xml files to the DB"
    rm -f $sqliteFile
    cmd=(java -jar TriggerTool.jar -up -release $release -l1_menu outputLVL1config.xml -hlt_menu outputHLTconfig.xml -name $menu -offlineconfig -dbConn "sqlite_file:$sqliteFile" --outputLevel FINER --output $uploadLog)
    echo "XMLDumperFromAthena: Upload command: java $cmd"
    ${cmd}
    cp $uploadLog $pkgdir/$CMTCONFIG/TriggerMenuUpload.log
    echo "Copying sqlite file $sqliteFile to $pkgdir/data/TriggerMenuSQLiteFile_${release}.sqlite"
    chmod 777 $sqliteFile
    cp $sqliteFile ${pkgdir}/data/TriggerMenuSQLiteFile_${release}.sqlite
    
    if [[ -e MenusKeys.txt ]]; then
        echo "Copying MenuKeys.txt to ${pkgdir}/data/MenusKeys_${release}.txt"
        cp MenusKeys.txt ${pkgdir}/data/MenusKeys_${release}.txt
    else
        echo "Error : MenuKeys.txt does not exist, likely a DB upload problem "
    fi
fi

if [[ -e outputLVL1config.xml ]]; then
    cp outputLVL1config.xml ${pkgdir}/data/LVL1config_${menu}_${release}.xml
    cp outputHLTconfig.xml ${pkgdir}/data/HLTconfig_${menu}_${release}.xml
fi

#this gives some more sensitivity to hidden problems
grep --colour ERROR $pkgdir/$CMTCONFIG/$logfile
grep --colour -A 100 "Shortened traceback" $pkgdir/$CMTCONFIG/$logfile

cd $pkgdir/cmt
rm -rf $rundir

# Do not return real athena exit code as we want to pretend everything was fine

