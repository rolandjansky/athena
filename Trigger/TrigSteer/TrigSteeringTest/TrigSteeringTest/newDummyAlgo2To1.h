/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteeringTest
 * @Class  : newDummyAlgo2To1
 *
 * @brief : PESA algorithm that runs on 2 input TEs, ie this is a so-called ComboAlgorithm !
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: newDummyAlgo2To1.h,v 1.4 2007-07-04 19:57:25 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERINGTEST_NEWDUMMYALGO2TO1_H
#define TRIGSTEERINGTEST_NEWDUMMYALGO2TO1_H

#include <string>

#include "TrigInterfaces/ComboAlgo.h"


class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

namespace PESA {

  class newDummyAlgo2To1 : public HLT::ComboAlgo
  {
  public:

    newDummyAlgo2To1(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize


    /** @brief take two input TriggerElement and process them

	@param input  vector of (2) TriggerElement to be processed
	@param output TriggerElement coming out of the algorithm
    */
    HLT::ErrorCode hltExecute(std::vector<const HLT::TriggerElement*>& input,
			      HLT::TriggerElement* output);

  private:
    unsigned m_rejectRate;
    unsigned m_rejectCounter; 

  };
} // end namespace

#endif
