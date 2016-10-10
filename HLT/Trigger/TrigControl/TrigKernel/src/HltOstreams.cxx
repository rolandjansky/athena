/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

std::ostream& operator<<(std::ostream& os, const hltinterface::HLTResult& rhs) {
  os << "[HLT Result]" ;
  os << "\n HLT Result: fragment_pointer = " << std::hex << rhs.fragment_pointer << std::dec
     << ", max_result_size [words] = " << rhs.max_result_size ;
  os << "\n HLT Result: Number of HLT result ROBs = " << rhs.hltResult_robs.size() ;
  os << "\n [";
  if (rhs.fragment_pointer != 0) {
    for (const auto& hltrob : rhs.hltResult_robs) {
      os << "\n HLT ROB: Source Identifier = 0x" << std::hex << (hltrob.source_id()) << std::dec
	 << ", ROB fragment size [words] = " << std::setw(6) << hltrob.fragment_size_word()
	 << ", # status words = " << hltrob.nstatus() << ", status words =" ;
      const uint32_t* it;
      hltrob.status(it);
      for (uint32_t i(0); i < hltrob.nstatus(); ++i) {
	os << std::hex <<  std::setfill( '0' ) << " 0x" << std::setw(8) << *(it+i);
	os << std::dec << std::setfill(' ');
	if (i == 1)
	  os << "(" << hltonl::PrintHltResultStatusCode((hltonl::ResultStatusCode) *(it+1)) << ")";
	if (i == 2)
	  os << "(" << hltonl::PrintPscErrorCode((hltonl::PSCErrorCode) *(it+2)) << ")";
      }
    }
  } else {
    os << "\n No HLT ROB Fragment: fragment_pointer pointer=0x" << std::hex << rhs.fragment_pointer << std::dec;
  } 
  os << "\n ]";

  os << "\n Stream tags (Name, Type, LumiBlock, PEB Robs, PEB SubDets) : # of tags = " << rhs.stream_tag.size()     
     << "\n [" << rhs.stream_tag << "\n ]";
  os << "\n HLT trigger info words: # of words = " << rhs.trigger_info.size()
     << "\n [" << rhs.trigger_info << "]";
  os << "\n HLT PSC error words:    # of words = " << rhs.psc_errors.size()
     << "\n [" << rhs.psc_errors << "]";
  return os;
}
