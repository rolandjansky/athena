# Author: Xanthe Hoad (xanthe.hoad@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

if [ -z "$AtlasProject" ]
then
    echo "Please setup a release first."
    echo "> setupATLAS"
    echo "> asetup ..."
    return 1
fi

echo
echo "     Trigger Menu-aware Monitoring Graphical User Interface"
echo "     for use with TRIGGERDBREPR"
echo "     For more info about Menu-aware Monitoring see"
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
TARGETDIR="/tmp/${USER}/MaMGUI"
mkdir -p $TARGETDIR

javac $SCRIPTDIR/../java/TrigMaMGUI.java -d $TARGETDIR
java -cp $TARGETDIR: TrigMaMGUI TRIGGERDBREPR
