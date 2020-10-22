#This file is to run standard reconstruction + Flow Elements on an ESD file (Primarily the Flow Element configuration)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput=['/atlas/shatlas/ParticleFlow/mc16_13TeV.426327.ParticleGun_single_piminus_logE5to2000.recon.ESD.e5661_s3170_r9857/mc16_13TeV/ESD.11980044._002219.pool.root.1']
#athenaCommonFlags.FilesInput=['/atlas/shatlas/ParticleFlow/ttbar_nonallhad_10788/mc16_13TeV/ESD.15852896._000467.pool.root.1']
athenaCommonFlags.FilesInput=['/atlas/shatlas/ParticleFlow/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.ESD.e6337_e5984_s3170_r11596/mc16_13TeV/ESD.19439305._000011.pool.root.1']
#athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
doDumpProperties=True

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

from eflowRec.eflowRecFlags import jobproperties
jobproperties.eflowRecFlags.useFlowElements.set_Value_and_Lock(True)

from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)

#change some calo flags
#from CaloRec.CaloRecFlags import jobproperties
#jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
#jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
#jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
#jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#Turn of TAG
rec.doWriteTAG.set_Value_and_Lock(False)

include("LArConditionsCommon/LArIdMap_MC_jobOptions.py")
from CaloRec import CaloClusterTopoCoolFolder
from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg()
CaloNoiseCondAlg(noisetype="electronicNoise")
athenaCommonFlags.EvtMax=100
#Run pflopw jet finding - this cannot be enabled via reconstruction flags currently! (without enabling other things we don't want)
UserAlgs = ["eflowRec/jetAlgs.py"]
include ("RecExCommon/RecExCommon_topOptions.py")

ServiceMgr.MessageSvc.defaultLimit = 9999999
include ("METReconstruction/METReconstruction_jobOptions.py")
