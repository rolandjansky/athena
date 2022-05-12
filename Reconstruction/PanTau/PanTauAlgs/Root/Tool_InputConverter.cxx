/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/Tool_InputConverter.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/PanTauSeed.h"
#include "PanTauAlgs/HelperFunctions.h"
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFODefs.h"


PanTau::Tool_InputConverter::Tool_InputConverter( const std::string& name ) :
  asg::AsgTool(name),
  m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore")
{
  declareProperty("Tool_InformationStore",     m_Tool_InformationStore, "Link to tool with all information");
  declareProperty("Tool_InformationStoreName", m_Tool_InformationStoreName="", "Optional Name for InformationStore insance in ABR");
}

PanTau::Tool_InputConverter::~Tool_InputConverter() {
}

StatusCode PanTau::Tool_InputConverter::initialize() {

  ATH_MSG_INFO(" initialize()");
  m_init=true;

  ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InformationStore, m_Tool_InformationStoreName) ); //ABR only

  ATH_CHECK( m_Tool_InformationStore.retrieve() );
    
  ATH_CHECK( m_Tool_InformationStore->getInfo_Int("TauConstituents_UsePionMass", m_Config_UsePionMass) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_Int("TauConstituents_UseShrinkingCone", m_Config_TauConstituents_UseShrinkingCone) );        
  ATH_CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_Types_DeltaRCore", m_Config_TauConstituents_Types_DeltaRCore) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_PreselectionMinEnergy", m_Config_TauConstituents_PreselectionMinEnergy) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_BinEdges_Eta", m_Config_CellBased_BinEdges_Eta) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_1prong", m_Config_CellBased_EtaBinned_Pi0MVACut_1prong) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_3prong", m_Config_CellBased_EtaBinned_Pi0MVACut_3prong) );  

  return StatusCode::SUCCESS;
}


bool PanTau::Tool_InputConverter::passesPreselectionEnergy(double energy) const {
  if (energy < m_Config_TauConstituents_PreselectionMinEnergy) return false;
  return true;
}


