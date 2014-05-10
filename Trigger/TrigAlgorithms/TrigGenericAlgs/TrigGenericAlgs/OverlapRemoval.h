/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 * @Package TrigGenericAlgs
 * 
 * @brief : the algorithm defined below is supposed to remove from trigger 
 * all combinations which are too close in eta/phi. It is based on the 
 * eta/phi values found in TrigRoiDescriptors
 *
 * @author Nicole Carlson - summer student project
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 *****************************************************************************/

#ifndef TRIGGENERICALGS_OVERLAPREMOVAL_H
#define TRIGGENERICALGS_OVERLAPREMOVAL_H


#include <string>

#include "TrigInterfaces/ComboAlgo.h"


class StoreGateSvc;

namespace HLT
{
  class TriggerElement;
}


class OverlapRemoval : public HLT::ComboAlgo
{
 public:

  OverlapRemoval(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize()   { return HLT::OK; }

  HLT::ErrorCode acceptInputs(HLT::TEConstVec&, bool& pass);

  HLT::ErrorCode hltExecute(std::vector<const HLT::TriggerElement*>& inputs,
			    HLT::TriggerElement* output);

 private:

  double m_MinCentDist; //!< this is the delta R which is used as a referenece for comparisons

  double m_MinPhiDist;  //!< User must take care not to set this distance smaller than the width of the RoI
  double m_MinEtaDist;  //!< User must take care not to set this distance smaller than the width of the RoI
  bool m_RoICheck;      //!< use eta,phi from the RoI nodes ? otherwise, eta,phi is used from the current TE's TrigRoIDescriptors

  float m_sqMinCentDist; //!< this is delta_R**2 to speed up the execute

  ///Variables used for monitoring
  double m_deltaRSq, m_deltaEta, m_deltaPhi;
  double m_eta, m_phi;

};


#endif
