#!/bin/zsh

in=root://eosatlas//eos/atlas/atlascerngroupdisk/det-zdc/ZdcData/standalone/data15_calib.00283741.calibration_.daq.RAW._lb0000._ROS-FWD-ZDC-00._0001.data
max=-1

if [ ! -z $1 ]; then
   in=$1
fi

if [ ! -z $2 ]; then
   max=$2
fi

echo Running on $in
echo Running $max events

Reco_tf.py --outputESDFile=esd.pool.root --outputAODFile=xaod.pool.root --preInclude ZdcRecConfig.py --postExec 'e2a:from ZdcRec.ZdcRecUtils import AppendOutputList;AppendOutputList(StreamAOD.ItemList);' --conditionsTag CONDBR2-BLKPA-2015-11  --geometryVersion ATLAS-R2-2015-03-01-00 --maxEvents $max --inputBSFile $in
