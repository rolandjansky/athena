include.block("HSG2DPDUtils/hsg2_d3pdmaker_2L2Q.prodJobOFragment.py")

############ FILTER ############

include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

import HSG2DPDUtils.HSG2DPDFlags
e_etCut =  jobproperties.HSG2.llqqElectronEtCut() 
mu_ptCut =  jobproperties.HSG2.llqqMuonPtCut() 
jet_ptCut = jobproperties.HSG2.llqqJetPtCut() 
mergedJet_ptCut = jobproperties.HSG2.llqqMergedJetPtCut() 
e_qual = jobproperties.HSG2.llqqElectronQual()
mu_qual = jobproperties.HSG2.llqqMuonQual()
calo_qual = jobproperties.HSG2.llqqCaloMuonQual()
jet_qual = jobproperties.HSG2.llqqJetQual()
mergedJet_qual = jobproperties.HSG2.llqqMergedJetQual()
e_coll = jobproperties.HSG2.llqqElectronCollection()
staco_coll = jobproperties.HSG2.llqqStacoMuonCollection()
muons_coll = jobproperties.HSG2.llqqMuonsCollection()
calo_coll = jobproperties.HSG2.llqqCaloMuonCollection()
jet_coll = jobproperties.HSG2.llqqJetCollection()
diLeptonMassCut = jobproperties.HSG2.llqqDiLeptonMassCut()
electronJetDRCut = jobproperties.HSG2.llqqElectronJetDRCut()

from AthenaCommon.Logging import logging
msg = logging.getLogger( "NTUP_2L2QHSG2_Filter" )

# AthElectronLikelihoodTool with PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools/data/ElectronLikelihoodPdfs.root
# and LikeEnum::Loose by following https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HiggsZZllllSummer2013#More_information
import ROOT
import cppyy
cppyy.loadDict('libElectronPhotonSelectorToolsDict')
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc, "AthElectronLikelihoodTool_VeryLoose"):
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
    ToolSvc += AthElectronLikelihoodTool( "AthElectronLikelihoodTool_VeryLoose",
                                          inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                          cutLikelihoodEnum = ROOT.LikeEnum.VeryLoose,
                                          useUserData = False ,
                                          forceRecalculateImpactParameter = True)
    msg.info("AthElectronLikelihoodTool/AthElectronLikelihoodTool_VeryLoose is added")
if not hasattr(ToolSvc, "AthElectronLikelihoodTool_Loose"):
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
    ToolSvc += AthElectronLikelihoodTool( "AthElectronLikelihoodTool_Loose",
                                          inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                          cutLikelihoodEnum = ROOT.LikeEnum.Loose,
                                          useUserData = False ,
                                          forceRecalculateImpactParameter = True)
    msg.info("AthElectronLikelihoodTool/AthElectronLikelihoodTool_Loose is added")
if not hasattr(ToolSvc, "AthElectronLikelihoodTool_Medium"):
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
    ToolSvc += AthElectronLikelihoodTool( "AthElectronLikelihoodTool_Medium",
                                          inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                          cutLikelihoodEnum = ROOT.LikeEnum.Medium,
                                          useUserData = False ,
                                          forceRecalculateImpactParameter = True)
    msg.info("AthElectronLikelihoodTool/AthElectronLikelihoodTool_Medium is added")


algsList=[]

from HSG2DPDUtils import HSG2_2L2QDPDMaker

# eeqq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2E2QHSG2_Filter",
                                               types=["e", "e", "jet", "jet"],\
                                               pTCuts=[e_etCut, e_etCut, jet_ptCut, jet_ptCut],\
                                               qualityCuts=[e_qual, e_qual, jet_qual, jet_qual],\
                                               collections=[e_coll, e_coll, jet_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut,\
                                               electronJetDRCut=electronJetDRCut)
algsList.append(theJob.NTUP_2E2QHSG2_Filter.name())
msg.info("Added eeqq filter")

# ssqq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU2QHSG2_Staco_Filter",
                                               types=["mu", "mu", "jet", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, jet_ptCut, jet_ptCut],\
                                               qualityCuts=[mu_qual, mu_qual, jet_qual, jet_qual],\
                                               collections=[staco_coll, staco_coll, jet_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU2QHSG2_Staco_Filter.name())
msg.info("Added mumuqq (staco) filter")

# uuqq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU2QHSG2_Muons_Filter",
                                               types=["mu", "mu", "jet", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, jet_ptCut, jet_ptCut],\
                                               qualityCuts=[mu_qual, mu_qual, jet_qual, jet_qual],\
                                               collections=[muons_coll, muons_coll, jet_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU2QHSG2_Muons_Filter.name())
msg.info("Added mumuqq (muons) filter")

