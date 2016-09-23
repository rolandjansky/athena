/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include <sstream>

std::vector<std::string> lineSplitter(const std::string& s, 
				      char delimiter){
  
  std::stringstream line (s);
  std::string seg;
  std::vector<std::string> segs;

  while(std::getline(line, seg, delimiter)){
    segs.push_back(seg);
  }
  
  return segs;
}
