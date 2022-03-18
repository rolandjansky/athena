Acts track fitting
========================

This is the implementation of the Acts track fitting within Athena. 

As of now only fit using measurement set and existing track have been implemented.

This use the ActsGeometry/ActsATLASConverterTool to convert those measurment Acts source link that can then be used for the fitting. Sue to the difficulty in converting Atlas TRT identifier into Acts one this conversion cannot be done for TRT measurement.

A track reffiting algorithm is also provided this can be used to refit resolved tracks (since they don't have TRT hits) to compare the acts fit with the ATLAS one. To do so one can use Reco_tf :

```
Reco_tf.py --conditionsTag all:OFLCOND-MC16-SDR-25 \
--ignoreErrors 'False' \
--autoConfiguration='everything' \
--maxEvents '25' \
--AMITag 'q221' \
--digiSeedOffset2 '1' \
--inputHITSFile='YOUR_INPUT_FILE' \
--postInclude 'RAWtoESD:../athena/Tracking/Acts/ActsTrkFitting/share/postInclude_ActsRefitting.py' \
--preExec  'all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.);from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from AthenaConfiguration.AllConfigFlags import ConfigFlags;ConfigFlags.Trigger.AODEDMSet="AODFULL"' \
'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["PhysicsList"];' \
'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False;' \
--postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii; saveToAscii("config_RAWtoESD_q221_job.txt");ServiceMgr.MessageSvc.defaultLimit = 9999999;' \
'RAWtoESD:include("InDetRecExample/InDetxAODCreator.py"); createTrackParticles("ResolvedTracks","ResolvedTracksTruthCollection","ResolvedTracks_TrackParticle",topSequence); createTrackParticles("ReFitted_Tracks","ResolvedTracksTruthCollection","ReFitted_Tracks_TrackParticle",topSequence); StreamESD.ItemList  += ["xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]; StreamESD.ItemList += ["xAOD::TrackParticleContainer#ResolvedTracks_TrackParticle"]; StreamESD.ItemList += ["xAOD::TrackParticleAuxContainer#ResolvedTracks_TrackParticleAux."]; StreamESD.ItemList += ["xAOD::TrackParticleContainer#ReFitted_Tracks_TrackParticle"]; StreamESD.ItemList += ["xAOD::TrackParticleAuxContainer#ReFitted_Tracks_TrackParticleAux."];' \
--digiSeedOffset1 '1' \
--DataRunNumber '284500' \
--outputESDFile 'myESD.q221.pool.root' \
```

To compare the two fits one can then use the ActsCompareTrackAlg for some simple comparison.

For more detailed comparison, use the InDetPhysValMonitoring.