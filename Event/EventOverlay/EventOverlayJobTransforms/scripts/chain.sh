
rm -f simpledummy.RAW tmp.*
trigb="-1" #240

#input="root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/mc15_valid/ad/84/DRAW.07898478._000025.pool.root.1"
#input="/afs/cern.ch/user/a/ahaas/temp/mc15_valid/DRAW.07898490._001594.pool.root.1" #eventnumber>2^32
input="/afs/cern.ch/user/a/ahaas/temp/data16_13TeV/data16_13TeV.00297170.physics_ZeroBias.merge.RAW._lb0600._SFO-ALL._0001.1"
#input="/afs/cern.ch/user/a/ahaas/temp/data16_13TeV/data16_13TeV.00297170.physics_TauOverlay.merge.RAW._lb0251._SFO-ALL._0001.1"

inputEVNT="/afs/cern.ch/user/a/ahaas/public/overlay/mu100.EVNT.pool.root"
#inputEVNT="root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/overlay/mc15_13TeV.147407.PowhegPythia8_AZNLO_Zmumu.evgen.EVNT.e4032/EVNT.05685490._000002.pool.root.1"
#inputEVNT="pion.EVNT.pool.root"

cond="CONDBR2-BLKPA-2016-12"
#cond="CONDBR2-BLKPA-2015-12"

dooverlay=0
if [ $dooverlay == 1 ]; then
OverlayChain_tf.py --ignorePatterns "L1TopoMenuLoader.+ERROR." --inputZeroBiasBSFile $input --DataRunNumber 2015 \
--outputBS_SKIMFile 'simpledummy.RAW' \
--outputHITSFile testRTT.HITS.pool.root \
--inputEVNTFile $inputEVNT \
--outputRDOFile testRTT.RDO.pool.root \
--triggerBit $trigb --skipEvents 0 --maxEvents 10 --randomSeed 123456789 \
--geometryVersion ATLAS-R2-2015-03-01-00 --conditionsTag $cond --digiSeedOffset1=211 --digiSeedOffset2=122 \
--samplingFractionDbTag FTFP_BERT_BIRK --fSampltag LARElecCalibMCfSampl-G496-19213- \
--preInclude 'sim:EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/magfield.py' 'overlayBS:EventOverlayJobTransforms/custom.py' \
--preExec "from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock('4samples1phase');" \
--postInclude 'sim:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py,EventOverlayJobTransforms/muAlign.py,EventOverlayJobTransforms/g4runnumber.py' 'overlayBS:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py' \
--triggerConfig "OverlayBS=DEFAULT" \
#--postExec "OverlayBS:job.MdtOverlay.CleanOverlaySignal=True;"
fi

doreco=1
if [ $doreco == 1 ]; then

#dojive="from RecExConfig.RecFlags import rec; rec.doJiveXML=True;"
#domon="rec.doMonitoring=True;"
#rdotrig="--outputRDO_TRIGFile testRTT.RDO_TRIG.pool.root --triggerConfig MCRECO:DBF:TRIGGERDBMC:2046,20,48 "
aodout="--outputAODFile testRTT.AOD.pool.root"
trigflag="from TriggerJobOpts.TriggerFlags import TriggerFlags as TF; TF.configForStartup.set_Value_and_Lock('HLToffline');"
#dotrig="rec.doTrigger=True;"

Reco_tf.py \
--inputRDOFile testRTT.RDO.pool.root --outputESDFile testRTT.ESD.pool.root $aodout $rdotrig \
--preInclude 'EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/recotrfpre.py' \
--postInclude 'r2e:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py,EventOverlayJobTransforms/muAlign_reco.py' \
--preExec "from LArConditionsCommon.LArCondFlags import larCondFlags;larCondFlags.OFCShapeFolder.set_Value_and_Lock('4samples1phase'); $dotrig $trigflag $domon $dojive "

#svcMgr.MdtCalibrationSvc.OutputLevel=VERBOSE; svcMgr.MessageSvc.verboseLimit=100000; '
fi

#Reco_tf.py --inputESDFile testRTT.ESD.pool.root --outputAODFile testRTT.AOD.pool.root
