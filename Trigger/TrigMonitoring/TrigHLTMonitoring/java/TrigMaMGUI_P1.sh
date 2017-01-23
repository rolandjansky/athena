# TO RUN THE GUI, PLEASE RUN THE COMMAND:
# source TrigMaMGUI_P1.sh
echo
echo "     Trigger Menu Aware Monitoring Graphical User Interface"
echo "     by Xanthe Hoad xanthe.hoad@cern.ch"
echo "     For more info about Menu Aware Monitoring see"
echo "     https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM"
echo

source /sw/atlas/AtlasSetup/scripts/asetup.sh p1hlt,20.11.0.23,here

export MAM_CORAL_DBLOOKUP_PATH=/det/tdaq/hlt/mam/authentication/
export MAM_CORAL_AUTH_PATH=$MAM_CORAL_DBLOOKUP_PATH

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MaMPythonFilesPath=${SCRIPTDIR%/*}/python

if [[ $PYTHONPATH == *$MaMPythonFilesPath* ]]
then
    echo $MaMPythonFilesPath "is in PYTHONPATH"
else
    echo "Adding" $MaMPythonFilesPath "to PYTHONPATH"
    export PYTHONPATH=$MaMPythonFilesPath:$PYTHONPATH
fi

#Uncomment the following line to recompile the GUI
#WARNING: If compiling, please call the script from an empty directory
#The java compiler at P1 scrambles files when run in a non-empty directory
#javac $SCRIPTDIR/GUI/TrigMaMGUI.java
java -cp $SCRIPTDIR/GUI: TrigMaMGUI TRIGGERDBR2MAM
