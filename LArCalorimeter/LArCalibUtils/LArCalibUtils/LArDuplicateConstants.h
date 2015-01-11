/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/**
 * @file LArDuplicateConstants.h
 * @author Marco Delmastro <Marco.Delmastro@cern.ch>
 * @date Apr 2012
 * @brief Algorithm implementing ad-hoc duplication ofconstants
 * @brief e.g. fill PS LOW gain from MEDIUM gains values
 */

#ifndef LARDUPLICATECONSTANTS_H
#define LARDUPLICATECONSTANTS_H
 
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"

#include <vector>
#include <string>

class LArDuplicateConstants : public AthAlgorithm 
{ 
 public:
  LArDuplicateConstants(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArDuplicateConstants();

  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:
  const LArOnlineID* m_onlineHelper;

  unsigned int m_gainIN;
  unsigned int m_gainOUT;
 
  std::string m_keyPed;
  std::string m_keyRamp;
  std::string m_keyMPMC;
  std::string m_keyOFC;
  std::string m_keyShape;

};

#endif

