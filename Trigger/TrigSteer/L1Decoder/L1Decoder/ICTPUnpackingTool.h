/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_ICTPUNPACKINGTOOL_H
#define L1DECODER_ICTPUNPACKINGTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


namespace ROIB {
  class RoIBResult;
}

/**
 * Interface for CTP unpacking tools
 */
class ICTPUnpackingTool : virtual public IAlgTool { 
public:
  DeclareInterfaceID(ICTPUnpackingTool, 1, 0);

  /**
   * The method decodes CTP bits content of the RoIBResult and fills the list of chains which are activated by those bits
   *
   * The mapping ctpToChain is expected to contain the CTP bit number mapping to the chain identifiers
   *
   * @warning if the mapping is empty it means an empty menu. This condition is NOT checked and not reported.
   * @warning if none of CTP bits is set this is also an error condition, this is the event should not have been passed to HLT
   */
  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const = 0;

  /**
   * The method decodes CTP bits content of the RoIBResult and checks if L1 Items
   * from a given list of names passed before prescale
   */
  virtual StatusCode passBeforePrescaleSelection(const ROIB::RoIBResult* roib, const std::vector<std::string>& l1ItemNames, bool& pass) const = 0;

};  


#endif
