/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***********************************************************************
//                            eFEXFillEDM - description
//                              ---------------------
//      begin                 : 22 04 2021
//      email                 : nluongo@uoregon.edu
//***********************************************************************

#include "L1CaloFEXSim/eFEXFillEDM.h" 

namespace LVL1 {

  // default constructor for persistency
  
  eFEXFillEDM::eFEXFillEDM(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent) 
  {
    declareInterface<IeFEXFillEDM>(this);
  }

  //----------------- Initialization ----------------------------

  StatusCode eFEXFillEDM::initialize()
  {
    return StatusCode::SUCCESS;
  }

  //----------------- Execute ----------------------------

  StatusCode eFEXFillEDM::execute()
  {
    return StatusCode::SUCCESS;
  }

  //----------------- Finalisation ------------------------------

  StatusCode eFEXFillEDM::finalize()
  {
    return StatusCode::SUCCESS;
  }
  
  void eFEXFillEDM::fillEmEDM(std::unique_ptr<xAOD::eFexEMRoIContainer> &container, uint8_t eFexNum, uint32_t tobWord)
  {
    uint32_t tobWord0 = tobWord;
    // Only needed for xTOBs, which aren't filled yet
    // uint32_t tobWord1 = 0;

    // Translate eFEX index into Shelf+eFEX:
    uint8_t shelf = int(eFexNum/12);
    uint8_t eFEX  = eFexNum%12;
    
    // Now create the object and fill it
    xAOD::eFexEMRoI* myEmEDM = new xAOD::eFexEMRoI();
    
    container->push_back(myEmEDM);
    
    myEmEDM->initialize(eFEX, shelf, tobWord0); 

    ATH_MSG_DEBUG(" setting eFEX Number:  " << +myEmEDM->eFexNumber() << " shelf: " << +myEmEDM->shelfNumber() << " et: " << myEmEDM->et() << " eta: " << myEmEDM->eta() <<  " phi: " << myEmEDM->phi() << " input eFexNum: " << +eFexNum << " TOB word: " << tobWord0 << MSG::dec );

  }

  void eFEXFillEDM::fillTauEDM(std::unique_ptr<xAOD::eFexTauRoIContainer> &container, uint8_t eFexNum, uint32_t tobWord)
  {
    uint32_t tobWord0 = tobWord;
    // Only needed for xTOBs, which aren't filled yet
    //  uint32_t tobWord1 = 0;

    // Translate eFEX index into Shelf+eFEX:
    uint8_t shelf = int(eFexNum/12);
    uint8_t eFEX  = eFexNum%12;
    
    xAOD::eFexTauRoI* myTauEDM = new xAOD::eFexTauRoI();

    container->push_back(myTauEDM);

    myTauEDM->initialize(eFEX, shelf, tobWord0);
    ATH_MSG_DEBUG(" setting tau eFEX Number: " << +myTauEDM->eFexNumber() << " shelf: " << +myTauEDM->shelfNumber() << " et: " << myTauEDM->et() << " eta: " << myTauEDM->eta() << " phi: " << myTauEDM->phi() << " input eFexNum: " << +eFexNum << " TOB word: " << tobWord0 << MSG::dec);

  }

} // end of namespace bracket

