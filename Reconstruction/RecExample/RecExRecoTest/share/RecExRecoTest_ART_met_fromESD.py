from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]

from RecExConfig.RecFlags import rec
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(True)

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)

#this turns off CaloCluster2xAOD
rec.doWritexAOD.set_Value_and_Lock(False)
#nothing to say on these
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doAODCaloCells.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
#Turns off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)

#disables VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disables AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=10

UserAlgs=["RecExRecoTest/RecExRecoTests_setupReadID.py"]
UserAlgs+=["RecExRecoTest/RecExRecoTest_setupJetEtMissPFlow.py"]
UserAlgs+=["METReconstruction/METReconstruction_jobOptions.py"]
include ("RecExCommon/RecExCommon_topOptions.py")

import AthenaCommon.PropertiesManip as manip

manip.appendItemsToList(StreamAOD,'ItemList','xAOD::PFOContainer_v1#CHSParticleFlowObjects')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::PFOAuxContainer_v1#CHSParticleFlowObjectsAux.')


