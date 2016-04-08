
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


theJob += H4lDPDMaker.H4lElectronFilter("HSG2_DAOD_2ElectronFilter",nElec=2,\
                                        eTCuts=jobproperties.HSG2.diLeptonEPtCuts(),\
                                        qualityCuts=jobproperties.HSG2.diLeptonEQual(),\
                                        mass=massCut_daod_2l,
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

msg.info("Added 2e filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_2StacoMuonFilter",nMu=2,\
                                    
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l)
msg.info("Added 2mu (staco) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_2MuidMuonFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l)

msg.info("Added 2mu (muid) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_2CaloMuonFilter",nMu=2,\
                                    collectionName="UniqueCaloMuonCollection",\
                                    etaCuts=[0.2,0.2],\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l)

msg.info("Added 2mu (calo) filter")


# now adding 1mu1mucalo

mixedFilterStacoCalo_daod_2l=H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterStacoCalo", nLept=2,\
                                                    pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    #electron quality checked before
                                                    qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                                    types=["mu","mu"],\
                                                    etaCuts=[0.2,9999],\
                                                    collections=["UniqueCaloMuonCollection","StacoMuonCollection"],\
                                                    mass=massCut_daod_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

# now adding 1mu1mucalo

mixedFilterMuidCalo_daod_2l=H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterMuidCalo", nLept=2,\
                                                    pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    #electron quality checked before
                                                    qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                                    types=["mu","mu"],\
                                                    etaCuts=[0.2,9999],\
                                                    collections=["UniqueCaloMuonCollection","MuidMuonCollection"],\
                                                    mass=massCut_daod_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

mixedFilterStaco_daod_2l=H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterStaco", nLept=2,\
                                                    pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                                    #electron quality checked before
                                                    qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                                    types=["e","mu"],\
                                                    collections=[eCollToUse,"StacoMuonCollection"],\
                                                    mass=massCut_daod_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

mixedFilterMuid_daod_2l=H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterMuid", nLept=2,\
                                                   pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                   eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                                   qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                                   
                                                   types=["e","mu"],\
                                                   collections=[eCollToUse,"MuidMuonCollection"],\
                                                   mass=massCut_daod_2l,\
                                                   acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

mixedFilterCalo_daod_2l=H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_2MixedFilterCalo", nLept=2,\
                                                    etaCuts=[9999,0.2],\
                                                    pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                                    #electron quality checked before
                                                    qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                                    types=["e","mu"],\
                                                    collections=[eCollToUse,"UniqueCaloMuonCollection"],\
                                                    mass=massCut_daod_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

theJob+=mixedFilterStaco_daod_2l

msg.info("Added 1e1mu(staco) filter")

theJob+=mixedFilterMuid_daod_2l

msg.info("Added 1e1mu(muid) filter")

theJob+=mixedFilterCalo_daod_2l

msg.info("Added 1e1mu(calo) filter")

theJob+=mixedFilterStacoCalo_daod_2l
msg.info("Added 1mu(staco)1mu(calo) filter")

theJob+=mixedFilterMuidCalo_daod_2l
msg.info("Added 1mu(muid)1mu(calo) filter")

# DEBUG 1mu staco and 1mu calo filters

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_1CaloMuonFilter",nMu=1,\
                                    collectionName="UniqueCaloMuonCollection",\
                                    etaCuts=[0.2],\
                                    pTCuts=[jobproperties.HSG2.diLeptonMuPtCuts()[0]],\
                                    qualityCuts=[jobproperties.HSG2.diLeptonMuQual()[0]],\
                                    mass=0)


theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_1StacoMuonFilter",nMu=1,\
                                    collectionName="StacoMuonCollection",\
                                    pTCuts=[jobproperties.HSG2.diLeptonMuPtCuts()[0]],\
                                    qualityCuts=[jobproperties.HSG2.diLeptonMuQual()[0]],\
                                    mass=0)



combination_daod_2l="HSG2_DAOD_2ElectronFilter or "+"HSG2_DAOD_2StacoMuonFilter or "+"HSG2_DAOD_2MuidMuonFilter or "+"HSG2_DAOD_2MixedFilterStaco or "+"HSG2_DAOD_2MixedFilterMuid or "+"HSG2_DAOD_2MixedFilterCalo or "+"HSG2_DAOD_2MixedFilterMuidCalo or "+"HSG2_DAOD_2MixedFilterStacoCalo"

totalDecision_daod_2l=LogicalFilterCombiner("HSG2_2LDAODFilter",cmdstring = combination_daod_2l)
theJob+=totalDecision_daod_2l

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

# define streams

#streamOut = MSMgr.NewPoolStream("StreamDAOD_2LHSG2",runArgs.outputDAOD_2LHSG2File)
streamOut_daod_2l = MSMgr.NewPoolStream("StreamDAOD_2LHSG2",runArgs.outputDAOD_2LHSG2File)
streamOut_daod_2l.AcceptAlgs([totalDecision_daod_2l.getName()])
streamOut_daod_2l.AddOtherAlgsToBookkeep(["HSG2_DAOD_2ElectronFilter","HSG2_DAOD_2StacoMuonFilter","HSG2_DAOD_2MuidMuonFilter","HSG2_DAOD_2MixedFilterStaco","HSG2_DAOD_2MixedFilterMuid","HSG2_DAOD_2MixedFilterCalo"])

dpdOutHelper.addAllItemsFromInputExceptExcludeList("StreamDAOD_2LHSG2",[])

streamOut_daod_2l.AddMetaDataItem("EventBookkeeperCollection#*")

