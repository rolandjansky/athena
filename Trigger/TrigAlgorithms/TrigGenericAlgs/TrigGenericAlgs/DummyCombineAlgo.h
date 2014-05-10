/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigGenericAlgs
 * @Class  : DummyCombineAlgo
 *
 * @brief : PESA algorithm that runs on every input TE combination
 *          (apart from combinations that overlap in the RoI objects).
 *          This algorithms is a "dummy" in the sense that nothing happens inside.
 *          The purpose of this algorithm is to combine more than one sequence of
 *          algorithm thus allowing the following algos to retrieve features from
 *          all previous algorithms.
 *          Example: 1. input: unseeded IDSCAN_fullScan sequence
 *                   2. input: regular calo sequence
 *                      algos="DummyCombineAlgo"
 *                      output: combinedTE
 *                   The next algorithm running on combinedTE can then use features from
 *                   all previous algorithms.
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: DummyCombineAlgo.h,v 1.1 2007-10-04 09:24:34 eifert Exp $
 **********************************************************************************/

#ifndef PESA_DUMMYCOMBINEALGO_H
#define PESA_DUMMYCOMBINEALGO_H

#include <string>

#include "TrigInterfaces/ComboAlgo.h"


class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}


namespace PESA
{

  class DummyCombineAlgo : public HLT::ComboAlgo
  {
  public:

    DummyCombineAlgo(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize

    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputs, HLT::TriggerElement* output); //!< hlt execute

  };
} // end of namespace

#endif
