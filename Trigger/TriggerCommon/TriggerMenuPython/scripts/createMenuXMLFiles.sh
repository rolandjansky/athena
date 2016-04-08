#!/usr/bin/env zsh

release=19.0.0
jo=TriggerMenuXML/runHLT_forXMLgeneration.py

menu_names=(
    Physics_pp_v4
)

((i = 1))
for menu in $menu_names; do
    echo "Running menu: $menu for $release"
    logfile=${menu}.log
    if [[ -e $logfile ]]; then
	rm -f $logfile
    fi
    athena.py -c "exitAfterConfig=True;TriggerMenuSetup='$menu'" $jo >& $logfile
    mv outputLVL1config.xml LVL1config_${menu}_${release}.xml
    mv outputHLTconfig.xml HLTconfig_${menu}_${release}.xml
    ((i = $i+1))
done

