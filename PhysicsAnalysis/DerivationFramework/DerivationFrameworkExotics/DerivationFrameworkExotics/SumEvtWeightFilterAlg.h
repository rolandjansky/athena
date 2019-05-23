/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_SUMEVTWEIGHTFILTERALG_H
#define DERIVATIONFRAMEWORK_SUMEVTWEIGHTFILTERALG_H
/**
 * @file SumEvtWeightFilterAlg.h
 * @brief class definition for SumEvtWeightFilterAlg
 */

/**
 * @class SumEvtWeightFilterAlg
 * @brief Simple Filter algorithm which simply sets pass=true for all events.
 * <br>
 * @author Doug Schaefer (schae@cern.ch)
 */
            
#include "AthenaBaseComps/AthFilterAlgorithm.h"
 
class string;
class ISvcLocator;
     
namespace DerivationFramework {

  class SumEvtWeightFilterAlg : public ::AthFilterAlgorithm 
  {
  public:
    SumEvtWeightFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SumEvtWeightFilterAlg();
    
    virtual StatusCode initialize() override; 
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:

    /// An event counter
    unsigned long m_eventsProcessed;
   /// Keep a vector of all cutIDs for the non-nominal MC event weights
    std::vector<CutIdentifier> m_mcCutIDs;
  };



}
#endif
