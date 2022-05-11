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
  
  void eFEXFillEDM::fillEmEDM(std::unique_ptr<xAOD::eFexEMRoIContainer> &container, uint8_t eFexNum, const eFEXegTOB& tobObject)
  {
    uint32_t tobWord0 = tobObject.getTobword();
    // Only needed for xTOBs, which aren't filled yet
    // uint32_t tobWord1 = 0;

    // Translate eFEX index into Shelf+eFEX:
    uint8_t shelf = int(eFexNum/12);
    uint8_t eFEX  = eFexNum%12;
    
    // Now create the object and fill it
    xAOD::eFexEMRoI* myEmEDM = new xAOD::eFexEMRoI();
    
    container->push_back(myEmEDM);
    
    myEmEDM->initialize(eFEX, shelf, tobWord0); 

    myEmEDM->setRetaCore(tobObject.getRetaCore());
    myEmEDM->setRetaEnv(tobObject.getRetaEnv());
    myEmEDM->setRhadEM(tobObject.getRhadEM());
    myEmEDM->setRhadHad(tobObject.getRhadHad());
    myEmEDM->setWstotNumerator(tobObject.getWstotNum());
    myEmEDM->setWstotDenominator(tobObject.getWstotDen());

    ATH_MSG_DEBUG(" setting eFEX Number:  " << +myEmEDM->eFexNumber() << " shelf: " << +myEmEDM->shelfNumber() << " et: " << myEmEDM->et() << " eta: " << myEmEDM->eta() <<  " phi: " << myEmEDM->phi() << " input eFexNum: " << +eFexNum << " TOB word: " << tobWord0 << MSG::dec );

  }

  void eFEXFillEDM::fillTauEDM(std::unique_ptr<xAOD::eFexTauRoIContainer> &container, uint8_t eFexNum, const eFEXtauTOB& tobObject)
  {
    uint32_t tobWord0 = tobObject.getTobword();
    // Only needed for xTOBs, which aren't filled yet
    //  uint32_t tobWord1 = 0;

    // Translate eFEX index into Shelf+eFEX:
    uint8_t shelf = int(eFexNum/12);
    uint8_t eFEX  = eFexNum%12;
    
    xAOD::eFexTauRoI* myTauEDM = new xAOD::eFexTauRoI();

    container->push_back(myTauEDM);

    myTauEDM->initialize(eFEX, shelf, tobWord0);
    // There is some ambiguity in what 'numerator'/'denominator' mean in each of the tau isolation 
    // variables rCore and rHad below, since in the more 'physical' definition, we would consider core/(core+env),
    // whereas in the firmware we calculate core/env. Here, I store core->numerator, env->denominator. 
    // Provided we remember this when using them, we can then calculate either the 'physical' or the 'firmware' values.
    myTauEDM->setRCoreNumerator(tobObject.getRcoreCore());
    myTauEDM->setRCoreDenominator(tobObject.getRcoreEnv());
    myTauEDM->setRHadNumerator(tobObject.getRhadCore());
    myTauEDM->setRHadDenominator(tobObject.getRhadEnv());
    ATH_MSG_DEBUG(" setting tau eFEX Number: " << +myTauEDM->eFexNumber() << " shelf: " << +myTauEDM->shelfNumber() << " et: " << myTauEDM->et() << " eta: " << myTauEDM->eta() << " phi: " << myTauEDM->phi() << " input eFexNum: " << +eFexNum << " TOB word: " << tobWord0 << MSG::dec);

  }

} // end of namespace bracket

