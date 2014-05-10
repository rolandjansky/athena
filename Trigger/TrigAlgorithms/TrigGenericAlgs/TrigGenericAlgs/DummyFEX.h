/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigGenericAlgs
 * @Class  : DummyFEX
 *
 * @brief : simple PESA algorithm, taking exactly 1 input TE. Does not require any
 *          detector data!
 *          This algorithm is a copy of the PESA::dummyAlg with further simplification. 
 *          It is to be used in cases when users for various reasons needs to have FEX
 *          algorithm just to be able to create sequence.
 *          
 *           
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: DummyFEX.h,v 1.1.1.1 2007-08-17 07:47:27 tbold Exp $
 **********************************************************************************/

#ifndef TRIGGENERICALGS_DUMMYFEX_H
#define TRIGGENERICALGS_DUMMYFEX_H

#include <string>

#include "TrigInterfaces/FexAlgo.h"


namespace HLT {
  class TriggerElement;
}

/**
   @class DummyFEX
   simple PESA algorithm, taking exactly 1 input TE. Does not require any
   Detector data!

   @author Till Eifert     <Till.Eifert@cern.ch>
   @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   @author Tomasz Bold     <Tomasz.Bold@cern.ch>
*/
class DummyFEX : public HLT::FexAlgo
{
 public:

  DummyFEX(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

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


#endif
