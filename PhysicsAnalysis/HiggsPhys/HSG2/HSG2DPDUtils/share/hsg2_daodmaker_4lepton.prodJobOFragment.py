
## DPDMakerScript to be included in rec.DPDMakerScripts()

include.block("HSG2DPDUtils/hsg2_daodmaker_4lepton.py")
include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

# shortcuts
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

etCut_electron_daod_4l=jobproperties.HSG2.fourLeptonElectronEtCut()
ptCut_muon_daod_4l=jobproperties.HSG2.fourLeptonMuonPtCut()
massCut_daod_4l=jobproperties.HSG2.fourLeptonMassCut()

from AthenaCommon.Logging import logging
msg = logging.getLogger( "HSG2DAODFilters" )


# AthElectronLikelihoodTool with PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools/data/ElectronLikelihoodPdfs.root
# and LikeEnum::Loose by following https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HiggsZZllllSummer2013#More_information
import ROOT
import cppyy
cppyy.loadDict('libElectronPhotonSelectorToolsDict')
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc, "AthElectronLikelihoodTool_Loose"):
    from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AthElectronLikelihoodTool
    ToolSvc += AthElectronLikelihoodTool( "AthElectronLikelihoodTool_Loose",
                                          inputPDFFileName  = "ElectronPhotonSelectorTools/ElectronLikelihoodPdfs.root",
                                          cutLikelihoodEnum = ROOT.LikeEnum.Loose,
                                          useUserData = False ,
                                          forceRecalculateImpactParameter = True)
    msg.info("AthElectronLikelihoodTool/AthElectronLikelihoodTool_Loose is added")

algsList=[]

# eeee
theJob += H4lDPDMaker.H4lElectronFilter("HSG2_DAOD_4ElectronFilter",nElec=4,\
                                        eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,etCut_electron_daod_4l,etCut_electron_daod_4l],\
                                        qualityCuts=[eQual,eQual,eQual,eQual],\
                                        mass=massCut_daod_4l,\
                                        collectionName=eCollToUse,\
                                        acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                        kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                        kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                        kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                        checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4ElectronFilter.name())
msg.info("Added 4e filter")

# ssss
theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_4StacoMuonFilter",nMu=4,\
                                    pTCuts=[ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                    qualityCuts=[muQual,muQual,muQual,muQual],\
                                    mass=massCut_daod_4l,\
                                    kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                    kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                    kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                    checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4StacoMuonFilter.name())
msg.info("Added 4mu (staco) filter")

# mmmm
theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_4MuidMuonFilter",nMu=4,\
                                    pTCuts=[ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                    collectionName="MuidMuonCollection",\
                                    qualityCuts=[muQual,muQual,muQual,muQual],\
                                    mass=massCut_daod_4l,\
                                    kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                    kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                    kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                    checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())                                    
algsList.append(theJob.HSG2_DAOD_4MuidMuonFilter.name())
msg.info("Added 4mu (muid) filter")

