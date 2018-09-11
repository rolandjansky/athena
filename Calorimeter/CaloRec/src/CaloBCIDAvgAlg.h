//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOBCIDAVGALG_H
#define CALOREC_CALOBCIDAVGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "CaloEvent/CaloBCIDAverage.h"

class CaloBCIDAvgAlg : public AthAlgorithm {
 public:

  // constructor 
  CaloBCIDAvgAlg(const std::string& type, const std::string& name,
		 const IInterface* parent);
  // destructor 
  virtual ~CaloBCIDAvgAlg() override;
  
  // Algorithm virtual methods 
  virtual StatusCode initialize() override;
  virtual StatusCode execute_r(const EventContext& ctx) const;


  private:
  // properties 
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey;
  SG::WriteHandleKey<CaloBCIDAverage> m_bcidAvgKey;
  
  bool isMC;
};

#endif
