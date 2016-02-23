# example joboption for running eflowRec in jet mode
# this should run in rel:  17.2.8.9 with some eflowRec tag added
# this needs eflowJetMode_example_myAlgs.py in your run dir to run

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/user/j/jboyd/gencomm/DESDM/mc12_8TeV.147911.Pythia8_AU2CT10_jetjet_JZ1W.recon.DESDM_ALLCELLS.e1126_s1479_s1470_r3578.root"]
#/afs/cern.ch/user/j/jboyd/gencomm/DESDM/mc12_8TeV.147911.Pythia8_AU2CT10_jetjet_JZ1W.recon.DESDM_ALLCELLS.e1126_s1479_s1470_r4534.root"]
#/afs/cern.ch/user/j/jboyd/gencomm/DESDM/mc12_8TeV.147911.Pythia8_AU2CT10_jetjet_JZ1W.recon.DESDM_ALLCELLS.e1126_s1479_s1470_r4535.root"]

doTrigger=False
doTAG=False

#Based on r3553

beamType='collisions' 
fastPoolMerge='False' 

from LArROD.LArRODFlags import larRODFlags
larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True)
larRODFlags.NumberOfCollisions.set_Value_and_Lock(20)
larRODFlags.UseDelta.set_Value_and_Lock(3)

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArCellEmMisCalib.set_Value_and_Lock(False)

autoConfiguration='everything' 
ignoreerrors='True'

from RecExConfig.RecFlags import rec,Rec
rec.doApplyAODFix.set_Value_and_Lock(False)
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthWriteHadrons=True
rec.doCBNT.set_Value_and_Lock(False)
#rec.doWriteESD.set_Value_and_Lock(False) 
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
##rec.doJetMissingETTag.set_Value_and_Lock(True)
rec.doESD.set_Value_and_Lock(False)
rec.doTrigger=False
rec.doTruth.set_Value_and_Lock( True )

#Benedict uses this....but UserAlgs are ignored if we set this, so cannot do that
#rec.Commissioning.set_Value_and_Lock(True)
rec.AutoConfiguration = [ "everything" ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Beam.energy.set_Value_and_Lock(4000*Units.GeV)
jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0)

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.AODEDMSet="AODSLIM"

from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.writeSDOs=True

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

#change some more calo flags

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(5.44) #2012? -> no its 5.44
#jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45) #2011
#jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0.) #2010

from eflowRec.eflowRecFlags import jobproperties
jobproperties.eflowRecFlags.runTauMode.set_Value_and_Lock(False)
jobproperties.eflowRecFlags.useSplitShowers.set_Value_and_Lock(True)


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax=10
UserAlgs = ["eflowJetMode_example_myAlgs.py"]
include("RecExCommon/RecExCommon_topOptions.py")

import AthenaCommon.PropertiesManip as manip

