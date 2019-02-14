# TO RUN THE GUI, PLEASE RUN THE COMMAND:
# source TrigMaMGUI_P1.sh
echo
echo "     Trigger Menu-aware Monitoring Graphical User Interface"
echo "     for use with TRIGGERDB"
echo "     For more info about Menu-aware Monitoring see"
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
#javac -version $SCRIPTDIR/GUI/TrigMaMGUI.java
#java -cp -showversion $SCRIPTDIR/GUI: TrigMaMGUI TRIGGERDBR2MAM
#Need higher java version, use 1.8
#/sw/atlas/sw/lcg/external/Java/JDK/1.8.0/amd64/bin/javac -version $SCRIPTDIR/GUI/TrigMaMGUI.java
/sw/atlas/sw/lcg/external/Java/JDK/1.8.0/amd64/jre/bin/java -cp -showversion $SCRIPTDIR/GUI: TrigMaMGUI TRIGGERDBR2MAM
