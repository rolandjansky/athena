/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @date      $Date: 2016-02-10 18:13:46 +0100 (Wed, 10 Feb 2016) $
 * @version   $Revision: 723173 $
 */

/** @todo */
/** @warnings */

/*--------------------------------------------------------------------*
 * headers
 *--------------------------------------------------------------------*/
#include "EventKernel/INavigable4Momentum.h"
#include "Particle/TrackParticle.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrigT1TGCRecRoiSvc/TGCRecRoiSvc.h"
#include "TrigT1RPCRecRoiSvc/RPCRecRoiSvc.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysisUtils/TIDARoiDescriptorBuilder.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"
#include "TrigParticle/TrigEFBphys.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

//#include "AnalysisTools/IAnalysisTools.h"
#include "TLorentzVector.h"

#include <limits>
#include <cmath>

#include "src/TrigEffJpsiTools.h"

/*--------------------------------------------------------------------*
 * constructor & destructor
 *--------------------------------------------------------------------*/
TrigEffJpsiTools::TrigEffJpsiTools(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_rpcRoiService("LVL1RPC::RPCRecRoiSvc", name),
    m_tgcRoiService("LVL1TGC::TGCRecRoiSvc", name),
    m_muonIdHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_Lvl1RoiName("LVL1_ROI"), // container name for level1 RoI
    m_endcapPivotPlaneZ(15525.), // z position of pivot plane in endcap region
    m_endcapPivotPlaneMinimumRadius(0.), // minimum radius of pivot plane in endcap region
    m_endcapPivotPlaneMaximumRadius(11977.), // maximum radius of pivot plane in endcap region
    m_barrelPivotPlaneRadius(7500.), // radius of pivot plane in barrel region
    m_barrelPivotPlaneHalfLength(9500.), // half length of pivot plane in barrel region
    m_invariantMassUpperLimit(3300.), // upper limit on invariant mass of di-muon system
    m_invariantMassLowerLimit(2880.), // lower limit on invariant mass of di-muon system
    m_trackPtCut(0.), // minimum pt of track
    m_trackEtaCut(5.), // maximum eta of track
    m_roiSizeBarrel(0.071), // half-diagonal lengh of barrel RoI in eta-phi plane
    m_roiSizeEndcap(0.019), // half-diagonal lengh of endcap RoI in eta-phi plane
    m_roiSizeForward(0.034), // half-diagonal lengh of forward RoI in eta-phi plane
    m_endcapForwardBoundary(1.95), // boundary of forward and endcap chambers in eta
    m_searchRangeTolerance(3.), // coefficient for extrapolation error to calculate search range
    m_searchRangeOffset(0.), // offset to calculate search range
    m_appendL1IfExists(true),
    m_appendL2IfExists(true),
    m_appendEFIfExists(true),
    m_useManualHypoCuts(false),
    p_rpcCablingSvc(0),
    m_cacheEndcap(),
    m_cacheBarrel(),
    m_MUON_MASS(105.658367) // MeV/c  ref - PL B667, 1 (2008) (pdg.lbl.gov)
{
  declareInterface<ITrigEffJpsiTools>(this);

  declareProperty("RPCRecRoiSvc", m_rpcRoiService);
  declareProperty("TGCRecRoiSvc", m_tgcRoiService);
  declareProperty("MuonIdHelperTool", m_muonIdHelper);
  declareProperty("Extrapolator", m_extrapolator);
  declareProperty("TrigDecisionTool", m_trigDecisionTool);

  declareProperty("Lvl1_RoiName", m_Lvl1RoiName);
  declareProperty("EndcapPivotPlaneZ", m_endcapPivotPlaneZ);
  declareProperty("EndcapPivotPlaneMinimumRadius", m_endcapPivotPlaneMinimumRadius);
  declareProperty("EndcapPivotPlaneMaximumRadius", m_endcapPivotPlaneMaximumRadius);
  declareProperty("BarrelPivotPlaneRadius", m_barrelPivotPlaneRadius);
  declareProperty("BarrelPivotPlaneHalfLength", m_barrelPivotPlaneHalfLength);
  declareProperty("InvariantMassUpperLimit", m_invariantMassUpperLimit);
  declareProperty("InvariantMassLowerLimit", m_invariantMassLowerLimit);
  declareProperty("MuonPtCut", m_trackPtCut);
  declareProperty("MuonEtaCut", m_trackEtaCut);
  declareProperty("roiSizeBarrel", m_roiSizeBarrel);
  declareProperty("roiSizeEndcap", m_roiSizeEndcap);
  declareProperty("roiSizeForward", m_roiSizeForward);
  declareProperty("endcapForwardBoundary", m_endcapForwardBoundary);
  declareProperty("searchRangeTolerance", m_searchRangeTolerance);
  declareProperty("searchRangeOffset", m_searchRangeOffset);
  declareProperty("appendL1IfExists", m_appendL1IfExists);
  declareProperty("appendL2IfExists", m_appendL2IfExists);
  declareProperty("appendEFIfExists", m_appendEFIfExists);
  declareProperty("useManualHypoCuts", m_useManualHypoCuts);

}


TrigEffJpsiTools::~TrigEffJpsiTools()
{
}



/*--------------------------------------------------------------------*
 * public methods
 *--------------------------------------------------------------------*/
StatusCode
TrigEffJpsiTools::initialize()
{
  ATH_MSG_DEBUG("initialize()");

  if (AthAlgTool::initialize().isFailure()) return StatusCode::FAILURE;


  // extrapolator
  if (m_extrapolator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_extrapolator);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_extrapolator);


  // trigger decision tool
  if (m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);


  // muon id helper tool
  if (m_muonIdHelper.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_muonIdHelper);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_muonIdHelper);


  // muon id helpers
  const MuonGM::MuonDetectorManager* muonDetectorManager;
  if (detStore()->retrieve(muonDetectorManager).isFailure() or
      !muonDetectorManager) {
    ATH_MSG_FATAL("Failed to retrieve MuonDetectorManager");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Muon id helper ready");


  // rpc cabling server service
  const IRPCcablingServerSvc* rpcCablingServer = 0;
  if (service("RPCcablingServerSvc", rpcCablingServer, 1).isFailure() or
      !rpcCablingServer) {
    ATH_MSG_FATAL("Failed to retrieve the RPCcablingServerSvc");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("RPCCablingServerSvc ready");
 

  // rpc cabling service
  if (rpcCablingServer->giveCabling(p_rpcCablingSvc).isFailure() or
      !p_rpcCablingSvc) {
    ATH_MSG_FATAL("Failed to get the IRPCcablingSvc");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("IRPCcablingSvc ready");


  // rpc rec roi service
  if (m_rpcRoiService.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve " << m_rpcRoiService);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved service: " << m_rpcRoiService);


  // tgc rec roi service
  if (m_tgcRoiService.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve " << m_tgcRoiService);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved service: " << m_tgcRoiService);

  m_cacheEndcap.evtNo = 0;
  m_cacheEndcap.runNo = 0;
  m_cacheEndcap.tp.clear();

  m_cacheBarrel.evtNo = 0;
  m_cacheBarrel.runNo = 0;
  m_cacheBarrel.tp.clear();

  Btrig_L2_initCuts();
  Btrig_EF_initCuts();


  ATH_MSG_DEBUG("initialize() successful");

  return StatusCode::SUCCESS;
}



StatusCode
TrigEffJpsiTools::finalize()
{
  ATH_MSG_DEBUG("finalize()");
  if (AthAlgTool::finalize().isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}



bool
TrigEffJpsiTools::isTriggeredMuonEF(const Rec::TrackParticle* track,
                                    const std::string& chainName) const
{
  if (not isConfigured(chainName, "EF")) return false;

  const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup(chainName);
  if (not (m_trigDecisionTool->isPassedBits(cg) &
           TrigDefs::EF_passedRaw)) {
    ATH_MSG_DEBUG("isTriggeredMuonEF:EF_passedRaw: no " << chainName);
    return false;
  } 
  ATH_MSG_DEBUG("isTriggeredMuonEF:EF_passedRaw: yes " << chainName);

  return isTriggeredMuonHLT(track, chainName, cg);
}


bool
TrigEffJpsiTools::isTriggeredMuonL2(const Rec::TrackParticle* track,
                                    const std::string& chainName) const
{
  if (not isConfigured(chainName, "L2")) return false;

  const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup(chainName);
  if (not (m_trigDecisionTool->isPassedBits(cg) &
           TrigDefs::L2_passedRaw)) {
    ATH_MSG_DEBUG("isTriggeredMuonEF:L2_passedRaw: no " << chainName);
    return false;
  } 
  ATH_MSG_DEBUG("isTriggeredMuonEF:L2_passedRaw: yes " << chainName);

  return isTriggeredMuonHLT(track, chainName, cg);
}



bool
TrigEffJpsiTools::isTriggeredMuonL1(const Rec::TrackParticle* track,
                                    const std::string& chainName) const
{
  if (not isConfigured(chainName, "L1")) return false;

  const int ptRequired = getL1PtThreshold(chainName);
  if (ptRequired < 0) {
    ATH_MSG_WARNING("isTriggeredMuonL1: problem on chainName '" << chainName << "'");
    return false;
  }

  const Trig::ChainGroup *cg = m_trigDecisionTool->getChainGroup("L1_MU[0-9]+");
  const std::vector<std::string> l1muonName = cg->getListOfTriggers();
  bool passed = false;
  for (size_t ii = 0; ii < l1muonName.size(); ii++) {
    int pt = getL1PtThreshold(l1muonName.at(ii));
    if (pt < ptRequired) continue;
    if (m_trigDecisionTool->isPassedBits(l1muonName.at(ii)) &
        TrigDefs::L1_isPassedBeforePrescale) {
      ATH_MSG_DEBUG("isTriggeredMuonL1:L1_isPassedBeforePrescale: yes " << l1muonName.at(ii));
      passed = true;
      break;
    } else {
      ATH_MSG_DEBUG("isTriggeredMuonL1:L1_isPassedBeforePrescale: no " << l1muonName.at(ii));
    }
  }

  if (not passed) return false;


  struct Match match;
  if (not getMatchedRoI(track, match)) {
    ATH_MSG_DEBUG("isTriggeredMuonL1:getMatchedRoI: no " << chainName);
    return false;
  }

  bool rc = (match.roiThrValue >= ptRequired);
  ATH_MSG_DEBUG("Pt passed: " << match.roiThrValue <<
                " >= Pt required: " << ptRequired <<
                " = " << rc);
  return rc;
}



bool
TrigEffJpsiTools::isTriggeredMuon(const Rec::TrackParticle* track,
                                  int thresholdNumber) const
{
  ATH_MSG_DEBUG("isTriggeredMuon()");

  if (!track) {
    ATH_MSG_WARNING("isTriggeredMuon: track pointer is null");
    return false;
  }

  if ((thresholdNumber < 1) or (thresholdNumber > 6)) {
    ATH_MSG_WARNING("isTriggeredMuon: logical threshold number out of range: " <<
                    thresholdNumber);
    return false;
  }

  const LVL1_ROI* lvl1Roi = 0;
  if (evtStore()->retrieve(lvl1Roi, m_Lvl1RoiName).isFailure() or !lvl1Roi) {
    ATH_MSG_WARNING("Failed to retrieve: " << m_Lvl1RoiName);
    return false;
  }

  LVL1_ROI::muons_type muonRoIs = lvl1Roi->getMuonROIs();

  bool rc = isTriggeredOnEndcap(track, muonRoIs, thresholdNumber) or
    isTriggeredOnBarrel(track, muonRoIs, thresholdNumber);
  return rc;
}



bool
TrigEffJpsiTools::isJPsiCandidate(const Rec::TrackParticle* track1,
                                  const Rec::TrackParticle* track2) const
{
  ATH_MSG_DEBUG("isJPsiCandidate()");

  if (!track1 or !track2) {
    ATH_MSG_WARNING("isTriggeredMuon: track pointer is null");
    return false;
  }

  if (track1->charge()*track2->charge() > 0.) {
    ATH_MSG_DEBUG("reject: same charge combination");
    return false;
  }
  ATH_MSG_DEBUG("accept: opposite charge combination");

  if (!isSeparated(track1, track2)) {
    ATH_MSG_DEBUG("reject: two tracks too close");
    return false;
  }
  ATH_MSG_DEBUG("accept: tracks well separated");

  TLorentzVector tvec1, tvec2;
  tvec1.SetPtEtaPhiM(track1->pt(), track1->eta(), track1->phi(), m_MUON_MASS);
  tvec2.SetPtEtaPhiM(track2->pt(), track2->eta(), track2->phi(), m_MUON_MASS);
  double invariantMass = (tvec1 + tvec2).M();

  if ((invariantMass < m_invariantMassLowerLimit) or
      (invariantMass > m_invariantMassUpperLimit)) {
    ATH_MSG_DEBUG("reject: invariant mass: " << invariantMass);
    return false;
  }
  ATH_MSG_DEBUG("accept: invariant mass: " << invariantMass);

  return true;
}


bool
TrigEffJpsiTools::isJPsiCandidate(const INavigable4Momentum* mom1,
                                  const INavigable4Momentum* mom2) const
{
  ATH_MSG_DEBUG("isJPsiCandidate");

  if (!mom1 or !mom2) {
    ATH_MSG_WARNING("isTriggeredMuon: momentum pointer is null");
    return false;
  }

  // invariant mass calculation
  TLorentzVector tvec1, tvec2;
  tvec1.SetPtEtaPhiM(mom1->pt(), mom1->eta(), mom1->phi(), m_MUON_MASS);
  tvec2.SetPtEtaPhiM(mom2->pt(), mom2->eta(), mom2->phi(), m_MUON_MASS);
  double invariantMass = (tvec1 + tvec2).M();

  if ((invariantMass < m_invariantMassLowerLimit) or
      (invariantMass > m_invariantMassUpperLimit)) {
    ATH_MSG_DEBUG("reject: invariant mass: " << invariantMass);
    return false;
  }
  ATH_MSG_DEBUG("accept: invariant mass: " << invariantMass);

  return true;
}



int
TrigEffJpsiTools::getMatchedRoI(const Rec::TrackParticle* track,
                                bool* barrel,
                                bool* endcap,
                                Amg::Vector2D* eta,
                                Amg::Vector2D* phi) const
{
  ATH_MSG_DEBUG("getMatchedRoI()");

  struct Match match;
  match.etaExTrk = eta;
  match.phiExTrk = phi;
  getMatchedRoI(track, match);
  if (barrel) *barrel = match.barrel;
  if (endcap) *endcap = match.endcap;
  return match.roiIndex; 
}



bool
TrigEffJpsiTools::extrapolateToRio(const Rec::TrackParticle* track,
                                   const Trk::RIO_OnTrack* rot,
                                   Amg::Vector2D* eta,
                                   Amg::Vector2D* phi,
                                   Amg::Vector2D* ptpz,
                                   bool getTsos,
                                   const std::vector<const Trk::TrackStateOnSurface*>* &tsos) const

{
  if (!track) {
    ATH_MSG_WARNING("extrapolateToRio: track pointer is null");
    return false;
  }

  if (!rot) {
    ATH_MSG_WARNING("extrapolateToRio: rot pointer is null");
    return false;
  }

  if (!eta) {
    ATH_MSG_WARNING("extrapolateToRio: eta pointer is null");
    return false;
  }

  if (!phi) {
    ATH_MSG_WARNING("extrapolateToRio: phi pointer is null");
    return false;
  }

  if (!ptpz) {
    ATH_MSG_WARNING("extrapolateToRio: ptpz pointer is null");
    return false;
  }

  const Identifier id = rot->identify();
  if (not m_muonIdHelper->isMuon(id)) return false;
  if (not m_muonIdHelper->isTrigger(id)) return false;

  const bool useCache = false;
  const Trk::TrackParameters* point = 0;
  Amg::Vector2D dEtaPhi;
  if (m_muonIdHelper->isTgc(id)) {
    const Trk::AtaDisc* p = extrapolateToEndcap(track, rot->globalPosition().z(), useCache);
    if (!p) return false;
    getExtrapolationErrorOnEndcap(p, &dEtaPhi);
    point = p;

  } else {
    double x = rot->globalPosition().x();
    double y = rot->globalPosition().y();
    double pivotRadius = sqrt(x*x + y*y);
    const Trk::AtaCylinder* p = extrapolateToBarrel(track, pivotRadius,
                                                            useCache);
    if (!p) return false;
    getExtrapolationErrorOnBarrel(p, &dEtaPhi);
    point = p;
  }
  (*eta)[Trk::locX] = point->position().eta();
  (*eta)[Trk::locY] = dEtaPhi[Trk::locX];
  (*phi)[Trk::locX] = point->position().phi();
  (*phi)[Trk::locY] = dEtaPhi[Trk::locY];

  (*ptpz)[Trk::locX] = point->momentum().perp();
  (*ptpz)[Trk::locY] = point->momentum().z();
  delete point;

  if (not getTsos) return true;

  if (m_muonIdHelper->isTgc(id)) {
    tsos = extrapolateToEndcapM(track, rot->globalPosition().z());
  } else {
    double x = rot->globalPosition().x();
    double y = rot->globalPosition().y();
    double pivotRadius = sqrt(x*x + y*y);
    tsos = extrapolateToBarrelM(track, pivotRadius);
  }

  return true;
}



bool
TrigEffJpsiTools::extrapolateToPoint(const Rec::TrackParticle* track,
                                     const Amg::Vector3D* pos,
                                     const int detector,
                                     Amg::Vector2D* eta,
                                     Amg::Vector2D* phi,
                                     Amg::Vector3D* mom) const

{
  if (!track) {
    ATH_MSG_WARNING("extrapolateToPoint: track pointer is null");
    return false;
  }

  if (!pos) {
    ATH_MSG_WARNING("extrapolateToPoint: pos pointer is null");
    return false;
  }

  if (!eta) {
    ATH_MSG_WARNING("extrapolateToPoint: eta pointer is null");
    return false;
  }

  if (!phi) {
    ATH_MSG_WARNING("extrapolateToPoint: phi pointer is null");
    return false;
  }

  if (!mom) {
    ATH_MSG_WARNING("extrapolateToPoint: mom pointer is null");
    return false;
  }


  const bool useCache = false;
  const Trk::TrackParameters* point = 0;
  Amg::Vector2D dEtaPhi;
  if (detector == ITrigEffJpsiTools::TGC) {
    const Trk::AtaDisc* p = extrapolateToEndcap(track, pos->z(), useCache);
    if (!p) return false;
    getExtrapolationErrorOnEndcap(p, &dEtaPhi);
    point = p;

  } else if (detector == ITrigEffJpsiTools::RPC) {
    double x = pos->x();
    double y = pos->y();
    double pivotRadius = sqrt(x*x + y*y);
    const Trk::AtaCylinder* p = extrapolateToBarrel(track, pivotRadius,
                                                            useCache);
    if (!p) return false;
    getExtrapolationErrorOnBarrel(p, &dEtaPhi);
    point = p;

  } else {
    ATH_MSG_WARNING("extrapolateToPoint: unknown target: " << detector);
    return false;
  }

  (*eta)[Trk::locX] = point->position().eta();
  (*eta)[Trk::locY] = dEtaPhi[Trk::locX];
  (*phi)[Trk::locX] = point->position().phi();
  (*phi)[Trk::locY] = dEtaPhi[Trk::locY];

  (*mom)[Trk::locX] = point->momentum().x();
  (*mom)[Trk::locY] = point->momentum().y();
  (*mom)[Trk::locZ] = point->momentum().z();

  delete point;

  return true;
}


// Accessing Trigger Elements using advanced expert features
// This functionality may disappear when TDT veriosn used is above 01-05-56
std::vector<HLT::TriggerElement*>
TrigEffJpsiTools::getExpertTEs(const std::string& chain) const
{
  Trig::ExpertMethods* expert = m_trigDecisionTool->ExperimentalAndExpertMethods();
  expert->enable();
  std::vector<HLT::TriggerElement*> tes;
  expert->getNavigation()->getAllOfType(chain, tes, true); ///EF_mu4 is the TE name, see NavigatioNcore or lxr for docu on it

  return tes;
}


bool
TrigEffJpsiTools::getTriggerObjects(const std::string& chain,
                                    TrigObjectMap* muonsEF,
                                    TrigObjectMap* muonsL2,
                                    TrigObjectMap* muonsL1) const
{
  std::string lvl = chain.substr(0,2);
  if (lvl == "EF") return getEFTriggerObjects( chain, muonsEF, muonsL2, muonsL1);
  if (lvl == "L2") return getL2TriggerObjects( chain, muonsL2, muonsL1);
  if (lvl == "L1") return getL1TriggerObjects( chain, muonsL1);
  return false;
}



bool
TrigEffJpsiTools::getL1TriggerObjects(const std::string& chain,
                                      TrigObjectMap* muonsL1) const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::getL1TriggerObjects() : " << chain);

  const LVL1_ROI* lvl1Roi = 0;
  if (evtStore()->retrieve(lvl1Roi, chain).isFailure() or !lvl1Roi) {
    ATH_MSG_WARNING("Failed to retrieve: " << chain);
    return false;
  }

  std::string keyPref = "L1_";

  std::vector<Muon_ROI>::const_iterator it;
  short iMuon=0;
  for (it = lvl1Roi->getMuonROIs().begin(); it != lvl1Roi->getMuonROIs().end(); ++it, ++iMuon) {
    ATH_MSG_DEBUG("L1 object # " << iMuon
                  << "  &: " << &(*it)
                  << "  pt: " << it->pt()
                  << "  eta: " << it->eta()
                  << "  phi: " << it->phi()
                  << "  word: " << it->getROIWord()
                  << "  Source: " << it->getSource()
                  << "  Hemisphere: " << it->getHemisphere() );

    if ( muonsL1 != NULL) {
      TrigObject trigObjectL1;
      int charge = -99;
      switch (it->getCharge()) {
      case Muon_ROI::Pos:
	charge = 1;
	break;
      case Muon_ROI::Neg:
	charge = -1;
	break;
      default:
	charge = -99;
      }
      trigObjectL1.names.push_back( keyPref + it->getThrName() );
      trigObjectL1.eta = it->eta();
      trigObjectL1.phi = it->phi();
      trigObjectL1.pt  = it->pt();
      trigObjectL1.q   = charge;
      trigObjectL1.id  = it->getROIWord();
      trigObjectL1.parentId = -1;
      trigObjectL1.grandparentId = -1;
      //trigObjectL1.daughterIds.push_back(-1);
      trigObjectL1.combIds.push_back(iMuon);
      trigObjectL1.efObjIds.push_back(-1);
      trigObjectL1.type = SPECTROMETER;
      addTrigObject(trigObjectL1, muonsL1, m_appendL1IfExists);
    }
  }

  //   Trig::FeatureContainer fc = m_trigDecisionTool->features(chain);
  //   ATH_MSG_INFO("Features: # " << fc.get<TrigRoiDescriptor>().size() );
  //   const std::vector<Trig::Combination>& combs = fc.getCombinations();
  //   std::vector<Trig::Combination>::const_iterator combIt;
  //   int iComb = 1;
  //   for ( combIt = combs.begin(); combIt != combs.end(); ++combIt, ++iComb ) {
  //     std::vector<Trig::Feature<TrigRoiDescriptor> > muons = fc.get<TrigRoiDescriptor>();
  //     std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator mit;
  //     int iObj = 0;
  //     for ( mit = muons.begin(); mit != muons.end(); ++mit, ++iObj) {
  //       const TrigRoiDescriptor* mu = mit->cptr();
  //       ATH_MSG_INFO("L1 combination # " << iComb << " object # " << iObj
  //                    << "  &: " << mu
  //                    //                   << "  pt: " << mu
  //                    << "  eta: " << mu->eta() << ", " << mu->eta0()
  //                    << "  phi: " << mu->phi() << ", " << mu->phi0()
  //                    << "  word: " << mu->roiWord() );
  //     }
  //   }

  return true;
}


bool
TrigEffJpsiTools::getL2TriggerObjects(const std::string& chain,
                                      TrigObjectMap* muonsL2,
                                      TrigObjectMap* muonsL1) const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::getL2TriggerObjects() : " << chain);

  //const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup(chain);
  // Getting the chain's features:
  const Trig::FeatureContainer fc = m_trigDecisionTool->features(chain);

  std::vector<Trig::Combination>::const_iterator it;
  short iComb = 1;
  short iMuon = 0;
  for ( it = fc.getCombinations().begin(); it != fc.getCombinations().end(); ++it, ++iComb ) { // Loop over combinations
    // TOOD: It is possible for L2 objects which did not pass L2 hypo (not active) to still go into the ntuple
    // via getEFTriggerObjects EF parents. Find solution
    const Trig::Combination& combination = *it;
    if (not combination.active()) {
      ATH_MSG_DEBUG("getEFTriggerObjects: combination " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("getEFTriggerObjects: combination " << combination << " is active");

    std::vector<Trig::Feature<CombinedMuonFeature> > muons = combination.get<CombinedMuonFeature>(); // TODO: Use MuonFeature as well to retrieve non-combined
    std::vector<Trig::Feature<CombinedMuonFeature> >::const_iterator mit;
    int iObj = 0;
    for ( mit = muons.begin(); mit != muons.end(); ++mit, ++iObj) {
      const CombinedMuonFeature* muonL2 = mit->cptr();
      Trig::Feature<Muon_ROI> muonL1Feature = m_trigDecisionTool->ancestor<Muon_ROI>(*mit);
      const Muon_ROI* muonL1 = muonL1Feature.cptr();
      if ( muonL2 != NULL ) {
        ATH_MSG_DEBUG("L2 combination # " << iComb << " object # " << iObj
                      << "  &:"    << (int64_t) muonL2
                      << "  TE id: " << mit->te()->getId()
                      << "  pt: "  << muonL2->pt()
                      << "  eta: " << muonL2->eta()
                      << "  phi: " << muonL2->phi());

        if ( muonsL2 != NULL) {
          TrigObject trigObjectL2;
          trigObjectL2.names.push_back(chain);
          trigObjectL2.eta = muonL2->eta();
          trigObjectL2.phi = muonL2->phi();
          trigObjectL2.pt  = muonL2->pt();
          trigObjectL2.q   = muonL2->pt() > 0.0 ? 1 : -1;
          trigObjectL2.id  = (int64_t) muonL2;
          trigObjectL2.parentId = (muonL1 != NULL) ? muonL1->getROIWord() : -1;
          trigObjectL2.grandparentId = -1;
          //trigObjectL2.daughterIds.push_back(-1);
          trigObjectL2.combIds.push_back(iComb);
          trigObjectL2.efObjIds.push_back(-1);
          trigObjectL2.type = COMBINED; // TODO: Add non-combined objects too
          addTrigObject(trigObjectL2, muonsL2, m_appendL2IfExists);
        }
      } else ATH_MSG_WARNING("getL2TriggerObjects(): muonL2 is NULL");

      if ( muonL1 != NULL ) {
        ATH_MSG_DEBUG("     parent:  &:" << (int64_t) muonL1
                      << "  pt: "   << muonL1->pt()
                      << "  eta: "  << muonL1->eta()
                      << "  phi: "  << muonL1->phi()
                      << "  word: " << muonL1->getROIWord() );
        if ( muonsL1 != NULL) {
          TrigObject trigObjectL1;
	  int charge = -99;
	  switch (muonL1->getCharge()) {
	  case Muon_ROI::Pos: charge = 1;  break;
	  case Muon_ROI::Neg: charge = -1; break;
	  default:
	    charge = -99;
	    ATH_MSG_DEBUG("muonL1 charge not defined " << muonL1->getCharge());
	  }
          trigObjectL1.names.push_back(chain);
          trigObjectL1.eta = muonL1->eta();
          trigObjectL1.phi = muonL1->phi();
          trigObjectL1.pt  = muonL1->pt();
          trigObjectL1.q   = charge;
          trigObjectL1.id  = muonL1->getROIWord();
          trigObjectL1.parentId = -1;
          trigObjectL1.grandparentId = -1;
          //trigObjectL1.daughterIds.push_back((int64_t) muonL2);
          trigObjectL1.combIds.push_back(iComb);
          trigObjectL1.efObjIds.push_back(-1);
          trigObjectL1.type = SPECTROMETER;
          addTrigObject(trigObjectL1, muonsL1, m_appendL1IfExists);
        }
      } else ATH_MSG_WARNING("getL2TriggerObjects(): muonL1 is NULL");
      ++iMuon;
    }
  } // End loop over combinations

  return true;
}


bool
TrigEffJpsiTools::getEFTriggerObjects(const std::string& chain,
                                      TrigObjectMap* muonsEF,
                                      TrigObjectMap* muonsL2,
                                      TrigObjectMap* muonsL1) const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::getEFTriggerObjects() : " << chain);

  //const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup(chain);
  // Getting the chain's features:
  const Trig::FeatureContainer fc = m_trigDecisionTool->features(chain);

  std::vector<Trig::Combination>::const_iterator it;
  short iComb = 1;
  for ( it = fc.getCombinations().begin(); it != fc.getCombinations().end(); ++it, ++iComb ) { // Loop over combinations
    const Trig::Combination& combination = *it;
    if (not combination.active()) {
      ATH_MSG_DEBUG("getEFTriggerObjects: combination " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("getEFTriggerObjects: combination " << combination << " is active");

    std::vector<Trig::Feature<TrigMuonEFInfo> > muons = combination.get<TrigMuonEFInfo>();
    std::vector<Trig::Feature<TrigMuonEFInfo> >::iterator mit;
    short iObj = 0;
    for ( mit = muons.begin(); mit != muons.end(); ++mit, ++iObj) { // Loop over objects
      const TrigMuonEFInfo* muonEF = mit->cptr();
      Trig::Feature<CombinedMuonFeature>  muonL2Feature       = m_trigDecisionTool->ancestor<CombinedMuonFeature>(*mit);
      Trig::Feature<Muon_ROI>             muonL1Feature       = m_trigDecisionTool->ancestor<Muon_ROI>(*mit);
      const CombinedMuonFeature*          muonL2              = muonL2Feature.cptr();
      const Muon_ROI*                     muonL1              = muonL1Feature.cptr();
      const TrigMuonEFInfoTrackContainer* muonEFInfoTrackCont = muonEF->TrackContainer();
      TrigMuonEFInfoTrackContainer::const_iterator trit;

      int iTrack    = 0;
      int iCbTrack = 0;
      for ( trit = muonEFInfoTrackCont->begin(); trit != muonEFInfoTrackCont->end(); ++trit, ++iTrack ) {
        const TrigMuonEFTrack* muonEFTrack = NULL;
        int                    trackType   = UNDEFINED;
        if      ( (*trit)->hasCombinedTrack() )     { trackType = COMBINED;     muonEFTrack = (*trit)->CombinedTrack(); }
        else if ( (*trit)->hasSpectrometerTrack() ) { trackType = SPECTROMETER; muonEFTrack = (*trit)->SpectrometerTrack(); }
        else if ( (*trit)->hasExtrapolatedTrack() ) { trackType = EXTRAPOLATED; muonEFTrack = (*trit)->ExtrapolatedTrack(); }

        if ( muonEFTrack != NULL ) {
	  ATH_MSG_DEBUG("EF combination # " << iComb << " object # " << iObj << " CbTrack #" << iCbTrack
			<< "  CbTrack&: " << (uint64_t) muonEFTrack
			<< "  TrigMuonEFInfo&: "   << (int64_t) muonEF
			<< "  pt: "  << muonEFTrack->pt()
			<< "  eta: " << muonEFTrack->eta()
			<< "  phi: " << muonEFTrack->phi()
			<< "  type: " << trackType );

	  TrigObject trigObjectEF;
	  trigObjectEF.names.push_back(chain);
	  trigObjectEF.eta = muonEFTrack->eta();
	  trigObjectEF.phi = muonEFTrack->phi();
	  trigObjectEF.pt  = muonEFTrack->pt();
	  trigObjectEF.q   = muonEFTrack->Charge();
	  trigObjectEF.id  = (int64_t) muonEFTrack;
	  trigObjectEF.parentId = (int64_t) muonL2;
	  trigObjectEF.grandparentId =  (muonL1 != NULL) ? muonL1->getROIWord() : -1;
	  //trigObjectEF.daughterIds.push_back(-1);
	  trigObjectEF.combIds.push_back(iComb);
	  trigObjectEF.efObjIds.push_back(iTrack);
	  trigObjectEF.type = trackType;
	  addTrigObject(trigObjectEF, muonsEF, m_appendEFIfExists);
	  ++iCbTrack; // TODO: Find a more proper name
        }
      } // End if muonEFTrack

      if ( muonEFInfoTrackCont->size() > 0 && iCbTrack == 0 )
        ATH_MSG_WARNING("getEFTriggerObjects(): Could not find a combined track");

      if ( muonL2 != NULL ) {
        ATH_MSG_DEBUG("   parent:  &: " << (int64_t) muonL2
                      << "  pt: "  << muonL2->pt()
                      << "  eta: " << muonL2->eta()
                      << "  phi: " << muonL2->phi() );

        if ( muonsL2 != NULL) {
          TrigObject trigObjectL2;
          trigObjectL2.names.push_back(chain);
          trigObjectL2.eta = muonL2->eta();
          trigObjectL2.phi = muonL2->phi();
          trigObjectL2.pt  = muonL2->pt();
          trigObjectL2.q   = muonL2->pt() > 0.0 ? 1 : -1;
          trigObjectL2.id  = (int64_t) muonL2;
          trigObjectL2.parentId = (muonL1 != NULL) ? muonL1->getROIWord() : -1;
          trigObjectL2.grandparentId = -1;
          //trigObjectL2.daughterIds.push_back((int64_t) muonEF);
          trigObjectL2.combIds.push_back(iComb);
          trigObjectL2.efObjIds.push_back(-1);
          trigObjectL2.type = COMBINED;
          addTrigObject(trigObjectL2, muonsL2, m_appendL2IfExists);
        }
      } else ATH_MSG_WARNING("getEFTriggerObjects(): muonL2 is NULL");

      if ( muonL1 != NULL ) {
        ATH_MSG_DEBUG("   grandparent:  &: " << (int64_t) muonL1
                      << "  pt: "   << muonL1->pt()
                      << "  eta: "  << muonL1->eta()
                      << "  word: " << muonL1->getROIWord()
                      << "  phi: "  << muonL1->phi() );
        if ( muonsL1 != NULL) {
          TrigObject trigObjectL1;
	  int charge = -99;
	  switch (muonL1->getCharge()) {
	  case Muon_ROI::Pos: charge = 1;  break;
	  case Muon_ROI::Neg: charge = -1; break;
	  default:
	    charge = -99;
	    ATH_MSG_WARNING("muonL1 charge not defined " << muonL1->getCharge());
	  }
          trigObjectL1.names.push_back(chain);
          trigObjectL1.eta = muonL1->eta();
          trigObjectL1.phi = muonL1->phi();
          trigObjectL1.pt  = muonL1->pt();
          trigObjectL1.q   = charge;
          trigObjectL1.id  = muonL1->getROIWord();
          trigObjectL1.parentId = -1;
          trigObjectL1.grandparentId = -1;
          //trigObjectL1.daughterIds.push_back((int64_t) muonL2);
          trigObjectL1.combIds.push_back(iComb);
          trigObjectL1.efObjIds.push_back(-1);
          trigObjectL1.type = SPECTROMETER;
          addTrigObject(trigObjectL1, muonsL1, m_appendL1IfExists);
        }
      } else ATH_MSG_WARNING("getEFTriggerObjects(): muonL1 is NULL");
    }
  } // End loop over combinations

  return true;
}

bool
TrigEffJpsiTools::getEFAllTriggerObjects(const std::string& chain,
					 TrigObjectMap* muonsEF,
					 TrigObjectMap* /*muonsL2*/,
					 TrigObjectMap* /*muonsL1*/) const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::getEFAllTriggerObjects() : " << chain);

  const Trig::ChainGroup* cg = m_trigDecisionTool->getChainGroup(chain);
  ATH_MSG_INFO( "The chain " << chain << "  is passing" << cg->isPassed() );

  std::vector<HLT::TriggerElement*> tes = getExpertTEs(chain);

  // now get features

  BOOST_FOREACH( const HLT::TriggerElement* te, tes ) {
    Trig::Feature<TrigMuonEFInfoContainer> FeatureCont = m_trigDecisionTool->ancestor<TrigMuonEFInfoContainer>(te);
    const TrigMuonEFInfoContainer* TmuonsCont = FeatureCont.cptr();
    ATH_MSG_INFO("Object container found !: " << FeatureCont.cptr() << " size " << TmuonsCont->size() );

    TrigMuonEFInfoContainer::const_iterator MuonItr   = TmuonsCont->begin();
    TrigMuonEFInfoContainer::const_iterator MuonItrE  = TmuonsCont->end();

    for (int imu=0; MuonItr != MuonItrE; ++MuonItr, ++imu ) {
      const TrigMuonEFInfo* muonInfo = (*MuonItr);
      const TrigMuonEFInfoTrackContainer* muonEFInfoTrackCont = muonInfo->TrackContainer();
      int iTrack    = 0;
      TrigMuonEFInfoTrackContainer::const_iterator trit;
      for ( trit = muonEFInfoTrackCont->begin(); trit != muonEFInfoTrackCont->end(); ++trit, ++iTrack ) {
        const TrigMuonEFTrack* muonEFTrack = NULL;
        int                    trackType   = UNDEFINED;
        if      ( (*trit)->hasCombinedTrack() )     { trackType = COMBINED;     muonEFTrack = (*trit)->CombinedTrack(); }
        else if ( (*trit)->hasSpectrometerTrack() ) { trackType = SPECTROMETER; muonEFTrack = (*trit)->SpectrometerTrack(); }
        else if ( (*trit)->hasExtrapolatedTrack() ) { trackType = EXTRAPOLATED; muonEFTrack = (*trit)->ExtrapolatedTrack(); }

        if ( muonEFTrack != NULL ) {
	  ATH_MSG_DEBUG("EF container, track " << imu << "  " << iTrack 
			<< "  CbTrack&: " << (uint64_t) muonEFTrack
			<< "  TrigMuonEFInfo&: "   << (int64_t) muonInfo
			<< "  pt: "  << muonEFTrack->pt()
			<< "  eta: " << muonEFTrack->eta()
			<< "  phi: " << muonEFTrack->phi()
			<< "  type: " << trackType
			<< "  combId: " << imu );

	  // store ntuple info
	  TrigObject trigObjectEF;
	  trigObjectEF.names.push_back("EF_mu4_All");
	  trigObjectEF.eta = muonEFTrack->eta();
	  trigObjectEF.phi = muonEFTrack->phi();
	  trigObjectEF.pt  = muonEFTrack->pt();
	  trigObjectEF.q   = muonEFTrack->Charge();
	  trigObjectEF.id  = (int64_t) muonEFTrack;
	  trigObjectEF.parentId = -1; // (int64_t) muonL2;
	  trigObjectEF.grandparentId =  -1; // (muonL1 != NULL) ? muonL1->getROIWord() : -1;
	  //trigObjectEF.daughterIds.push_back(-1);
	  trigObjectEF.combIds.push_back(-1);
	  trigObjectEF.efObjIds.push_back(iTrack);
	  trigObjectEF.type = trackType;
	  addTrigObject(trigObjectEF, muonsEF, m_appendEFIfExists);
	  //            ++iCbTrack; // TODO: Find a more proper name
        }
      } // End if muonEFTrack


    } // end loop over muon conatiner


  } // end loop over TEs


  return true;
}

bool
TrigEffJpsiTools::getEFIDTriggerObjects(const std::string& chain,
					TrigObjectMap* muonsEF,
					TrigObjectMap* /*muonsL2*/,
					TrigObjectMap* /*muonsL1*/) const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::getEFIDTriggerObjects() : " << chain);
  
  const Trig::FeatureContainer fc = m_trigDecisionTool->features(chain);

  ATH_MSG_DEBUG("getEFIDTriggerObjects: number of combination " << fc.getCombinations().size());

  std::vector<Trig::Combination>::const_iterator it;
  short iComb = 0;
  for ( it = fc.getCombinations().begin(); it != fc.getCombinations().end(); ++it, ++iComb ) { // Loop over combinations
    const Trig::Combination& combination = *it;
    if (not combination.active()) {
      ATH_MSG_DEBUG("getEFIDTriggerObjects: combination " << iComb << "  " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("getEFIDTriggerObjects: combination " << iComb << "  " <<  combination << " is active");

    std::vector<Trig::Feature<Rec::TrackParticleContainer> > tracks = combination.get<Rec::TrackParticleContainer>("InDetTrigParticleCreation_Muon_EFID");
    std::vector<Trig::Feature<Rec::TrackParticleContainer> >::iterator trit;

    short iTrk = 0;
    ATH_MSG_DEBUG("getEFIDTriggerObjects: number of track collections " << tracks.size());
    for ( trit = tracks.begin(); trit != tracks.end(); ++trit, ++iTrk) { // Loop over objects
      const Rec::TrackParticleContainer* trackEF = trit->cptr();

      ATH_MSG_DEBUG("getEFIDTriggerObjects: number of tracks " << trackEF->size());
      Rec::TrackParticleContainer::const_iterator trkIt    = trackEF->begin();
      Rec::TrackParticleContainer::const_iterator lastTrkIt = trackEF->end();

      for (int itrk=0 ; trkIt != lastTrkIt; itrk++, trkIt++) {
	const Trk::Perigee* perigee = (*trkIt)->measuredPerigee();

        double px = perigee->momentum()[Trk::px];
        double py = perigee->momentum()[Trk::py];
        double ptSquare=std::pow(px,2) + std::pow(py,2);
        double pT = sqrt(ptSquare) * perigee->parameters()[Trk::qOverP]/fabs(perigee->parameters()[Trk::qOverP]);

	ATH_MSG_DEBUG("getEFIDTriggerObjects: track " << itrk
		      << "  pt: "  << pT
		      << "  eta: " << perigee->eta()
		      << "  phi: " << perigee->parameters()[Trk::phi]
		      << "  type: " << IDONLY
		      << "  combId: " << iComb );

	// store ntuple info
	TrigObject trigObjectEF;
	trigObjectEF.names.push_back(chain);
	trigObjectEF.eta = perigee->eta();
	trigObjectEF.phi = perigee->parameters()[Trk::phi];
	trigObjectEF.pt  = pT;
	trigObjectEF.q   = perigee->parameters()[Trk::qOverP]/fabs(perigee->parameters()[Trk::qOverP]);
	trigObjectEF.id  = (int64_t) (*trkIt);
	trigObjectEF.parentId = -1;
	trigObjectEF.grandparentId = -1;
	// trigObjectEF.daughterIds.push_back(-1);
	trigObjectEF.combIds.push_back(iComb);
	trigObjectEF.efObjIds.push_back(iTrk);
	trigObjectEF.type = IDONLY;
	addTrigObject(trigObjectEF, muonsEF, m_appendEFIfExists);

      } // end loop over tracks
    } // end loop over track collections
  } // end loopp over combinations


  return true;
}

bool
TrigEffJpsiTools::getEFMSTriggerObjects(const std::string& chain,
					TrigObjectMap* muonsEF,
					TrigObjectMap* /*muonsL2*/,
					TrigObjectMap* /*muonsL1*/) const
{
  std::vector< const TrigMuonEFTrack* > muonExtrapolatedTracks;
  ATH_MSG_DEBUG("TrigEffJpsiTools::getEFMSTriggerObjects() : " << chain);
  const Trig::FeatureContainer fc = m_trigDecisionTool->features(chain);
  ATH_MSG_DEBUG("getEFMSTriggerObjects: number of combination " << fc.getCombinations().size());

  std::vector<Trig::Combination>::const_iterator it;
  short iComb = 0;
  for ( it = fc.getCombinations().begin(); it != fc.getCombinations().end(); ++it, ++iComb ) { // Loop over combinations
    const Trig::Combination& combination = *it;
    if (not combination.active()) {
      ATH_MSG_DEBUG("getEFMSTriggerObjects: combination " << iComb << "  " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("getEFMSTriggerObjects: combination " << iComb << "  " <<  combination << " is active");

    //from Peter    
    std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = combination.get<TrigRoiDescriptor>("initialRoI", TrigDefs::Physics); 
    std::vector< Trig::Feature< TrigMuonEFInfoContainer > > muons = combination.get< TrigMuonEFInfoContainer >("MuonEFInfo", TrigDefs::Physics);

    for( unsigned roiIt1 = 0; roiIt1 < muons.size(); roiIt1++ ){
      Trig::Feature< TrigMuonEFInfoContainer > trackFeature1 = muons.at(roiIt1);
      const TrigMuonEFInfoContainer* trigMuon1 = trackFeature1.cptr();
      TrigMuonEFInfoContainer::const_iterator muonItr1  = trigMuon1->begin();
      const TrigMuonEFInfo* muoninfo1 = (*muonItr1);

      if( !initRois.empty() ){
	const TrigRoiDescriptor* roid1 = initRois[roiIt1].cptr();
	TIDARoiDescriptor roiInfo1 = TIDARoiDescriptorBuilder(*roid1);
	if( muoninfo1->hasTrack() ){
	  const TrigMuonEFInfoTrackContainer* tc1 = muoninfo1->TrackContainer();
	  bool selected = false;
	  for( TrigMuonEFInfoTrackContainer::const_iterator trackItr = tc1->begin(); trackItr != tc1->end(); trackItr++){
	    const TrigMuonEFInfoTrack* muonInfo1 = (*trackItr);
	    if( muonInfo1->hasExtrapolatedTrack() ){
	      TrigMuonEFTrack* eTrack1 = muonInfo1->ExtrapolatedTrack();

	      //              std::cout << "RoI " << muoninfo1->RoINum() << " extrapolated track " << *eTrack1 << std::endl;
	      for( unsigned roiIt2 = 0; roiIt2 < muons.size(); roiIt2++ ){
		if( roiIt1 != roiIt2 ){
		  Trig::Feature< TrigMuonEFInfoContainer > trackFeature2 = muons.at(roiIt2);
		  const TrigMuonEFInfoContainer* trigMuon2 = trackFeature2.cptr();
		  TrigMuonEFInfoContainer::const_iterator muonItr2  = trigMuon2->begin();
		  const TrigMuonEFInfo* muoninfo2 = (*muonItr2);
		  if( !initRois.empty() ){
		    const TrigRoiDescriptor* roid2 = initRois[roiIt2].cptr();
		    TIDARoiDescriptor roiInfo2 = TIDARoiDescriptorBuilder(*roid2);

		    if( muoninfo2->hasTrack() ){
		      const TrigMuonEFInfoTrackContainer* tc2 = muoninfo2->TrackContainer();
		      for( TrigMuonEFInfoTrackContainer::const_iterator trackItr = tc2->begin(); trackItr != tc2->end(); trackItr++){
			const TrigMuonEFInfoTrack* muonInfo2 = (*trackItr);
			if( muonInfo2->hasExtrapolatedTrack() ){
			  TrigMuonEFTrack* eTrack2 = muonInfo2->ExtrapolatedTrack();
			  //      std::cout << "RoI " << muoninfo2->RoINum() << " extrapolated track " << *eTrack2 << std::endl;
			  double deta = getEtaDistance(eTrack1->eta(), eTrack2->eta());
			  double dphi = getPhiDistance(eTrack1->phi(), eTrack2->phi());

			  if( muoninfo1->RoINum() != muoninfo2->RoINum() && ((std::fabs(deta) < 0.08) && (std::fabs(dphi) < 0.08)) ){
			    double deltarT1Roi1 = getEtaPhiDistance(eTrack1->eta(), roiInfo1.eta(), eTrack1->phi(), roiInfo1.phi() );
			    double deltarT2Roi2 = getEtaPhiDistance(eTrack2->eta(), roiInfo2.eta(), eTrack2->phi(), roiInfo2.phi() );

			    if( deltarT1Roi1 > deltarT2Roi2 ){
			      eTrack1 = eTrack2;
			      //  std::cout << "Switched!" << std::endl;
			    }
			  }//check duplcates - pick 1 closest to centre of its RoI
			}
		      }
		    }
		  }
		}//roi1!=roi2
	      }//start 2nd RoI loop

	      for( std::vector<const TrigMuonEFTrack*>::const_iterator setIt = muonExtrapolatedTracks.begin(); setIt != muonExtrapolatedTracks.end(); ++setIt){
		double deta = getEtaDistance( (*setIt)->eta(), eTrack1->eta() );
		double dphi = getPhiDistance( (*setIt)->phi(), eTrack1->phi() );
		if( (std::fabs(deta) < 0.08) && (std::fabs(dphi) < 0.08) ){
		  selected = true;
		}
	      }
	      if( !selected ){
		muonExtrapolatedTracks.push_back(eTrack1);
		// std::cout << "\nExtrapolatedTrack1 inserted." << *eTrack1 << "\n" << std::endl;
	      }
	    }//extrapolated track?
	  }
	}
      }
    }
    // END PETER

    int  MSsize = muonExtrapolatedTracks.size();
    ATH_MSG_DEBUG("getEFMSTriggerObjects: number of MStracks " <<  MSsize );

    for (int iMS=0 ; iMS < MSsize; iMS++) {
      double px = (*muonExtrapolatedTracks.at(iMS)).px(); //perigee->momentum()[Trk::px];
      double py = (*muonExtrapolatedTracks.at(iMS)).py(); //perigee->momentum()[Trk::py];
      double ptSquare=std::pow(px,2) + std::pow(py,2);
      double charge = (*muonExtrapolatedTracks.at(iMS)).Charge();
      double pT = sqrt(ptSquare) * charge;
      double eta = (*muonExtrapolatedTracks.at(iMS)).eta();
      double phi = (*muonExtrapolatedTracks.at(iMS)).phi();

      ATH_MSG_DEBUG("getEFMSTriggerObjects: track " << iMS << " Pt eta phi  " << pT  << " / " << eta << " / " <<   phi );
      // store ntuple info
      TrigObject trigObjectEF;
      trigObjectEF.names.push_back(chain);
      trigObjectEF.eta = eta;
      trigObjectEF.phi = phi;
      trigObjectEF.pt  = pT;
      trigObjectEF.q   = charge;
      trigObjectEF.id  = (int64_t) (muonExtrapolatedTracks.at(iMS));
      trigObjectEF.parentId = -1;
      trigObjectEF.grandparentId = -1;
      //trigObjectEF.daughterIds.push_back(-1);
      trigObjectEF.combIds.push_back(iComb);
      trigObjectEF.efObjIds.push_back(iMS);
      trigObjectEF.type = EXTRAPOLATED; //2999;
      addTrigObject(trigObjectEF, muonsEF, m_appendEFIfExists);
    } // end loop over tracks
  }
  return true;
}


bool
TrigEffJpsiTools::getBPhysL2TriggerObjects(const std::string& chain,
                                           TrigObjectMap* muonsL2,
                                           TrigObjectMap* muonsL1) const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::getBPhysL2TriggerObjects() : " << chain);
  int nTrig=-1;
  bool muTrkJpsi = (chain.find("_mu") && chain.find("Trk")); // Checks if chain is a mu+Trk chain. Notice: Case sensitive!

  for (unsigned int iTrig=0; iTrig < m_Btrig_L2_chain.size(); iTrig++) {
    if (m_Btrig_L2_chain[iTrig] == chain) nTrig=iTrig;
  }
  if (m_useManualHypoCuts) {
    if (nTrig<0) {
      ATH_MSG_WARNING("getBPhysL2TriggerObjects: do not have HYPO cuts for chain " << chain);
      return true;
    }
    ATH_MSG_DEBUG("getBPhysL2TriggerObjects: cuts for chain " << chain);
    ATH_MSG_DEBUG("getBPhysL2TriggerObjects: mass : " << m_Btrig_L2_massMin[nTrig] << " - " << m_Btrig_L2_massMax[nTrig]);
    ATH_MSG_DEBUG("getBPhysL2TriggerObjects: applyMassMax " << m_Btrig_L2_applyMassMax[nTrig]);
    ATH_MSG_DEBUG("getBPhysL2TriggerObjects: checkOS " << m_Btrig_L2_checkOS[nTrig]);
    ATH_MSG_DEBUG("getBPhysL2TriggerObjects: nHits " << m_Btrig_L2_nHits[nTrig]);
    ATH_MSG_DEBUG("getBPhysL2TriggerObjects: chi2 " << m_Btrig_L2_chi2[nTrig]);
  }


  const Trig::FeatureContainer fc = m_trigDecisionTool->features(chain);
  const std::vector<Trig::Combination>& combinations = fc.getCombinations();
  ATH_MSG_DEBUG("getBPhysL2TriggerObjects: number of combinations: " << combinations.size());

  std::vector<Trig::Combination>::const_iterator cit;
  int iComb = 1;
  for (cit = combinations.begin(); cit != combinations.end(); ++cit, ++iComb) { // Loop over combinations
    const Trig::Combination& combination = *cit;
    if (not combination.active()) {
      ATH_MSG_DEBUG("getBPhysL2TriggerObjects: combination " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("getBPhysL2TriggerObjects: combination " << combination << " is active");


    const std::vector<const HLT::TriggerElement*> tes = combination.tes();
    std::vector<const HLT::TriggerElement*>::const_iterator tesIt;
    int iTrigElement = 0;
    ATH_MSG_DEBUG("Trigger elements: " << tes.size());
    for (tesIt = tes.begin(); tesIt != tes.end(); ++tesIt, ++iTrigElement) {
      ATH_MSG_DEBUG("Trigger element #" << iTrigElement << ": id = " << (*tesIt)->getId() << " activeState = " << (*tesIt)->getActiveState());
    }

    std::vector<Trig::Feature<TrigL2Bphys> > triggerFeatures = combination.get<TrigL2Bphys>();
    std::vector<Trig::Feature<TrigL2Bphys> >::const_iterator tfIt;
    int iObj = 0;
    ATH_MSG_DEBUG("Trigger features: " << triggerFeatures.size());
    for (tfIt = triggerFeatures.begin(); tfIt != triggerFeatures.end(); ++tfIt, ++iObj) { // Loop over trigger features
      if(tfIt->te()) ATH_MSG_DEBUG("Trigger feature #" << iObj << ": id = " << tfIt->te()->getId() << " activeState = " << tfIt->te()->getActiveState());

      Trig::Feature<TrigL2Bphys>  triggerFeature  = *tfIt;
      const TrigL2Bphys*          trigObj         = triggerFeature.cptr();
      Trig::Feature<Muon_ROI>     muonL1Feature   = m_trigDecisionTool->ancestor<Muon_ROI>(triggerFeature);
      const Muon_ROI*             muonL1          = muonL1Feature.cptr();

      const ElementLinkVector<TrigInDetTrackCollection> trackVector = trigObj->trackVector();
      // check broken ElementLinks
      if( !trackVector.at(0).isValid() || !trackVector.at(1).isValid() ) {
        ATH_MSG_WARNING("Broken ElementLink. Chain = " << chain);
        continue;
      }
      int nTracks = trackVector.size();
      ATH_MSG_DEBUG("Number of tracks in vertex:  " << nTracks);
      if (nTracks != 2) {
        ATH_MSG_ERROR("Number of trigger tracks in J/psi vertex differs from 2");
        continue;
      }

      const TrigInDetTrack* trigObjTracks[2] = { *(trackVector.at(0)), *(trackVector.at(1)) };
      if ( trigObjTracks[0]->param() == NULL || trigObjTracks[1]->param() == NULL) {
        ATH_MSG_ERROR("Some trigger tracks are without fitted parameters");
        continue;
      }

      bool passedHypo = m_useManualHypoCuts ? selectL2TrigDiMuon(trigObj,
								 trigObjTracks[0],
								 trigObjTracks[1],
								 m_Btrig_L2_checkOS[nTrig],
								 m_Btrig_L2_massMin[nTrig],
								 m_Btrig_L2_massMax[nTrig],
								 m_Btrig_L2_applyMassMax[nTrig],
								 m_Btrig_L2_nHits[nTrig],
								 m_Btrig_L2_chi2[nTrig]) : true;

      if (!passedHypo) {
        ATH_MSG_DEBUG("This object failed hypo, will not store on ntuple");
        continue;
      }

      // Get trigger object tracks
      for (int iTrack = 0; iTrack < nTracks; ++iTrack) { // Loop over trigger object tracks
        int64_t trkId = (int64_t) trigObjTracks[iTrack]; // TODO: Get a unique ID
        const TrigInDetTrackFitPar* param = trigObjTracks[iTrack]->param();
        ATH_MSG_DEBUG("BPhys L2 comb # " << iComb <<" object # " << iObj
                      << "  &:"    << trkId
                      << "  pt: "  << param->pT()
                      << "  eta: " << param->eta()
                      << "  phi: " << param->phi0()
                      << "  passHypo: " << passedHypo);
        if ( muonsL2 != NULL ) {
	  TrackType trackType = COMBINED;
	  if (muTrkJpsi) {
	    trackType = (iTrack == 0) ? COMBINED : IDONLY;
	  }
          TrigObject trigObjectL2;
          trigObjectL2.names.push_back(chain);
          trigObjectL2.eta = param->eta();
          trigObjectL2.phi = param->phi0();
          trigObjectL2.pt  = param->pT();
          trigObjectL2.q   = param->pT() > 0.0 ? 1 : -1 ;
          trigObjectL2.id  = trkId;
          trigObjectL2.parentId = (muonL1 != NULL) ? muonL1->getROIWord() : -1;
          trigObjectL2.grandparentId = -1;
          trigObjectL2.combIds.push_back(1000*iComb + iObj);
          trigObjectL2.efObjIds.push_back(-1);
          trigObjectL2.type = trackType;
          addTrigObject(trigObjectL2, muonsL2, m_appendL2IfExists);
        }
      } // End loop over trigger object tracks

      if ( muonL1 != NULL ) {
        ATH_MSG_DEBUG("     parent:  &:" << (int64_t) muonL1
                      << "  pt: "   << muonL1->pt()
                      << "  eta: "  << muonL1->eta()
                      << "  phi: "  << muonL1->phi()
                      << "  word: " << muonL1->getROIWord() );
        if ( muonsL1 != NULL) {
          TrigObject trigObjectL1;
	  int charge = -99;
	  switch (muonL1->getCharge()) {
	  case Muon_ROI::Pos: charge = 1;  break;
	  case Muon_ROI::Neg: charge = -1; break;
	  default:
	    charge = -99;
	    ATH_MSG_WARNING("muonL1 charge not defined " << muonL1->getCharge());
	  }
          trigObjectL1.names.push_back(chain);
          trigObjectL1.eta = muonL1->eta();
          trigObjectL1.phi = muonL1->phi();
          trigObjectL1.pt  = muonL1->pt();
          trigObjectL1.q   = charge;
          trigObjectL1.id  = muonL1->getROIWord();
          trigObjectL1.parentId = -1;
          trigObjectL1.grandparentId = -1;
          trigObjectL1.combIds.push_back(iObj);
          trigObjectL1.efObjIds.push_back(-1);
          trigObjectL1.type = SPECTROMETER;
          addTrigObject(trigObjectL1, muonsL1, m_appendL1IfExists);
        }
      } else ATH_MSG_WARNING("getBPhysL2TriggerObjects(): muonL1 is NULL");
    } // End loop over trigger features
  } // End loop over combinations

  return true;
}

bool
TrigEffJpsiTools::getBPhysEFTriggerObjects(const std::string& chain,
                                           TrigObjectMap* muonsEF,
                                           TrigObjectMap* /*muonsL2*/,
                                           TrigObjectMap* muonsL1) const
{
  // TOOD: Finish this
  ATH_MSG_DEBUG("TrigEffJpsiTools::getBPhysEFTriggerObjects() : " << chain);
  int nTrig      = -1;
  bool muTrkJpsi = (chain.find("_mu") && chain.find("Trk")); // Checks if chain is a mu+Trk chain. Notice: Case sensitive!

  for (unsigned int iTrig=0; iTrig < m_Btrig_EF_chain.size(); iTrig++) {
    if (m_Btrig_EF_chain[iTrig] == chain) nTrig=iTrig;
  }

  if (m_useManualHypoCuts) {
    if (nTrig<0) {
      ATH_MSG_WARNING("getBPhysEFTriggerObjects: do not have HYPO cuts for chain " << chain);
      return true;
    }
    ATH_MSG_DEBUG("getBPhysEFTriggerObjects: cuts for chain " << chain);
    ATH_MSG_DEBUG("getBPhysEFTriggerObjects: mass : " << m_Btrig_EF_massMin[nTrig] << " -> " << m_Btrig_EF_massMax[nTrig]);
    ATH_MSG_DEBUG("getBPhysEFTriggerObjects: applyMassMax " << m_Btrig_EF_applyMassMax[nTrig]);
    ATH_MSG_DEBUG("getBPhysEFTriggerObjects: checkOS " << m_Btrig_EF_checkOS[nTrig]);
    ATH_MSG_DEBUG("getBPhysEFTriggerObjects: chi2 " << m_Btrig_EF_chi2[nTrig]);
  }

  // Getting the chain's features:
  const Trig::FeatureContainer fc = m_trigDecisionTool->features(chain/*, TrigDefs::alsoDeactivateTEs*/ );
  const std::vector<Trig::Combination>& combinations = fc.getCombinations();
  ATH_MSG_DEBUG("getBPhysEFTriggerObjects: number of combinations: " << combinations.size());

  std::vector<Trig::Combination>::const_iterator cit;
  int iComb = 1;
  for (cit = combinations.begin(); cit != combinations.end(); ++cit, ++iComb) { // Loop over combinations
    const Trig::Combination& combination = *cit;
    if (not combination.active()) {
      ATH_MSG_DEBUG("getBPhysEFTriggerObjects: combination " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("getBPhysEFTriggerObjects: combination " << combination << " is active");


    const std::vector<const HLT::TriggerElement*> tes = combination.tes();
    std::vector<const HLT::TriggerElement*>::const_iterator tesIt;
    int iTrigElement = 0;
    ATH_MSG_DEBUG("Trigger elements: " << tes.size());
    for (tesIt = tes.begin(); tesIt != tes.end(); ++tesIt, ++iTrigElement) {
      ATH_MSG_DEBUG("Trigger element #" << iTrigElement << ": id = " << (*tesIt)->getId() << " activeState = " << (*tesIt)->getActiveState());
    }

    std::vector<Trig::Feature<TrigEFBphys> > triggerFeatures = combination.get<TrigEFBphys>();
    std::vector<Trig::Feature<TrigEFBphys> >::const_iterator tfIt;
    int iObj = 0;
    ATH_MSG_DEBUG("Trigger features: " << triggerFeatures.size());
    for (tfIt = triggerFeatures.begin(); tfIt != triggerFeatures.end(); ++tfIt, ++iObj) { // Loop over trigger features
      if(tfIt->te()) ATH_MSG_DEBUG("Trigger feature #" << iObj << ": id = " << tfIt->te()->getId() << " activeState = " << tfIt->te()->getActiveState());


      Trig::Feature<TrigEFBphys> triggerFeature = *tfIt;
      const TrigEFBphys*         trigObj        = triggerFeature.cptr();
      Trig::Feature<Muon_ROI>    muonL1Feature  = m_trigDecisionTool->ancestor<Muon_ROI>(triggerFeature);
      const Muon_ROI*            muonL1         = muonL1Feature.cptr();
      if (muonL1 != NULL) ATH_MSG_DEBUG(" L1 muon feature: " <<  muonL1->getROIWord());
      Trig::Feature<TrigL2Bphys> muonL2Feature  = m_trigDecisionTool->ancestor<TrigL2Bphys>(triggerFeature);
      ATH_MSG_DEBUG(" L2 muon feature: " <<  (int64_t) muonL2Feature.te() );
      const TrigL2Bphys* muonL2                 = muonL2Feature.cptr();

      const ElementLinkVector<Rec::TrackParticleContainer> trackVector   = trigObj->trackVector();
      // check broken ElementLinks
      if( !trackVector.at(0).isValid() || !trackVector.at(1).isValid() ) {
        ATH_MSG_WARNING("Broken ElementLink. Chain = " << chain);
        continue;
      }
      int nTracks = trackVector.size();
      ATH_MSG_DEBUG("Number of tracks in vertex:  " << nTracks);
      if (nTracks != 2) {
        ATH_MSG_ERROR("Number of trigger tracks in J/psi vertex differs from 2");
        continue;
      }

      const Rec::TrackParticle* trigObjTracks[2] = { *(trackVector.at(0)),   *(trackVector.at(1)) };
      const TrigInDetTrack*     l2Tracks[2]      = { NULL, NULL };
      if (muonL2 != NULL) {
	l2Tracks[0]   = *(muonL2->trackVector().at(0));
	l2Tracks[1]   = *(muonL2->trackVector().at(1));
      }
      //      if ( trigObjTracks[0]->param() == NULL || trigObjTracks[1]->param() == NULL) {
      //  ATH_MSG_ERROR("Some trigger tracks are without fitted parameters");
      //  continue;
      // }

      // check HYPO cuts - need to make these explicitly now, in 2011 will be able to use TrigPassBits
      ATH_MSG_DEBUG("B trigger mass ( " << chain << ") = " << trigObj->mass() );
      bool passedHypo = m_useManualHypoCuts ? selectEFTrigDiMuon(trigObj,
								 trigObjTracks[0],
								 trigObjTracks[1],
								 m_Btrig_EF_checkOS[nTrig],
								 m_Btrig_EF_massMin[nTrig],
								 m_Btrig_EF_massMax[nTrig],
								 m_Btrig_EF_applyMassMax[nTrig],
								 m_Btrig_EF_chi2[nTrig]) : true;

      if (!passedHypo) {
        ATH_MSG_DEBUG("This object failed hypo, will not store on ntuple");
        continue;
      }

      // Get trigger object tracks
      for (int iTrack = 0; iTrack < nTracks; ++iTrack) { // Loop over trigger object tracks
        int64_t trkId    = (int64_t) trigObjTracks[iTrack]; // TODO: Get a unique ID
	int64_t parentId = (int64_t) l2Tracks[iTrack];
        ATH_MSG_DEBUG("BPhys EF comb # " << iComb <<" object # " << iObj
                      << "  &:"          << trkId
                      << "  pt: "        << trigObjTracks[iTrack]->pt()
                      << "  eta: "       << trigObjTracks[iTrack]->eta()
                      << "  phi: "       << trigObjTracks[iTrack]->phi()
                      << "  passHypo: "  << passedHypo);
        if ( muonsEF != NULL ) {
	  TrackType trackType = COMBINED;
	  if (muTrkJpsi) {
	    trackType = (iTrack == 0) ? COMBINED : IDONLY;
	  }

          TrigObject trigObjectEF;
          trigObjectEF.names.push_back(chain);
          trigObjectEF.eta = trigObjTracks[iTrack]->eta();
          trigObjectEF.phi = trigObjTracks[iTrack]->phi();

	  int charge = trigObjTracks[iTrack]->measuredPerigee()->parameters()[Trk::qOverP] > 0 ? 1. : -1. ;
          trigObjectEF.pt  = charge * trigObjTracks[iTrack]->pt();
	  trigObjectEF.q   = charge;
          trigObjectEF.id  = trkId;
          trigObjectEF.parentId = parentId;
//           trigObjectEF.parentId = (int64_t) muonL2;
	  ATH_MSG_DEBUG("     parentId = " << trigObjectEF.parentId);
          trigObjectEF.grandparentId = (muonL1 != NULL) ? muonL1->getROIWord() : -1;
          //trigObjectEF.daughterIds.push_back(-1);
          trigObjectEF.combIds.push_back(1000*iComb + iObj); // Multiply by 1000 to differentiate BPhys trigs from single-muon ones
          trigObjectEF.efObjIds.push_back(iTrack);
          trigObjectEF.type = trackType;
          addTrigObject(trigObjectEF, muonsEF, m_appendEFIfExists);
        }
      } // End loop over trigger object tracks


      // TODO:
      // Need to add muonL2 acquisition step.
      // Provenance is currently working only if the L2
      // chain is specified in the BPhysL2 chains
      if ( muonL1 != NULL ) {
        ATH_MSG_DEBUG("     grandparent:  &:" << (int64_t) muonL1
                      << "  pt: "   << muonL1->pt()
                      << "  eta: "  << muonL1->eta()
                      << "  phi: "  << muonL1->phi()
                      << "  word: " << muonL1->getROIWord() );
        if ( muonsL1 != NULL) {
          TrigObject trigObjectL1;
	  int charge = -99;
	  switch (muonL1->getCharge()) {
	  case Muon_ROI::Pos: charge = 1;  break;
	  case Muon_ROI::Neg: charge = -1; break;
	  default:
	    charge = -99;
	    ATH_MSG_WARNING("muonL1 charge not defined " << muonL1->getCharge());
	  }
          trigObjectL1.names.push_back(chain);
          trigObjectL1.eta = muonL1->eta();
          trigObjectL1.phi = muonL1->phi();
          trigObjectL1.pt  = muonL1->pt();
          trigObjectL1.q   = charge;
          trigObjectL1.id  = muonL1->getROIWord();
          trigObjectL1.parentId = -1;
          trigObjectL1.grandparentId = -1;
          //trigObjectL1.daughterIds.push_back((int64_t) muonL2);
          trigObjectL1.combIds.push_back(iObj);
          trigObjectL1.efObjIds.push_back(-1);
          trigObjectL1.type = SPECTROMETER;
          addTrigObject(trigObjectL1, muonsL1, m_appendL1IfExists);
        }
      } else ATH_MSG_WARNING("getBPhysEFTriggerObjects(): muonL1 is NULL");
    } // End loop over trigger features
  } // End loop over combinations

  return true;
}


int
TrigEffJpsiTools::muctpiBcidDiff(const int roiBcid) const
{
  const EventInfo* eventInfo = 0;
  if (evtStore()->retrieve(eventInfo).isFailure()) {
    ATH_MSG_WARNING("muctpiBcidDiff: couldn't retrieve EventInfo");
    return 0;
  }
  const unsigned int eventBcid = eventInfo->event_ID()->bunch_crossing_id();

  int diff = roiBcid - (eventBcid & 0x7);
  switch (eventBcid) {
  case 0: // cannot distinguish 3563 and 3, 3562 and 2, 3561 and 1
    if (roiBcid == 3) {
      diff = -1;
    } else if (roiBcid == 2) {
      diff = -2;
    } else if (roiBcid == 1) {
      diff = -3;
    }
    break;

  case 1: // cannot distinguish 3563 and 3, 3562 and 2
    if (roiBcid == 3) {
      diff = -2;
    } else if (roiBcid == 2) {
      diff = -3;
    }
    break;

  case 2: // cannot distinguish 3563 and 3
    if (roiBcid == 3) {
      diff = -3;
    }
    break;

  default:
    break;
  }

  if (abs(diff) > 4) {
    diff = (8 - abs(diff)) * ((diff < 0) ? 1 : -1);
  }

  return diff;
}


int
TrigEffJpsiTools::getMuonRoiBcidDiff(const uint32_t roiWord) const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::getMuonRoiBcidDiff()");

  const EventInfo* eventInfo = 0;
  if (evtStore()->retrieve(eventInfo).isFailure()) {
    ATH_MSG_WARNING("getMuonRoiBcidDiff: couldn't retrieve EventInfo");
    return 0;
  }
  const unsigned int eventBcid = eventInfo->event_ID()->bunch_crossing_id();

  // Create the RoI vector from the RDO data:
  const std::string key = "MUCTPI_RDO";
  const MuCTPI_RDO* muctpi_rdo = 0;
  if (evtStore()->retrieve(muctpi_rdo, key).isFailure()) {
    ATH_MSG_WARNING("TrigEffJpsiTools::getMuonRoiBcidDiff: couldn't retrieve " << key);
    return 0;
  }
  ATH_MSG_VERBOSE("Retrieved the MuCTPI_RDO object with key: "
                  << key);

  ATH_MSG_VERBOSE("Now creating the RoIs in extended bunches...");
  MuCTPI_MultiplicityWord_Decoder mwDecoder(muctpi_rdo->candidateMultiplicity());
  uint32_t currentBcid = mwDecoder.getBCID();
  if (currentBcid != (eventBcid & 0x7)) {
    ATH_MSG_WARNING("MuonRoiWithExtendedBunches: BCID mis-match: " << eventBcid << ":" << currentBcid);
    return 0;
  }

  // Loop over all "data words" and calculate bunch crossing difference of the given roiWord
  ATH_MSG_VERBOSE("found " << muctpi_rdo->dataWord().size() << " muctpi_rdo dataWords");

  std::vector<uint32_t>::const_iterator cit = muctpi_rdo->dataWord().begin();
  std::vector<uint32_t>::const_iterator end = muctpi_rdo->dataWord().end();
  for(; cit != end; ++cit) {
    if (roiWord != RDOtoRoI(*cit)) continue;

    MuCTPI_DataWord_Decoder dwDecoder(*cit);
    uint32_t bcid = dwDecoder.getBCID();
    int bcDiff = muctpiBcidDiff(bcid);
    ATH_MSG_DEBUG(std::dec << bcid << " : " << currentBcid << " diff = " << bcDiff);
    return bcDiff;
  }

  ATH_MSG_WARNING("TrigEffJpsiTools::getMuonRoiBcidDiff: "
                  "No corresponding RoI found: " << roiWord);
  return 10000;
}


bool
TrigEffJpsiTools::isOffTimeTgcTrigger() const
{
  ATH_MSG_DEBUG("TrigEffJpsiTools::isOffTimeTgcTrigger()");
  
  // apply for the run periods D2...D5 [158443, 159113]
  const EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("failed to get EventInfo");
    return false;
  }
  const EventID* eventId = eventInfo->event_ID();
  int runNumber = eventId->run_number();
  if (runNumber < 158443) return false;
  if (runNumber > 159113) return false;


  // check trigger configuration
  const std::string EF_mu0_missingRoi = "EF_mu0_missingRoi";
  if (not isConfigured(EF_mu0_missingRoi, "EF")) {
    ATH_MSG_WARNING(EF_mu0_missingRoi << "not configured");
    return false;
  }

  const std::string EF_mu0_rpcOnly = "EF_mu0_rpcOnly";
  if (not isConfigured(EF_mu0_rpcOnly, "EF")) {
    ATH_MSG_WARNING(EF_mu0_rpcOnly << "not configured");
    return false;
  }

  const std::string EF_mu4_MSonly = "EF_mu4_MSonly";
  if (not isConfigured(EF_mu4_MSonly, "EF")) {
    ATH_MSG_WARNING(EF_mu4_MSonly << "not configured");
    return false;
  }

  const std::string L1_2MU0 = "L1_2MU0";
  if (not isConfigured(L1_2MU0, "L1")) {
    ATH_MSG_WARNING(L1_2MU0 << "not configured");
    return false;
  }


  // accept if event has triggers other than EF_mu0_missingRoi and l1_2mu0
  bool ef_mu4_msonly = m_trigDecisionTool->isPassed(EF_mu4_MSonly);
  bool ef_mu0_rpconly = m_trigDecisionTool->isPassed(EF_mu0_rpcOnly);
  if (ef_mu4_msonly or ef_mu0_rpconly) return false;

  bool ef_mu0_missingRoi = m_trigDecisionTool->isPassed(EF_mu0_missingRoi);
  bool l1_2mu0 = m_trigDecisionTool->isPassed(L1_2MU0);
  if ((not ef_mu0_missingRoi) and (not l1_2mu0)) return false;


  // get level1 muon rois
  const LVL1_ROI* lvl1Roi = 0;
  if (evtStore()->retrieve(lvl1Roi, m_Lvl1RoiName).isFailure() or
      !lvl1Roi) {
    ATH_MSG_WARNING("Failed to retrieve " << m_Lvl1RoiName);
    return false;
  }  

  bool tgcOffTime = false;

  LVL1_ROI::muons_type muonRoIs = lvl1Roi->getMuonROIs();
  typedef LVL1_ROI::muons_type::const_iterator iterator;
  iterator cit = muonRoIs.begin();
  iterator end = muonRoIs.end();

  for (; cit != end; cit++) {
    bool inTime = false;
    int bcDiff = getMuonRoiBcidDiff(cit->getROIWord());
    switch (bcDiff) {
    case -2:
    case -1:
    case  0:
      inTime = true;
      break;

    default:
      break;
    }

    if (inTime and (cit->getSource() == Muon_ROI::Barrel)) return false;
    if (inTime and (bcDiff < 0)) tgcOffTime = true;
  }

  return tgcOffTime;
}


bool
TrigEffJpsiTools::EF_mu4_MSOnly_from_EF_mu0_outOfTime1() const
{
  const std::string L2_mu0_outOfTime1 = "L2_mu0_outOfTime1";

  if (not m_trigDecisionTool->isPassed("EF_mu0_outOfTime1")) return false;
  if (not m_trigDecisionTool->isPassed(L2_mu0_outOfTime1)) return false;

  Trig::FeatureContainer fc = m_trigDecisionTool->features(L2_mu0_outOfTime1);
  std::vector<Trig::Feature<MuonFeature> > l2muons = fc.get<MuonFeature>();
  std::vector<Trig::Feature<MuonFeature> >::const_iterator cit;
  for (cit = l2muons.begin(); cit != l2muons.end(); cit++) {
    const MuonFeature* l2mf = cit->cptr();
    if (EF_mu4_MSOnly_with_Muid(l2mf)) return true;
  }

  return false;
}


/*--------------------------------------------------------------------*
 * private methods
 *--------------------------------------------------------------------*/
void
TrigEffJpsiTools::getExtrapolationErrorOnEndcap(const Trk::AtaDisc* p,
                                                Amg::Vector2D* dEtaPhi) const
{
  if (!p or !dEtaPhi) {
    ATH_MSG_WARNING("getExtrapolationErrorOnEndcap: null input pointer");
    return;
  }

  (*dEtaPhi)[Trk::locX] = getEtaErrFromRPhi(p);

  (*dEtaPhi)[Trk::locY] = 9999.;
  const AmgSymMatrix(5) *covMatrix = p->covariance();
  if(covMatrix){
    (*dEtaPhi)[Trk::locY] = sqrt((*covMatrix)(Trk::locPhi, Trk::locPhi));
  }
}


void
TrigEffJpsiTools::getExtrapolationErrorOnBarrel(const Trk::AtaCylinder* p,
                                                Amg::Vector2D* dEtaPhi) const
{
  if (!p or !dEtaPhi) {
    ATH_MSG_WARNING("getExtrapolationErrorOnBarrel: null input pointer");
    return;
  }

  double radius = p->position().perp();
  (*dEtaPhi)[Trk::locX] = getEtaErrFromRPhiZ(p);
  (*dEtaPhi)[Trk::locY] = 9999.;
  const AmgSymMatrix(5) *covMatrix = p->covariance();
  if(covMatrix){
    (*dEtaPhi)[Trk::locY] = sqrt((*covMatrix)(Trk::locRPhi, Trk::locRPhi))/radius;
  }
}


// dR limit to match 
double
TrigEffJpsiTools::getSearchRangeOnEndcap(const Trk::AtaDisc* p) const
{
  if (!p) {
    ATH_MSG_WARNING("getSearchRangeOnEndcap: null input pointer");
    return false;
  }

  Amg::Vector2D dEtaPhi;
  getExtrapolationErrorOnEndcap(p, &dEtaPhi);
  double searchRange = m_searchRangeOffset + m_searchRangeTolerance *
    sqrt(dEtaPhi.x()*dEtaPhi.x() + dEtaPhi.y()*dEtaPhi.y());

  double eta = p->position().eta();
  if (fabs(eta) > m_endcapForwardBoundary) {
    searchRange += m_roiSizeForward;
  } else {
    searchRange += m_roiSizeEndcap;
  }
  return searchRange;
}


double
TrigEffJpsiTools::getSearchRangeOnBarrel(const Trk::AtaCylinder* p) const
{
  if (!p) {
    ATH_MSG_WARNING("getSearchRangeOnBarrel: null input pointer");
    return false;
  }

  Amg::Vector2D dEtaPhi;
  getExtrapolationErrorOnBarrel(p, &dEtaPhi);
  double searchRange = m_searchRangeOffset + m_searchRangeTolerance *
    sqrt(dEtaPhi.x()*dEtaPhi.x() + dEtaPhi.y()*dEtaPhi.y()) +
    m_roiSizeBarrel;
  return searchRange;
}


double TrigEffJpsiTools::getEtaPhiDistanceAtIp(const Rec::TrackParticle* track1,
                                               const Rec::TrackParticle* track2) const
{
  if (!track1 or !track2) {
    ATH_MSG_WARNING("getEtaPhiDistanceAtIp: null input pointer");
    return -1.;
  }

  double eta[2] = {track1->eta(), track2->eta()};
  double phi[2] = {track1->phi(), track2->phi()};
  return getEtaPhiDistance(eta[0], eta[1], phi[0], phi[1]);
}


double
TrigEffJpsiTools::getEtaPhiDistanceOnEndcap(const Trk::AtaDisc* p1,
                                            const Trk::AtaDisc* p2) const
{
  if (!p1 or !p2) {
    ATH_MSG_WARNING("getEtaPhiDistanceOnEndcap: null input pointer");
    return -1.;
  }

  return getEtaPhiDistance(p1->position().eta(), p2->position().eta(),
                           p1->position().phi(), p2->position().phi());
}


double
TrigEffJpsiTools::getEtaPhiDistanceOnBarrel(const Trk::AtaCylinder* p1,
                                            const Trk::AtaCylinder* p2) const
{
  if (!p1 or !p2) {
    ATH_MSG_WARNING("getEtaPhiDistanceOnBarrel: null input pointer");
    return -1.;
  }

  return getEtaPhiDistance(p1->position().eta(), p2->position().eta(),
                           p1->position().phi(), p2->position().phi());
}


double
TrigEffJpsiTools::getEtaPhiDistance(double eta1,
                                    double eta2,
                                    double phi1,
                                    double phi2) const
{
  double deta = getEtaDistance(eta1, eta2);
  double dphi = getPhiDistance(phi1, phi2);

  double dr = sqrt(deta*deta + dphi*dphi);
  return dr;
}


double
TrigEffJpsiTools::getEtaDistance(double eta1,
				 double eta2) const
{
  return fabs(eta1 - eta2);
}


double
TrigEffJpsiTools::getPhiDistance(double phi1,
				 double phi2) const
{
  double dphi = fabs(phi1 - phi2);

  if (dphi > M_PI) dphi = 2.*M_PI - dphi;
  return dphi;
}


bool
TrigEffJpsiTools::isSeparated(const Rec::TrackParticle* track1,
                              const Rec::TrackParticle* track2) const
{
  if (!track1 or !track2) {
    ATH_MSG_WARNING("isSeparated: null input pointer");
    return false;
  }

  return isSeparatedOnEndcap(track1, track2) and
    isSeparatedOnBarrel(track1, track2);

}

bool
TrigEffJpsiTools::isSeparatedOnEndcap(const Rec::TrackParticle* track1,
                                      const Rec::TrackParticle* track2) const
{
  if (!track1 or !track2) {
    ATH_MSG_WARNING("isSeparatedOnEndcap: null input pointer");
    return false;
  }

  bool isSeparated = true;

  double pivotZ1 = track1->eta() < 0. ? -m_endcapPivotPlaneZ : m_endcapPivotPlaneZ;
  const Trk::AtaDisc* measurement1 = extrapolateToEndcap(track1, pivotZ1);
  double pivotZ2 = track2->eta() < 0. ? -m_endcapPivotPlaneZ : m_endcapPivotPlaneZ;
  const Trk::AtaDisc* measurement2 = extrapolateToEndcap(track2, pivotZ2);

  if (measurement1 and measurement2) {
    double searchRange = getSearchRangeOnEndcap(measurement1) +
      getSearchRangeOnEndcap(measurement2);  
    double dr = getEtaPhiDistanceOnEndcap(measurement1, measurement2);
    if (dr < searchRange) isSeparated = false;
  }

  delete measurement1;
  delete measurement2;

  return isSeparated;
}

bool
TrigEffJpsiTools::isSeparatedOnBarrel(const Rec::TrackParticle* track1,
                                      const Rec::TrackParticle* track2) const
{
  if (!track1 or !track2) {
    ATH_MSG_WARNING("isSeparatedOnBarrel: null input pointer");
    return false;
  }

  bool isSeparated = true;

  double pivotRadius = m_barrelPivotPlaneRadius;
  const Trk::AtaCylinder* measurement1 = extrapolateToBarrel(track1, pivotRadius);
  const Trk::AtaCylinder* measurement2 = extrapolateToBarrel(track2, pivotRadius);

  if (measurement1 and measurement2) {
    double searchRange = getSearchRangeOnBarrel(measurement1) +
      getSearchRangeOnBarrel(measurement2);  
    double dr = getEtaPhiDistanceOnBarrel(measurement1, measurement2);
    if (dr < searchRange) isSeparated = false;
  }

  delete measurement1;
  delete measurement2;

  return isSeparated;
}


//--- convert local errors to global ---//
// Disc to global
double
TrigEffJpsiTools::getEtaErrFromRPhi(const Trk::AtaDisc* p) const
{
  if (!p) {
    ATH_MSG_WARNING("getEtaErrFromRPhi: null input pointer");
    return -1;
  }

  double localR = p->localPosition().x();
  double globalZ = p->position().z();
  double dLocalR = 9999.;
  const AmgSymMatrix(5) *covMatrix = p->covariance();
  if(covMatrix){
    dLocalR = sqrt((*covMatrix)(Trk::locR, Trk::locR));
  }

  double radius = sqrt(localR*localR + globalZ*globalZ);
  double etaError = fabs(globalZ/localR/radius*dLocalR);  
  return etaError;
}


// Cylinder to global
double
TrigEffJpsiTools::getEtaErrFromRPhiZ(const Trk::AtaCylinder* p) const
{
  if (!p) {
    ATH_MSG_WARNING("getEtaErrFromRPhiZ: null input pointer");
    return -1;
  }

  double localR = p->position().perp();
  double localZ = p->localPosition().y();
  double dLocalZ = 9999.;
  const AmgSymMatrix(5) *covMatrix = p->covariance();
  if(covMatrix){
    dLocalZ = sqrt((*covMatrix)(Trk::locZ, Trk::locZ));
  }

  double radius = sqrt(localR*localR + localZ*localZ);
  double etaError = fabs(1./radius*dLocalZ);
  return etaError;
}



void
TrigEffJpsiTools::validateCache(Cache& map) const
{
  const EventInfo* eventInfo = 0;
  if (evtStore()->retrieve(eventInfo).isFailure() or !eventInfo) {
    ATH_MSG_WARNING("validateCache: Failed to retrieve EventInfo");
    return;
  }

  const EventID* eventId = eventInfo->event_ID();
  if (!eventId) {
    ATH_MSG_WARNING("validateCache: Failed to get EventID");
    return;
  }

  if (((map.runNo == eventId->run_number()) and // new evt
       (map.evtNo != eventId->event_number())) or
      (map.runNo != eventId->run_number())) {   // new run
    map.runNo = eventId->run_number();
    map.evtNo = eventId->event_number();
    ATH_MSG_DEBUG("validateCache: clear cache: " << map.runNo <<
                  ", " << map.evtNo);
    CacheMap::const_iterator cit;
    for (cit = map.tp.begin(); cit != map.tp.end(); cit++) {
      delete cit->second;
    }
    map.tp.clear();
  }

  return;
}



const Trk::AtaDisc*
TrigEffJpsiTools::extrapolateToEndcap(const Rec::TrackParticle *track,
                                      const double pivotZ,
                                      const bool useCache) const
{
  if (!track) {
    ATH_MSG_WARNING("extrapolateToEndcap: null input pointer");
    return 0;
  }

  if (useCache) {
    validateCache(m_cacheEndcap);
    CacheMap::const_iterator cit;
    cit = m_cacheEndcap.tp.find(track);
    if (cit != m_cacheEndcap.tp.end()) {
      // ATH_MSG_DEBUG("extrapolateToEndcap: use cache: " << track); // TODO: Removed as track dump segfaults in 17.3.1
      if (cit->second)
        return dynamic_cast<const Trk::AtaDisc*>(cit->second);
      return 0;
    }

    m_cacheEndcap.tp.insert(CachePair(track, 0));
  }
  // ATH_MSG_DEBUG("extrapolateToEndcap: " << *track); // TODO: Removed as track dump segfaults in 17.3.1


  const Trk::Perigee *perigee = track->measuredPerigee();
  if (!perigee) {
    ATH_MSG_WARNING("extrapolateToEndcap: null perigee");
    return 0;
  }

  Amg::Vector3D endcapCentre(0., 0., pivotZ);
  Amg::Transform3D* matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), endcapCentre);

  Trk::DiscSurface* disc = 
    new Trk::DiscSurface(matrix,
                         m_endcapPivotPlaneMinimumRadius,
                         m_endcapPivotPlaneMaximumRadius);
  if (!disc) {
    ATH_MSG_WARNING("new Trk::DiscSurface failed."); 
    delete matrix; 
    matrix = 0; 
    return 0;
  }

  bool boundaryCheck = true;
  const Trk::Surface* surface = disc;
  const Trk::TrackParameters* p = m_extrapolator->extrapolate(*perigee,
							      *surface,
							      Trk::alongMomentum,
							      boundaryCheck,
							      Trk::muon);
  delete disc; 
  disc = 0; 
  surface = 0;
  matrix = 0; 

  if (useCache) m_cacheEndcap.tp[track] = p;
  if (!p) return 0;
  const Trk::AtaDisc *measurement = 
    dynamic_cast<const Trk::AtaDisc*>(p);
  return measurement;
}


const Trk::AtaCylinder*
TrigEffJpsiTools::extrapolateToBarrel(const Rec::TrackParticle *track,
                                      const double pivotRadius,
                                      const bool useCache) const
{
  if (!track) {
    ATH_MSG_WARNING("extrapolateToBarrel: null input pointer");
    return 0;
  }

  if (useCache) {
    validateCache(m_cacheBarrel);
    CacheMap::const_iterator cit;
    cit = m_cacheBarrel.tp.find(track);
    if (cit != m_cacheBarrel.tp.end()) {
      ATH_MSG_DEBUG("extrapolateToBarrel: use cache: " << track);
      if (cit->second)
        return dynamic_cast<const Trk::AtaCylinder*>(cit->second);
      return 0;
    }

    m_cacheBarrel.tp.insert(CachePair(track, 0));
  }
  ATH_MSG_DEBUG("extrapolateToBarrel: " << *track);

  const Trk::Perigee *perigee = track->measuredPerigee();
  if (!perigee) {
    ATH_MSG_WARNING("extrapolateToBarrel: null perigee");
    return 0;
  }

  Amg::Vector3D barrelCentre(0., 0., 0.);
  Amg::Transform3D* matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), barrelCentre);

  Trk::CylinderSurface* cylinder = 
    new Trk::CylinderSurface(matrix,
                             pivotRadius,
                             m_barrelPivotPlaneHalfLength);
  if (!cylinder) {
    ATH_MSG_WARNING("new Trk::CylinderSurface failed.");
    delete matrix;
    matrix = 0;
    return 0;
  }

  bool boundaryCheck = true;
  const Trk::Surface* surface = cylinder;
  const Trk::TrackParameters* p = m_extrapolator->extrapolate(*perigee,
							      *surface,
							      Trk::alongMomentum,
							      boundaryCheck,
							      Trk::muon);
  delete cylinder;
  cylinder = 0;
  surface = 0;
  matrix = 0;

  if (useCache) m_cacheBarrel.tp[track] = p;
  if (!p) return 0;
  const Trk::AtaCylinder* measurement = 
    dynamic_cast<const Trk::AtaCylinder*>(p);
  return measurement; 
}


const std::vector<const Trk::TrackStateOnSurface*>*
TrigEffJpsiTools::extrapolateToEndcapM(const Rec::TrackParticle *track,
                                       const double pivotZ) const
{
  if (!track) {
    ATH_MSG_WARNING("extrapolateToEndcapM: null input pointer");
    return 0;
  }

  const Trk::Perigee *perigee = track->measuredPerigee();
  if (!perigee) {
    ATH_MSG_WARNING("extrapolateToEndcapM: null perigee");
    return 0;
  }

  Amg::Vector3D endcapCentre(0., 0., pivotZ);
  Amg::Transform3D* matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), endcapCentre);

  Trk::DiscSurface* disc = 
    new Trk::DiscSurface(matrix,
                         m_endcapPivotPlaneMinimumRadius,
                         m_endcapPivotPlaneMaximumRadius);
  if (!disc) {
    ATH_MSG_WARNING("new Trk::DiscSurface failed.");
    delete matrix;
    matrix = 0;
    return 0;
  }

  bool boundaryCheck = true;
  const Trk::Surface* surface = disc;
  const std::vector<const Trk::TrackStateOnSurface*>* p =
    m_extrapolator->extrapolateM(*perigee, *surface, Trk::alongMomentum,
                                 boundaryCheck, Trk::muon);
  
  delete disc;
  disc = 0;
  surface = 0;
  matrix = 0; 

  return p;
}


const std::vector<const Trk::TrackStateOnSurface*>*
TrigEffJpsiTools::extrapolateToBarrelM(const Rec::TrackParticle *track,
                                       const double pivotRadius) const
{
  if (!track) {
    ATH_MSG_WARNING("extrapolateToBarrelM: null input pointer");
    return 0;
  }

  const Trk::Perigee *perigee = track->measuredPerigee();
  if (!perigee) {
    ATH_MSG_WARNING("extrapolateToBarrelM: null perigee");
    return 0;
  }

  Amg::Vector3D barrelCentre(0., 0., 0.);
  Amg::Transform3D* matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), barrelCentre);

  Trk::CylinderSurface* cylinder;
  cylinder = new Trk::CylinderSurface(matrix,
                                      pivotRadius,
                                      m_barrelPivotPlaneHalfLength);

  if (!cylinder) {
    ATH_MSG_WARNING("new Trk::CylinderSurface failed.");
    delete matrix;
    matrix = 0;
    return 0;
  }

  bool boundaryCheck = true;
  const Trk::Surface* surface = cylinder;
  const std::vector<const Trk::TrackStateOnSurface*>* p = 
    m_extrapolator->extrapolateM(*perigee, *surface, Trk::alongMomentum,
                                 boundaryCheck, Trk::muon);
  
  delete cylinder;
  cylinder = 0;
  surface = 0;
  matrix = 0; 

  return p;
}


