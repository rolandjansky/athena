/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteeringTest
 * @Class  : dummyAlgo
 *
 * @brief : simple PESA algorithm, taking exactly 1 input TE. Does not require any
 *          detector data!
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: dummyAlgo.h,v 1.9 2007-07-04 19:57:25 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERINGTEST_DUMMYALGO_H
#define TRIGSTEERINGTEST_DUMMYALGO_H

#include <string>

#include "TrigInterfaces/FexAlgo.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

namespace PESA {

  /**
     @class dummyAlgo
     simple PESA algorithm, taking exactly 1 input TE. Does not require any
     detector data!

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
     @author Tomasz Bold     <Tomasz.Bold@cern.ch>
 */
  class dummyAlgo : public HLT::FexAlgo
  {
  public:

    dummyAlgo(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize -> do nothing
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize   -> do nothing

    /** @brief take one input TriggerElement and process it
	It tries to retrieve any attache TrigRoIDescriptor features and prints them.
	@param input  TriggerElement to be processed
	@param output TriggerElement coming out of the algorithm
    */
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
			      HLT::TriggerElement* output);

  private:

  };
} // end namespace

#endif
