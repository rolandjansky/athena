#!/bin/sh

# fun start here : need different configuration for 19.1
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
echo "Will run Qtest on run2 MC on release $release build from nightly $nn $pn $nr"

REL191_preExec=''
if  [[ $release == 19.1.* ]] ;
then
    echo "Need special egamma options for releases in the 19.1 series"
    REL191_preExec='"RAWtoESD:ToolSvc.egammaConversionFinderTool.VertexFitterTool=ToolSvc.InDetAdaptiveVxFitterTool;ToolSvc.egammaAmbiguityTool.minNoPixHits=3"'
fi

# Run 2 (RDO)
Reco_tf.py --AMITag r5787 --DBRelease="current" --autoConfiguration="everything" --conditionsTag="OFLCOND-RUN12-SDR-22" --geometryVersion="ATLAS-R2-2015-01-01-00" --maxEvents 25 --postExec "CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"]" "HITtoRDO:job.StandardPileUpToolsAlg.PileUpTools[\"MergeMcEventCollTool\"].OnlySaveSignalTruth=True" $REL191_preeExec --postInclude="RecJobTransforms/UseFrontier.py" --preExec "rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25);from LArROD.LArRODFlags import larRODFlags;larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4)" "RAWtoESD:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup=\"MC_pp_v5\";from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False" "ESDtoAOD:TriggerFlags.AODEDMSet=\"AODFULL\"" --preInclude HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsUpgradeConfig1_25ns.py,RunDependentSimData/configLumi_run222222.py --inputRDOFile /afs/cern.ch/atlas/project/rig/referencefiles/QTests-Run2/RDO-run2.Nov5.2014-500events.pool.root --outputAODFile="AOD.pool.root" --outputESDFile="ESD.pool.root"