bool
TrigEffJpsiTools::isTriggered(const struct Search& centre,
                              const int thresholdNumber,
                              const LVL1_ROI::muons_type& muonRoIs) const
{
  typedef LVL1_ROI::muons_type::const_iterator iterator;
  iterator cit_end = muonRoIs.end();
  for (iterator cit = muonRoIs.begin(); cit != cit_end; cit++) {
    const Muon_ROI *roi = &(*cit);
    if (roi->getThrNumber() < thresholdNumber) continue;

    double eta = roi->eta(); 
    double phi = roi->phi();
    double dr = getEtaPhiDistance(centre.eta, eta, centre.phi, phi);
    if (dr < centre.searchRange) return true;
  }
  return false;
}


bool
TrigEffJpsiTools::isTriggeredOnEndcap(const Rec::TrackParticle* track,
                                      LVL1_ROI::muons_type& muonRoIs,
                                      int thresholdNumber) const
{
  if (!track) {
    ATH_MSG_WARNING("isTriggeredOnEndcap: null input pointer");
    return 0;
  }

  double pivotZ = track->eta() < 0. ? -m_endcapPivotPlaneZ : m_endcapPivotPlaneZ;
  const Trk::AtaDisc* measurement = extrapolateToEndcap(track, pivotZ);
  if (!measurement) {
    ATH_MSG_DEBUG("Failed to extrapolate to endcap");
    return false;
  }

  struct Search centre;
  centre.eta = measurement->position().eta();
  centre.phi = measurement->position().phi();
  centre.searchRange = getSearchRangeOnEndcap(measurement);

  return isTriggered(centre, thresholdNumber, muonRoIs);
}


