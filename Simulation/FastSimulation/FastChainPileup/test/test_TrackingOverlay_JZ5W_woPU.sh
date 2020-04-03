#!/bin/sh
#
# art-description: Run the full reconstruction on HITS without pile-up JZ5W samples (leading jet pt: 800-1300 GeV) for validation of tracking overlay
# art-include: 21.0/Athena
# art-include: master/Athena
# art-type: grid

export inputHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrackingOverlay/JZ5W_HITS
export outputRDOFile=RDO.JZ5W_woPU.root
export outputESDFile=ESD.JZ5W_woPU.root
export outputAODFile=AOD.JZ5W_woPU.root
export outputNTUP_PHYSVALFile=NTUP_PHYSVAL.JZ5W_woPU.root

# Copied from r11627 (asetup Athena,21.0.97)
# Running also with asetup 21.0,Athena,2020-02-28T2145
Reco_tf.py --inputHITSFile $inputHitsFile/*root.? \
           --triggerConfig "RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2232,86,278" \
           --steering "doRDO_TRIG" \
           --autoConfiguration everything \
           --conditionsTag "default:OFLCOND-MC16-SDR-25" \
           --geometryVersion "default:ATLAS-R2-2016-01-00-01" \
           --postExec "all:CfgMgr.MessageSvc().setError+=['HepMcParticleLink']" \
           --postInclude "default:PyJobTransforms/UseFrontier.py" \
           --preExec "all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)" 'HITtoRDO:userRunLumiOverride={"run":310000,"lb":61,"starttstamp":1550003600,"mu":60.500};' "from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True);" \
           --valid=True \
           --validationFlags doEgamma,doInDet,doZee,doJet \
           --outputRDOFile $outputRDOFile \
           --outputESDFile $outputESDFile \
           --outputAODFile $outputAODFile \
           --outputNTUP_PHYSVALFile $outputNTUP_PHYSVALFile \
           --jobNumber=2 \
           --maxEvents=2000
