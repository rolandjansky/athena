/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteeringTest
 * @Class  : dummyAlgoHLTErrorCode
 *
 * @brief : simple PESA algorithm, taking exactly 1 input TE. Does not require any
 *          detector data! This algorithm can be configured to throw C++ exception
 *          or return an HLT::ErrorCode. This way, one can exercise the error handling
 *          and propagation inside the high level trigger!
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 *
 * File and Version Information:
 * $Id: dummyAlgoHLTErrorCode.h,v 1.4 2008-12-18 11:26:14 fwinkl Exp $
 **********************************************************************************/

#ifndef TRIGSTEERINGTEST_DUMMYALGOHLTERRORCODE_H
#define TRIGSTEERINGTEST_DUMMYALGOHLTERRORCODE_H

#include <string>

#include "TrigInterfaces/FexAlgo.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

namespace PESA {

  /**
     @class dummyException
     simple exception class.
  */
  class dummyException
  {
  public:
    const char* what() const { return  "a dummy exception"; }
  };

  /**
     @class dummyAlgoHLTErrorCode
     simple PESA algorithm, taking exactly 1 input TE. Does not require any
     detector data! This algorithm can be configured to throw C++ exception
     or return an HLT::ErrorCode. This way, one can exercise the error handling
     and propagation inside the high level trigger!

     @author Till Eifert     <Till.Eifert@cern.ch>
  */
  class dummyAlgoHLTErrorCode : public HLT::FexAlgo
  {
  public:

    dummyAlgoHLTErrorCode(const std::string& name, ISvcLocator* pSvcLocator); //!< std constructor

    HLT::ErrorCode hltInitialize(); //!< hlt initialize -> set output ErrorCode from jobProperty string
    HLT::ErrorCode hltFinalize() { return HLT::OK; }    //!< hlt finalize -> do nothing

    /** @brief hlt execute, throw the configured exception and or return an ErrorCode
     */
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
			      HLT::TriggerElement* output);

  private:
    std::string m_paramSet;  //!< string of ErrorCode to return
    HLT::ErrorCode m_error;  //!< the ErrorCode, is transformed internally
    bool m_stdException;     //!< throw an std exception ?
    bool m_gaudiException;   //!< throw an gaudi exception ?
    bool m_unknownException; //!< throw an unkown exception ?
    float m_burnTime;        //!< burn time in nanoseconds
  };
} // end namespace

#endif
