/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
// L1Decoder includes
#include "IRoIsUnpackingTool.h"



IRoIsUnpackingTool::~IRoIsUnpackingTool()
{}



StatusCode IRoIsUnpackingTool::decodeMapping() {
  std::istringstream input;
  for ( auto entry: m_thresholdToChainProperty ) {
    input.clear();
    input.str(entry);
    std::string thresholdName;
    char delim;
    std::string chainName;
    input >> thresholdName >> delim >> chainName;
    if ( delim != ':' ) {
      return StatusCode::FAILURE;
    }
    m_thresholdToChainMapping[HLT::Identifier(thresholdName)].push_back(HLT::Identifier(chainName));
  }
  return StatusCode::SUCCESS;
}