bool
TrigEffJpsiTools::isTriggeredOnBarrel(const Rec::TrackParticle* track,
                                      LVL1_ROI::muons_type& muonRoIs,
                                      int thresholdNumber) const
{
  if (!track) {
    ATH_MSG_WARNING("isTriggeredOnBarrel: null input pointer");
    return 0;
  }

  double pivotRadius = m_barrelPivotPlaneRadius;
  const Trk::AtaCylinder* measurement = extrapolateToBarrel(track,
							    pivotRadius);
  if (!measurement) {
    ATH_MSG_DEBUG("Failed to extrapolate to barrel");
    return false;
  }
  struct Search centre;
  centre.eta = measurement->position().eta();
  centre.phi = measurement->position().phi();
  centre.searchRange = getSearchRangeOnBarrel(measurement);

  //delete measurement;
  return isTriggered(centre, thresholdNumber, muonRoIs);
}


int
TrigEffJpsiTools::getRoiRegion(int roiWord) const
{
  // ref: Muon_ROI::getSource() 
  unsigned int word = (unsigned int) roiWord;
  unsigned int sectorAddress = ((word >> 14) & 0xff);

  if (sectorAddress & 0x80) { // endcap
    return ENDCAP;

  } else if (sectorAddress & 0x40) { // forward
    return ENDCAP;

  } else { // barrel
    return BARREL;
  }

  return -1;
}


