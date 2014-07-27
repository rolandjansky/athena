/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
//
// NAME: AlgoOverlap.h
// PACKAGE: Trigger/TrigSteer/TrigSteeringTest
//
// AUTHOR: N. Carlson
//
// ****************************************************************************

#ifndef TRIGSTEERINGTEST_ALGOOVERLAP_H
#define TRIGSTEERINGTEST_ALGOOVERLAP_H

#include <string>

#include "TrigInterfaces/ComboAlgo.h"


class StoreGateSvc;

namespace HLT
 {
   class TriggerElement;
 }

namespace PESA
{

  class AlgoOverlap : public HLT::ComboAlgo
  {
  public:

    AlgoOverlap(const std::string& name, ISvcLocator* pSvcLocator);

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize()   { return HLT::OK; }

    HLT::ErrorCode acceptInputs(HLT::TEConstVec&, bool& pass);

    HLT::ErrorCode hltExecute(std::vector<const HLT::TriggerElement*>& /*input*/,
			      HLT::TriggerElement* /*output*/){ return HLT::OK; }

  private:

    double m_MinCentDist; //!< this is the delta R which is used as a referenec for comparisons

    double m_MinPhiDist;  //!< User must take care not to set this distance smaller than the width of the RoI
    double m_MinEtaDist;  //!< User must take care not to set this distance smaller than the width of the RoI
    bool m_RoICheck;      //!< use eta,phi from the RoI nodes ? otherwise, eta,phi is used from the current TE's TrigRoIDescriptors

    bool m_sqMinCentDist; //!< this is delta_R**2 to speed up the execute

  };
} // end namespace

#endif
