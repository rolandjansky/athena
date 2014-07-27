/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteeringTest
 * @Class  : dummyAlgo
 *
 * @brief : simple PESA algorithm, taking exactly 1 input TE. Does not require any
 *          detector data! This is modification to test some monitoring capabilities of the 
 *          framework. 
 *          For another more complete example have look at TrigMonitorBase/test/MonitoredAlgo_test.cxx
 *
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/

#ifndef TRIGSTEERINGTEST_DUMMYMONITOREDFEX_H
#define TRIGSTEERINGTEST_DUMMYMONITOREDFEX_H

#include <string>
#include <vector>
#include "TrigInterfaces/FexAlgo.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

class TrigTimer;

namespace PESA {

  /**
     @class dummyMonitoredFex
     simple PESA algorithm, taking exactly 1 input TE. Does not require any
     detector data!

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
     @author Tomasz Bold     <Tomasz.Bold@cern.ch>
 */
  class dummyMonitoredFex : public HLT::FexAlgo
  {
  public:

    dummyMonitoredFex(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize   -> do nothing

    /** @brief take one input TriggerElement and process it
	It tries to retrieve any attache TrigRoIDescriptor features and prints them.
	@param input  TriggerElement to be processed
	@param output TriggerElement coming out of the algorithm
    */
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
			      HLT::TriggerElement* output);

    const std::vector<double>& trkPhi() const { return m_trkPhi; }  //!< fake
    double clusterPhi() const { return 0; } //!< fake 

  private:
    // test timers
    TrigTimer* m_timer1;  //!< extra timer to test timers monitoring functionality
    TrigTimer* m_timer2;  
    TrigTimer* m_timer3;
    
    std::vector<double> m_trkPhi; //!< fake

    std::vector<int>  m_vectorOfFixedSize1;
    std::vector<int>  m_vectorOfFixedSize2;

    
    

  };
} // end namespace

#endif
