include.block("HSG2DPDUtils/hsg2_daodmaker_Jpsi.prodJobOFragment.py")
include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

############ FILTER ############

include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

# shortcuts
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

muonQualityCuts_Jpsi=jobproperties.HSG2.JpsiMuQual()
muonPtCuts_Jpsi=jobproperties.HSG2.JpsiMuPtCuts()
massCut_Up_Jpsi=jobproperties.HSG2.JpsiMassCutUp()
massCut_Do_Jpsi=jobproperties.HSG2.JpsiMassCutDo()
massCut_Up_Upsilon=jobproperties.HSG2.UpsilonMassCutUp()
massCut_Do_Upsilon=jobproperties.HSG2.UpsilonMassCutDo()
photonPtCut_Jpsi=jobproperties.HSG2.JpsiPhotonPtCut()
photonEtaCut_Jpsi=jobproperties.HSG2.JpsiPhotonEtaCut()
doPhoton_Jpsi=jobproperties.HSG2.JpsidoPhoton()
doOS_Jpsi=jobproperties.HSG2.JpsidoOS()
checkLArError=jobproperties.HSG2.JpsiCheckLArError()

from AthenaCommon.Logging import logging

msg = logging.getLogger( "HSG2JpsiFilters" )

# Trigger filters 
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter

mu_trigsList=[]
for iTrigger in range(len(jobproperties.HSG2.JpsiMuonTriggers())):
    triggerItem = jobproperties.HSG2.JpsiMuonTriggers()[iTrigger]
    filterName = "TriggerFilter_"+triggerItem
    theJob += TriggerFilter(filterName,
                            trigger = triggerItem)
    mu_trigsList.append(filterName)

# Lepton filters 
mu_algsList=[]
theJob += H4lDPDMaker.H4lJpsiMuonFilter("HSG2_Jpsi_2StacoMuonFilter",nMu=2,\
                                            pTCuts=muonPtCuts_Jpsi,\
                                            qualityCuts=muonQualityCuts_Jpsi,\
                                            massUp=massCut_Up_Jpsi,\
                                            massDo=massCut_Do_Jpsi,\
                                            doOS=doOS_Jpsi,\
                                            doPhoton=doPhoton_Jpsi,\
                                            photonPt=photonPtCut_Jpsi,\
                                            photonEta=photonEtaCut_Jpsi,\
                                            checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_Jpsi_2StacoMuonFilter.name())
msg.info("Added 2mu (staco) J/psi filter")

theJob += H4lDPDMaker.H4lJpsiMuonFilter("HSG2_Jpsi_2MuidMuonFilter",nMu=2,\
                                            pTCuts=muonPtCuts_Jpsi,\
                                            collectionName="MuidMuonCollection",\
                                            qualityCuts=muonQualityCuts_Jpsi,\
                                            massUp=massCut_Up_Jpsi,\
                                            massDo=massCut_Do_Jpsi,\
                                            doOS=doOS_Jpsi,\
                                            doPhoton=doPhoton_Jpsi,\
                                            photonPt=photonPtCut_Jpsi,\
                                            photonEta=photonEtaCut_Jpsi,\
                                            checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_Jpsi_2MuidMuonFilter.name())
msg.info("Added 2mu (muid) J/psi filter")

theJob += H4lDPDMaker.H4lJpsiMuonFilter("HSG2_Jpsi_2MuonsFilter",nMu=2,\
                                            pTCuts=muonPtCuts_Jpsi,\
                                            collectionName="Muons",\
                                            qualityCuts=muonQualityCuts_Jpsi,\
                                            massUp=massCut_Up_Jpsi,\
                                            massDo=massCut_Do_Jpsi,\
                                            doOS=doOS_Jpsi,\
                                            doPhoton=doPhoton_Jpsi,\
                                            photonPt=photonPtCut_Jpsi,\
                                            photonEta=photonEtaCut_Jpsi,\
                                            checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_Jpsi_2MuonsFilter.name())
msg.info("Added 2mu (unified) J/psi filter")

theJob += H4lDPDMaker.H4lJpsiMuonFilter("HSG2_Upsilon_2StacoMuonFilter",nMu=2,\
                                            pTCuts=muonPtCuts_Jpsi,\
                                            qualityCuts=muonQualityCuts_Jpsi,\
                                            massUp=massCut_Up_Upsilon,\
                                            massDo=massCut_Do_Upsilon,\
                                            doOS=doOS_Jpsi,\
                                            doPhoton=doPhoton_Jpsi,\
                                            photonPt=photonPtCut_Jpsi,\
                                            photonEta=photonEtaCut_Jpsi,\
                                            checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_Upsilon_2StacoMuonFilter.name())
