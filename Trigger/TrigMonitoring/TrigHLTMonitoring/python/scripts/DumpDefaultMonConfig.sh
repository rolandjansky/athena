#!/bin/bash
if [ "$#" -ne 5 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

cleanheaddir="$1"
cleandir="$2"
release="$3"
filename="$4"
monmode="$5"

echo $cleanheaddir $cleandir $release $filename $monmode
mkdir -p $cleanheaddir
cd $cleanheaddir
mkdir -p $cleandir
cd $cleandir
source $AtlasSetup/scripts/asetup.sh $release
if [[ !  -z  $monmode  ]]; then
    athena.py -i -c "from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring;mam = MenuAwareMonitoring();mam.setup_all_local_tools('"$monmode"');mam.dump_local_config_to_json(output_json_filename='"$filename"',comment='"$release" default',default=1);print 'Made default';quit()"
else
    athena.py -i -c "from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring;mam = MenuAwareMonitoring();mam.setup_all_local_tools();mam.dump_local_config_to_json(output_json_filename='"$filename"',comment='"$release" default',default=1);print 'Made default';quit()"
fi