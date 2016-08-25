# TO RUN THE GUI, PLEASE RUN THE COMMAND:
# source start_TrigMaMGUI_TRIGGERDBREPR.sh
# This script assumes the following file locations, relative to the location of the script: 
# MenuAwareMonitoringStandalone.py and OracleInterface.py are in ../python
# and ./GUI/TrigMaMGUI_TRIGGERDBREPR.java

if [ -z "$AtlasPatch" ]
then 
echo "Please setup a release first."
echo "> setupATLAS"
echo "> asetup ..."
return 1
fi

echo
echo "     Trigger Menu Aware Monitoring Graphical User Interface"
echo "     for use with TRIGGERDBREPR"
echo "     by Xanthe Hoad xanthe.hoad@cern.ch"
echo "     For more info about Menu Aware Monitoring see" 
echo "     https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM"
echo

if [[ $(python -V 2>&1) == *"2.6"* ]]
then 
    echo "  MAM needs Python version > 2.7, setting up..."
    echo
    setupATLAS
    lsetup python
fi

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MaMPythonFilesPath=${SCRIPTDIR%/*}/python

if [[ $PYTHONPATH == *$MaMPythonFilesPath* ]]
then
    echo $MaMPythonFilesPath "is in PYTHONPATH"
else
    echo "Adding" $MaMPythonFilesPath "to PYTHONPATH"
    export PYTHONPATH=$MaMPythonFilesPath:$PYTHONPATH
fi

javac $SCRIPTDIR/GUI/TrigMaMGUI_TRIGGERDBREPR.java
java -cp $SCRIPTDIR/GUI: TrigMaMGUI_TRIGGERDBREPR