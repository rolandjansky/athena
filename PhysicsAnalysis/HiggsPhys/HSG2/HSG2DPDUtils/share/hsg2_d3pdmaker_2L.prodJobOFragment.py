include.block("HSG2DPDUtils/hsg2_d3pdmaker_2L.prodJobOFragment.py")

############ FILTER ############

include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

# shortcuts
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

ptCut_ntup_2l=jobproperties.HSG2.diLeptonPtCut()
massCut_ntup_2l=jobproperties.HSG2.diLeptonMassCut()

from AthenaCommon.Logging import logging

msg = logging.getLogger( "NTUP_2LHSG2_Filters" )

algsList=[]

theJob += H4lDPDMaker.H4lElectronFilter("HSG2_NTUP_2ElectronFilter",nElec=2,\
                                        eTCuts=jobproperties.HSG2.diLeptonEPtCuts(),\
                                        qualityCuts=jobproperties.HSG2.diLeptonEQual(),\
                                        mass=massCut_ntup_2l,
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                        checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2ElectronFilter.name())
msg.info("Added 2e filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_NTUP_2StacoMuonFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_ntup_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2StacoMuonFilter.name())
msg.info("Added 2mu (staco) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_NTUP_2MuidMuonFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_ntup_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MuidMuonFilter.name())
msg.info("Added 2mu (muid) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_NTUP_2MuonsFilter",nMu=2,\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    collectionName="Muons",\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_ntup_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MuonsFilter.name())
msg.info("Added 2mu (unified) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_NTUP_2CaloMuonFilter",nMu=2,\
                                    collectionName="UniqueCaloMuonCollection",\
                                    etaCuts=[0.2,0.2],\
                                    pTCuts=jobproperties.HSG2.diLeptonMuPtCuts(),\
                                    qualityCuts=jobproperties.HSG2.diLeptonMuQual(),\
                                    mass=massCut_ntup_2l,\
                                    checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2CaloMuonFilter.name())
msg.info("Added 2mu (calo) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_NTUP_2MixedFilterStacoCalo", nLept=2,\
                                     pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","StacoMuonCollection"],\
                                     mass=massCut_ntup_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MixedFilterStacoCalo.name())
msg.info("Added 2mu (calo/staco) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_NTUP_2MixedFilterMuidCalo", nLept=2,\
                                     pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","MuidMuonCollection"],\
                                     mass=massCut_ntup_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MixedFilterMuidCalo.name())
msg.info("Added 2mu (calo/muid) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_NTUP_2MixedFilterMuonsCalo", nLept=2,\
                                     pTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[1],jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     qualityCuts=[jobproperties.HSG2.diLeptonMixQual()[1],jobproperties.HSG2.diLeptonMixQual()[1]],\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","Muons"],\
                                     mass=massCut_ntup_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MixedFilterMuonsCalo.name())
msg.info("Added 2mu (calo/unified) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_NTUP_2MixedFilterStaco", nLept=2,\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"StacoMuonCollection"],\
                                     mass=massCut_ntup_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())                                                    
algsList.append(theJob.HSG2_NTUP_2MixedFilterStaco.name())
msg.info("Added 1e1mu (staco) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_NTUP_2MixedFilterMuid", nLept=2,\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"MuidMuonCollection"],\
                                     mass=massCut_ntup_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MixedFilterMuid.name())
msg.info("Added 1e1mu (muid) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_NTUP_2MixedFilterMuons", nLept=2,\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"Muons"],\
                                     mass=massCut_ntup_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MixedFilterMuons.name())
msg.info("Added 1e1mu (unified) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_NTUP_2MixedFilterCalo", nLept=2,\
                                     etaCuts=[9999,0.2],\
                                     pTCuts=[0,jobproperties.HSG2.diLeptonMixPtCuts()[1]],\
                                     eTCuts=[jobproperties.HSG2.diLeptonMixPtCuts()[0],0],\
                                     qualityCuts=jobproperties.HSG2.diLeptonMixQual(),\
                                     types=["e","mu"],\
                                     collections=['ElectronAODCollection',"UniqueCaloMuonCollection"],\
                                     mass=massCut_ntup_2l,\
                                     acceptFrwdE=jobproperties.HSG2.diLeptonAcceptFrwdE(),\
                                     checkLArError=jobproperties.HSG2.diLeptonCheckLArError())
algsList.append(theJob.HSG2_NTUP_2MixedFilterCalo.name())
msg.info("Added 1e1mu (calo) filter")

combination_ntup_2l=""
for iAlg in range(len(algsList)):
    if iAlg==0:
        combination_ntup_2l=algsList[iAlg]
    else:
        combination_ntup_2l+=" or "+algsList[iAlg]

totalDecision_ntup_2l=LogicalFilterCombiner("HSG2_2LNTUPFilter",cmdstring = combination_ntup_2l)
theJob+=totalDecision_ntup_2l

############ NTUPLING ############

from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
streamName = prodFlags.Write2LHSG2D3PD.StreamName

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
fileName = buildFileName(prodFlags.Write2LHSG2D3PD)

from HSG2DPDUtils.HSG2D3PD import HSG2physicsD3PD

# Perform MV3 b-tagging when creating D3PD for 2013 Moriond analysis
if jobproperties.HSG2.doMV3BTagInD3PD():
    include("BTagging/BTagging_LoadTools.py")
    include("BTagging/BTagging_jobOptions.py")

stream_d3pd_2L=HSG2physicsD3PD(streamName,fileName)
stream_d3pd_2L.AcceptAlgs = [totalDecision_ntup_2l.getName()] 