unsigned int
TrigEffJpsiTools::getBitMaskValue(const unsigned int value,
                                  const unsigned int mask) const
{
  // ref: RecMuonRoI::getBitMaskValue()
  unsigned int maskcopy = mask;
  unsigned int result = value & mask;

  if (mask != 0) {
    while ((maskcopy & 0x00000001) == 0) {
      maskcopy = maskcopy >> 1;
      result = result >> 1;
    }
  }
  return result;
}


StatusCode
TrigEffJpsiTools::getRoiSize(const unsigned int roiWord,
                             double* etaSize,
                             double* phiSize) const
{
  switch (getRoiRegion(roiWord)) {
  case ENDCAP:
    return getRoiSizeTgc(roiWord, etaSize, phiSize);

  case BARREL:
    return getRoiSizeRpc(roiWord, etaSize, phiSize);

  default:
    break;
  }

  return StatusCode::FAILURE;
}


StatusCode
TrigEffJpsiTools::getRoiSizeTgc(const unsigned int roiWord,
                                double* etaSize,
                                double* phiSize) const
{
  if (!etaSize or !phiSize) {
    ATH_MSG_FATAL("getRoiSizeTgc: null input pointer");
    return StatusCode::FAILURE;
  }

  double etaMin, etaMax, phiMin, phiMax;
  m_tgcRoiService->RoIsize(roiWord, etaMin, etaMax, phiMin, phiMax);

  *etaSize = fabs(etaMax - etaMin);
  *phiSize = fabs(phiMax - phiMin);

  return StatusCode::SUCCESS;
}


