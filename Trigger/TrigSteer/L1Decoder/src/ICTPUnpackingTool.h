// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_ICTPUNPACKINGTOOL_H
#define L1DECODER_ICTPUNPACKINGTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"


namespace ROIB {
  class RoIBResult;
}


const InterfaceID IID_ICTPUnpackingTool("ICTPUnpackingTool", 1, 0);

class ICTPUnpackingTool
  : virtual public ::IAlgTool
{ 
public:
  static const InterfaceID& interfaceID();
  virtual ~ICTPUnpackingTool();


  /*
    @brief The method decodes CTP bits content of the RoIBResult and fills the list of chains which are activated by those bits
    The mapping ctpToChain is expected to contain the CTP bit number mapping to the chain identifiers
    @warning if the mapping is empty it means an empty menu. This condition is NOT checked and not reported.
    @warning if none of CTP bits is set this is also an error condition, this is the event should not have been passed to HLT
   */
  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const = 0;

  virtual StatusCode initialize() = 0;
  
  
protected:

  StatusCode decodeCTPToChainMapping();
  typedef std::map<size_t, HLT::IDVec> IndexToIdentifiers;
  IndexToIdentifiers       m_ctpToChain;
  std::vector<std::string> m_ctpToChainProperty;
  bool m_forceEnable; 

}; 

inline const InterfaceID& ICTPUnpackingTool::interfaceID() 
{ 
   return IID_ICTPUnpackingTool; 
}


#endif // L1DECODER_ICTPUNPACKINGTOOL_H
