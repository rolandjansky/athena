/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTRates2COOL/JobConfig.h"

hltca::JobConfig::JobConfig() :
   fPartitionName(""),
   fCoolDb(""),
   fISServer(""),
   fCurrentRunNumber(0),
   fCOOLconnStat(false)
{}
	
hltca::JobConfig::~JobConfig() {}


std::ostream& hltca::operator<<(std::ostream& os, const hltca::JobConfig& c) {
  os << "Job configuration:" << std::endl
     << "  Partition             : " << c.fPartitionName << std::endl
     << "  COOL connection       : " << c.fCoolDb << std::endl
     << "  Current run number    : " << c.fCurrentRunNumber << std::endl
     << "  COOL connection status: " << (c.fCOOLconnStat?"connected":"disconnected") << std::endl
     << "  IS Server             : " << c.fISServer << std::endl
     << "  OH Provider L2        : " << c.fProvider[0] << std::endl
     << "  Histogram L2          : " << c.fHistogram[0] << std::endl
     << "  OH Provider L2        : " << c.fProvider[1] << std::endl
     << "  Histogram L2          : " << c.fHistogram[1] << std::endl;
  return os;
}
