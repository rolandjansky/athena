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
    uint8_t eFEXNumber = eFexNum;
    uint32_t tobWord0 = tobWord;
    uint32_t tobWord1 = 0;

    // Kludge to test new eFEX number format
    int offset = 0;
    if (eFEXNumber < 0xC0) offset = 1;
    if (eFEXNumber < 0xB0) offset = 2;
    int octant = eFEXNumber&0xf;
    int residual = octant%4;

    uint8_t shelf = (octant > 3 ? 1 : 0);
    uint8_t eFEX = residual*3 + offset;

    xAOD::eFexEMRoI* myEmEDM = new xAOD::eFexEMRoI();
    
    container->push_back(myEmEDM);
    
    myEmEDM->initialize(eFEX, shelf, tobWord0, tobWord1);
    ATH_MSG_DEBUG(" setting eFEX Number: " << +myEmEDM->eFexNumber() << " shelf: " << myEmEDM->shelfNumber() << " et: " << myEmEDM->et() << " eta: " << myEmEDM->eta() << " phi: " << myEmEDM->phi() );

  }

  void eFEXFillEDM::fillTauEDM(std::unique_ptr<xAOD::eFexTauRoIContainer> &container, uint8_t eFexNum, uint32_t tobWord)
  {
    uint8_t eFEXNumber = eFexNum;
    uint32_t tobWord0 = tobWord;
    uint32_t tobWord1 = 0;

    xAOD::eFexTauRoI* myTauEDM = new xAOD::eFexTauRoI();

    container->push_back(myTauEDM);

    myTauEDM->initialize(eFEXNumber, tobWord0, tobWord1);
    ATH_MSG_DEBUG(" setting eFEX Number: " << +myTauEDM->eFexNumber() << " et: " << myTauEDM->et() << " eta: " << myTauEDM->eta() << " phi: " << myTauEDM->phi() );

  }

} // end of namespace bracket

