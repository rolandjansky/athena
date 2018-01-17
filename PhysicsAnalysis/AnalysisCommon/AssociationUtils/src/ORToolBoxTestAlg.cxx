#include "ORToolBoxTestAlg.h"
#include "AssociationUtils/OverlapRemovalDefs.h"
#include "AssociationUtils/OverlapRemovalInit.h"

namespace
{
  /// Unit conversion constant
  const float GeV = 1000.;
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
ORToolBoxTestAlg::ORToolBoxTestAlg(const std::string& name,
                                   ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_toolBox("ORTBox", this)
{
  declareProperty("SelectionLabel", m_flags.inputLabel="isSelected",
                  "Input label for the OverlapRemovalTool");
  declareProperty("OverlapLabel", m_flags.outputLabel="overlaps",
                  "Output label for the OverlapRemovalTool");
  declareProperty("BJetLabel", m_flags.bJetLabel="isBJet",
                  "Input label for b-tagged jets");
}

//-----------------------------------------------------------------------------
// Initialize the algorithm
//-----------------------------------------------------------------------------
StatusCode ORToolBoxTestAlg::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() );

  // Configuration helper function
  ATH_CHECK( ORUtils::recommendedTools(m_flags, m_toolBox) );

  // Initialize the tools
  ATH_CHECK( m_toolBox.initialize() );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execute the algorithm
//-----------------------------------------------------------------------------
StatusCode ORToolBoxTestAlg::execute()
{
  ATH_MSG_INFO( "Executing " << name() );

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
  ATH_CHECK( m_toolBox.masterTool->removeOverlaps(electrons, muons, jets, taus, photons) );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Apply selection to a container
//-----------------------------------------------------------------------------
template<class ContainerType>
void ORToolBoxTestAlg::applySelection(const ContainerType& container)
{
  static ort::inputDecorator_t selDec(m_flags.inputLabel);
  for(auto obj : container){
    selDec(*obj) = selectObject(*obj);
  }
}
//-----------------------------------------------------------------------------
template<class ObjType>
bool ORToolBoxTestAlg::selectObject(const ObjType& obj)
{
  if(obj.pt() < 10.*GeV) return false;
  if(std::abs(obj.eta()) > 2.5) return false;
  return true;
}
//-----------------------------------------------------------------------------
template<>
bool ORToolBoxTestAlg::selectObject<xAOD::Jet>(const xAOD::Jet& jet)
{
  // Label bjets
  static ort::inputDecorator_t bJetDec(m_flags.bJetLabel);
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
bool ORToolBoxTestAlg::selectObject<xAOD::TauJet>(const xAOD::TauJet& obj)
{
  if(obj.pt() < 20.*GeV) return false;
  if(std::abs(obj.eta()) > 2.5) return false;
  return true;
}
