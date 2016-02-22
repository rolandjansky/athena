# TO RUN THE GUI, PLEASE RUN THE COMMAND:
# source start_TrigMaMGUI.sh
# This script assumes the following file locations, relative to the location of the script: 
# MenuAwareMonitoringStandalone.py and OracleInterface.py are in ../python
# and ./GUI/TrigMaMGUI.java
echo
echo "     Trigger Menu Aware Monitoring Graphical User Interface"
echo "     by Xanthe Hoad xanthe.hoad@cern.ch"
echo "     For more info about Menu Aware Monitoring see" 
echo "     https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM"
echo

source /sw/atlas/AtlasSetup/scripts/asetup.sh p1hlt,20.2.3.2,gcc48

CORAL_DBLOOKUP_PATH=/det/tdaq/hlt/mam/authentication/
CORAL_AUTH_PATH=$CORAL_DBLOOKUP_PATH

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export MaMPythonFilesPath=${SCRIPTDIR%/*}/python

if [[ $PYTHONPATH == *$MaMPythonFilesPath* ]]
then
    echo $MaMPythonFilesPath "is in PYTHONPATH"
else
    echo "Adding" $MaMPythonFilesPath "to PYTHONPATH"
    export PYTHONPATH=$MaMPythonFilesPath:$PYTHONPATH
fi

javac $SCRIPTDIR/GUI/TrigMaMGUI.java
java -cp $SCRIPTDIR/GUI: TrigMaMGUI