// PFO converter
StatusCode PanTau::Tool_InputConverter::ConvertToTauConstituent(xAOD::PFO* pfo,
								PanTau::TauConstituent* &tauConstituent,
								const xAOD::TauJet* tauJet) const {
    
  // check for invalid eta, phi, e "NAN" values:
  if (pfo->eta() != pfo->eta()) {
    ATH_MSG_WARNING("Will not convert PFO with eta value of " << pfo->eta() << " -> return to Tool_TauConstituentGetter");
    return StatusCode::SUCCESS;
  }
  if (pfo->phi() != pfo->phi()) {
    ATH_MSG_WARNING("Will not convert PFO with phi value of " << pfo->phi() << " -> return to Tool_TauConstituentGetter");
    return StatusCode::SUCCESS;
  }
  if (pfo->e() != pfo->e()) {
    ATH_MSG_WARNING("Will not convert PFO with e value of " << pfo->e() << " -> return to Tool_TauConstituentGetter");
    return StatusCode::SUCCESS;
  }
  
  // Check whether neutral input pfo has pion mass (it may have if xAOD is being reprocessed)
  // If it does, make it massless again and use that
  if (!pfo->isCharged() && pfo->m() != 0) {
    TLorentzVector tlvUpdate; 
    PanTau::SetP4EEtaPhiM( tlvUpdate, pfo->e(), pfo->eta(), pfo->phi(), 0);
    pfo->setP4(tlvUpdate.Pt(), pfo->eta(), pfo->phi(), 0);
  }
    
  // preselection to veto very low energy PFOs
  int pfoCharge = (int)(pfo->charge());
  double constituentEnergy = pfo->e();
  if (!passesPreselectionEnergy(constituentEnergy)) {
    ATH_MSG_DEBUG("EFO of charge " << pfoCharge << " and energy " << constituentEnergy << " does not pass presel Energy cut of " << m_Config_TauConstituents_PreselectionMinEnergy);
    return StatusCode::SUCCESS;
  }
        
  // get the mass correct & build 4-vector
  double constituentMass = pfo->m();
  if (m_Config_UsePionMass) {
    
    // clusters: don't touch the measured energy. set mass to pion mass, so momentum will be altered
    if (!pfo->isCharged()) {
      constituentMass = 134.98;
    }
  }
 
  TLorentzVector momentum; 
  PanTau::SetP4EEtaPhiM( momentum, constituentEnergy, pfo->eta(), pfo->phi(), constituentMass);
    
  // get type (based on charge and DR to tau)
  std::vector<int> typeFlags = std::vector<int>((unsigned int)PanTau::TauConstituent::t_nTypes, 0);
  typeFlags.at((int)PanTau::TauConstituent::t_NoType) = 1;
    
  double mvaValue = PanTau::TauConstituent::DefaultBDTValue();

  TLorentzVector tlv_intAxis = tauJet->p4(xAOD::TauJetParameters::IntermediateAxis);
  double deltaR_toTauJet = tlv_intAxis.DeltaR( pfo->p4() );
    
  if (deltaR_toTauJet > m_Config_TauConstituents_Types_DeltaRCore) {
    if (pfo->isCharged()) {
      typeFlags.at((int)PanTau::TauConstituent::t_Charged) = 1;
    }
    else {
      typeFlags.at((int)PanTau::TauConstituent::t_OutNeut) = 1;
      mvaValue = pfo->bdtPi0Score();
    }
  }//end if pfo is not in core
    
  if (deltaR_toTauJet <= m_Config_TauConstituents_Types_DeltaRCore) {
    
    if (pfo->isCharged()) {
      typeFlags.at((int)PanTau::TauConstituent::t_Charged) = 1;
    }
    else {
      typeFlags.at((int)PanTau::TauConstituent::t_Neutral) = 1;
      typeFlags.at((int)PanTau::TauConstituent::t_NeutLowA) = 1;
      typeFlags.at((int)PanTau::TauConstituent::t_NeutLowB) = 1;
            
      //neutral PFO arranging --- check for pi0 tag
      mvaValue = pfo->bdtPi0Score();
            
      int nPi0sPerCluster = 0;
      if ( !pfo->attribute(xAOD::PFODetails::nPi0Proto, nPi0sPerCluster) ) {
	ATH_MSG_WARNING("WARNING: Could not retrieve nPi0Proto. Will set it to 1.");
	nPi0sPerCluster = 1;
      }
      if (nPi0sPerCluster > 0) typeFlags.at((int)PanTau::TauConstituent::t_Pi0Neut) = 1;

    }
  }//end if pfo is in core
    
  // create the tau constituent
  tauConstituent = new PanTau::TauConstituent(momentum, pfoCharge, typeFlags, mvaValue, pfo);
  tauConstituent->makePrivateStore();
    
  // Check if the pfo object has shots:
  std::vector<const xAOD::IParticle*> list_TauShots = std::vector<const xAOD::IParticle*>(0);
  bool shotsOK = pfo->associatedParticles(xAOD::PFODetails::TauShot, list_TauShots);
  if (!shotsOK) {
    ATH_MSG_DEBUG("WARNING: Could not get shots from current pfo");
  }
    
  for (unsigned int iShot=0; iShot<list_TauShots.size(); iShot++) {
    
    if (list_TauShots.at(iShot) == nullptr) {
      ATH_MSG_WARNING("Shot number " << iShot << " points to 0! Skip it");
      continue;
    }
    
    xAOD::PFO*              curShot         = const_cast<xAOD::PFO*>( dynamic_cast<const xAOD::PFO*>(list_TauShots.at(iShot)) );
    TLorentzVector          shotMomentum;
    PanTau::SetP4EEtaPhiM( shotMomentum, curShot->e(), curShot->eta(), curShot->phi(), curShot->m());
    std::vector<int>        shotTypeFlags   = std::vector<int>((unsigned int)PanTau::TauConstituent::t_nTypes, 0);
    double                  shotMVAValue    = PanTau::TauConstituent::DefaultBDTValue();
    PanTau::TauConstituent* shotConstituent = new PanTau::TauConstituent(shotMomentum, 0, typeFlags, shotMVAValue, curShot);
    shotConstituent->makePrivateStore();
    
    int nPhotons = 0;
    if (!curShot->attribute(xAOD::PFODetails::tauShots_nPhotons, nPhotons)) {
      nPhotons = -1;
      ATH_MSG_DEBUG("WARNING: Could not get nPhotons for this shot! Set to -1");
    }
    shotConstituent->setNPhotonsInShot(nPhotons);
    tauConstituent->addShot(shotConstituent);            
  }//end loop over shots
    
  return StatusCode::SUCCESS;
}
