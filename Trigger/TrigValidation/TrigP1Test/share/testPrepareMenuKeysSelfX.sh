#!/bin/bash
       
echo 'Preparing SMKey for P1 and/or CAF reprocessing'

#get the L1 file (common) for v4 menu by default if $2 is not specified 
l1menu=LVL1config_Physics_pp_v4.xml
if [ $2 ] 
then
    l1menu="$2"
fi
echo l1menu is $l1menu
get_files -xmls -copy $l1menu
#configuration for physicsV4_caf or mcV4_caf or physicsV4_menu
configType=$1
#prepare files for first key: l2 and ef menus are the same (full menu)
hltmenu1=`find ../AllMT_${configType}/ -name outputHLTconfig_L2PU\*.xml`

mkdir -p setup/$configType
mkdir $configType

cp ../AllMT_$configType/l2_Default_setup.txt $configType/.
cp ../AllMT_$configType/l2_Default_setup_setup.txt $configType/.
cp ../AllPT_$configType/ef_Default_setup.txt $configType/.
cp ../AllPT_$configType/ef_Default_setup_setup.txt $configType/.
cp ../AllMT_$configType/outputHLTconfig_L2PU-1.xml $configType/.

cd $configType
ConvertHLTSetup_txt2xml.py ./l2_Default_setup.txt ./l2_Default_setup_setup.txt > convertMT1
ConvertHLTSetup_txt2xml.py ./ef_Default_setup.txt ./ef_Default_setup_setup.txt > convertPT1
cd -

l2__setup1=./$configType/l2_Default_setup.xml
ef__setup1=./$configType/ef_Default_setup.xml

cp $l1menu setup/$configType/lvl1Menu.xml
cp $configType/l2_Default_setup.xml setup/$configType/l2setup.xml
cp $configType/ef_Default_setup.xml setup/$configType/efsetup.xml
cp $configType/outputHLTconfig_L2PU-1.xml setup/$configType/hltMenu.xml

p1_rel="P1HLTtest"
if [ $NICOS_ATLAS_RELEASE ]
then
    p1_rel=$NICOS_ATLAS_RELEASE
fi

buildConfigInstaller.sh $configType $p1_rel

