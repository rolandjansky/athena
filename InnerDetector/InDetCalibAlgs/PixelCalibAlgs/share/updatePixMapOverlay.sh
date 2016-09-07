#!/bin/bash
#####################################################################
# * Usuage: test.sh [run#] [password for cool server]
#####################################################################
if [ $1 -lt 200000 ]; then
    echo "No correct input. Do \"test.sh [run#] [cool password]\""
    echo "exiting."
    exit 0
fi

if [ $2 = "WCOOLOFL4PIXEL11" ]; then
    echo "Start processing..."
else
    echo "No correct input. Do \"test.sh [run#] [cool password]\""
    echo "exiting."
    exit 0
fi

runid=$1
coolpd=$2 #WCOOLOFL4PIXEL11

data_path=root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-pixdq/Tier0/data16_13TeV/express_express/00$runid/data16_13TeV.00$runid.express_express.pixmerge.ROOT_OCCUPANCYMAP.c1
data=data16_13TeV.00$runid.express_express.pixmerge.ROOT_OCCUPANCYMAP.c1._0001.1

echo xrdcp $data_path/$data

xrdcp $data_path/$data /tmp/${USER}/

makeInactiveModuleList.exe $runid /tmp/${USER}/$data > log_run$1.txt
sleep 1

python PixMapOverlay_run$runid.py
sleep 1

comment="updating the inactive module list for $runid."
/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --batch --nobackup --comment="$comment" PixMapOverlay_run$runid.db CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_PIXEL_W $coolpd

#test.exe first