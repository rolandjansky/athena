/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/Tool_TauConstituentGetter.h"
#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/Tool_InputConverter.h"
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"

PanTau::Tool_TauConstituentGetter::Tool_TauConstituentGetter(const std::string& name) :
  asg::AsgTool(name),
  m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
  m_Tool_InputConverter("PanTau::Tool_InputConverter/Tool_InputConverter", this)
{
  declareProperty("Tool_InformationStore",    m_Tool_InformationStore,   "Link to tool with all information");
  declareProperty("Tool_InputConverter",      m_Tool_InputConverter,     "Link to tool to convert into TauConstituents");
  declareProperty("Tool_InformationStoreName",    m_Tool_InformationStoreName,   "Link to tool with all information");
  declareProperty("Tool_InputConverterName",      m_Tool_InputConverterName,     "Link to tool to convert into TauConstituents");
}

PanTau::Tool_TauConstituentGetter::~Tool_TauConstituentGetter() {
}

StatusCode PanTau::Tool_TauConstituentGetter::initialize() {

  ATH_MSG_INFO(" initialize()");
  m_init=true;

  ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InformationStore, m_Tool_InformationStoreName ) );
  ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InputConverter, m_Tool_InputConverterName ) );
    
  ATH_CHECK( m_Tool_InformationStore.retrieve() );
  ATH_CHECK( m_Tool_InputConverter.retrieve() );
    
  return StatusCode::SUCCESS;
}


/**
 * Function to get the PFOs for a given TauJet object (Shots in each PFO etc are collected in "ConvertToTauConstituent")
 */
StatusCode PanTau::Tool_TauConstituentGetter::GetTauConstituents(const xAOD::TauJet* tauJet,
                                                                 std::vector<TauConstituent*>& outputConstituents,
                                                                 const std::string& algName) const {
    
  if(algName != "CellBased") {
    ATH_MSG_WARNING("Unknown input algorithm: " << algName << " -> Pantau BDT Training not done for this algorithm!");
    return StatusCode::FAILURE;
  }
    
  //loop over charged PFOs
  unsigned int nChargedPFO = tauJet->nProtoChargedPFOs();

  for(unsigned int iChrgPFO=0; iChrgPFO<nChargedPFO; iChrgPFO++) {
    xAOD::PFO* curChrgPFO = const_cast<xAOD::PFO*>(tauJet->protoChargedPFO( iChrgPFO ));

    //convert to tau constituent
    PanTau::TauConstituent* curConst = nullptr;
    ATH_CHECK(m_Tool_InputConverter->ConvertToTauConstituent(curChrgPFO, curConst, tauJet) );
    if(curConst == nullptr) {
      ATH_MSG_DEBUG("Problems converting charged PFO into tau constituent -> skip PFO");
      continue;
    }
        
    //add to list of tau constituents
    outputConstituents.push_back(curConst);
        
  }//end loop over charged PFOs
  
  // Pi0 tagged PFOs are not collected!
    
  //loop over neutral PFOs
  unsigned int nNeutPFO = tauJet->nProtoNeutralPFOs();
    
  for(unsigned int iNeutPFO=0; iNeutPFO<nNeutPFO; iNeutPFO++) {
    xAOD::PFO* curNeutPFO = const_cast<xAOD::PFO*>(tauJet->protoNeutralPFO( iNeutPFO ));
        
    //convert to tau constituent
    PanTau::TauConstituent* curConst = nullptr;
    ATH_CHECK( m_Tool_InputConverter->ConvertToTauConstituent(curNeutPFO, curConst, tauJet) );
    if(curConst == nullptr) {
      ATH_MSG_DEBUG("Problems converting neutral PFO into tau constituent -> skip PFO");
      continue;
    }
        
    //add to list of tau constituents
    outputConstituents.push_back(curConst);
        
  }//end loop over charged PFOs
       
  return StatusCode::SUCCESS;    
}