# ccqq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU2QHSG2_Calo_Filter",
                                               types=["mu", "mu", "jet", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, jet_ptCut, jet_ptCut],\
                                               qualityCuts=[calo_qual, calo_qual, jet_qual, jet_qual],\
                                               collections=[calo_coll, calo_coll, jet_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU2QHSG2_Calo_Filter.name())
msg.info("Added mumuqq (calo) filter")

# scqq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU2QHSG2_StacoCalo_Filter",
                                               types=["mu", "mu", "jet", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, jet_ptCut, jet_ptCut],\
                                               qualityCuts=[mu_qual, calo_qual, jet_qual, jet_qual],\
                                               collections=[staco_coll, calo_coll, jet_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU2QHSG2_StacoCalo_Filter.name())
msg.info("Added mumuqq (staco/calo) filter")

# ucqq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU2QHSG2_MuonsCalo_Filter",
                                               types=["mu", "mu", "jet", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, jet_ptCut, jet_ptCut],\
                                               qualityCuts=[mu_qual, calo_qual, jet_qual, jet_qual],\
                                               collections=[muons_coll, calo_coll, jet_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU2QHSG2_MuonsCalo_Filter.name())
msg.info("Added mumuqq (muons/calo) filter")

# eeq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2E1QHSG2_Filter",
                                               types=["e", "e", "jet"],\
                                               pTCuts=[e_etCut, e_etCut, mergedJet_ptCut],\
                                               qualityCuts=[e_qual, e_qual, mergedJet_qual],\
                                               collections=[e_coll, e_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut,\
                                               electronJetDRCut=electronJetDRCut)
algsList.append(theJob.NTUP_2E1QHSG2_Filter.name())
msg.info("Added eeq filter")

# ssq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU1QHSG2_Staco_Filter",
                                               types=["mu", "mu", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, mergedJet_ptCut],\
                                               qualityCuts=[mu_qual, mu_qual, mergedJet_qual],\
                                               collections=[staco_coll, staco_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU1QHSG2_Staco_Filter.name())
msg.info("Added mumuq (staco) filter")

# uuq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU1QHSG2_Muons_Filter",
                                               types=["mu", "mu", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, mergedJet_ptCut],\
                                               qualityCuts=[mu_qual, mu_qual, mergedJet_qual],\
                                               collections=[muons_coll, muons_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU1QHSG2_Muons_Filter.name())
msg.info("Added mumuq (muons) filter")

# ccq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU1QHSG2_Calo_Filter",
                                               types=["mu", "mu", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, mergedJet_ptCut],\
                                               qualityCuts=[calo_qual, calo_qual, mergedJet_qual],\
                                               collections=[calo_coll, calo_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU1QHSG2_Calo_Filter.name())
msg.info("Added mumuq (calo) filter")

# scq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU1QHSG2_StacoCalo_Filter",
                                               types=["mu", "mu", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, mergedJet_ptCut],\
                                               qualityCuts=[mu_qual, calo_qual, mergedJet_qual],\
                                               collections=[staco_coll, calo_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU1QHSG2_StacoCalo_Filter.name())
msg.info("Added mumuq (staco/calo) filter")

# ucq
theJob += HSG2_2L2QDPDMaker.HSG2_2L2QDPDFilter("NTUP_2MU1QHSG2_MuonsCalo_Filter",
                                               types=["mu", "mu", "jet"],\
                                               pTCuts=[mu_ptCut, mu_ptCut, mergedJet_ptCut],\
                                               qualityCuts=[mu_qual, calo_qual, mergedJet_qual],\
                                               collections=[muons_coll, calo_coll, jet_coll],\
                                               diLeptonMassCut=diLeptonMassCut)
algsList.append(theJob.NTUP_2MU1QHSG2_MuonsCalo_Filter.name())
msg.info("Added mumuq (muons/calo) filter")

combination=""
for iAlg in range(len(algsList)):
    if iAlg==0: 
        combination=algsList[iAlg]
    else:
        combination+=" or "+algsList[iAlg]

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
totalDecision_2L2Q=LogicalFilterCombiner("NTUP_2L2QHSG2_Filter",cmdstring = combination)
theJob+=totalDecision_2L2Q

############ NTUPLING ############

from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
streamName = prodFlags.Write2L2QHSG2D3PD.StreamName

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
fileName = buildFileName(prodFlags.Write2L2QHSG2D3PD)

from HSG2DPDUtils.HSG2D3PD import HSG2physicsD3PD

# Perform MV3 b-tagging when creating D3PD for 2013 Moriond analysis
if jobproperties.HSG2.doMV3BTagInD3PD():
    include("BTagging/BTagging_LoadTools.py")
    include("BTagging/BTagging_jobOptions.py")

stream_d3pd_2L2Q=HSG2physicsD3PD(streamName,fileName)
stream_d3pd_2L2Q.AcceptAlgs = [totalDecision_2L2Q.getName()]
