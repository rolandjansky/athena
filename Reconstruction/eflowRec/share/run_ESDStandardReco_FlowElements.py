#This file is to run standard reconstruction + Flow Elements on an ESD file (Primarily the Flow Element configuration)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
doDumpProperties=True

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

from eflowRec.eflowRecFlags import jobproperties
jobproperties.eflowRecFlags.useCalibHitTruth.set_Value_and_Lock(True)
#include FlowElements in run
jobproperties.eflowRecFlags.useFlowElements.set_Value_and_Lock(True)
#include FlowElement validation plots
jobproperties.eflowRecFlags.doFlowElementValidation.set_Value_and_Lock(True)
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)

#change some calo flags
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#nothing to say on these
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doAODCaloCells.set_Value_and_Lock(False)
#Turns off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)

#Turn of TAG
rec.doWriteTAG.set_Value_and_Lock(False)

#disables VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disables AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)

include("LArConditionsCommon/LArIdMap_MC_jobOptions.py")
from CaloRec import CaloClusterTopoCoolFolder
from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg()
CaloNoiseCondAlg(noisetype="electronicNoise")

athenaCommonFlags.EvtMax=30
#Run pflopw jet finding - this cannot be enabled via reconstruction flags currently! (without enabling other things we don't want)
UserAlgs = ["eflowRec/jetAlgs.py"]
include ("RecExCommon/RecExCommon_topOptions.py")

ServiceMgr.MessageSvc.defaultLimit = 9999999
include ("METReconstruction/METReconstruction_jobOptions.py")


