/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteeringTest
 * @Class  : dummyHypo
 *
 * @brief : simple PESA hypothesis algorithm
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: dummyHypo.h,v 1.2 2007-07-04 19:57:25 eifert Exp $
 **********************************************************************************/
#ifndef TRIGSTEERINGTEST_DUMMYHYPO_H
#define TRIGSTEERINGTEST_DUMMYHYPO_H


#include "TrigInterfaces/HypoAlgo.h"

namespace PESA {
  class dummyHypo : public HLT::HypoAlgo {
  public:
    dummyHypo(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    virtual HLT::ErrorCode hltInitialize(); //!< hlt initialize
    virtual HLT::ErrorCode hltFinalize();   //!< hlt finalize

    /** @brief
	execute the hypothesis on one TriggerElement!

	@param outputTE TriggerElement coming out of the Sequence, from this guy we can access all features!
	@param pass hypothesis okay ?
    */
    virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  private:
    unsigned int m_prescale; //!< how often should we accept ? every prescale'th event is accepted, ie 1 means accept all!
    unsigned int m_counter;  //!< event counter, for accepting a fraction only

  };
}

#endif /*TRIGSTEERINGTEST_DUMMYHYPO_H */
