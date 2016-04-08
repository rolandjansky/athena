## DPDMakerScript to be included in rec.DPDMakerScripts()

include.block("HSG2DPDUtils/hsg2_daodmaker_2lepton.py")
include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

# shortcuts
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

ptCut_daod_2l=jobproperties.HSG2.diLeptonPtCut()
massCut_daod_2l=jobproperties.HSG2.diLeptonMassCut()

from AthenaCommon.Logging import logging

msg = logging.getLogger( "HSG2DAODFilters" )

algsList=[]

theJob += H4lDPDMaker.H4lElectronFilter("HSG2_DAOD_2ElectronFilter",nElec=2,\
                                        eTCuts=jobproperties.HSG2.diLeptonEPtCuts(),\
                                        qualityCuts=jobproperties.HSG2.diLeptonEQual(),\
                                        mass=massCut_daod_2l,
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                        checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2ElectronFilter.name())
msg.info("Added 2e filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_2StacoMuonFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2StacoMuonFilter.name())
msg.info("Added 2mu (staco) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_2MuidMuonFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MuidMuonFilter.name())
msg.info("Added 2mu (muid) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_2MuonsFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    collectionName="Muons",\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MuonsFilter.name())
msg.info("Added 2mu (unified) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_2CaloMuonFilter",nMu=2,\
                                    collectionName="UniqueCaloMuonCollection",\
                                    etaCuts=[0.2,0.2],\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2CaloMuonFilter.name())
msg.info("Added 2mu (calo) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterStacoCalo", nLept=2,\
                                     pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","StacoMuonCollection"],\
                                     mass=massCut_daod_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MixedFilterStacoCalo.name())
msg.info("Added 2mu (calo/staco) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterMuidCalo", nLept=2,\
                                     pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","MuidMuonCollection"],\
                                     mass=massCut_daod_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MixedFilterMuidCalo.name())
msg.info("Added 2mu (calo/muid) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterMuonsCalo", nLept=2,\
                                     pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","Muons"],\
                                     mass=massCut_daod_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MixedFilterMuonsCalo.name())
msg.info("Added 2mu (calo/unified) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterStaco", nLept=2,\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"StacoMuonCollection"],\
                                     mass=massCut_daod_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())                                                    
algsList.append(theJob.HSG2_DAOD_2MixedFilterStaco.name())
msg.info("Added 1e1mu (staco) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterMuid", nLept=2,\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"MuidMuonCollection"],\
                                     mass=massCut_daod_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MixedFilterMuid.name())
msg.info("Added 1e1mu (muid) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterMuons", nLept=2,\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"Muons"],\
                                     mass=massCut_daod_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MixedFilterMuons.name())
msg.info("Added 1e1mu (unified) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterCalo", nLept=2,\
                                     etaCuts=[9999,0.2],\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"UniqueCaloMuonCollection"],\
                                     mass=massCut_daod_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_2MixedFilterCalo.name())
msg.info("Added 1e1mu (calo) filter")

combination_daod_2l=""
for iAlg in range(len(algsList)):
    if iAlg==0:
        combination_daod_2l=algsList[iAlg]
    else:
        combination_daod_2l+=" or "+algsList[iAlg]

totalDecision_daod_2l=LogicalFilterCombiner("HSG2_2LDAODFilter",cmdstring = combination_daod_2l)
theJob+=totalDecision_daod_2l

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

# define streams

from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D2PDFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Construct the stream and file names for the jp:
streamName = prodFlags.WriteDAOD_2LHSG2Stream.StreamName
fileName   = buildFileName( prodFlags.WriteDAOD_2LHSG2Stream )

#streamOut = MSMgr.NewPoolStream("StreamDAOD_2LHSG2",runArgs.outputDAOD_2LHSG2File)
streamOut_daod_2l = MSMgr.NewPoolStream(streamName,fileName)
streamOut_daod_2l.AcceptAlgs([totalDecision_daod_2l.getName()])
streamOut_daod_2l.AddOtherAlgsToBookkeep(algsList)

dpdOutHelper.addAllItemsFromInputExceptExcludeList(streamName,[])
streamOut_daod_2l.AddMetaDataItem("EventBookkeeperCollection#*")
