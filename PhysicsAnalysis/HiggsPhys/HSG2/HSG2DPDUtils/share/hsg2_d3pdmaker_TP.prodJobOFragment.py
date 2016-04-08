include.block("HSG2DPDUtils/hsg2_d3pdmaker_TP.prodJobOFragment.py")

############ FILTER ############

include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

# shortcuts
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

electronQualityCuts_tp=jobproperties.HSG2.tagAndProbeEQual()
muonQualityCuts_tp=jobproperties.HSG2.tagAndProbeMuQual()
mixMuonQualityCuts_tp=[jobproperties.HSG2.tagAndProbeMixQual()[1],jobproperties.HSG2.tagAndProbeMixQual()[1]]

electronEtCuts_tp=jobproperties.HSG2.tagAndProbeEPtCuts()
muonPtCuts_tp=jobproperties.HSG2.tagAndProbeMuPtCuts()
mixMuonPtCuts_tp=[jobproperties.HSG2.tagAndProbeMixPtCuts()[0],jobproperties.HSG2.tagAndProbeMixPtCuts()[1]]
mixMuonPtCuts_REVESE_tp=[jobproperties.HSG2.tagAndProbeMixPtCuts()[1],jobproperties.HSG2.tagAndProbeMixPtCuts()[0]]

massCut_tp=jobproperties.HSG2.tagAndProbeMassCut()

acceptFrwdE_tp=jobproperties.HSG2.tagAndProbeAcceptFrwdE()
checkLArError=jobproperties.HSG2.tagAndProbeCheckLArError()

from AthenaCommon.Logging import logging

msg = logging.getLogger( "HSG2TPFilters" )

# Trigger filters 
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter

el_trigsList=[]
for iTrigger in range(len(jobproperties.HSG2.tagAndProbeElectronTriggers())):
    triggerItem = jobproperties.HSG2.tagAndProbeElectronTriggers()[iTrigger]
    filterName = "TriggerFilter_"+triggerItem
    theJob += TriggerFilter(filterName,
                            trigger = triggerItem)
    el_trigsList.append(filterName)

mu_trigsList=[]
for iTrigger in range(len(jobproperties.HSG2.tagAndProbeMuonTriggers())):
    triggerItem = jobproperties.HSG2.tagAndProbeMuonTriggers()[iTrigger]
    filterName = "TriggerFilter_"+triggerItem
    theJob += TriggerFilter(filterName,
                            trigger = triggerItem)
    mu_trigsList.append(filterName)

# Lepton filters 
el_algsList=[]
theJob += H4lDPDMaker.H4lElectronFilter("HSG2_TP_2ElectronFilter",nElec=2,\
                                        eTCuts=electronEtCuts_tp,\
                                        qualityCuts=electronQualityCuts_tp,\
                                        mass=massCut_tp,
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=acceptFrwdE_tp,\
                                        checkLArError=checkLArError)
el_algsList.append(theJob.HSG2_TP_2ElectronFilter.name())
msg.info("Added 2e filter")

mu_algsList=[]
theJob += H4lDPDMaker.H4lMuonFilter("HSG2_TP_2StacoMuonFilter",nMu=2,\
                                    pTCuts=muonPtCuts_tp,\
                                    qualityCuts=muonQualityCuts_tp,\
                                    mass=massCut_tp,\
                                    checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2StacoMuonFilter.name())
msg.info("Added 2mu (staco) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_TP_2MuidMuonFilter",nMu=2,\
                                    pTCuts=muonPtCuts_tp,\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=muonQualityCuts_tp,\
                                    mass=massCut_tp,\
                                    checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MuidMuonFilter.name())
msg.info("Added 2mu (muid) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_TP_2MuonsFilter",nMu=2,\
                                    pTCuts=muonPtCuts_tp,\
                                    collectionName="Muons",\
                                    qualityCuts=muonQualityCuts_tp,\
                                    mass=massCut_tp,\
                                    checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MuonsFilter.name())
msg.info("Added 2mu (unified) filter")

theJob += H4lDPDMaker.H4lMuonFilter("HSG2_TP_2CaloMuonFilter",nMu=2,\
                                    collectionName="UniqueCaloMuonCollection",\
                                    etaCuts=[0.2,0.2],\
                                    pTCuts=muonPtCuts_tp,\
                                    qualityCuts=muonQualityCuts_tp,\
                                    mass=massCut_tp,\
                                    checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2CaloMuonFilter.name())
