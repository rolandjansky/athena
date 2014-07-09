#/afs/cern.ch/atlas/software/builds/AtlasCAFHLT/16.1.2.8.1/InstallArea/share/TriggerMenuXML-release.txt
get_files -remove -data TriggerMenuXML-release.txt
rel=`cat TriggerMenuXML-release.txt`

#TrigConfConsistencyChecker --lvl1xml LVL1config_Physics_pp_v3.xml --hltxml HLTconfig_Physics_pp_v3_${rel}.xml --exceptions TriggerMenuPython/menu_check_exceptions.xml 

TrigConfConsistencyChecker --lvl1xml LVL1config_Physics_HI_v1_${rel}.xml --hltxml HLTconfig_Physics_HI_v1_${rel}.xml --exceptions TriggerMenuPython/menu_check_exceptions.xml 

#TrigConfConsistencyChecker --cool --run 131443

#TrigConfConsistencyChecker --trigdb --keys 572 674 --verbose 1
