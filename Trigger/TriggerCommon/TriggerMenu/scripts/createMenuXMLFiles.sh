#!/usr/bin/env zsh

release=14.4.0
jo=TriggerMenu/testAthenaModernRDO_forXMLgeneration.py

menu_names=(
    # default # not needed anymore
    #
    full
    full_no_Bphysics
    full_no_prescale
    full_no_Bphysics_no_prescale
    # lumi1E31
    lumi1E31
    lumi1E31_no_prescale
    lumi1E31_no_Bphysics
    lumi1E31_no_Bphysics_no_prescale
    # lumi1E32
    lumi1E32
    lumi1E32_no_prescale
    lumi1E32_no_Bphysics
    lumi1E32_no_Bphysics_no_prescale
    # lumi1E33
    lumi1E33
    lumi1E33_no_prescale
    lumi1E33_no_Bphysics
    lumi1E33_no_Bphysics_no_prescale
    # 
    Physics_startup
)

((i = 1))
for menu in $menu_names; do
    echo "Running menu: $menu for $release"
    logfile=${i}.log
    if [[ -e $logfile ]]; then
	rm -f $logfile
    fi
    athena.py -c "exitAfterConfig=True;TriggerMenuSetup='$menu'" $jo >& $logfile
    mv outputLVL1config.xml LVL1config_${menu}_${release}.xml
    mv outputHLTconfig.xml HLTconfig_${menu}_${release}.xml
    ((i = $i+1))
done

