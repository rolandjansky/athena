/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGTOOL_H
#define L1DECODER_CTPUNPACKINGTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"


namespace ROIB {
  class RoIBResult;
}


const InterfaceID IID_CTPUnpackingTool("CTPUnpackingTool", 1, 0);

class CTPUnpackingTool
  : virtual public ::IAlgTool, virtual public AthAlgTool
{ 
public:
  static const InterfaceID& interfaceID();
  
  CTPUnpackingTool( const std::string& type,
		    const std::string& name, 
		    const IInterface* parent );
  virtual ~CTPUnpackingTool();
  typedef std::map<size_t, HLT::IDVec> IndexToIdentifiers;

  /*
    @brief The method decodes CTP bits content of the RoIBResult and fills the list of chains which are activated by those bits
    The mapping ctpToChain is expected to contain the CTP bit number mapping to the chain identifiers
    @warning if the mapping is empty it means an empty menu. This condition is NOT checked and not reported.
    @warning if none of CTP bits is set this is also an error condition, this is the event should not have been passed to HLT
   */
  StatusCode decode(const ROIB::RoIBResult*, const IndexToIdentifiers& ctpToChain, HLT::IDVec& enabledChains) const;

  StatusCode initialize(){ return StatusCode::SUCCESS; }

protected: 

}; 

inline const InterfaceID& CTPUnpackingTool::interfaceID() 
{ 
   return IID_CTPUnpackingTool; 
}


#endif // L1DECODER_CTPUNPACKINGTOOL_H
