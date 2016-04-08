
## DPDMakerScript to be included in rec.DPDMakerScripts()

include.block("HSG2DPDUtils/hsg2_daodmaker_4lepton.py")
include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

# shortcuts
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

ptCut_daod_4l=jobproperties.HSG2.fourLeptonPtCut()
massCut_daod_4l=jobproperties.HSG2.fourLeptonMassCut()

from AthenaCommon.Logging import logging

msg = logging.getLogger( "HSG2DODFilters" )


theJob += H4lDPDMaker.H4lElectronFilter("HSG2_DAOD_4ElectronFilter",nElec=4,\
                                        eTCuts=[ptCut_daod_4l,ptCut_daod_4l,ptCut_daod_4l,ptCut_daod_4l],\
                                        # electron quality checked before
                                        qualityCuts=[eQual,eQual,eQual,eQual],\
                                        mass=massCut_daod_4l,\
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE())

msg.info("Added 4e filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_4StacoMuonFilter",nMu=4,\
                                    pTCuts=[ptCut_daod_4l,ptCut_daod_4l,ptCut_daod_4l,ptCut_daod_4l],\
                                    qualityCuts=[muQual,muQual,muQual,muQual],\
                                    mass=massCut_daod_4l)
msg.info("Added 4mu (staco) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_4MuidMuonFilter",nMu=4,\
                                    pTCuts=[ptCut_daod_4l,ptCut_daod_4l,ptCut_daod_4l,ptCut_daod_4l],\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=[muQual,muQual,muQual,muQual],\
                                    mass=massCut_daod_4l)

msg.info("Added 4mu (muid) filter")

mixedFilterStaco_daod_4l=H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterStaco", nLept=4,\
                                                    pTCuts=[0,0,ptCut_daod_4l,ptCut_daod_4l],\
                                                    eTCuts=[ptCut_daod_4l,ptCut_daod_4l,0,0],\
                                                    #electron quality checked before
                                                    qualityCuts=[eQual,eQual,muQual,muQual],\
                                                    types=["e","e","mu","mu"],\
                                                    collections=[eCollToUse,eCollToUse,"StacoMuonCollection","StacoMuonCollection"],\
                                                    mass=massCut_daod_4l,\
                                                    acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE())

mixedFilterMuid_daod_4l=H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterMuid", nLept=4,\
                                                   pTCuts=[0,0,ptCut_daod_4l,ptCut_daod_4l],\
                                                   eTCuts=[ptCut_daod_4l,ptCut_daod_4l,0,0],\
                                                   qualityCuts=[eQual,eQual,muQual,muQual],\
                                                   types=["e","e","mu","mu"],\
                                                   collections=[eCollToUse,eCollToUse,"MuidMuonCollection","MuidMuonCollection"],\
                                                   mass=massCut_daod_4l,\
                                                   acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE())

theJob+=mixedFilterStaco_daod_4l

msg.info("Added 2e2mu(staco) filter")

theJob+=mixedFilterMuid_daod_4l

msg.info("Added 2e2mu(muid) filter")

combination="HSG2_DAOD_4ElectronFilter or "+"HSG2_DAOD_4StacoMuonFilter or "+"HSG2_DAOD_4MuidMuonFilter or "+"HSG2_DAOD_4MixedFilterStaco or "+"HSG2_DAOD_4MixedFilterMuid"

totalDecision_daod_4l=LogicalFilterCombiner("HSG2_DAOD_4l_Filter",cmdstring = combination)
theJob+=totalDecision_daod_4l

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

# define streams

#streamOut = MSMgr.NewPoolStream("StreamDAOD_2LHSG2",runArgs.outputDAOD_2LHSG2File)
streamOut_daod_4l = MSMgr.NewPoolStream("StreamDAOD_HSG2",runArgs.outputDAOD_HSG2File)
streamOut_daod_4l.AcceptAlgs([totalDecision_daod_4l.getName()])
streamOut_daod_4l.AddOtherAlgsToBookkeep(["HSG2_DAOD_4ElectronFilter","HSG2_DAOD_4StacoMuonFilter","HSG2_DAOD_4MuidMuonFilter","HSG2_DAOD_4MixedFilterStaco","HSG2_DAOD_4MixedFilterMuid"])

dpdOutHelper.addAllItemsFromInputExceptExcludeList("StreamDAOD_HSG2",[])

streamOut_daod_4l.AddMetaDataItem("EventBookkeeperCollection#*")
