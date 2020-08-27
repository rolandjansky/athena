/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "TrigT1Result/EMTauResult.h"

// stl includes
#include <sstream>
#include <iomanip>

// Local include(s):
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

namespace ROIB {

  EMTauResult::EMTauResult( Header&& head, Trailer&& trail, std::vector< EMTauRoI >&& roIVector )
    : m_EMTauResultHeader( std::move(head) ), m_EMTauResultTrailer( std::move(trail) ),
      m_EMTauResultRoIVec( std::move(roIVector) ) {

  }

  EMTauResult::EMTauResult()
    : m_EMTauResultHeader(), m_EMTauResultTrailer(),
      m_EMTauResultRoIVec() {

  }

  EMTauResult::~EMTauResult() {

  }

  const Header& EMTauResult::header() const {
    return m_EMTauResultHeader;
  }

  const Trailer& EMTauResult::trailer() const {
    return m_EMTauResultTrailer;
  }

  const std::vector< EMTauRoI >& EMTauResult::roIVec() const {
    return m_EMTauResultRoIVec;
  }

  const std::string EMTauResult::dump() const
  {
    std::ostringstream s;

    s << " [" << this->header().dump() << "] ";
    s << " [";
      for (std::vector<EMTauRoI>::size_type i(0); i < roIVec().size(); ++i) {
	s << LVL1CTP::convertToHex(roIVec()[i].roIWord());
      }
    s << "] ";
    s << " [" << this->trailer().dump() << "] ";
    
    return s.str();
  }

  const std::string EMTauResult::print(const bool longFormat) const
  {
    std::ostringstream s;

    s << "\n  header [" << this->header().print(longFormat) << "] ";
    if (longFormat) s << std::endl;

    for (std::vector<EMTauRoI>::size_type i(0); i < roIVec().size(); ++i) {

      if (i == 0 || longFormat) s << "data";
      if (longFormat) s << "[" << std::setw(2) << i << "]";

      s << " [";
      if (longFormat) s << LVL1CTP::convertToHex(roIVec()[i].roIWord());
      if (longFormat) s << std::setw(12) << "\n Type: ";
      s << " " << roIVec()[i].roIType();
      if (longFormat) s << std::setw(12) << "\n ID: ";
      s << " " << roIVec()[i].electronicsID();
      if (longFormat) s << std::setw(12) << "\n Threshold: ";
      using namespace LVL1CTP; // for importing operator<<
      s << " " << roIVec()[i].thresholds();
      s << "] ";
    }
    s << "trailer [" << this->trailer().print(longFormat) << "] ";

    return s.str();
  }

} // namespace ROIB
