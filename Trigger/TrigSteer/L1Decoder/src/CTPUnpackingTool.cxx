/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CTPUnpackingTool.h"

using namespace HLT;

CTPUnpackingTool::~CTPUnpackingTool()
{}

StatusCode CTPUnpackingTool::decode(const ROIB::RoIBResult*, const IDtoVIDMap& ctpToChain, std::vector<Identifier>& activeChains) const{ return StatusCode::SUCCESS; }


