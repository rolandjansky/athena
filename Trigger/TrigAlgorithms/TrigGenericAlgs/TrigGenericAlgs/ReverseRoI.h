/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 * @Package TrigGenericAlgs
 * 
 * @brief : the algorithm defined below updates TrigRoIDescriptor with direction opposite 
 * to given by TE. Allows to look in opposite hemisphere
 *
 * @author Olga Igonkina - Nikhef
 *
 *****************************************************************************/

#ifndef TRIGGENERICALGS_REVERSEROI_H
#define TRIGGENERICALGS_REVERSEROI_H


#include "TrigInterfaces/FexAlgo.h"


class StoreGateSvc;

namespace HLT
{
  class TriggerElement;
}


class ReverseRoI : public HLT::FexAlgo
{
 public:

  ReverseRoI(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize -> do nothing
  HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize   -> do nothing

  /** @brief take one input TriggerElement and revert TrigRoIDescriptor
      @param input  TriggerElement to be processed
      @param output TriggerElement coming out of the algorithm
  */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);

 private:
  float m_phi_shift;
  bool m_flip_eta;
  std::string m_input_label;
  std::string m_output_label;
  
};


#endif
