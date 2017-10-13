from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/data/hodgkinson/scratchFiles/mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s2832_r7968/ESD.08355655._001904.pool.root.1"] 


doDumpProperties=True

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

from RecExConfig.RecFlags import rec
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)

#change some calo flags
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

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

athenaCommonFlags.EvtMax=1
include ("RecExCommon/RecExCommon_topOptions.py")
topSequence.CaloTopoCluster.ClustersOutputName="CaloCalTopoClusterV2"
#topSequence.eflowEMCaloObjectBuilder.CalClustersName="CaloCalTopoClusterV2"
topSequence.PFClusterSelector.calClustersName="CaloCalTopoClusterV2"
topSequence.PFOChargedCreatorAlgorithm.PFOOutputName="JetETMissChargedParticleFlowObjectsV2"
#topSequence.PFOChargedCreatorAlgorithm.OutputLevel=DEBUG
topSequence.PFONeutralCreatorAlgorithm.PFOOutputName="JetETMissNeutralParticleFlowObjectsV2"
#topSequence.PFONeutralCreatorAlgorithm.OutputLevel=DEBUG
#topSequence.PFTrackSelector.OutputLevel=DEBUG
#topSequence.PFLeptonSelector.OutputLevel=DEBUG
#topSequence.PFAlgorithm.OutputLevel=DEBUG
#ServiceMgr.MessageSvc.debugLimit = 1000000
import AthenaCommon.PropertiesManip as manip
manip.removeItemsFromList(StreamAOD,'ItemList','xAOD::PFOContainer#JetETMissChargedParticleFlowObjects')
manip.removeItemsFromList(StreamAOD,'ItemList','xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsAux.')
manip.removeItemsFromList(StreamAOD,'ItemList','xAOD::PFOContainer#JetETMissNeutralParticleFlowObjects')
manip.removeItemsFromList(StreamAOD,'ItemList','xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux.')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::PFOContainer#JetETMissChargedParticleFlowObjectsV2')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsV2Aux.')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::PFOContainer#JetETMissNeutralParticleFlowObjectsV2')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsV2Aux.')

