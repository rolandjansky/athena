
## DPDMakerScript to be included in rec.DPDMakerScripts()

include.block("HSG2DPDUtils/hsg2_d3pdmaker.py")
include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from AthenaCommon.Logging import logging
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

msg = logging.getLogger( "HSG2D3PDFilters" )

ptCut_d3pd_2l=jobproperties.HSG2.diLeptonPtCut()
massCut_d3pd_2l=jobproperties.HSG2.diLeptonMassCut()

# add di-leptons event skims


theJob += H4lDPDMaker.H4lElectronFilter("HSG2_D3PD_2ElectronFilter",nElec=2,\
                                        eTCuts=jobproperties.HSG2.diLeptonEPtCuts(),\
                                        qualityCuts=jobproperties.HSG2.diLeptonEQual(),\
                                        mass=massCut_d3pd_2l,\
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

msg.info("Added 2e filter")
    
theJob += H4lDPDMaker.H4lMuonFilter("HSG2_D3PD_2StacoMuonFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_d3pd_2l)
msg.info("Added 2mu (staco) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_D3PD_2MuidMuonFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    collectionName="MuidMuonCollection",\
                                    mass=massCut_d3pd_2l)

msg.info("Added 2mu (muid) filter")


theJob += H4lDPDMaker.H4lMuonFilter("HSG2_D3PD_2CaloMuonFilter",nMu=2,\
                                    collectionName="UniqueCaloMuonCollection",\
                                    etaCuts=[0.2,0.2],\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_daod_2l)

msg.info("Added 2mu (calo) filter")



# now adding 1mu1mucalo

mixedFilterStacoCalo_d3pd_2l=H4lDPDMaker.H4lMixedFilter("HSG2_D3PD_2MixedFilterStacoCalo", nLept=2,\
                                                    pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    #electron quality checked before
                                                    qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                                    types=["mu","mu"],\
                                                    etaCuts=[0.2,9999],\
                                                    collections=["UniqueCaloMuonCollection","StacoMuonCollection"],\
                                                    mass=massCut_daod_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

# now adding 1mu1mucalo

mixedFilterMuidCalo_d3pd_2l=H4lDPDMaker.H4lMixedFilter("HSG2_D3PD_2MixedFilterMuidCalo", nLept=2,\
                                                    pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    #electron quality checked before
                                                    qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                                    types=["mu","mu"],\
                                                    etaCuts=[0.2,9999],\
                                                    collections=["UniqueCaloMuonCollection","MuidMuonCollection"],\
                                                    mass=massCut_daod_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())





mixedFilterStaco_d3pd_2l=H4lDPDMaker.H4lMixedFilter("HSG2_D3PD_2MixedFilterStaco", nLept=2,\
                                                    pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                                    qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                                    types=["e","mu"],\
                                                    collections=[eCollToUse,"StacoMuonCollection"],\
                                                    mass=massCut_d3pd_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())

mixedFilterMuid_d3pd_2l=H4lDPDMaker.H4lMixedFilter("HSG2_D3PD_2MixedFilterMuid", nLept=2,\
                                                   pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                   eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                                   qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                                   types=["e","mu"],\
                                                   collections=[eCollToUse,"MuidMuonCollection"],\
                                                   mass=massCut_d3pd_2l,\
                                                   acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())


mixedFilterCalo_d3pd_2l=H4lDPDMaker.H4lMixedFilter("HSG2_D3PD_2MixedFilterCalo", nLept=2,\
                                                    etaCuts=[9999,0.2],\
                                                    pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                                    eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                                    #electron quality checked before
                                                    qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                                    types=["e","mu"],\
                                                    collections=[eCollToUse,"UniqueCaloMuonCollection"],\
                                                    mass=massCut_daod_2l,\
                                                    acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE())



theJob+=mixedFilterStaco_d3pd_2l

msg.info("Added 1e1mu(staco) filter")

theJob+=mixedFilterMuid_d3pd_2l

msg.info("Added 1e1mu(muid) filter")


theJob+=mixedFilterCalo_d3pd_2l

msg.info("Added 1e1mu(calo) filter")

theJob+=mixedFilterStacoCalo_d3pd_2l
msg.info("Added 1mu(staco)1mu(calo) filter")

theJob+=mixedFilterMuidCalo_d3pd_2l
msg.info("Added 1mu(muid)1mu(calo) filter")


combination_d3pd_2l="HSG2_D3PD_2ElectronFilter or "+"HSG2_D3PD_2StacoMuonFilter or "+"HSG2_D3PD_2MuidMuonFilter or "+"HSG2_D3PD_2MixedFilterStaco or "+"HSG2_D3PD_2MixedFilterMuid or "+"HSG2_D3PD_2MixedFilterCalo or "+"HSG2_D3PD_2MixedFilterMuidCalo or "+"HSG2_D3PD_2MixedFilterStacoCalo"

totalDecision_d3pd_2l=LogicalFilterCombiner("HSG2_2LD3PDFilter",cmdstring = combination_d3pd_2l)
theJob+=totalDecision_d3pd_2l

from HSG2DPDUtils.HSG2D3PD import HSG2physicsD3PD

stream_d3pd_2l=HSG2physicsD3PD("StreamNTUP_2LHSG2",runArgs.outputNTUP_2LHSG2File)

stream_d3pd_2l.AcceptAlgs = [totalDecision_d3pd_2l.getName()]