StatusCode
TrigEffJpsiTools::getRoiSizeRpc(const unsigned int roiWord,
                                double* etaSize,
                                double* phiSize) const
{
  if (!etaSize or !phiSize) {
    ATH_MSG_FATAL("getRoiSizeRpc: null input pointer");
    return StatusCode::FAILURE;
  }

  // ref: RPCRecRoiSvc::reconstruct()
  *etaSize = *phiSize = -1.e30;
  
  double etaMin, etaMax, phiMin, phiMax;
  m_rpcRoiService->RoIsize(roiWord, etaMin, etaMax, phiMin, phiMax);
  
  *etaSize = fabs(etaMax - etaMin);
  *phiSize = fabs(phiMax - phiMin);
  
  return StatusCode::SUCCESS;
}

StatusCode
TrigEffJpsiTools::getRioOnTriggerChamber(const Trk::Track* track,
                                         std::vector<const Trk::RIO_OnTrack*>& rots,
                                         bool pivotOnly) const
{
  if (!track) {
    ATH_MSG_FATAL("getRioOnTriggerChamber: null input pointer");
    return StatusCode::FAILURE;
  }

  const DataVector<const Trk::TrackStateOnSurface>* tsos;
  tsos = track->trackStateOnSurfaces();

  if (!tsos or tsos->empty()) return StatusCode::SUCCESS;

  typedef DataVector<const Trk::TrackStateOnSurface>::const_iterator iterator;
  iterator cit_end = tsos->end();
  for (iterator cit = tsos->begin(); cit != cit_end; cit++) {
    if (not (*cit)->type(Trk::TrackStateOnSurface::Measurement)) continue;

    const Trk::MeasurementBase* measurement = (*cit)->measurementOnTrack();

    const Trk::RIO_OnTrack* rot = 0;
    Trk::RoT_Extractor::extract(rot, measurement);
    if (!rot) continue;

    const Identifier id = rot->identify();
    if (not m_muonIdHelper->isMuon(id)) continue;
    if (not m_muonIdHelper->isTrigger(id)) continue;

    bool isTgc = m_muonIdHelper->isTgc(id);
    bool isRpc = m_muonIdHelper->isRpc(id);

    if (isTgc and isRpc) {
      ATH_MSG_FATAL("rot belonging to both rpc and tgc()");
      return StatusCode::FAILURE;
    }

    // ref; MuonChamberNameConverter.cxx - convertChamberName(...)
    bool isPivot = false;
    if (isRpc) {
      const RpcIdHelper & rpcIdHelper = m_muonIdHelper->rpcIdHelper();
      switch (rpcIdHelper.stationName(id)) {
      case 2: // BML
      case 3: // BMS
      case 8: // BMF
	if (rpcIdHelper.doubletR(id) == 2) { // RPC2
	  isPivot = true;
	}
	break;

      default:
	isPivot = false;
	break;
      }
    }
      
    if (isTgc) {
      const TgcIdHelper & tgcIdHelper = m_muonIdHelper->tgcIdHelper();
      switch (tgcIdHelper.stationName(id)) {
      case 45: // T3F
      case 46: // T3E
	isPivot = true;
	break;

      default:
	isPivot = false;
	break;
      }
    }

    if (pivotOnly) {
      if (isPivot) {
        rots.push_back(rot);
      }
    } else {
      rots.push_back(rot);
    }

  } // TrackStateOnSurface

  return StatusCode::SUCCESS;
}


