/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes

// Local includes
#include "AssociationUtils/OverlapRemovalDefs.h"
#include "OverlapRemovalECAlg.h"

namespace
{
  /// Unit conversion constants
  const float GeV = 1000.;
  const float invGeV = 1./GeV;
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
OverlapRemovalECAlg::OverlapRemovalECAlg(const std::string& name,
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
StatusCode OverlapRemovalECAlg::initialize()
{
	ATH_MSG_INFO("Initialize");

	// Try to retrieve the tool
	ATH_CHECK( m_orTool.retrieve() );

	return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------
StatusCode OverlapRemovalECAlg::execute()
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
	  ATH_MSG_WARNING("No primary vertices found, cannot do overlap removal! Will return all fails."); 
	  if(electrons) resetDecorations(*electrons);
	  if(muons)     resetDecorations(*muons);
	  if(jets)      resetDecorations(*jets);
	  if(taus)      resetDecorations(*taus);
	  if(photons)   resetDecorations(*photons);
  }

  // Dump the objects
  ATH_MSG_DEBUG("Dumping results");
  printObjects(*electrons, "ele");
  printObjects(*muons, "muo");
  printObjects(*taus, "tau");
  printObjects(*jets, "jet");
  printObjects(*photons, "pho");

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
// Reset output decoration
//---------------------------------------------------------------------------
	template<class ContainerType>
void OverlapRemovalECAlg::resetDecorations(const ContainerType& container)
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
void OverlapRemovalECAlg::applySelection(const ContainerType& container)
{
	static ort::inputDecorator_t selDec(m_selectionLabel);
	for(auto obj : container){
		selDec(*obj) = selectObject(*obj);
	}
}
//-----------------------------------------------------------------------------
template<class ObjType>
bool OverlapRemovalECAlg::selectObject(const ObjType& obj)
{
  if(obj.pt() < 10.*GeV) return false;
  if(std::abs(obj.eta()) > 4.5) return false;
  return true;
}
//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalECAlg::selectObject<xAOD::Jet>(const xAOD::Jet& jet)
{
  // Label bjets
  static ort::inputDecorator_t bJetDec(m_bJetLabel);
  bJetDec(jet) = false;
  double mv2c10 = 0.;
  auto btag = jet.btagging();
  if(btag && btag->MVx_discriminant("MV2c10", mv2c10)){
    if(mv2c10 > -0.1416) bJetDec(jet) = true;
  }
  else ATH_MSG_WARNING("BTag info unavailable!");
  if(jet.pt() < 20.*GeV) return false;
  if(std::abs(jet.eta()) > 4.5) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalECAlg::selectObject<xAOD::Electron>(const xAOD::Electron& elec)
{
  const static SG::AuxElement::ConstAccessor<char> acc_ElectronPassLHLoose("DFCommonElectronsLHLoose");
  if(!acc_ElectronPassLHLoose(elec)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalECAlg::selectObject<xAOD::Photon>(const xAOD::Photon& photon)
{
  const static SG::AuxElement::ConstAccessor<char> acc_PhotonPassIsEMLoose("DFCommonPhotonsIsEMLoose");
  if(!acc_PhotonPassIsEMLoose(photon)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalECAlg::selectObject<xAOD::Muon>(const xAOD::Muon& mu)
{
  const static SG::AuxElement::ConstAccessor<char> acc_passMuon("DFCommonGoodMuon");
  if(!acc_passMuon(mu)) return false;
  return true;
}

//-----------------------------------------------------------------------------
template<>
bool OverlapRemovalECAlg::selectObject<xAOD::TauJet>(const xAOD::TauJet& obj)
{
  const static SG::AuxElement::ConstAccessor<char> acc_passTau("DFCommonTausLoose");
  if(!acc_passTau(obj)) return false;
  return true;
}


//-----------------------------------------------------------------------------
// Print object
//-----------------------------------------------------------------------------
void OverlapRemovalECAlg::printObjects(const xAOD::IParticleContainer& container,
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