msg.info("Added 2mu (calo) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_TP_2MixedFilterStacoCalo", nLept=2,\
                                     pTCuts=mixMuonPtCuts_tp,\
                                     qualityCuts=mixMuonQualityCuts_tp,\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","StacoMuonCollection"],\
                                     mass=massCut_tp,\
                                     acceptFrwdE=acceptFrwdE_tp,\
                                     checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MixedFilterStacoCalo.name())
msg.info("Added 2mu (calo/staco) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_TP_2MixedFilterMuidCalo", nLept=2,\
                                     pTCuts=mixMuonPtCuts_tp,\
                                     qualityCuts=mixMuonQualityCuts_tp,\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","MuidMuonCollection"],\
                                     mass=massCut_tp,\
                                     acceptFrwdE=acceptFrwdE_tp,\
                                     checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MixedFilterMuidCalo.name())
msg.info("Added 2mu (calo/muid) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_TP_2MixedFilterMuonsCalo", nLept=2,\
                                     pTCuts=mixMuonPtCuts_tp,\
                                     qualityCuts=mixMuonQualityCuts_tp,\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","Muons"],\
                                     mass=massCut_tp,\
                                     acceptFrwdE=acceptFrwdE_tp,\
                                     checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MixedFilterMuonsCalo.name())
msg.info("Added 2mu (calo/unified) filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_TP_2MixedFilterStacoCaloRev", nLept=2,\
                                     pTCuts=mixMuonPtCuts_REVESE_tp,\
                                     qualityCuts=mixMuonQualityCuts_tp,\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","StacoMuonCollection"],\
                                     mass=massCut_tp,\
                                     acceptFrwdE=acceptFrwdE_tp,\
                                     checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MixedFilterStacoCaloRev.name())
msg.info("Added 2mu (calo/staco) rev filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_TP_2MixedFilterMuidCaloRev", nLept=2,\
                                     pTCuts=mixMuonPtCuts_REVESE_tp,\
                                     qualityCuts=mixMuonQualityCuts_tp,\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","MuidMuonCollection"],\
                                     mass=massCut_tp,\
                                     acceptFrwdE=acceptFrwdE_tp,\
                                     checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MixedFilterMuidCaloRev.name())
msg.info("Added 2mu (calo/muid) rev filter")

theJob += H4lDPDMaker.H4lMixedFilter("HSG2_TP_2MixedFilterMuonsCaloRev", nLept=2,\
                                     pTCuts=mixMuonPtCuts_REVESE_tp,\
                                     qualityCuts=mixMuonQualityCuts_tp,\
                                     types=["mu","mu"],\
                                     etaCuts=[0.2,9999],\
                                     collections=["UniqueCaloMuonCollection","Muons"],\
                                     mass=massCut_tp,\
                                     acceptFrwdE=acceptFrwdE_tp,\
                                     checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_TP_2MixedFilterMuonsCaloRev.name())
msg.info("Added 2mu (calo/unified) rev filter")

combination_tp="(("
for iAlg in range(len(el_algsList)):
    if iAlg==0:
        combination_tp+=el_algsList[iAlg]
    else:
        combination_tp+=" or "+el_algsList[iAlg]
combination_tp+=") and ("

for iTrig in range(len(el_trigsList)):
    if iTrig==0:
        combination_tp+=el_trigsList[iTrig]
    else:
        combination_tp+=" or "+el_trigsList[iTrig]
combination_tp+=")) or (("

for iAlg in range(len(mu_algsList)):
    if iAlg==0:
        combination_tp+=mu_algsList[iAlg]
    else:
        combination_tp+=" or "+mu_algsList[iAlg]
combination_tp+=") and ("

for iTrig in range(len(mu_trigsList)):
    if iTrig==0:
        combination_tp+=mu_trigsList[iTrig]
    else:
        combination_tp+=" or "+mu_trigsList[iTrig]
combination_tp+="))"

algsList = el_algsList + el_trigsList + mu_algsList + mu_trigsList

totalDecision_tp=LogicalFilterCombiner("HSG2_TPFilter",cmdstring = combination_tp)
theJob+=totalDecision_tp

############ NTUPLING ############

from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
streamName = prodFlags.WriteTPHSG2D3PD.StreamName

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
fileName = buildFileName(prodFlags.WriteTPHSG2D3PD)

from HSG2DPDUtils.HSG2D3PD import HSG2physicsD3PD

# Perform MV3 b-tagging when creating D3PD for 2013 Moriond analysis
if jobproperties.HSG2.doMV3BTagInD3PD():
    include("BTagging/BTagging_LoadTools.py")
    include("BTagging/BTagging_jobOptions.py")

stream_d3pd_tp=HSG2physicsD3PD(streamName,fileName)
stream_d3pd_tp.AcceptAlgs = [totalDecision_tp.getName()]