int
TrigEffJpsiTools::getL1PtThreshold(const std::string& ptname) const
{
  int pt = -1;

  if (ptname == "L1_MU0_COMM" or ptname == "MU0_COMM") {
    // RPC commissioning trigger, 3-station full open
    // PT3 MU10, PT4 MU0_COMM, PT5 MU15 in early 2010 data taking
    // assign 11 GeV for this trigger to get valid integer.
    return 11;
  }

  const std::string prefix = "MU";
  const size_t found = ptname.find(prefix);
  if (found == std::string::npos) {
    ATH_MSG_WARNING("getL1PtThreshold: cannot parse input string: " << ptname);
    return pt;
  }

  const std::string delimiter = "_";
  const size_t first = ptname.find_first_of(delimiter);
  if (first != ptname.find_last_of(delimiter)) {
    ATH_MSG_DEBUG("getL1PtThreshold: skip " << ptname);
    return pt;  // consider single muon trigger only
  }
  
  size_t offset = found + prefix.size();
  std::string value = ptname.substr(offset);
    
  std::istringstream iss(value);
  if (not (iss >> pt)) {
    ATH_MSG_WARNING("getL1PtThreshold: Failed int conversion " << value);
  }

  ATH_MSG_DEBUG("getL1PtThreshold: " << ptname << " = " << pt);

  return pt;
}


