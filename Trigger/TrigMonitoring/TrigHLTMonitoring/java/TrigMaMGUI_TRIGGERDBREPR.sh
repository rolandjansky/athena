# TO RUN THE GUI, PLEASE RUN THE COMMAND:
# source TrigMaMGUI_TRIGGERDBREPR.sh

if [ -z "$AtlasProject" ]
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

javac $SCRIPTDIR/../java/TrigMaMGUI.java
java -cp $SCRIPTDIR/../java: TrigMaMGUI TRIGGERDBREPR
