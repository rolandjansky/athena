from AthenaCommon.AthenaCommonFlags import athenaCommonFlags 
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.recon.ESD.e3569_s3170_r12310_r12253_r12310/ESD.23850840._000295.pool.root.1"]

#Disable reco of trigger, egamma, muons and taus
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)

from eflowRec.eflowRecFlags import jobproperties as jobprops
jobprops.eflowRecFlags.useFlowElements.set_Value_and_Lock(False)

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
UserAlgs = ["PFlowUtils/PFlowCalibHitDecorator_algorithms.py"]
include ("RecExCommon/RecExCommon_topOptions.py")
#Add decoration to output item list
StreamAOD.ItemList+=['xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux.+calpfo_NLeadingTruthParticleBarcodeEnergyPairs']
StreamAOD.ItemList+=['xAOD::FlowElementAuxContainer#JetETMissNeutralFlowElementssAux.+calfe_NLeadingTruthParticleBarcodeEnergyPairs']
StreamAOD.ItemList+=['xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList.calclus_NLeadingTruthParticleBarcodeEnergyPairs']