bool
TrigEffJpsiTools::isConfigured(const std::string& chainName,
                               const std::string& prefix) const
{
  if (chainName.find(prefix) != 0) {
    ATH_MSG_WARNING("isConfigured: unexpected chain name: " << chainName);
    return false;
  }

  const Trig::ChainGroup* chain = m_trigDecisionTool->getChainGroup(chainName);
  if (chain->getListOfTriggers().empty()) {
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("isConfigured: available triggers for " + prefix);
      const Trig::ChainGroup *chain = m_trigDecisionTool->getChainGroup(prefix + "_.*");
      std::vector<std::string> tmpChainName = chain->getListOfTriggers();
      for (size_t ii = 0; ii < tmpChainName.size(); ii++) {
        ATH_MSG_DEBUG("                   " << tmpChainName.at(ii));
      }
    }

    ATH_MSG_WARNING("isConfigured: " << chainName << " not configured");
    return false;
  }

  ATH_MSG_DEBUG("isConfigured: " << chainName << " configured");
  return true;
}


bool
TrigEffJpsiTools::getMatchedRoI(const Rec::TrackParticle* track,
                                struct Match& match) const
{
  ATH_MSG_DEBUG("getMatchedRoI(private)");

  match.roiIndex = -1;
  match.roiWord = 0;
  match.roiThrNumber = -1;
  
  if (!track) {
    ATH_MSG_WARNING("getMatchedRoI: track pointer is null");
    return false;
  }

  double searchRange[NUM_REGION] = {0.};
  double etaAtPivot[NUM_REGION] = {0.};
  double phiAtPivot[NUM_REGION] = {0.};
  double detaAtPivot[NUM_REGION] = {0.};
  double dphiAtPivot[NUM_REGION] = {0.};

  if (match.etaExTrk) {
    (*(match.etaExTrk))[Trk::locX] = -1.e30;
    (*(match.etaExTrk))[Trk::locY] = -1.e30;
  }

  if (match.phiExTrk) {
    (*(match.phiExTrk))[Trk::locX] = -1.e30;
    (*(match.phiExTrk))[Trk::locY] = -1.e30;
  }

  // the extrapolation
  double pivotZ = track->eta() < 0. ? -m_endcapPivotPlaneZ : m_endcapPivotPlaneZ;
  const Trk::AtaDisc* disc = extrapolateToEndcap(track, pivotZ);
  double pivotRadius = m_barrelPivotPlaneRadius;
  const Trk::AtaCylinder* cylinder = extrapolateToBarrel(track, pivotRadius);

  if (!disc and !cylinder) return false;

  if (disc) {
    Amg::Vector2D dEtaPhi;
    getExtrapolationErrorOnEndcap(disc, &dEtaPhi);

    searchRange[ENDCAP] = getSearchRangeOnEndcap(disc);
    etaAtPivot[ENDCAP] = disc->position().eta();
    phiAtPivot[ENDCAP] = disc->position().phi();
    detaAtPivot[ENDCAP] = dEtaPhi.x();
    dphiAtPivot[ENDCAP] = dEtaPhi.y();
    match.endcap = true;

    if (match.etaExTrk) {
      (*(match.etaExTrk))[Trk::locX] = etaAtPivot[ENDCAP];
      (*(match.etaExTrk))[Trk::locY] = detaAtPivot[ENDCAP];
    }
    if (match.phiExTrk) {
      (*(match.phiExTrk))[Trk::locX] = phiAtPivot[ENDCAP];
      (*(match.phiExTrk))[Trk::locY] = dphiAtPivot[ENDCAP];
    }
    ATH_MSG_DEBUG("getMatchedRoI: endcap: search range = " << searchRange[ENDCAP]);
    //delete disc;
  }

  if (cylinder) {
    Amg::Vector2D dEtaPhi;
    getExtrapolationErrorOnBarrel(cylinder, &dEtaPhi);

    searchRange[BARREL] = getSearchRangeOnBarrel(cylinder);
    etaAtPivot[BARREL] = cylinder->position().eta();
    phiAtPivot[BARREL] = cylinder->position().phi();
    detaAtPivot[BARREL] = dEtaPhi.x();
    dphiAtPivot[BARREL] = dEtaPhi.y();
    match.barrel = true;

    if (match.etaExTrk) {
      (*(match.etaExTrk))[Trk::locX] = etaAtPivot[BARREL];
      (*(match.etaExTrk))[Trk::locY] = detaAtPivot[BARREL];
    }
    if (match.phiExTrk) {
      (*(match.phiExTrk))[Trk::locX] = phiAtPivot[BARREL];
      (*(match.phiExTrk))[Trk::locY] = dphiAtPivot[BARREL];
    }
    ATH_MSG_DEBUG("getMatchedRoI: barrel: search range = " << searchRange[BARREL]);
    //delete cylinder;
  }

  // get level1 muon rois
  const LVL1_ROI* lvl1Roi = 0;
  if (evtStore()->retrieve(lvl1Roi, m_Lvl1RoiName).isFailure() or
      !lvl1Roi) {
    ATH_MSG_WARNING("Failed to retrieve " << m_Lvl1RoiName);
    return false;
  }  
  LVL1_ROI::muons_type muonRoIs = lvl1Roi->getMuonROIs();

  // search for the closest roi
  typedef LVL1_ROI::muons_type::const_iterator iterator;
  iterator cit = muonRoIs.begin();
  iterator cit_end = muonRoIs.end();
  double closest = -1;
  for (int nRoi = 0; cit != cit_end; cit++, nRoi++) {
    const Muon_ROI* roi = &(*cit);
    ATH_MSG_DEBUG("getMatchedRoI: " << *roi);
    double roiEta = roi->eta(); 
    double roiPhi = roi->phi();
    int roiRegion = (roi->getSource() == Muon_ROI::Barrel) ? BARREL : ENDCAP;
    double dr = getEtaPhiDistance(etaAtPivot[roiRegion], roiEta,
                                  phiAtPivot[roiRegion], roiPhi);
    if (dr > searchRange[roiRegion]) continue; 
    if (closest < 0. or dr < closest) {
      closest = dr;
      match.roiIndex = nRoi;
      if (match.etaExTrk) {
        (*(match.etaExTrk))[Trk::locX] = etaAtPivot[roiRegion];
        (*(match.etaExTrk))[Trk::locY] = detaAtPivot[roiRegion];
      }
      if (match.phiExTrk) {
        (*(match.phiExTrk))[Trk::locX] = phiAtPivot[roiRegion];
        (*(match.phiExTrk))[Trk::locY] = dphiAtPivot[roiRegion];
      }
    }
  }

  if (match.roiIndex < 0) return false;

  const Muon_ROI roi = muonRoIs.at(match.roiIndex);
  match.roiWord = roi.getROIWord();
  match.roiThrNumber = roi.getThrNumber();
  match.roiThrValue = getL1PtThreshold(roi.getThrName());
  match.roiEta = roi.getEta();
  match.roiPhi = roi.getPhi();
  if (roi.getSource() == Muon_ROI::Barrel) {
    match.barrel = true;
    match.endcap = false;
  } else {
    match.barrel = false;
    match.endcap = true;
  }
  ATH_MSG_DEBUG("getMatchedRoI: matched " << &roi << " : " << roi);

  return true;
}



