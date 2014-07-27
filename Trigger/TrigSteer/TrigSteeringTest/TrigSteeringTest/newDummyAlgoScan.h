/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteeringTest
 * @Class  : newDummyAlgoScan
 *
 * @brief : PESA algorithm that runs on all given input TE types, ie this is a so-called AllTEAlgo !
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: newDummyAlgoScan.h,v 1.3 2007-07-04 19:57:25 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERINGTEST_NEWDUMMYALGOSCAN_H
#define TRIGSTEERINGTEST_NEWDUMMYALGOSCAN_H

#include <string>

#include "TrigInterfaces/AllTEAlgo.h"


class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

namespace PESA {

  class newDummyAlgoScan : public HLT::AllTEAlgo
  {
  public:

    newDummyAlgoScan(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize

    /** @brief take all input TriggerElement of the configured type(s)

	@param input  outer vector is for the configured types, inner vector for the TE objects of that type
	@param output output TriggerElement type

	Note: no outpute TE is created in the base class, this dummy will create a configurable number of output TEs (default is 1)
    */
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*input*/,
			      unsigned int output);

  private:
    unsigned int m_numberOfOutputTEs; //!< number of output TEs this algorithm will create
  };
} // end namespace

#endif
