/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Sector.cxx 796872 2017-02-13 15:03:25Z fwinkl $

// STL include(s):
#include <sstream>

// Local include(s):
#include "Sector.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Common/SectorConstants.h"

namespace LVL1MUCTPI {

   Sector::Sector( const Hemisphere theHemisphere, const unsigned int theSectorNumber,
                   EventReader* reader )
      : m_bitField(0), m_registered( false ), m_rapidityRegion( theHemisphere ),
        m_sectorNumber( theSectorNumber ), m_reader( reader ),
        m_cand1Supressed( false ), m_cand2Supressed( false ) {

   }

   Sector::~Sector() {
   }

   std::string Sector::getRapidityString() const {

      std::string result;
      if( m_rapidityRegion == POSITIVE ) result = "+";
      if( m_rapidityRegion == NEGATIVE ) result = "-";
      return result;

   }
  std::string Sector::getIDString() const {
    std::string result = "";
    //offset for differnt counting scheme in MuCTPI geometry file for L1Topo
    unsigned int offset =0;

    if (this->getDetectorString() == "Barrel") {
      result = "B";
      if (this->getRapidityString() == "+") {
	offset = 32;
      }
    } else if (this->getDetectorString() == "Endcap") {
      if (this->getRapidityString() == "+") {
	result = "EA";
      }else if (this->getRapidityString() == "-") {
	result = "EC";
      }
    } else if (this->getDetectorString() == "Forward") {
      if (this->getRapidityString() == "+") {
	result = "FA";
      }else if (this->getRapidityString() == "-") {
	result = "FC";
      }
    }
    std::string secNumString = "";
    if (m_sectorNumber+offset < 10) {
      secNumString = "0" + std::to_string(m_sectorNumber + offset);
    } else {
      secNumString = std::to_string(m_sectorNumber + offset);
    }

    result = result + secNumString ;

    return result;
  }

   void Sector::printID( std::string& IDString ) const {

      std::ostringstream outStream ;
      outStream << "Sector : " << m_sectorNumber
                << "  Detector : " << this->getDetectorString()
                << "  Rapidity region : " << this->getRapidityString();
      IDString = outStream.str();

      return;
   }

} // namespace LVL1MUCTPI
