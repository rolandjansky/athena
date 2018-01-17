// EDM includes

// Local includes
#include "AssociationUtils/OverlapRemovalDefs.h"
#include "OverlapRemovalTestAlg.h"

namespace
{
  /// Unit conversion constants
  const float GeV = 1000.;
  const float invGeV = 1./GeV;
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
OverlapRemovalTestAlg::OverlapRemovalTestAlg(const std::string& name,
                                             ISvcLocator* svcLoc)
    : AthAlgorithm(name, svcLoc),
      m_orTool("OverlapRemovalTool", this)
{
  declareProperty("OverlapRemovalTool", m_orTool);
  declareProperty("SelectionLabel", m_selectionLabel="selected",
                  "Input label for the OverlapRemovalTool");
  declareProperty("OverlapLabel", m_overlapLabel="overlaps",
                  "Output label for the OverlapRemovalTool");
  declareProperty("BJetLabel", m_bJetLabel="isBJet",
                  "Input label for b-tagged jets");
}

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTestAlg::initialize()
{
  ATH_MSG_INFO("Initialize");

  // Try to retrieve the tool
  ATH_CHECK( m_orTool.retrieve() );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalTestAlg::execute()
{
  // Electrons
  const xAOD::ElectronContainer* electrons = 0;
  ATH_CHECK( evtStore()->retrieve(electrons, "Electrons") );
  applySelection(*electrons);
  // Muons
  const xAOD::MuonContainer* muons = 0;
  ATH_CHECK( evtStore()->retrieve(muons, "Muons") );
  applySelection(*muons);
  // Taus
  const xAOD::TauJetContainer* taus = 0;
  ATH_CHECK( evtStore()->retrieve(taus, "TauJets") );
  applySelection(*taus);
  // Jets
  const xAOD::JetContainer* jets = 0;
  ATH_CHECK( evtStore()->retrieve(jets, "AntiKt4EMTopoJets") );
  applySelection(*jets);
  // Photons
  const xAOD::PhotonContainer* photons = 0;
  ATH_CHECK( evtStore()->retrieve(photons, "Photons") );
  applySelection(*photons);

  // Apply the overlap removal
  ATH_CHECK( m_orTool->removeOverlaps(electrons, muons, jets, taus, photons) );

  // Dump the objects
  ATH_MSG_DEBUG("Dumping results");
  printObjects(*electrons, "ele");
  printObjects(*muons, "muo");
  printObjects(*taus, "tau");
  printObjects(*jets, "jet");
  printObjects(*photons, "pho");

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Apply selection to a container
//-----------------------------------------------------------------------------
template<class ContainerType>
void OverlapRemovalTestAlg::applySelection(const ContainerType& container)
{
  static ort::inputDecorator_t selDec(m_selectionLabel);
  for(auto obj : container){
    selDec(*obj) = selectObject(*obj);
  }
}
//-----------------------------------------------------------------------------
template<class ObjType>
bool OverlapRemovalTestAlg::selectObject(const ObjType& obj)
{
  if(obj.pt() < 10.*GeV) return false;
  if(std::abs(obj.eta()) > 2.5) return false;
  return true;
}
//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalTestAlg::selectObject<xAOD::Jet>(const xAOD::Jet& jet)
{
  // Label bjets
  static ort::inputDecorator_t bJetDec(m_bJetLabel);
  bJetDec(jet) = false;
  double mv2c20 = 0.;
  if(jet.btagging()->MVx_discriminant("MV2c20", mv2c20)){
    if(mv2c20 > -0.5911) bJetDec(jet) = true;
  }
  else ATH_MSG_WARNING("BTag info unavailable!");
  if(jet.pt() < 20.*GeV) return false;
  if(std::abs(jet.eta()) > 2.5) return false;
  return true;
}
//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalTestAlg::selectObject<xAOD::TauJet>(const xAOD::TauJet& obj)
{
  if(obj.pt() < 20.*GeV) return false;
  if(std::abs(obj.eta()) > 2.5) return false;
  return true;
}


//-----------------------------------------------------------------------------
// Print object
//-----------------------------------------------------------------------------
void OverlapRemovalTestAlg::printObjects(const xAOD::IParticleContainer& container,
                                         const std::string& type)
{
  static ort::inputAccessor_t selectAcc(m_selectionLabel);
  static ort::outputAccessor_t overlapAcc(m_overlapLabel);
  for(auto obj : container){
    if(selectAcc(*obj)){
      bool overlaps = overlapAcc(*obj);
      ATH_MSG_DEBUG("  " << type << " pt " << obj->pt()*invGeV
                    << " eta " << obj->eta() << " phi " << obj->phi()
                    << " overlaps " << overlaps);
    }
  }
}