bool
TrigEffJpsiTools::isTriggeredMuonHLT(const Rec::TrackParticle* track,
                                     const std::string& chainName,
                                     const Trig::ChainGroup* cg) const
{
  struct Match match;
  if (not getMatchedRoI(track, match)) {
    ATH_MSG_DEBUG("isTriggeredMuonHLT:getMatchedRoI: no " << chainName);
    return false;
  }
  ATH_MSG_DEBUG("match roi: " << match.roiIndex);


  const Trig::FeatureContainer fc = cg->features(TrigDefs::alsoDeactivateTEs);
  const std::vector<Trig::Combination>& combinations = fc.getCombinations();
  ATH_MSG_DEBUG("isTriggeredMuonHLT: number of combinations:" << combinations.size());
  bool isTriggered = false;

  std::vector<Trig::Combination>::const_iterator cit;
  for (cit = combinations.begin(); cit != combinations.end(); cit++) {
    const Trig::Combination& combination = *cit;
    if (not combination.active()) {
      ATH_MSG_DEBUG("isTriggeredMuonHLT: combination " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("isTriggeredMuonHLT: combination " << combination << " is active");

    if (msgLvl(MSG::DEBUG)) {
      const std::vector<const HLT::TriggerElement*>& tes = combination.tes();
      for (size_t jj = 0; jj < tes.size(); jj++) {
        ATH_MSG_DEBUG("TE name[" << jj << "] = " << Trig::getTEName(*(tes.at(jj))));
      }
    }


    const std::vector<Trig::Feature<TrigRoiDescriptor> > features =
      combination.get<TrigRoiDescriptor>("initialRoI", TrigDefs::alsoDeactivateTEs);
    ATH_MSG_DEBUG("isTriggeredMuonHLT: number of muon features: " << features.size());

    for (size_t ii = 0; ii < features.size(); ii++) {
      const Trig::Feature<TrigRoiDescriptor> feature = features[ii];
      const TrigRoiDescriptor* roi = feature.cptr();
      const HLT::TriggerElement* te = features[ii].te();
      ATH_MSG_DEBUG("feature: " << feature);
      ATH_MSG_DEBUG("roi word: " << roi->roiWord() << " ref: " << match.roiWord);
      ATH_MSG_DEBUG("TE name(): " << Trig::getTEName(*te));

      if (roi->roiWord() == match.roiWord) {
        isTriggered = true;
        ATH_MSG_DEBUG("isTriggeredMuonHLT: " << chainName << " triggered");
        break;
      }
    }
    if (isTriggered) break;
  }

  return isTriggered;
}



bool
TrigEffJpsiTools::isTriggeredMuonHLTwMuonFeature(const Rec::TrackParticle* track,
                                                 const std::string& chainName,
                                                 const Trig::ChainGroup* cg) const
{
  struct Match match;
  if (not getMatchedRoI(track, match)) {
    ATH_MSG_DEBUG("isTriggeredMuonHLTwMuonFeature:getMatchedRoI: no " << chainName);
    return false;
  }
  ATH_MSG_DEBUG("match roi: " << match.roiIndex);


  const Trig::FeatureContainer fc = cg->features(TrigDefs::alsoDeactivateTEs);
  const std::vector<Trig::Combination>& combinations = fc.getCombinations();
  ATH_MSG_DEBUG("isTriggeredMuonHLTwMuonFeature: number of combinations:" << combinations.size());
  bool isTriggered = false;

  std::vector<Trig::Combination>::const_iterator cit;
  for (cit = combinations.begin(); cit != combinations.end(); cit++) {
    const Trig::Combination& combination = *cit;
    if (not combination.active()) {
      ATH_MSG_DEBUG("isTriggeredMuonHLTwMuonFeature: combination " << combination << " is not active");
      continue;
    }
    ATH_MSG_DEBUG("isTriggeredMuonHLTwMuonFeature: combination " << combination << " is active");

    if (msgLvl(MSG::DEBUG)) {
      const std::vector<const HLT::TriggerElement*>& tes = combination.tes();
      for (size_t jj = 0; jj < tes.size(); jj++) {
        ATH_MSG_DEBUG("TE name[" << jj << "] = " << Trig::getTEName(*(tes.at(jj))));
      }
    }


    const std::vector<Trig::Feature<MuonFeature> > features =
      combination.get<MuonFeature>("", TrigDefs::alsoDeactivateTEs);
    ATH_MSG_DEBUG("isTriggeredMuonHLTwMuonFeature: number of muon features: " << features.size());

    for (size_t ii = 0; ii < features.size(); ii++) {
      const Trig::Feature<MuonFeature> feature = features[ii];
      const HLT::TriggerElement* te = features[ii].te();
      Trig::Feature<Muon_ROI> roiFeature = m_trigDecisionTool->ancestor<Muon_ROI>(te);
      const Muon_ROI* roi = roiFeature.cptr(); 
      //const MuonFeature* mf = feature.cptr();
      //ATH_MSG_DEBUG("MuonFeature: " << str(mf) << " algoid = " << mf->algoId()); // MuonFeatureDetails.h
      ATH_MSG_DEBUG("feature: " << feature);
      ATH_MSG_DEBUG("roi word: " << roi->getROIWord() << " ref: " << match.roiWord);
      ATH_MSG_DEBUG("TE name(): " << Trig::getTEName(*te));

      if (roi->getROIWord() == match.roiWord) {
        isTriggered = true;
        ATH_MSG_DEBUG("isTriggeredMuonHLTwMuonFeature: " << chainName << " triggered");
        break;
      }
    }
    if (isTriggered) break;
  }

  return isTriggered;
}



bool
TrigEffJpsiTools::addTrigObject(TrigObject& trig, TrigObjectMap* map, bool appendIfExists) const
{
  TrigObjectMap::iterator mIt = map->find( trig.id );
  if (mIt == map->end()) { // Element does not exist in map
    ATH_MSG_DEBUG( "track not in map, will add , comb Id: " << trig.id);
    std::pair<TrigObjectMap::iterator, bool> rc;
    rc = map->insert(TrigObjectPair(trig.id, trig));
    if (not rc.second) {
      ATH_MSG_WARNING( "pointerMap: map key " << trig.id << " duplicated ");
    }
    return true;
  } else { // Element exists in map
    if (appendIfExists) {
      // Add element's name and combinations to existing element
      ATH_MSG_DEBUG( "track already in map will add name, combId " << trig.id);
      mIt->second.names.push_back(trig.names[0]);
      //mIt->second.daughterIds.push_back(trig.daughterIds[0]);
      mIt->second.combIds.push_back(trig.combIds[0]);
      mIt->second.efObjIds.push_back(trig.efObjIds[0]);
    }
    return false;
  }
}


/**
 * @param data_word Data word from the MuCTPI_RDO object
 * @returns The RoI word created from the data word
 */
uint32_t
TrigEffJpsiTools::RDOtoRoI(uint32_t data_word) const
{
  // MuCTPI_RDO to MuCTPIRoI
  // ref: Fig 3.6 and Fig 3.8 in https://edms.cern.ch/file/248757/1/mirod.pdf
  return (((data_word & 0x8000000) >> 4) | ((data_word & 0x3fe0000) >> 3) |
          (data_word & 0x3fff));
}


bool
TrigEffJpsiTools::EF_mu4_MSOnly_with_Muid(const MuonFeature* mf) const
{
  // apply hypo cut
  if (not hasPassedMuFast(mf)) return false;


  // find matching level roi
  const LVL1_ROI* lvl1Roi = 0;
  if (evtStore()->retrieve(lvl1Roi, m_Lvl1RoiName).isFailure() or
      !lvl1Roi) {
    ATH_MSG_WARNING("TrigEffJpsiTools::EF_mu4_MSOnly_with_Muid: Failed to retrieve " << m_Lvl1RoiName);
    return false;
  }  

  LVL1_ROI::muons_type muonRoIs = lvl1Roi->getMuonROIs();
  typedef LVL1_ROI::muons_type::const_iterator roi_it;
  const double eta = mf->eta();
  const double phi = mf->phi();
  double minDist = 1.e30;
  for (roi_it cit = muonRoIs.begin(); cit != muonRoIs.end(); cit++) {
    double dist = getEtaPhiDistance(cit->eta(), eta, cit->phi(), phi);
    if (dist < minDist) minDist = dist;
  }

  if (minDist > 0.5) return false;


  // find matching muid muon
  const Analysis::MuonContainer* muons = 0;
  if (evtStore()->retrieve(muons, "MuidMuonCollection").isFailure() or
      !muons) {
    ATH_MSG_WARNING("TrigEffJpsiTools::EF_mu4_MSOnly_with_MuidSA: Failed to retrieve MuidMuonCollection");
    return false;
  }

  minDist = 0.1;
  typedef Analysis::MuonContainer::const_iterator mc_it;
  for (mc_it cit = muons->begin(); cit != muons->end(); cit++) {
    const Analysis::Muon* muon = *cit;
    uint16_t allAuthors = muon->allAuthors();
    if ((allAuthors & (0x1 << 10)) or // MuTagIMO
        (allAuthors & (0x1 << 3)) or  // MuidSA
        (allAuthors & (0x1 << 4))) {  // MuidCo
      double dist = getEtaPhiDistance(muon->eta(), eta, muon->phi(), phi);
      if (dist < minDist) {
        const Rec::TrackParticle* tp;
        tp = muon->muonSpectrometerTrackParticle();
        if (!tp) tp = muon->innerExtrapolatedTrackParticle();
        if (!tp) tp = muon->inDetTrackParticle();
        if (hasPassedTrigMuonEF(tp)) return true;
      }
    }
  }

  return false;
}


bool
TrigEffJpsiTools::hasPassedMuFast(const MuonFeature* mf) const
{
  // https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigHypothesis/TrigMuonHypo/trunk/python/TrigMuonHypoConfig.py
  double pt = mf->pt();
  double eta = std::abs(mf->eta());

  if (eta < 1.05) {
    if (pt < 3.0) return false;

  } else if (eta < 1.5) {
    if (pt < 2.5) return false;

  } else if (eta < 2.0) {
    if (pt < 2.5) return false;

  } else if (eta < 9.9) {
    if (pt < 3.0) return false;

  } else {
    return false;
  }

  return true;
}


bool
TrigEffJpsiTools::hasPassedTrigMuonEF(const Rec::TrackParticle* tp) const
{
  // https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigHypothesis/TrigMuonHypo/trunk/python/TrigMuonHypoConfig.py
  double pt = tp->pt();
  double eta = std::abs(tp->eta());

  if (eta < 1.05) {
    if (pt < 3.0) return false;

  } else if (eta < 1.5) {
    if (pt < 2.5) return false;

  } else if (eta < 2.0) {
    if (pt < 2.5) return false;

  } else if (eta < 9.9) {
    if (pt < 2.5) return false;

  } else {
    return false;
  }

  return true;
}

// **************************************************************************
bool
TrigEffJpsiTools::selectL2TrigDiMuon(const TrigL2Bphys* trigger,
				     const TrigInDetTrack* track1,
				     const TrigInDetTrack* track2,
				     bool checkOS, // if true, tracks must have opposite charge
				     double massMin, // invariant mass lower limit
				     double massMax, // invariant mass upper limit
				     bool applyMassMax, // apply upper invariant mass cut
				     int nHits, // number of matching muon hits
				     double chi2) const // chi2 cut. If < 0, no cut is applied
{
  // apply chi2 cut if requested
  if(chi2>=0 && trigger->fitchi2() > chi2) return false;

  //apply invariant mass cut
  if(trigger->mass() < massMin) return false;
  if(applyMassMax && trigger->mass()>massMax) return false;

  //apply OS cut
  if (checkOS && track1->param()->pT() * track2->param()->pT() >= 0.0) return false;

  //apply number of hits cut
  // if nHits<0 then don't apply cut - used for topological triggers
  if (nHits > 0) {
    // decode number of hits
    int32_t word1 = track1->HitPattern();
    Encoding* pEncoding1 = (Encoding*)&word1;

    int32_t word2 = track2->HitPattern();
    Encoding* pEncoding2 = (Encoding*)&word2;

    // Make sure to cast to int32_t!
    int32_t nMdtHits[2] = { static_cast<int32_t>(pEncoding1->mdt), static_cast<int32_t>(pEncoding2->mdt) } ;
    int32_t nRpcHits[2] = { static_cast<int32_t>(pEncoding1->rpc), static_cast<int32_t>(pEncoding2->rpc) } ;
    int32_t nTgcHits[2] = { static_cast<int32_t>(pEncoding1->tgc), static_cast<int32_t>(pEncoding2->tgc) } ;
    int32_t nHits1 = nMdtHits[0] + nRpcHits[0] + nTgcHits[0];
    int32_t nHits2 = nMdtHits[1] + nRpcHits[1] + nTgcHits[1];
    if ( nHits1 < nHits || nHits2 < nHits) return false;
    ATH_MSG_VERBOSE("nMdtHits: " << nMdtHits[0] << ", " << nMdtHits[1]);
    ATH_MSG_VERBOSE("nRpcHits: " << nRpcHits[0] << ", " << nRpcHits[1]);
    ATH_MSG_VERBOSE("nTgcHits: " << nTgcHits[0] << ", " << nTgcHits[1]);
    ATH_MSG_VERBOSE("Sum:      " << nHits1      << ", " << nHits2);
  }
  // if we got this far, candidate was accepted
  ATH_MSG_DEBUG("This candidate passed hypo selection " );
  return true;

}
// **************************************************************************
bool
TrigEffJpsiTools::selectEFTrigDiMuon(const TrigEFBphys* trigger,
				     const Rec::TrackParticle* track1,
				     const Rec::TrackParticle* track2,
				     bool checkOS, // if true, tracks must have opposite charge
				     double massMin, // invariant mass lower limit
				     double massMax, // invariant mass upper limit
				     bool applyMassMax, // apply upper invariant mass cut
				     double chi2) const // chi2 cut. If < 0, no cut is applied
{
  // apply chi2 cut if requested
  if(chi2>=0 && trigger->fitchi2() > chi2) return false;

  //apply invariant mass cut
  if(trigger->mass() < massMin) return false;
  if(applyMassMax && trigger->mass()>massMax) return false;

  //apply OS cut
  if (checkOS && track1->measuredPerigee()->parameters()[Trk::qOverP] * track2->measuredPerigee()->parameters()[Trk::qOverP] >= 0.0) return false;

  // if we got this far, candidate was accepted
  ATH_MSG_DEBUG("This candidate passed hypo selection " );
  return true;

}

// **************************************************************************
void
TrigEffJpsiTools::Btrig_L2_initCuts()
{
  ATH_MSG_DEBUG("Use manual hypo cuts: " << m_useManualHypoCuts);
  //L2_mu4_DiMu
  m_Btrig_L2_chain.push_back("L2_mu4_DiMu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(500.);
  m_Btrig_L2_massMax.push_back(13000.);
  m_Btrig_L2_applyMassMax.push_back(false);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);

  //L2_mu4_Jpsimumu
  m_Btrig_L2_chain.push_back("L2_mu4_Jpsimumu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(2500.);
  m_Btrig_L2_massMax.push_back(4300.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);

  //L2_mu4_Upsimumu
  m_Btrig_L2_chain.push_back("L2_mu4_Upsimumu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(8000.);
  m_Btrig_L2_massMax.push_back(12000.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);

  //L2_mu4_Bmumu
  m_Btrig_L2_chain.push_back("L2_mu4_Bmumu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(8000.);
  m_Btrig_L2_massMax.push_back(12000.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);

  //L2_2mu4_DiMu
  m_Btrig_L2_chain.push_back("L2_2mu4_DiMu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(500.);
  m_Btrig_L2_massMax.push_back(13000.);
  m_Btrig_L2_applyMassMax.push_back(false);
  m_Btrig_L2_nHits.push_back(-1);
  m_Btrig_L2_chi2.push_back(20.);

  //L2_2mu4_Jpsimumu
  m_Btrig_L2_chain.push_back("L2_2mu4_Jpsimumu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(2500.);
  m_Btrig_L2_massMax.push_back(4300.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(-1);
  m_Btrig_L2_chi2.push_back(20.);

  //L2_2mu4_Upsimumu
  m_Btrig_L2_chain.push_back("L2_2mu4_Upsimumu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(8000.);
  m_Btrig_L2_massMax.push_back(12000.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(-1);
  m_Btrig_L2_chi2.push_back(20.);

  //L2_2mu4_Bmumu
  m_Btrig_L2_chain.push_back("L2_2mu4_Bmumu");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(4000.);
  m_Btrig_L2_massMax.push_back(7000.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(-1);
  m_Btrig_L2_chi2.push_back(20.);


  //L2_mu4_DiMu_FS
  m_Btrig_L2_chain.push_back("L2_mu4_DiMu_FS");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(500.);
  m_Btrig_L2_massMax.push_back(13000.);
  m_Btrig_L2_applyMassMax.push_back(false);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);

  //L2_mu4_Jpsimumu_FS
  m_Btrig_L2_chain.push_back("L2_mu4_Jpsimumu_FS");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(2500.);
  m_Btrig_L2_massMax.push_back(4300.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);

  //L2_mu4_Upsimumu_FS
  m_Btrig_L2_chain.push_back("L2_mu4_Upsimumu_FS");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(8000.);
  m_Btrig_L2_massMax.push_back(12000.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);

  //L2_mu4_Bmumu_FS
  m_Btrig_L2_chain.push_back("L2_mu4_Bmumu_FS");
  m_Btrig_L2_checkOS.push_back(true);
  m_Btrig_L2_massMin.push_back(4000.);
  m_Btrig_L2_massMax.push_back(7000.);
  m_Btrig_L2_applyMassMax.push_back(true);
  m_Btrig_L2_nHits.push_back(3);
  m_Btrig_L2_chi2.push_back(-1.);



  return;

}

void
TrigEffJpsiTools::Btrig_EF_initCuts()
{
  ATH_MSG_DEBUG("Use manual hypo cuts: " << m_useManualHypoCuts);
  //Ef_mu4_DiMu
  m_Btrig_EF_chain.push_back("EF_mu4_DiMu");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(500.);
  m_Btrig_EF_massMax.push_back(13000.);
  m_Btrig_EF_applyMassMax.push_back(false);
  m_Btrig_EF_chi2.push_back(-1.);


  //EF_mu4_Jpsimumu
  m_Btrig_EF_chain.push_back("EF_mu4_Jpsimumu");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(2500.);
  m_Btrig_EF_massMax.push_back(4300.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(-1.);

  //EF_mu4_Bmumu
  m_Btrig_EF_chain.push_back("EF_mu4_Bmumu");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(4000.);
  m_Btrig_EF_massMax.push_back(7000.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(-1.);

  //EF_2mu4_DiMu
  m_Btrig_EF_chain.push_back("EF_2mu4_DiMu");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(500.);
  m_Btrig_EF_massMax.push_back(13000.);
  m_Btrig_EF_applyMassMax.push_back(false);
  m_Btrig_EF_chi2.push_back(20.);

  //EF_2mu4_Bmumu
  m_Btrig_EF_chain.push_back("EF_2mu4_Bmumu");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(4000.);
  m_Btrig_EF_massMax.push_back(7000.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(20.);

  //EF_2mu4_Jpsimumu
  m_Btrig_EF_chain.push_back("EF_2mu4_Jpsimumu");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(2500.);
  m_Btrig_EF_massMax.push_back(4300.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(20.);

  //EF_2mu4_Upsimumu
  m_Btrig_EF_chain.push_back("EF_2mu4_Upsimumu");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(8000.);
  m_Btrig_EF_massMax.push_back(12000.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(20.);

  //EF_mu4_DiMumumu_FS
  m_Btrig_EF_chain.push_back("EF_mu4_DiMu_FS");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(500.);
  m_Btrig_EF_massMax.push_back(13000.);
  m_Btrig_EF_applyMassMax.push_back(false);
  m_Btrig_EF_chi2.push_back(-1.);

  //EF_mu4_Jpsimumu_FS
  m_Btrig_EF_chain.push_back("EF_mu4_Jpsimumu_FS");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(2500.);
  m_Btrig_EF_massMax.push_back(4300.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(-1.);

  //EF_mu4_Upsimumu_FS
  m_Btrig_EF_chain.push_back("EF_mu4_Upsimumu_FS");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(8000.);
  m_Btrig_EF_massMax.push_back(12000.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(-1.);

  //EF_mu4_Bmumu_FS
  m_Btrig_EF_chain.push_back("EF_mu4_Bmumu_FS");
  m_Btrig_EF_checkOS.push_back(true);
  m_Btrig_EF_massMin.push_back(4000.);
  m_Btrig_EF_massMax.push_back(7000.);
  m_Btrig_EF_applyMassMax.push_back(true);
  m_Btrig_EF_chi2.push_back(-1.);



  return;

}




// **************************************************************************

/* eof */