msg.info("Added 2mu (staco) Upsilon filter")

theJob += H4lDPDMaker.H4lJpsiMuonFilter("HSG2_Upsilon_2MuidMuonFilter",nMu=2,\
                                            pTCuts=muonPtCuts_Jpsi,\
                                            collectionName="MuidMuonCollection",\
                                            qualityCuts=muonQualityCuts_Jpsi,\
                                            massUp=massCut_Up_Upsilon,\
                                            massDo=massCut_Do_Upsilon,\
                                            doOS=doOS_Jpsi,\
                                            doPhoton=doPhoton_Jpsi,\
                                            photonPt=photonPtCut_Jpsi,\
                                            photonEta=photonEtaCut_Jpsi,\
                                            checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_Upsilon_2MuidMuonFilter.name())
msg.info("Added 2mu (muid) Upsilon filter")

theJob += H4lDPDMaker.H4lJpsiMuonFilter("HSG2_Upsilon_2MuonsFilter",nMu=2,\
                                            pTCuts=muonPtCuts_Jpsi,\
                                            collectionName="Muons",\
                                            qualityCuts=muonQualityCuts_Jpsi,\
                                            massUp=massCut_Up_Upsilon,\
                                            massDo=massCut_Do_Upsilon,\
                                            doOS=doOS_Jpsi,\
                                            doPhoton=doPhoton_Jpsi,\
                                            photonPt=photonPtCut_Jpsi,\
                                            photonEta=photonEtaCut_Jpsi,\
                                            checkLArError=checkLArError)
mu_algsList.append(theJob.HSG2_Upsilon_2MuonsFilter.name())
msg.info("Added 2mu (unified) Upsilon filter")


combination_Jpsi="(("
for iAlg in range(len(mu_algsList)):
    if iAlg==0:
        combination_Jpsi+=mu_algsList[iAlg]
    else:
        combination_Jpsi+=" or "+mu_algsList[iAlg]
combination_Jpsi+=") and ("

for iTrig in range(len(mu_trigsList)):
    if iTrig==0:
        combination_Jpsi+=mu_trigsList[iTrig]
    else:
        combination_Jpsi+=" or "+mu_trigsList[iTrig]
combination_Jpsi+="))"

algsList = mu_algsList + mu_trigsList

totalDecision_Jpsi=LogicalFilterCombiner("HSG2_JpsiFilter",cmdstring = combination_Jpsi)
theJob+=totalDecision_Jpsi

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutHelper

# define streams
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D2PDFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Construct the stream and file names for the jp:
streamName = prodFlags.WriteDAOD_JPSIHSG2Stream.StreamName
fileName   = buildFileName( prodFlags.WriteDAOD_JPSIHSG2Stream )


streamOut_daod_Jpsi = MSMgr.NewPoolStream(streamName,fileName)
streamOut_daod_Jpsi.AcceptAlgs([totalDecision_Jpsi.getName()])
streamOut_daod_Jpsi.AddOtherAlgsToBookkeep(algsList)

dpdOutHelper.addAllItemsFromInputExceptExcludeList(streamName,[])

streamOut_daod_Jpsi.AddMetaDataItem("EventBookkeeperCollection#*")

# needed for e Bremm
streamOut_daod_Jpsi.AddItem('ElectronContainer#GSFElectronAODCollection')
streamOut_daod_Jpsi.AddItem('egDetailContainer#GSFegDetailAOD')
streamOut_daod_Jpsi.AddItem('Rec::TrackParticleContainer#GSFTrackParticleCandidate')
streamOut_daod_Jpsi.AddItem('TrackCollection#GSFTracks')
streamOut_daod_Jpsi.AddItem('CaloClusterContainer#GSFElectronsClusters')
streamOut_daod_Jpsi.AddItem('CaloCellLinkContainer#GSFElectronsClusters_Link')
streamOut_daod_Jpsi.AddItem('INav4MomAssocs#GSFElectronsAssociation')
streamOut_daod_Jpsi.AddItem('TrackParticleAssocs#GSFTrackAssociation')
streamOut_daod_Jpsi.AddItem('INav4MomAssocs#GSFElectronPhotonAssociation')
streamOut_daod_Jpsi.AddItem('TrackParticleTruthCollection#GSFTrackParticleTruthCollection') 
