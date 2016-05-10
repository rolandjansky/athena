#!/bin/sh

# fun starts here : need different configuration for 19.X
#RELEASEDATA=`cmt show projects | grep -v '(current)' | head -1 | sed "s/.\+(in \(.\+\)).\+/\1/g"`
#RELEASEDATA="$RELEASEDATA/InstallArea/$CMTCONFIG/ReleaseData"
IFS=:
set -f
helper=( $CMAKE_PREFIX_PATH )
RELEASEDATA="${helper[0]}/ReleaseData"


release=`grep '^release:' $RELEASEDATA | gawk -F \: '{print $2}'`

nn=`grep '^nightly name:' $RELEASEDATA | gawk -F \: '{print $2}'`
pn=`grep '^project name:' $RELEASEDATA | gawk -F \: '{print $2}'`
nr=`grep '^nightly release:' $RELEASEDATA | gawk -F \: '{print $2}'`
echo "Will run Qtest on run1 MC on release $release build from nightly $nn $pn $nr"

REL19_preExec=''
if  [[ $release == 19.* ]] ;
then
    echo "Need special MaterialVersion V17 for releases in the 19.X series"
    REL19_preExec=';from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.MaterialVersion=17'

fi

# Run 1 (RDO)
Reco_tf.py --DBRelease='current' --autoConfiguration='everything' --conditionsTag='OFLCOND-RUN12-SDR-22' --geometryVersion='ATLAS-R1-2012-02-00-00' --maxEvents 25 --postExec 'CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'HITtoRDO:job.StandardPileUpToolsAlg.PileUpTools["MergeMcEventCollTool"].OnlySaveSignalTruth=True' --postInclude='RecJobTransforms/UseFrontier.py' --preExec 'rec.doTrigger.set_Value_and_Lock(False);rec.Commissioning.set_Value_and_Lock(True);jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0)' 'HITtoRDO:from LArROD.LArRODFlags import larRODFlags;larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.UseDelta.set_Value_and_Lock(3)' "RAWtoESD:from LArROD.LArRODFlags import larRODFlags;larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.UseDelta.set_Value_and_Lock(3);from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False$REL19_preExec" "ESDtoAOD:TriggerFlags.AODEDMSet='AODFULL'$REL19_preExec" --skipEvents='0' --inputRDOFile /afs/cern.ch/atlas/project/rig/referencefiles/QTests-Run2/RDO-run1.Nov5.2014-500events.pool.root --outputAODFile='AOD.pool.root' --outputESDFile='ESD.pool.root'
