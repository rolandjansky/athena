/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGTOOL_H
#define L1DECODER_CTPUNPACKINGTOOL_H 1

#include "CTPUnpackingToolBase.h"

#include "DecisionHandling/HLTIdentifier.h"


class CTPUnpackingTool : public CTPUnpackingToolBase {
public:
  
  CTPUnpackingTool( const std::string& type,
                    const std::string& name, 
                    const IInterface* parent );

  virtual StatusCode decode(const ROIB::RoIBResult& roib, HLT::IDVec& enabledChains) const override;

  virtual StatusCode initialize() override;

private:

}; 


#endif
