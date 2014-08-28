
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

// tdaq-common includes for CTP format definition
#include "CTPfragment/CTPdataformat.h"

namespace LVL1CTP {

  const std::string convertRawCTPData(const std::vector<uint32_t>& data, const bool longFormat)
  {
    std::ostringstream s;

    // time is only stored once for the whole fragment
    for (size_t i(0); (i < CTPdataformat::NumberTimeWords) && (i < data.size()); ++i) {
      if (i == 0 || longFormat) s << "\nTime";
      if (longFormat) s << std::setw(1) << i;
      s << " " << std::setw(8) << data[i];
      if (longFormat) s << std::endl;
    }

    // return if data content is too short
    if (data.size() < 2) return s.str();
    

    // loop over the rest of the data fragment
    for (unsigned int k(0); k < (data.size()-2)/CTPdataformat::DAQwordsPerBunch; ++k) {
      
      // print single fragment

      // PIT
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::PITpos);
	   (i < CTPdataformat::PITwords) && (p < data.size()); 
	   ++i, ++p) {
	if (i == 0 || longFormat) s << "\nPIT";
	if (longFormat) s << std::setw(1) << i;
	s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
	if (longFormat) s << std::endl;
      }
      
      // TBP
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::TBPpos);
	   (i < CTPdataformat::TBPwords) && (p < data.size()); 
	   ++i, ++p) {
	if (i == 0 || longFormat) s << "\nTBP";
	if (longFormat) s << std::setw(1) << i;
	s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
	if (longFormat) s << std::endl;
      }
      
      // TAP
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::TAPpos);
	   (i < CTPdataformat::TAPwords) && (p < data.size()); 
	   ++i, ++p) {
	if (i == 0 || longFormat) s << "\nTAP";
	if (longFormat) s << std::setw(1) << i;
	s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
	if (longFormat) s << std::endl;
      }

      // TAV
      for (size_t i(0), p(k*CTPdataformat::DAQwordsPerBunch + CTPdataformat::TAVpos);
	   (i < CTPdataformat::TAVwords) && (p < data.size()); 
	   ++i, ++p) {
	if (i == 0 || longFormat) s << "\nTAV";
	if (longFormat) s << std::setw(1) << i;
	s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
	if (longFormat) s << std::endl;
      }
      
    }

    return s.str();
  }
  
} //namespace LVL1CTP
