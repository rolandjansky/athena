/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes

// Local includes
#include "AssociationUtils/OverlapRemovalDefs.h"
#include "AssociationUtils/OverlapRemovalGenUseAlg.h"

namespace
{
  /// Unit conversion constants
  const float GeV = 1000.;
  const float invGeV = 1./GeV;
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
OverlapRemovalGenUseAlg::OverlapRemovalGenUseAlg(const std::string& name,
		ISvcLocator* svcLoc)
	: EL::AnaAlgorithm(name, svcLoc),
	m_orTool("OverlapRemovalTool", this)
{
    declareProperty("OverlapRemovalTool", m_orTool);
    declareProperty("SelectionLabel", m_selectionLabel="selected",
                    "Input label for the OverlapRemovalTool");
    declareProperty("OverlapLabel", m_overlapLabel="overlaps",
                    "Output label for the OverlapRemovalTool");
    declareProperty("DefaultValue", m_defaultValue=true,
                    "Default value for objects failing OR");
    declareProperty("JetKey", m_jetKey="AntiKt4EMTopoJets",
                    "StoreGate/TEvent key for jets");
    declareProperty("BJetLabel", m_bJetLabel="",
                    "Input label for b-tagged jets");
    declareProperty("ElectronKey", m_electronKey="Electrons",
                    "StoreGate/TEvent key for electrons");
    declareProperty("ElectronLabel", m_electronLabel="DFCommonElectronsLHLoose",
                    "Input label for passing electrons");
    declareProperty("PhotonKey", m_photonKey="Photons",
                    "StoreGate/TEvent key for photons");
    declareProperty("PhotonLabel", m_photonLabel="DFCommonPhotonsIsEMLoose",
                    "Input label for passing photons");
    declareProperty("MuonKey", m_muonKey="Muons",
                    "StoreGate/TEvent key for muons");
    declareProperty("MuonLabel", m_muonLabel="DFCommonMuonPassIDCuts",
                    "Input label for passing muons");
    declareProperty("TauKey", m_tauKey="TauJets",
                    "StoreGate/TEvent key for taus");
    declareProperty("TauLabel", m_tauLabel="DFCommonTausLoose",
                    "Input label for passing taus");
    declareProperty("PtCut", m_ptCut = 20.0,
                    "Minimum pt for consideration");
    declareProperty("EtaCut", m_etaCut = 4.5,
                    "Maximum eta for consideration");

}


//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalGenUseAlg::initialize()
{
	ATH_MSG_INFO("Initialize");

	// Try to retrieve the tool
	ATH_CHECK( m_orTool.retrieve() );

	return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalGenUseAlg::execute()
{
    // Electrons
    const xAOD::ElectronContainer* electrons = 0;
    ATH_CHECK( evtStore()->retrieve(electrons, m_electronKey) );
    applySelection(*electrons);
    // Muons
    const xAOD::MuonContainer* muons = 0;
    ATH_CHECK( evtStore()->retrieve(muons, m_muonKey) );
    applySelection(*muons);
    // Jets
    const xAOD::JetContainer* jets = 0;
    ATH_CHECK( evtStore()->retrieve(jets, m_jetKey) );
    applySelection(*jets);
    // Taus
    const xAOD::TauJetContainer* taus = 0;
    if(!m_tauKey.empty()) {
        ATH_CHECK( evtStore()->retrieve(taus, m_tauKey) );
        applySelection(*taus);
    }
    // Photons
    const xAOD::PhotonContainer* photons = 0;
    if(!m_photonKey.empty()) {
        ATH_CHECK( evtStore()->retrieve(photons, m_photonKey) );
        applySelection(*photons);
    }

    // Primary Vertices
    const xAOD::VertexContainer* vertices = nullptr;
    int checkVtx = 0;
    if(evtStore()->retrieve(vertices, "PrimaryVertices").isSuccess()) {
        for(auto vtx : *vertices) {
            if(vtx->vertexType() == xAOD::VxType::PriVtx)
                checkVtx = 1;
        }
    }

    if(checkVtx==1){
        // Apply the overlap removal
        ATH_CHECK( m_orTool->removeOverlaps(electrons, muons, jets, taus, photons) );}
    else{
        // Reset all decorations to failing
        ATH_MSG_DEBUG("No primary vertices found, cannot do overlap removal! Will return all fails.");
        setDefaultDecorations(*jets);
        setDefaultDecorations(*electrons);
        setDefaultDecorations(*muons);
        if(taus)      setDefaultDecorations(*taus);
        if(photons)   setDefaultDecorations(*photons);
    }

    // Dump the objects
    ATH_MSG_VERBOSE("Dumping results");
#ifdef XAOD_STANDALONE
    auto msglvl = msg().level();
#else
    auto msglvl = msgLevel();
#endif
    if(msglvl >= MSG::VERBOSE){
        printObjects(*electrons, "ele");
        printObjects(*muons, "muo");
        printObjects(*jets, "jet");
        if(taus) printObjects(*taus, "tau");
        if(photons) printObjects(*photons, "pho");
    }

    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
// Reset output decoration
//---------------------------------------------------------------------------
	template<class ContainerType>
void OverlapRemovalGenUseAlg::setDefaultDecorations(const ContainerType& container)
{
	static ort::inputDecorator_t defaultDec(m_overlapLabel);
	for(auto obj : container){
		defaultDec(*obj) = m_defaultValue; //default to all objects being overlaps if we can't get primary vertices. Ensures the event cleaning decision fails.
	}
}


//-----------------------------------------------------------------------------
// Apply selection to a container
//-----------------------------------------------------------------------------
	template<class ContainerType>
void OverlapRemovalGenUseAlg::applySelection(const ContainerType& container)
{
	static ort::inputDecorator_t selDec(m_selectionLabel);
	for(auto obj : container){
		selDec(*obj) = selectObject(*obj);
        ATH_MSG_VERBOSE("  Obj " << obj->index() << " of type " << obj->type()
                        << " selected? " << int(selDec(*obj)));
	}
}
//-----------------------------------------------------------------------------
template<class ObjType>
bool OverlapRemovalGenUseAlg::selectObject(const ObjType& obj)
{
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  return true;
}
//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::Jet>(const xAOD::Jet& obj)
{
  // Label bjets
  //const static SG::AuxElement::ConstAccessor<float> acc_applyBTag("DFCommonJets_FixedCutBEff_85_MV2c10");
  //static ort::inputDecorator_t bJetDec(m_bJetLabel);
  //bJetDec(obj) = acc_applyBTag(obj);
  // Selection
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::Electron>(const xAOD::Electron& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_ElectronPass(m_electronLabel);
  if(m_electronLabel.empty()) return true;    //disable selection for objects with empty labels
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  if(!acc_ElectronPass(obj)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::Photon>(const xAOD::Photon& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_PhotonPass(m_photonLabel);
  if(m_photonLabel.empty()) return true;    //disable selection for objects with empty labels
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  if(!acc_PhotonPass(obj)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::Muon>(const xAOD::Muon& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_MuonPass(m_muonLabel);
  if(m_muonLabel.empty()) return true;    //disable selection for objects with empty labels
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  if(!acc_MuonPass(obj)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::TauJet>(const xAOD::TauJet& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_TauPass(m_tauLabel);
  if(m_tauLabel.empty()) return true;    //disable selection for objects with empty labels
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  if(!acc_TauPass(obj)) return false;
  return true;
}


//-----------------------------------------------------------------------------
// Print object
//-----------------------------------------------------------------------------
void OverlapRemovalGenUseAlg::printObjects(const xAOD::IParticleContainer& container,
                                         const std::string& type)
{
  static ort::inputAccessor_t selectAcc(m_selectionLabel);
  static ort::outputAccessor_t overlapAcc(m_overlapLabel);
  for(auto obj : container){
    if(selectAcc(*obj)){
      bool overlaps = overlapAcc(*obj);
      ATH_MSG_VERBOSE("  " << type << " pt " << obj->pt()*invGeV
                    << " eta " << obj->eta() << " phi " << obj->phi()
                    << " overlaps " << overlaps);
    }
  }
}
