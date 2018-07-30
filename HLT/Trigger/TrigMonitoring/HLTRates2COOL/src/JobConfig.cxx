/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTRates2COOL/JobConfig.h"

hltca::JobConfig::JobConfig() :
   m_partitionName(""),
   m_coolDb(""),
   m_ISServer(""),
   m_currentRunNumber(0),
   m_COOLconnStat(false)
{}
	
hltca::JobConfig::~JobConfig() {}


std::ostream& hltca::operator<<(std::ostream& os, const hltca::JobConfig& c) {
  os << "Job configuration:" << std::endl
     << "  Partition             : " << c.m_partitionName << std::endl
     << "  COOL connection       : " << c.m_coolDb << std::endl
     << "  Current run number    : " << c.m_currentRunNumber << std::endl
     << "  COOL connection status: " << (c.m_COOLconnStat?"connected":"disconnected") << std::endl
     << "  IS Server             : " << c.m_ISServer << std::endl
     << "  OH Provider L2        : " << c.m_provider[0] << std::endl
     << "  Histogram L2          : " << c.m_histogram[0] << std::endl
     << "  OH Provider L2        : " << c.m_provider[1] << std::endl
     << "  Histogram L2          : " << c.m_histogram[1] << std::endl;
  return os;
}
