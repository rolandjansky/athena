/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAMPCORR_H
#define LARRAMPCORR_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h" 
#include "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawConditions/LArRawRampContainer.h"
#include "LArRawConditions/LArRampComplete.h"

#include <vector>

// Simple algorithm to produce correction for Ramps from an ascii file
// author: G.Unal

class LArRampCorr : public AthAlgorithm//, public IIncidentListener 
{
 public:
  LArRampCorr(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArRampCorr();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

private:
  // Output key
  std::string m_keyoutput;
    
  // Grouping type
  std::string m_groupingType;

  // list of corrections
  std::vector<std::string> m_inputStringIDs;
  
  const LArOnlineID* m_onlineHelper;
};

#endif