# uuuu
theJob += H4lDPDMaker.H4lMuonFilter("HSG2_DAOD_4MuonsFilter",nMu=4,\
                                    pTCuts=[ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                    collectionName="Muons",\
                                    qualityCuts=[muQual,muQual,muQual,muQual],\
                                    mass=massCut_daod_4l,\
                                    kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                    kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                    kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                    checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MuonsFilter.name())
msg.info("Added 4mu (unified) filter")

# eess
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronStaco_2e2mu", nLept=4,\
                                     pTCuts=[0,0,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,0,0],\
                                     qualityCuts=[eQual,eQual,muQual,muQual],\
                                     types=["e","e","mu","mu"],\
                                     collections=[eCollToUse,eCollToUse,"StacoMuonCollection","StacoMuonCollection"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronStaco_2e2mu.name())
msg.info("Added 2e2mu(staco) filter")

# eemm
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuid_2e2mu", nLept=4,\
                                     pTCuts=[0,0,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,0,0],\
                                     qualityCuts=[eQual,eQual,muQual,muQual],\
                                     types=["e","e","mu","mu"],\
                                     collections=[eCollToUse,eCollToUse,"MuidMuonCollection","MuidMuonCollection"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuid_2e2mu.name())
msg.info("Added 2e2mu(muid) filter")

# eeuu
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuons_2e2mu", nLept=4,\
                                     pTCuts=[0,0,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,0,0],\
                                     qualityCuts=[eQual,eQual,muQual,muQual],\
                                     types=["e","e","mu","mu"],\
                                     collections=[eCollToUse,eCollToUse,"Muons","Muons"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuons_2e2mu.name())
msg.info("Added 2e2mu(unified) filter")

# eesc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronStacoCalo_2e2mu", nLept=4,\
                                     pTCuts=[0,0,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,0,0],\
                                     qualityCuts=[eQual,eQual,muQual,muQual],\
                                     types=["e","e","mu","mu"],\
                                     collections=[eCollToUse,eCollToUse,"StacoMuonCollection","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronStacoCalo_2e2mu.name())
msg.info("Added 2e2mu(staco/calo) filter")

# eemc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuidCalo_2e2mu", nLept=4,\
                                     pTCuts=[0,0,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,0,0],\
                                     qualityCuts=[eQual,eQual,muQual,muQual],\
                                     types=["e","e","mu","mu"],\
                                     collections=[eCollToUse,eCollToUse,"MuidMuonCollection","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuidCalo_2e2mu.name())
msg.info("Added 2e2mu(muid/calo) filter")

# eeuc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuonsCalo_2e2mu", nLept=4,\
                                     pTCuts=[0,0,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,0,0],\
                                     qualityCuts=[eQual,eQual,muQual,muQual],\
                                     types=["e","e","mu","mu"],\
                                     collections=[eCollToUse,eCollToUse,"Muons","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuonsCalo_2e2mu.name())
msg.info("Added 2e2mu(muid/calo) filter")

# sssc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterStacoCalo_4mu", nLept=4,\
                                     pTCuts=[ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     qualityCuts=[muQual,muQual,muQual,muQual],\
                                     types=["mu","mu","mu","mu"],\
                                     collections=["StacoMuonCollection","StacoMuonCollection","StacoMuonCollection","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterStacoCalo_4mu.name())
msg.info("Added 4mu(staco/calo) filter")

# mmmc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterMuidCalo_4mu", nLept=4,\
                                     pTCuts=[ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     qualityCuts=[muQual,muQual,muQual,muQual],\
                                     types=["mu","mu","mu","mu"],\
                                     collections=["MuidMuonCollection","MuidMuonCollection","MuidMuonCollection","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterMuidCalo_4mu.name())
msg.info("Added 4mu(muid/calo) filter")

# uuuc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterMuonsCalo_4mu", nLept=4,\
                                     pTCuts=[ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     qualityCuts=[muQual,muQual,muQual,muQual],\
                                     types=["mu","mu","mu","mu"],\
                                     collections=["Muons","Muons","Muons","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterMuonsCalo_4mu.name())
msg.info("Added 4mu(unified/calo) filter")

# eees
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronStaco_3e1mu", nLept=4,\
                                     pTCuts=[0,0,0,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,etCut_electron_daod_4l,0],\
                                     qualityCuts=[eQual,eQual,eQual,muQual],\
                                     types=["e","e","e","mu"],\
                                     collections=[eCollToUse,eCollToUse,eCollToUse,"StacoMuonCollection"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronStaco_3e1mu.name())
msg.info("Added 3e1mu(staco) filter")

# eeem
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuid_3e1mu", nLept=4,\
                                     pTCuts=[0,0,0,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,etCut_electron_daod_4l,0],\
                                     qualityCuts=[eQual,eQual,eQual,muQual],\
                                     types=["e","e","e","mu"],\
                                     collections=[eCollToUse,eCollToUse,eCollToUse,"MuidMuonCollection"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuid_3e1mu.name())
msg.info("Added 3e1mu(muid) filter")

# eeec
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronCalo_3e1mu", nLept=4,\
                                     pTCuts=[0,0,0,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,etCut_electron_daod_4l,0],\
                                     qualityCuts=[eQual,eQual,eQual,muQual],\
                                     types=["e","e","e","mu"],\
                                     collections=[eCollToUse,eCollToUse,eCollToUse,"UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronCalo_3e1mu.name())
msg.info("Added 3e1mu(calo) filter")

# eeeu
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuons_3e1mu", nLept=4,\
                                     pTCuts=[0,0,0,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,etCut_electron_daod_4l,etCut_electron_daod_4l,0],\
                                     qualityCuts=[eQual,eQual,eQual,muQual],\
                                     types=["e","e","e","mu"],\
                                     collections=[eCollToUse,eCollToUse,eCollToUse,"Muons"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuons_3e1mu.name())
msg.info("Added 3e1mu(unfied) filter")

# esss
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronStaco_1e3mu", nLept=4,\
                                     pTCuts=[0,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,0,0,0],\
                                     qualityCuts=[eQual,muQual,muQual,muQual],\
                                     types=["e","mu","mu","mu"],\
                                     collections=[eCollToUse,"StacoMuonCollection","StacoMuonCollection","StacoMuonCollection"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronStaco_1e3mu.name())
msg.info("Added 1e3mu(staco) filter")

# emmm
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuid_1e3mu", nLept=4,\
                                     pTCuts=[0,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,0,0,0],\
                                     qualityCuts=[eQual,muQual,muQual,muQual],\
                                     types=["e","mu","mu","mu"],\
                                     collections=[eCollToUse,"MuidMuonCollection","MuidMuonCollection","MuidMuonCollection"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuid_1e3mu.name())
msg.info("Added 1e3mu(muid) filter")

# euuu
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuons_1e3mu", nLept=4,\
                                     pTCuts=[0,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,0,0,0],\
                                     qualityCuts=[eQual,muQual,muQual,muQual],\
                                     types=["e","mu","mu","mu"],\
                                     collections=[eCollToUse,"Muons","Muons","Muons"],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuons_1e3mu.name())
msg.info("Added 1e3mu(unfied) filter")

# essc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronStacoCalo_1e3mu", nLept=4,\
                                     pTCuts=[0,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,0,0,0],\
                                     qualityCuts=[eQual,muQual,muQual,muQual],\
                                     types=["e","mu","mu","mu"],\
                                     collections=[eCollToUse,"StacoMuonCollection","StacoMuonCollection","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronStacoCalo_1e3mu.name())
msg.info("Added 1e3mu(staco/calo) filter")

# emmc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuidCalo_1e3mu", nLept=4,\
                                     pTCuts=[0,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,0,0,0],\
                                     qualityCuts=[eQual,muQual,muQual,muQual],\
                                     types=["e","mu","mu","mu"],\
                                     collections=[eCollToUse,"MuidMuonCollection","MuidMuonCollection","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuidCalo_1e3mu.name())
msg.info("Added 1e3mu(muid/calo) filter")

# euuc
theJob += H4lDPDMaker.H4lMixedFilter("HSG2_DAOD_4MixedFilterElectronMuonsCalo_1e3mu", nLept=4,\
                                     pTCuts=[0,ptCut_muon_daod_4l,ptCut_muon_daod_4l,ptCut_muon_daod_4l],\
                                     eTCuts=[etCut_electron_daod_4l,0,0,0],\
                                     qualityCuts=[eQual,muQual,muQual,muQual],\
                                     types=["e","mu","mu","mu"],\
                                     collections=[eCollToUse,"Muons","Muons","UniqueCaloMuonCollection"],\
                                     etaCuts=[9999,9999,9999,0.2],\
                                     mass=massCut_daod_4l,\
                                     acceptFrwdE=jobproperties.HSG2.fourLeptonAcceptFrwdE(),\
                                     kinematicsPtCuts=jobproperties.HSG2.fourLeptonKinematicsPtCuts(),\
                                     kinematicsMassCuts=jobproperties.HSG2.fourLeptonKinematicsMassCuts(),\
                                     kinematicsDeltaRCut=jobproperties.HSG2.fourLeptonKinematicsDeltaRCut(),\
                                     checkLArError=jobproperties.HSG2.fourLeptonCheckLArError())
algsList.append(theJob.HSG2_DAOD_4MixedFilterElectronMuonsCalo_1e3mu.name())
msg.info("Added 1e3mu(unified/calo) filter")

combination=""
for iAlg in range(len(algsList)):
    if iAlg==0: 
        combination=algsList[iAlg]
    else:
        combination+=" or "+algsList[iAlg]

totalDecision_daod_4l=LogicalFilterCombiner("HSG2_DAOD_4l_Filter",cmdstring = combination)
theJob+=totalDecision_daod_4l

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

# define streams


from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D2PDFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Construct the stream and file names for the jp:
streamName = prodFlags.WriteDAOD_4LHSG2Stream.StreamName
fileName   = buildFileName( prodFlags.WriteDAOD_4LHSG2Stream )


#streamOut = MSMgr.NewPoolStream("StreamDAOD_2LHSG2",runArgs.outputDAOD_2LHSG2File)
streamOut_daod_4l = MSMgr.NewPoolStream(streamName,fileName)
streamOut_daod_4l.AcceptAlgs([totalDecision_daod_4l.getName()])
streamOut_daod_4l.AddOtherAlgsToBookkeep(algsList)

dpdOutHelper.addAllItemsFromInputExceptExcludeList(streamName,[])

streamOut_daod_4l.AddMetaDataItem("EventBookkeeperCollection#*")

# needed for e Bremm
streamOut_daod_4l.AddItem('ElectronContainer#GSFElectronAODCollection')
streamOut_daod_4l.AddItem('egDetailContainer#GSFegDetailAOD')
streamOut_daod_4l.AddItem('Rec::TrackParticleContainer#GSFTrackParticleCandidate')
streamOut_daod_4l.AddItem('TrackCollection#GSFTracks')
streamOut_daod_4l.AddItem('CaloClusterContainer#GSFElectronsClusters')
streamOut_daod_4l.AddItem('CaloCellLinkContainer#GSFElectronsClusters_Link')
streamOut_daod_4l.AddItem('INav4MomAssocs#GSFElectronsAssociation')
streamOut_daod_4l.AddItem('TrackParticleAssocs#GSFTrackAssociation')
streamOut_daod_4l.AddItem('INav4MomAssocs#GSFElectronPhotonAssociation')
streamOut_daod_4l.AddItem('TrackParticleTruthCollection#GSFTrackParticleTruthCollection') 
