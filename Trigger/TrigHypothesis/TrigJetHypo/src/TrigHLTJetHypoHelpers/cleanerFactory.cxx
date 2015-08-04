/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     cleanerFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include <stdexcept>
#include <memory>

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // ICleaner
#include "./cleanerFactory.h"

#include "./BasicCleaner.h"
#include "./AntiBasicCleaner.h"
#include "./NullCleaner.h"

std::shared_ptr<ICleaner>
cleanerFactory(double m_n90Threshold,
               double m_presamplerThreshold,
               double m_negativeEThreshold,
               const std::string& key){

  using pIC = std::shared_ptr<ICleaner>;

  if (key == "antiBasicCleaning"){
        return pIC (new AntiBasicCleaner(m_n90Threshold, 
                                         m_presamplerThreshold,
                                         m_negativeEThreshold));

  } else if (key == "basicCleaning"){
    return pIC (new BasicCleaner(m_n90Threshold, 
                                 m_presamplerThreshold,
                                 m_negativeEThreshold));

  } else if (key == "noCleaning"){
    return pIC (new NullCleaner);

  } else {
    throw std::invalid_argument{"Unknown cleaner specified :" + key};

  }
}



