
## DPDMakerScript to be included in rec.DPDMakerScripts()

include.block("HSG2DPDUtils/hsg2_d3pdmaker_4lepton.py")
include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from AthenaCommon.Logging import logging
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

msg = logging.getLogger( "HSG2D3PDFilters" )

# add 4-leptons event skims

ptCut_d3pd_4l=jobproperties.HSG2.fourLeptonPtCut()
massCut_d3pd_4l=jobproperties.HSG2.fourLeptonMassCut()

theJob += H4lDPDMaker.H4lElectronFilter("HSG2_D3PD_4ElectronFilter",nElec=4,\
                                        eTCuts=[ptCut_d3pd_4l,ptCut_d3pd_4l,ptCut_d3pd_4l,ptCut_d3pd_4l],\
                                        # electron quality checked before
                                        qualityCuts=[eQual,eQual,eQual,eQual],\
                                        mass=massCut_d3pd_4l,
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE())

msg.info("Added 4e filter")
    
theJob += H4lDPDMaker.H4lMuonFilter("HSG2_D3PD_4StacoMuonFilter",nMu=4,\
                                    pTCuts=[ptCut_d3pd_4l,ptCut_d3pd_4l,ptCut_d3pd_4l,ptCut_d3pd_4l],\
                                    qualityCuts=[muQual,muQual,muQual,muQual],\
                                    mass=massCut_d3pd_4l)
msg.info("Added 4mu (staco) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_D3PD_4MuidMuonFilter",nMu=4,\
                                    pTCuts=[ptCut_d3pd_4l,ptCut_d3pd_4l,ptCut_d3pd_4l,ptCut_d3pd_4l],\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=[muQual,muQual,muQual,muQual],\
                                    mass=massCut_d3pd_4l)

msg.info("Added 4mu (muid) filter")

mixedFilterStaco_d3pd_4l=H4lDPDMaker.H4lMixedFilter("HSG2_D3PD_4MixedFilterStaco", nLept=4,\
                                                    pTCuts=[0,0,ptCut_d3pd_4l,ptCut_d3pd_4l],\
                                                    eTCuts=[ptCut_d3pd_4l,ptCut_d3pd_4l,0,0],\
                                                    #electron quality checked before
                                                    qualityCuts=[eQual,eQual,muQual,muQual],\
                                                    types=["e","e","mu","mu"],\
                                                    collections=[eCollToUse,eCollToUse,"StacoMuonCollection","StacoMuonCollection"],\
                                                    mass=massCut_d3pd_4l,\
                                                    acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE())

mixedFilterMuid_d3pd_4l=H4lDPDMaker.H4lMixedFilter("HSG2_D3PD_4MixedFilterMuid", nLept=4,\
                                                   pTCuts=[0,0,ptCut_d3pd_4l,ptCut_d3pd_4l],\
                                                   eTCuts=[ptCut_d3pd_4l,ptCut_d3pd_4l,0,0],\
                                                   qualityCuts=[eQual,eQual,muQual,muQual],\
                                                   types=["e","e","mu","mu"],\
                                                   collections=[eCollToUse,eCollToUse,"MuidMuonCollection","MuidMuonCollection"],\
                                                   mass=massCut_d3pd_4l,\
                                                   acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE())

theJob+=mixedFilterStaco_d3pd_4l

msg.info("Added 2e2mu(staco) filter")

theJob+=mixedFilterMuid_d3pd_4l

msg.info("Added 2e2mu(muid) filter")

combination_d3pd_4l="HSG2_D3PD_4ElectronFilter or "+"HSG2_D3PD_4StacoMuonFilter or "+"HSG2_D3PD_4MuidMuonFilter or "+"HSG2_D3PD_4MixedFilterStaco or "+"HSG2_D3PD_4MixedFilterMuid"

totalDecision_d3pd_4l=LogicalFilterCombiner("HSG2_D3PD_4l_Filter",cmdstring = combination_d3pd_4l)
theJob+=totalDecision_d3pd_4l


from HSG2DPDUtils.HSG2D3PD import HSG2physicsD3PD


stream_d3pd_4l=HSG2physicsD3PD("StreamNTUP_HSG2",runArgs.outputNTUP_HSG2File)
stream_d3pd_4l.AcceptAlgs = [totalDecision_d3pd_4l.getName()]
