#!/bin/bash

echo 'XMLDumperFromDB.sh:    Reading menu from the TriggerDB'


if [[ -e TriggerTool.jar ]]; then
    echo "XMLDumperFromDB.sh:    TriggerTool.jar already exists. Setting up environment."
    source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
    export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
else
    source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
    echo "XMLDumperFromDB.sh:    Getting TriggerTool"
    get_files -data -symlink TriggerTool.jar
    export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
fi

#download from data-base
echo "XMLDumperFromDB.sh:    Downloading L1 files from MC DB (SMK = $1, L1 PSK = $2) ..." 
export DBConn="oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_MC"

java -jar TriggerTool.jar -down -k $1 -l1psk $2 -dbConn $DBConn > XMLdumplog_$1_$2

if [[ -e LVL1Menu_$1_$2.xml ]]; then
    echo "XMLDumperFromDB.sh:    Moving LVL1Menu_$1_$2.xml ../data/LVL1config_$3.xml" 
    mv LVL1Menu_$1_$2.xml ../data/LVL1config_$3.xml
    echo "XMLDumperFromDB.sh:    Dump log in ../$CMTCONFIG/XMLdumplog_$1_$2" 
    mv XMLdumplog_$1_$2 ../$CMTCONFIG
else
    echo "XMLDumperFromDB.sh:    ERROR: something went wrong with the DB download. Please, see a logfile in ../$CMTCONFIG/XMLdumplog_$1_$2"
fi

echo "XMLDumperFromDB.sh:    ------------------------------------------------------------------------------------------------------------"
