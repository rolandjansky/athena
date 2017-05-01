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
  StatusCode decode(const ROIB::RoIBResult*, const HLT::IDtoVIDMap& ctpToChain, std::vector<HLT::Identifier>& activeChains) const;

  StatusCode initialize(){ return StatusCode::SUCCESS; }

protected: 

}; 

inline const InterfaceID& CTPUnpackingTool::interfaceID() 
{ 
   return IID_CTPUnpackingTool; 
}


#endif // L1DECODER_CTPUNPACKINGTOOL_H
