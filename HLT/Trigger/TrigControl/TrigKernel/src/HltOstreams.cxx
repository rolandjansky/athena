/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//=========================================================================
// output operators for HLTResult
//=========================================================================

#include "TrigKernel/HltOstreams.h"
#include "TrigKernel/HltAcceptFlag.h"
#include "TrigKernel/HltResultStatusCode.h"
#include "TrigKernel/HltPscErrorCode.h"

std::ostream& operator<<(std::ostream& os, const std::vector<eformat::helper::StreamTag>& rhs)
{
  std::string prefix("\n(");
  for (std::vector<eformat::helper::StreamTag>::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
    os << prefix << (*it).name << "," << (*it).type << "," << (*it).obeys_lumiblock
       << ",[Rob="  << (*it).robs.size() << ":" << (*it).robs
       << "],[Det=" << (*it).dets.size() << ":" << (*it).dets
       << "])";
    prefix=",\n(";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<uint32_t>& rhs)
{
  std::string prefix("0x");
  os << std::hex << std::setfill( '0' );
  for (std::vector<uint32_t>::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
    os << prefix << std::setw(8) << *it;
    prefix = ", 0x";
  }
  os << std::dec << std::setfill(' ');
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<eformat::SubDetector>& rhs)
{
  std::string prefix("0x");
  for (std::vector<eformat::SubDetector>::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
    os << prefix << std::hex << *it << std::dec << "="
       << eformat::helper::SourceIdentifier(*it,0).human_detector();
    prefix = ", 0x";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::set<uint32_t>& rhs)
{
  std::string prefix("0x");
  os << std::hex << std::setfill( '0' );
  for (std::set<uint32_t>::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
    os << prefix << std::setw(8) << *it;
    prefix = ", 0x";
  }
  os << std::dec << std::setfill(' ');
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::set<eformat::SubDetector>& rhs)
{
  std::string prefix("0x");
  for (std::set<eformat::SubDetector>::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
    os << prefix << std::hex << *it << std::dec << "="
       << eformat::helper::SourceIdentifier(*it,0).human_detector();
    prefix = ", 0x";
  }
  return os;
}
