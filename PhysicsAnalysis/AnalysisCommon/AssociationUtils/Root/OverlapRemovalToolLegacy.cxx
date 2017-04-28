// EDM includes
#include "AthContainers/AuxElement.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

// Local includes
#include "AssociationUtils/OverlapRemovalTool.h"

const float invGeV = 0.001;

//-----------------------------------------------------------------------------
// Standard constructor
//-----------------------------------------------------------------------------
OverlapRemovalTool::OverlapRemovalTool(const std::string& name)
        : asg::AsgTool(name),
          m_inputAccessor(nullptr),
          m_outputDecorator(nullptr),
          m_bJetAccessor(nullptr),
          m_overlapLinkDecorator(nullptr),
          m_sharedTrackDecorator(nullptr),
          m_sharedTrackAccessor(nullptr)
{
  // Input/output labels
  declareProperty("InputLabel", m_inputLabel = "selected");
  declareProperty("OverlapLabel", m_overlapLabel = "overlaps",
                  "Decoration given to objects that fail OR");
  // BJet label
  declareProperty("BJetLabel", m_bJetLabel = "",
                  "Input b-jet flag. Disable by default.");
  // Overlap object link decorations
  declareProperty("LinkOverlapObjects", m_linkOverlapObjects = false,
                  "Turn on overlap object link decorations");
  // El-mu shared track decorations
  declareProperty("WriteSharedTrackFlag", m_writeSharedTrackFlag = false,
                 "Turn on electron flag if track overlaps with muon track");
  declareProperty("UseSharedTrackFlag", m_useSharedTrackFlag = false,
                 "Read electron flag for muon-electron track OR.");
  // dR cones for defining overlap
  declareProperty("ElectronJetDRCone",    m_electronJetDR    = 0.2);
  declareProperty("JetElectronDRCone",    m_jetElectronDR    = 0.4);
  declareProperty("MuonJetDRCone",        m_muonJetDR        = 0.4);
  declareProperty("TauJetDRCone",         m_tauJetDR         = 0.2);
  declareProperty("TauElectronDRCone",    m_tauElectronDR    = 0.2);
  declareProperty("TauMuonDRCone",        m_tauMuonDR        = 0.2);
  declareProperty("PhotonElectronDRCone", m_photonElectronDR = 0.4);
  declareProperty("PhotonMuonDRCone",     m_photonMuonDR     = 0.4);
  declareProperty("PhotonPhotonDRCone",   m_photonPhotonDR   = 0.4);
  declareProperty("PhotonJetDRCone",      m_photonJetDR      = 0.4);

  // Selection properties
  const float GeV = 1000.;
  declareProperty("TauElectronOverlapID", m_tauEleOverlapID = "LHLoose",
                  "Electron ID selection for tau-ele OR");
  declareProperty("TauMuonOverlapMuPtMin", m_tauMuOverlapMuPt = 2.*GeV,
                  "Muon pt threshold for tau-muon OR");
  declareProperty("TauMuonOverlapTauPt", m_tauMuOverlapTauPt = 50.*GeV,
                  "Tau pt above which only combined muons are considered");
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
OverlapRemovalTool::~OverlapRemovalTool()
{
  delete m_inputAccessor;
  delete m_outputDecorator;
  delete m_bJetAccessor;
  delete m_overlapLinkDecorator;
  delete m_sharedTrackDecorator;
  delete m_sharedTrackAccessor;
}

//-----------------------------------------------------------------------------
// Initialize the tool
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::initialize()
{
  if(!m_inputLabel.empty())
    m_inputAccessor = new ort::inputAccessor_t(m_inputLabel);
  if(!m_bJetLabel.empty())
    m_bJetAccessor = new ort::inputAccessor_t(m_bJetLabel);
  m_outputDecorator = new ort::outputDecorator_t(m_overlapLabel);
  m_overlapLinkDecorator = new ort::objLinkDecorator_t("overlapObject");
  // These flags could be done optional as well
  m_sharedTrackDecorator = new ort::sharedTrackDecorator_t("hasSharedTrack");
  m_sharedTrackAccessor = new ort::sharedTrackAccessor_t("hasSharedTrack");

  // Consistency check
  if(m_writeSharedTrackFlag && m_useSharedTrackFlag) {
    ATH_MSG_ERROR("Need to decide: either write or read flag " <<
                  "on muon-electron track OR!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove all overlapping objects according to the official
// harmonization prescription
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::
removeOverlaps(const xAOD::ElectronContainer* electrons,
               const xAOD::MuonContainer* muons,
               const xAOD::JetContainer* jets,
               const xAOD::TauJetContainer* taus,
               const xAOD::PhotonContainer* photons)
{
  return removeOverlaps(electrons, muons, jets, taus,
                        electrons, muons, photons);
}
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::
removeOverlaps(const xAOD::ElectronContainer* electrons,
               const xAOD::MuonContainer* muons,
               const xAOD::JetContainer* jets,
               const xAOD::TauJetContainer* taus,
               const xAOD::ElectronContainer* looseElectrons,
               const xAOD::MuonContainer* looseMuons,
               const xAOD::PhotonContainer* photons)
{
  // Check pointer validity. I can add more flexibility later,
  // but for now, if users don't want to use one of these object types,
  // they can use the lower-level methods instead.
  if(!electrons || !muons || !jets){
    ATH_MSG_ERROR("Encountered NULL pointer in required object");
    return StatusCode::FAILURE;
  }
  if(taus && !(looseElectrons && looseMuons)){
    ATH_MSG_ERROR("Taus provided but loose leptons are NULL!");
    return StatusCode::FAILURE;
  }

  // Reset all decorations to passing
  resetDecorations(*electrons);
  resetDecorations(*muons);
  resetDecorations(*jets);
  if(photons) resetDecorations(*photons);
  if(taus) resetDecorations(*taus);

  // Check that we have what we need for the track-sharing decoration
  if(m_useSharedTrackFlag){
    // Check that electron is decorated for this mode
    if(!checkSharesTrackDecoration(*electrons)){
      ATH_MSG_ERROR("Flag for electron-muon track OR not found in input!");
      if(checkTrackParticles(*electrons))
        ATH_MSG_ERROR("But TrackParticles are available in input to run " <<
                      "nominal muon-electron OR!");
      return StatusCode::FAILURE;
    }
  }

  // Following recommended removal sequence from harmonization note.
  // NOTE: some details are still unspecific in the recommendations.
  // Talk to me if this does not support your needs.

  // Tau and loose ele/mu OR
  if(taus){
    ATH_CHECK( removeTauEleOverlap(*taus, *looseElectrons) );
    ATH_CHECK( removeTauMuonOverlap(*taus, *looseMuons) );
  }
  // e-mu OR
  ATH_CHECK( removeEleMuonOverlap(*electrons, *muons) );
  // photon and e/mu OR
  if(photons){
    // TODO: need a pho-tau OR?
    // Turning off pho-pho for now. Resolved by reco..?
    //ATH_CHECK( removePhotonPhotonOverlap(*photons) );
    ATH_CHECK( removePhotonEleOverlap(*photons, *electrons) );
    ATH_CHECK( removePhotonMuonOverlap(*photons, *muons) );
  }
  // lep/photon and jet OR
  ATH_CHECK( removeEleJetOverlap(*electrons, *jets) );
  ATH_CHECK( removeMuonJetOverlap(*muons, *jets) );
  if(taus) ATH_CHECK( removeTauJetOverlap(*taus, *jets) );
  if(photons) ATH_CHECK( removePhotonJetOverlap(*photons, *jets) );
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping electrons and jets
// Need two steps so as to avoid using rejected jets in the 2nd step.
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removeEleJetOverlap
(const xAOD::ElectronContainer& electrons, const xAOD::JetContainer& jets)
{
  ATH_MSG_DEBUG("Removing overlapping electrons and jets");

  // Initialize output decoration if necessary
  initializeDecorations(electrons);
  initializeDecorations(jets);

  // Remove jets that overlap with electrons
  for(const auto jet : jets){
    // Check that this jet passes the input selection
    if(isSurvivingObject(jet) && !isBJet(jet)){
      // Use the generic OR method
      if(objectOverlaps(jet, electrons, m_electronJetDR)){
        ATH_MSG_DEBUG("  Found overlap jet: " << jet->pt()*invGeV);
        setObjectFail(jet);
      }
    }
  }

  // Remove electrons that overlap with surviving jets
  // Maybe this should get its own method.
  for(const auto electron : electrons){
    // Check that this electron passes the input selection
    if(isSurvivingObject(electron)){
      // Use the generic OR method
      if(objectOverlaps(electron, jets, m_jetElectronDR)){
        ATH_MSG_DEBUG("  Found overlap ele: " << electron->pt()*invGeV);
        setObjectFail(electron);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping muons and jets
// Note that because of the numTrack requirement on the jet,
// we are able to do this in just one double loop, unlike ele-jet.
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removeMuonJetOverlap
(const xAOD::MuonContainer& muons, const xAOD::JetContainer& jets)
{
  ATH_MSG_DEBUG("Removing overlapping muons and jets");

  // Initialize output decoration if necessary
  initializeDecorations(muons);
  initializeDecorations(jets);

  // Convenience macro
  #define numTrk(jet) \
    jet->getAttribute< std::vector<int> >(xAOD::JetAttribute::NumTrkPt500)[0]

  // Remove jets that overlap with muons
  for(const auto jet : jets){
    // Only remove low-trk-multiplicity jets
    if(isSurvivingObject(jet) && numTrk(jet) <= 2){
      if(objectOverlaps(jet, muons, m_muonJetDR)){
        ATH_MSG_DEBUG("  Found overlap jet: " << jet->pt()*invGeV);
        setObjectFail(jet);
      }
    }
  }
  // Remove muons that overlap with jets
  for(const auto muon : muons){
    if(isSurvivingObject(muon)){
      for(const auto jet : jets){
        // Only remove from high-trk-multiplicity jets
        if(isSurvivingObject(jet) && numTrk(jet) > 2){
          if(objectsOverlap(muon, jet, m_muonJetDR)){
            ATH_MSG_DEBUG("  Found overlap muon: " << muon->pt()*invGeV);
            setObjectFail(muon);
          }
        }
      }
    }
  }

  #undef numTrk

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping electrons and muons
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removeEleMuonOverlap
(const xAOD::ElectronContainer& electrons, const xAOD::MuonContainer& muons)
{
  ATH_MSG_DEBUG("Removing electrons that overlap with muons");

  // Initialize output decorations if necessary
  initializeDecorations(electrons);
  initializeDecorations(muons);
  if(m_writeSharedTrackFlag)
    initializeSharesTrackDecoration(electrons);

  // Loop over electrons
  for(const auto electron : electrons){
    if(isSurvivingObject(electron)){
      if(m_useSharedTrackFlag){
        if(sharesTrack(electron)) setObjectFail(electron);
      }
      else{
        const xAOD::TrackParticle* elTrk =
          xAOD::EgammaHelpers::getOriginalTrackParticle(electron);
        bool sharesTrack = false;
        // Loop over muons
        for(const auto muon : muons){
          const xAOD::TrackParticle* muTrk =
            muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
          // Discard electron if they share an ID track
          if(isSurvivingObject(muon) && (elTrk == muTrk)){
            ATH_MSG_DEBUG("  Found overlap ele: " << electron->pt()*invGeV
                          << " with mu: " << muon->pt()*invGeV);
            if(m_linkOverlapObjects){
              ATH_CHECK( addOverlapObjectLink(electron, muon) );
            }
            setObjectFail(electron);
            sharesTrack = true;
            break;
          }
        }
        if(m_writeSharedTrackFlag){
          ATH_CHECK( setSharesTrackDecoration(electron,sharesTrack) );
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping hadronic taus and jets
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removeTauJetOverlap(const xAOD::TauJetContainer& taus,
                                                   const xAOD::JetContainer& jets)
{
  ATH_MSG_DEBUG("Removing jets that overlap with taus");

  // Initialize output decoration if necessary
  initializeDecorations(taus);
  initializeDecorations(jets);

  // Loop over jets
  for(const auto jet : jets){
    // Check that this jet passes the input selection
    if(isSurvivingObject(jet)){
      if(objectOverlaps(jet, taus, m_tauJetDR)){
        ATH_MSG_DEBUG("  Found overlap jet: " << jet->pt()*invGeV);
        setObjectFail(jet);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping hadronic taus and electrons
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removeTauEleOverlap
(const xAOD::TauJetContainer& taus, const xAOD::ElectronContainer& electrons)
{
  ATH_MSG_DEBUG("Removing taus that overlap with loose electrons");

  // Initialize output decoration if necessary
  initializeDecorations(taus);
  initializeDecorations(electrons);

  // Remove tau if overlaps with a loose electron
  for(const auto tau : taus){
    if(isSurvivingObject(tau)){
      for(const auto electron : electrons){
        // Loose electron criteria doesn't use the input label
        // decoration. For now we use our own custom criteria.
        if(!isRejectedObject(electron)){
          // TODO: use faster method. This is slow.
          bool passID = false;
          // TODO: move to helper method: isLooseEleForTauEleOR
          if(!electron->passSelection(passID, m_tauEleOverlapID)){
            ATH_MSG_ERROR("Electron ID for tau-ele OR not available: "
                          << m_tauEleOverlapID);
            return StatusCode::FAILURE;
          }
          if(passID && objectsOverlap(tau, electron, m_tauElectronDR)){
            ATH_MSG_DEBUG("  Found overlapping tau with pt "
                          << tau->pt()*invGeV);
            setObjectFail(tau);
            break;
          } // electron overlaps
        } // is surviving electron
      } // electron loop
    } // is surviving tau
  } // tau loop
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping hadronic taus and muons
// This function loop could be combined with the electron one above for speed.
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removeTauMuonOverlap
(const xAOD::TauJetContainer& taus, const xAOD::MuonContainer& muons)
{
  ATH_MSG_DEBUG("Removing taus that overlap with loose muons");

  // Initialize output decoration if necessary
  initializeDecorations(taus);
  initializeDecorations(muons);

  // Remove tau if overlaps with a loose muon
  for(const auto tau : taus){
    if(isSurvivingObject(tau)){
      for(const auto muon : muons){
        // Loose muon criteria doesn't use the input label
        // decoration. For now we use own own custom criteria.
        if(!isRejectedObject(muon) && muon->pt() > m_tauMuOverlapMuPt){
          // Higher pt taus only rejected if overlapping with combined muon
          if(tau->pt() < m_tauMuOverlapTauPt ||
             muon->muonType() == xAOD::Muon::Combined)
          {
            if(objectsOverlap(tau, muon, m_tauMuonDR)){
              ATH_MSG_DEBUG("  Found overlapping tau with pt "
                            << tau->pt()*invGeV);
              setObjectFail(tau);
              break;
            } // mu-tau overlap
          } // soft tau or combined muon
        } // is loose muon
      } // muon loop
    } // is surviving tau
  } // tau loop
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping photons and electrons
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removePhotonEleOverlap
(const xAOD::PhotonContainer& photons, const xAOD::ElectronContainer& electrons)
{
  ATH_MSG_DEBUG("Removing photons overlapping with electrons");

  // Initialize output decoration if necessary
  initializeDecorations(photons);
  initializeDecorations(electrons);

  for(const auto photon : photons){
    if(isSurvivingObject(photon)){
      if(objectOverlaps(photon, electrons, m_photonElectronDR)){
        ATH_MSG_DEBUG("  Found overlap pho: " << photon->pt()*invGeV);
        setObjectFail(photon);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping photons and muons
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removePhotonMuonOverlap
(const xAOD::PhotonContainer& photons, const xAOD::MuonContainer& muons)
{
  ATH_MSG_DEBUG("Removing photons overlapping with muons");

  // Initialize output decoration if necessary
  initializeDecorations(photons);
  initializeDecorations(muons);

  for(const auto photon : photons){
    if(isSurvivingObject(photon)){
      if(objectOverlaps(photon, muons, m_photonMuonDR)){
        ATH_MSG_DEBUG("  Found overlap pho: " << photon->pt()*invGeV);
        setObjectFail(photon);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping photons and electrons
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removePhotonPhotonOverlap
(const xAOD::PhotonContainer& photons)
{
  ATH_MSG_DEBUG("Removing overlapping photons");

  // Initialize output decoration if necessary
  initializeDecorations(photons);

  for(const auto photon : photons){
    if(isSurvivingObject(photon)){
      if(objectOverlaps(photon, photons, m_photonPhotonDR)){
        ATH_MSG_DEBUG("  Found overlap pho: " << photon->pt()*invGeV);
        setObjectFail(photon);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Remove overlapping photons and jets
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::removePhotonJetOverlap
(const xAOD::PhotonContainer& photons, const xAOD::JetContainer& jets)
{
  ATH_MSG_DEBUG("Removing jets overlapping with photons");

  // Initialize output decoration if necessary
  initializeDecorations(photons);
  initializeDecorations(jets);

  for(const auto jet : jets){
    if(isSurvivingObject(jet)){
      if(objectOverlaps(jet, photons, m_photonJetDR)){
        ATH_MSG_DEBUG("  Found overlap jet: " << jet->pt()*invGeV);
        setObjectFail(jet);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Generic dR-based overlap check between one object and a container
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::objectOverlaps(const xAOD::IParticle* obj,
                                        const xAOD::IParticleContainer& container,
                                        double dR)
{
  for(const auto contObj : container){
    if(isSurvivingObject(contObj)){
      // Make sure these are not the same object
      if(obj == contObj) continue;
      if(objectsOverlap(obj, contObj, dR)) return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
// Loose lepton IDs for tau-lepton OR
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::isLooseEleForTauEleOR
(const xAOD::Electron* electron, bool& pass)
{
  if(!electron->passSelection(pass, m_tauEleOverlapID)){
    ATH_MSG_ERROR("Electron ID for tau-ele OR not available: "
                  << m_tauEleOverlapID);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::isLooseMuonForTauMuonOR
(const xAOD::Muon* muon, bool& pass)
{
  if(muon->pt() > m_tauMuOverlapMuPt) pass = true;
  else pass = false;
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Check if jet is labeled as a b-jet
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::isBJet(const xAOD::Jet* jet)
{
  return m_bJetAccessor && (*m_bJetAccessor)(*jet);
}

//-----------------------------------------------------------------------------
// Check if two objects overlap in a dR window
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::objectsOverlap(const xAOD::IParticle* p1,
                                        const xAOD::IParticle* p2,
                                        double dRMax, double dRMin)
{
  double dR2 = deltaR2(p1, p2);
  // TODO: use fpcompare utilities
  if(dR2 < (dRMax*dRMax) && dR2 >= (dRMin*dRMin)){
    // Is this the best place for setting the link?
    if(m_linkOverlapObjects){
      ATH_CHECK( addOverlapObjectLink(p1, p2) );
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Add overlap object link
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::addOverlapObjectLink(const xAOD::IParticle* p1,
                                                    const xAOD::IParticle* p2)
{
  // For first simple implementation, use a single object link.
  // Might migrate to an array of links, to allow future expansion of the
  // functionality to include all possible object overlaps.
  const xAOD::IParticleContainer* overlapContainer =
    static_cast<const xAOD::IParticleContainer*>(p2->container());
  (*m_overlapLinkDecorator)(*p1) =
    ort::objLink_t(*overlapContainer, p2->index());

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Calculate delta R between two particles
//-----------------------------------------------------------------------------
double OverlapRemovalTool::deltaR2(const xAOD::IParticle* p1,
                                   const xAOD::IParticle* p2)
{
  double dY = p1->rapidity() - p2->rapidity();
  double dPhi = TVector2::Phi_mpi_pi(p1->phi() - p2->phi());
  return dY*dY + dPhi*dPhi;
}
//-----------------------------------------------------------------------------
double OverlapRemovalTool::deltaR(const xAOD::IParticle* p1,
                                  const xAOD::IParticle* p2)
{ return sqrt(deltaR2(p1, p2)); }

//-----------------------------------------------------------------------------
// Determine if object is currently OK for input to OR
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::isInputObject(const xAOD::IParticle* obj)
{
  // Input label is turned off if empty string
  if(m_inputLabel.empty()) return true;
  return (*m_inputAccessor)(*obj);
}

//-----------------------------------------------------------------------------
// Determine if object is currently rejected by OR.
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::isRejectedObject(const xAOD::IParticle* obj)
{
  if((*m_outputDecorator)(*obj) == true) return true;
  return false;
}

//-----------------------------------------------------------------------------
void OverlapRemovalTool::setOverlapDecoration(const xAOD::IParticle* obj,
                                              bool overlaps)
{ (*m_outputDecorator)(*obj) = overlaps; }

//-----------------------------------------------------------------------------
// Check if output decoration has been applied to a container
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::isDecorated(const xAOD::IParticleContainer& container)
{
  return container.size() > 0 &&
         (*m_outputDecorator).isAvailable(*container[0]);
}

//-----------------------------------------------------------------------------
// Initialize output decoration
//-----------------------------------------------------------------------------
void OverlapRemovalTool::initializeDecorations
(const xAOD::IParticleContainer& container)
{
  if(!isDecorated(container))
    resetDecorations(container);
}
//-----------------------------------------------------------------------------
// Reset output decoration
//-----------------------------------------------------------------------------
void OverlapRemovalTool::resetDecorations
(const xAOD::IParticleContainer& container)
{
  for(auto obj : container) setObjectPass(obj);
}

//-----------------------------------------------------------------------------
// Determine from / decorate flag if electron shares track with muon
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::checkTrackParticles(const xAOD::ElectronContainer& container)
{
  if(container.size()){
    const xAOD::TrackParticle* elTrk = 0;
    elTrk = container[0]->trackParticle();
    if(!elTrk) return false;
  }
  return true;
}
//-----------------------------------------------------------------------------

bool OverlapRemovalTool::checkSharesTrackDecoration(const xAOD::ElectronContainer& container)
{
  if(container.size())
    return (*m_sharedTrackAccessor).isAvailable(*container[0]);

  return true;
}

//-----------------------------------------------------------------------------
void OverlapRemovalTool::initializeSharesTrackDecoration
(const xAOD::ElectronContainer& container)
{
  for(auto obj : container){
    (*m_sharedTrackDecorator)(*obj) = false;
  }
}
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTool::setSharesTrackDecoration(const xAOD::IParticle* obj,
                                                        bool sharesTrack)

{
  (*m_sharedTrackDecorator)(*obj) = sharesTrack;
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------------
bool OverlapRemovalTool::sharesTrack(const xAOD::IParticle* obj)
{
  if((*m_sharedTrackAccessor)(*obj) == true) return true;
  return false;
}
