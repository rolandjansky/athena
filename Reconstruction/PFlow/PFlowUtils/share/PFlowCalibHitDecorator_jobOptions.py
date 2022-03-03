from AthenaCommon.AthenaCommonFlags import athenaCommonFlags 
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.ESD.e6337_e5984_s3170_r12674/ESD.25732025._000034.pool.root.1"]

#Disable reco of trigger, egamma, muons and taus
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)

#Do not write out TAG file
rec.doWriteTAG.set_Value_and_Lock(False)
#Disable truth reco
rec.doTruth.set_Value_and_Lock(False)
#Don't build AOD CaloCell container
rec.doAODCaloCells.set_Value_and_Lock(False)
#Turn off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)

#Use a current conditions tag to avoid crash in pixel conditions algorithms
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC16-SDR-RUN2-08")

#Disable VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disable AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=100
rec.UserAlgs = ["PFlowUtils/PFlowCalibHitDecorator_algorithms.py"]
include ("RecExCommon/RecExCommon_topOptions.py")
#Add decoration to output item list
StreamAOD.ItemList+=['xAOD::FlowElementAuxContainer#JetETMissNeutralParticleFlowObjectsAux.+calfe_NLeadingTruthParticleBarcodeEnergyPairs']
StreamAOD.ItemList+=['xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList.calclus_NLeadingTruthParticleBarcodeEnergyPairs']
#Update the name of the FlowElement container to use (not needed in newest ESD because the string used in the c++ matches what is stored in those - this test uses an older ESD which contains old container names)
topSequence.PFlowCalibPFODecoratorAlgorithm.PFOWriteDecorHandleKey_NLeadingTruthParticles="JetETMissNeutralFlowElements.calpfo_NLeadingTruthParticleBarcodeEnergyPairs"
