#!/bin/sh
# File: create-cmthome.sh
#
# Create cmthome with a customised requirements file
# and a test area directory to use with the distribution kit
#
# Usage: ./create-cmthome.sh # from where the kit has been installed
#
# version 0.0.1
#
# Report bugs to <Grigori.Rybkine@cern.ch>.
# Written by Grigori Rybkine.
#

[ -d cmtsite ] || { echo "cmtsite: No such directory"; exit 2; }
if [ -d $HOME/cmthome ]; then
    echo $HOME/cmthome exists
    echo Trying to make a backup of $HOME/cmthome
    mv -v $HOME/cmthome $HOME/cmthome.bak || exit 1
fi

echo Copying
cp -av cmtsite $HOME/cmthome

[ -f $HOME/cmthome/requirements ] || { echo "$HOME/cmthome/requirements: No such file"; exit 2; }

echo Customising $HOME/cmthome/requirements to use with the kit
sed -i -e 's%#.*$%%' -e '/^$/d' -e 's/apply_tag noTest/#apply_tag noTest/' \
-e 's#\(use AtlasLogin.*\)#macro ATLAS_TEST_AREA ${HOME}/testarea\n\n\1#' \
$HOME/cmthome/requirements
echo ATLAS_TEST_AREA will be set to ${HOME}/testarea
echo "check https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasLoginPackage for further details"

[ -d ${HOME}/testarea ] || mkdir -v ${HOME}/testarea

echo Configuring to use CMT from the kit
(. cmtsite/setup.sh >/dev/null 2>&1; cd $HOME/cmthome; cmt config)
