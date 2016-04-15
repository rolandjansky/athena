#!/bin/zsh

zrun=285947
if [ ! -z $1 ]; then
   zrun=$1
fi

zmax=100
if [ ! -z $2 ]; then
   zmax=$2
fi

zfile=data15_calib.00$zrun.calibration_.daq.RAW._lb0000._ROS-FWD-ZDC-00._0001.data
zin=root://eosatlas//eos/atlas/atlascerngroupdisk/det-zdc/ZdcData/calib/$zfile
zdir=run$zrun
zesd=zdclaser.run00$zrun.ESD.pool.root
zaod=zdclaser.run00$zrun.AOD.pool.root

mkdir $zdir
cd $zdir

xrdcp $zin .

echo Running on $zin
echo Running $zmax events

Reco_tf.py --outputESDFile=$zesd --outputAODFile=$zaod --preInclude ZdcRec/ZdcRecConfig.py --postExec 'e2a:from ZdcRec.ZdcRecUtils import AppendOutputList;AppendOutputList(StreamAOD.ItemList);' --conditionsTag CONDBR2-BLKPA-2015-11  --geometryVersion ATLAS-R2-2015-03-01-00 --maxEvents $zmax --inputBSFile $zfile

rm $zesd
mv $zaod ..