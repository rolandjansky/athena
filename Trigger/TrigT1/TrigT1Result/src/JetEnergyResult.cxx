/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "TrigT1Result/JetEnergyResult.h"

// Local include(s):
#include "TrigT1Interfaces/TrigT1CTPDefs.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// STL includes
#include <sstream>
#include <iomanip>

namespace ROIB {

  JetEnergyResult::JetEnergyResult( Header&& head, Trailer&& trail, std::vector< JetEnergyRoI >&& roIVector )
    : m_JetEnergyResultHeader( std::move(head) ), m_JetEnergyResultTrailer( std::move(trail) ),
      m_JetEnergyResultRoIVec( std::move(roIVector) ) {

  }

  JetEnergyResult::JetEnergyResult()
    : m_JetEnergyResultHeader(), m_JetEnergyResultTrailer(),
      m_JetEnergyResultRoIVec() {

  }

  const Header& JetEnergyResult::header() const {
    return m_JetEnergyResultHeader;
  }

  const Trailer& JetEnergyResult::trailer() const {
    return m_JetEnergyResultTrailer;
  }

  const std::vector< JetEnergyRoI >& JetEnergyResult::roIVec() const {
    return m_JetEnergyResultRoIVec;
  }

  const std::string JetEnergyResult::dump() const
  {
    std::ostringstream s;

    s << " [" << this->header().dump() << "] ";
    s << " [";
      for (std::vector<JetEnergyRoI>::size_type i(0); i < roIVec().size(); ++i) {
	s << LVL1CTP::convertToHex(roIVec()[i].roIWord());
      }
    s << "] ";
    s << " [" << this->trailer().dump() << "] ";
    
    return s.str();
  }

  const std::string JetEnergyResult::print(const bool longFormat) const
  {
    std::ostringstream s;

    s << "\n  header [" << this->header().print(longFormat) << "] ";
    if (longFormat) s << std::endl;

    for (std::vector<JetEnergyRoI>::size_type i(0); i < roIVec().size(); ++i) {

      unsigned int type = roIVec()[i].roIType();

      if (i == 0 || longFormat) s << "data";
      if (longFormat) s << "[" << std::setw(2) << i << "]";

      s << " [";
      if (longFormat) s << LVL1CTP::convertToHex(roIVec()[i].roIWord());
      if (longFormat) s << std::setw(12) << "\n Type: ";
      s << " " << type;
      if (longFormat) s << std::setw(12) << "\n ID: ";
      s << " " << roIVec()[i].electronicsID();
      if (type == LVL1::TrigT1CaloDefs::JetRoIWordType) {
	if (longFormat) s << std::setw(12) << "\n Threshold: ";
	using namespace LVL1CTP; // for importing operator<<
	s << " " << roIVec()[i].thresholds();
      } else if (type == LVL1::TrigT1CaloDefs::JetEtRoIWordType) {
	if (longFormat) s << std::setw(12) << "\n jetEt: ";
	s << " " << roIVec()[i].jetEt();
      } else if (type == LVL1::TrigT1CaloDefs::EnergyRoIWordType0) {
	if (longFormat) s << std::setw(12) << "\n energyX: ";
	s << " " << roIVec()[i].energyX();
      } else if (type == LVL1::TrigT1CaloDefs::EnergyRoIWordType1) {
	if (longFormat) s << std::setw(12) << "\n energyY: ";
	s << " " << roIVec()[i].energyY();
	if (longFormat) s << std::setw(12) << "\n etSumThresholds: ";
	using namespace LVL1CTP; // for importing operator<<
	s << " " << roIVec()[i].etSumThresholds();
      } else if (type == LVL1::TrigT1CaloDefs::EnergyRoIWordType2) {
	if (longFormat) s << std::setw(12) << "\n energySum: ";
	s << " " << roIVec()[i].energySum();
	if (longFormat) s << std::setw(12) << "\n etMissThresholds: ";
	using namespace LVL1CTP; // for importing operator<<
	s << " " << roIVec()[i].etMissThresholds();
      }
      s << "] ";
    }
    s << "trailer [" << this->trailer().print(longFormat) << "] ";

    return s.str();
  }

} // namespace ROIB
