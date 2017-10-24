/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes

// Local includes
#include "AssociationUtils/OverlapRemovalDefs.h"
#include "OverlapRemovalGenUseAlg.h"

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
	declareProperty("ElectronLabel", m_electronLabel="DFCommonElectronsLHLoose",
			"Input label for passing electrons");
	declareProperty("PhotonLabel", m_photonLabel="DFCommonPhotonsIsEMLoose",
			"Input label for passing photons");
	declareProperty("MuonLabel", m_muonLabel="DFCommonGoodMuon",
			"Input label for passing muons");
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
	  if(electrons) setDefaultDecorations(*electrons);
	  if(muons)     setDefaultDecorations(*muons);
	  if(jets)      setDefaultDecorations(*jets);
	  if(taus)      setDefaultDecorations(*taus);
	  if(photons)   setDefaultDecorations(*photons);
  }

  // Dump the objects
  ATH_MSG_VERBOSE("Dumping results");
  if(msgLevel() >= MSG::VERBOSE){
  printObjects(*electrons, "ele");
  printObjects(*muons, "muo");
  printObjects(*taus, "tau");
  printObjects(*jets, "jet");
  printObjects(*photons, "pho");}

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
		defaultDec(*obj) = 1; //default to all objects being overlaps if we can't get primary vertices. Ensures the event cleaning decision fails. 
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
  const static SG::AuxElement::ConstAccessor<float> acc_applyBTag("DFCommonJets_FixedCutBEff_85");
  static ort::inputDecorator_t bJetDec(m_bJetLabel);
  bJetDec(obj) = acc_applyBTag(obj);
  // Selection
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::Electron>(const xAOD::Electron& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_ElectronPass(m_electronLabel);
  if(m_electronLabel=="") return false;    //disable objects with empty labels
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  if(!acc_ElectronPass(obj)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::Photon>(const xAOD::Photon& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_PhotonPass(m_photonLabel);
  if(m_photonLabel=="") return false;    //disable objects with empty labels
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  if(!acc_PhotonPass(obj)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::Muon>(const xAOD::Muon& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_MuonPass(m_muonLabel);
  if(m_muonLabel=="") return false;    //disable objects with empty labels
  if(obj.pt() < m_ptCut*GeV || std::abs(obj.eta()) > m_etaCut) return false;
  if(!acc_MuonPass(obj)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalGenUseAlg::selectObject<xAOD::TauJet>(const xAOD::TauJet& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_TauPass(m_tauLabel);
  if(m_tauLabel=="") return false;    //disable objects with empty labels
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
