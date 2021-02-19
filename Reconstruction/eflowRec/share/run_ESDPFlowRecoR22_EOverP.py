#This file is to run topocluster + pflow reconstruction on an ESD file produced in the release 22 mc16 campaign

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.recon.ESD.e3569_s3170_r12310_r12253_r12310/ESD.23850840._000295.pool.root.1"]

doDumpProperties=True

#Turn on calotopocluster algorithms, such that we rebuild CaloTopoCluster
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#Turn pflow on
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

#Disable reco of trigger, egamma, muons and taus
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)

#Disable linking of PFO/FE to and from other objects - this won't work because e.g electrons already have links to PFO/FE and
#we cannot overwrite those.
from eflowRec.eflowRecFlags import jobproperties
jobproperties.eflowRecFlags.usePFEGammaPFOAssoc.set_Value_and_Lock(False)
jobproperties.eflowRecFlags.usePFFlowElementAssoc.set_Value_and_Lock(False)

#Enable e/p mode
jobproperties.eflowRecFlags.eflowAlgType.set_Value_and_Lock("EOverP")

#Disable thinning, which would mess up links for e.g taus
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)
AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False)

#Do not write out TAG file
rec.doWriteTAG.set_Value_and_Lock(False)
#Disable truth reco
rec.doTruth.set_Value_and_Lock(False)
#Don't build AOD CaloCell container
rec.doAODCaloCells.set_Value_and_Lock(False)
#Turn off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)

#Disable VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disable AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=100
include ("RecExCommon/RecExCommon_topOptions.py")
#Add in some extra containers that are used for the study of e/p and that are 
#not in the default AOD
import AthenaCommon.PropertiesManip as manip
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::CaloClusterContainer#PFCaloCluster')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::CaloClusterAuxContainer#PFCaloClusterAux.') 
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::TrackParticleContainer#InDetTrackParticles')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.')