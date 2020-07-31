/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/CTPSLink.h"

// STL includes(s):
#include <sstream>
#include <iostream>
#include <iomanip>

// local LVL1 include(s):
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

namespace LVL1CTP {

  // CTPSLink contains only information of triggered bunch!
  //const unsigned int CTPSLink::wordsPerCTPSLink = CTPdataformat::NumberTimeWords+CTPdataformat::DAQwordsPerBunch;

  CTPSLink::CTPSLink( const std::vector<uint32_t>& roiVec, unsigned int ctpVersionNumber )
    : m_CTPSLinkVector( roiVec ), m_ctpVersionNumber(ctpVersionNumber),
      m_wordsPerHeader(0), m_wordsPerDataElement(0), m_wordsPerTrailer(0)
  {
    m_ctpVersion = new CTPdataformatVersion(m_ctpVersionNumber);
    m_wordsPerCTPSLink = m_ctpVersion->getNumberTimeWords() + m_ctpVersion->getDAQwordsPerBunch();
    
    m_wordsPerHeader = m_CTPSLinkVector.empty() ? 0 : m_CTPSLinkVector[1];
    if (m_CTPSLinkVector.size() > (m_wordsPerHeader+3)) {
      m_wordsPerDataElement = m_CTPSLinkVector[m_CTPSLinkVector.size()-2];
      m_wordsPerTrailer = m_CTPSLinkVector[m_CTPSLinkVector.size()-3]+3;
    }

    if (m_wordsPerHeader ==  0 ||
	m_wordsPerTrailer ==  0 ||
	m_wordsPerHeader+m_wordsPerDataElement+m_wordsPerTrailer != m_CTPSLinkVector.size()) {
      std::cout << "Warning: CTPSLink inconsistency! " << std::endl;
    }
  }

  CTPSLink::~CTPSLink() {
    if(m_ctpVersion) delete m_ctpVersion;
    m_ctpVersion = NULL;
    
  }

  unsigned int CTPSLink::getSize() const 
  {
    return m_CTPSLinkVector.size();
  }

  unsigned int CTPSLink::getHeaderSize() const 
  {
    return m_wordsPerHeader;
  }

  unsigned int CTPSLink::getDataElementSize() const 
  {
    return m_wordsPerDataElement;
  }

  unsigned int CTPSLink::getTrailerSize() const 
  {
    return m_wordsPerTrailer;
  }

  const std::vector< uint32_t > CTPSLink::getHeader() const 
  {
    std::vector<uint32_t> result;

    unsigned int start(0);
    unsigned int end(getHeaderSize());

    for (size_t i(start); i < end; ++i) {
      result.push_back(m_CTPSLinkVector[i]);
    }

    return result;
  }

  const std::vector< uint32_t > CTPSLink::getTrailer() const 
  {
    std::vector<uint32_t> result;

    unsigned int start(getSize() - getTrailerSize());
    unsigned int end(getSize());

    for (size_t i(start); i < end; ++i) {
      result.push_back(m_CTPSLinkVector[i]);
    }

    return result;
  }

  const std::vector< uint32_t > CTPSLink::getCTPToRoIBWords() const {

    return m_CTPSLinkVector;

  }

  const std::vector< uint32_t > CTPSLink::getDataElements() const 
  {
    // strip of header and trailer
    std::vector<uint32_t> dataElements;

    unsigned int start(getHeaderSize());
    unsigned int end(getSize() - getTrailerSize());

    for (size_t i(start); i < end; ++i) {
      dataElements.push_back(m_CTPSLinkVector[i]);
    }

    return dataElements;
  }

  const std::vector< uint32_t > CTPSLink::getTBP() const 
  {
    std::vector<uint32_t> result;

    unsigned int start(getHeaderSize()+m_ctpVersion->getTBPpos());
    unsigned int end(start+m_ctpVersion->getTBPwords());

    for (size_t i(start); i < end; ++i) {
      result.push_back(m_CTPSLinkVector[i]);
    }

    return result;
  }

  const std::vector< uint32_t > CTPSLink::getTAP() const 
  {
    std::vector<uint32_t> result;

    unsigned int start(getHeaderSize()+m_ctpVersion->getTAPpos());
    unsigned int end(start+m_ctpVersion->getTAPwords());

    for (size_t i(start); i < end; ++i) {
      result.push_back(m_CTPSLinkVector[i]);
    }

    return result;
  }

  const std::vector< uint32_t > CTPSLink::getTAV() const 
  {
    std::vector<uint32_t> result;

    unsigned int start(getHeaderSize()+m_ctpVersion->getTAVpos());
    unsigned int end(start+m_ctpVersion->getTAVwords());

    for (size_t i(start); i < end; ++i) {
      result.push_back(m_CTPSLinkVector[i]);
    }

    return result;
  }

  bool CTPSLink::getAcceptBP() const
  {
    const std::vector<uint32_t> result(this->getTBP());

    for (size_t i(0); i < result.size(); ++i) {
      if (result[i] > 0) return true;
    }
    
    return false;
  }
  
  bool CTPSLink::getAcceptAP() const
  {
    const std::vector<uint32_t> result(this->getTAP());

    for (size_t i(0); i < result.size(); ++i) {
      if (result[i] > 0) return true;
    }
    
    return false;
  }
  
  bool CTPSLink::getAcceptAV() const
  {
    const std::vector<uint32_t> result(this->getTAV());

    for (size_t i(0); i < result.size(); ++i) {
      if (result[i] > 0) return true;
    }
    
    return false;
  }


  const std::string CTPSLink::dump() const
  {
    std::ostringstream s;


    s << "CTPSLink: header  [" << LVL1CTP::convertToHex(this->getHeader()) << "]" << std::endl;
    s << "CTPSLink: data    [" << LVL1CTP::convertToHex(this->getDataElements()) << "]" << std::endl;
    s << "CTPSLink: trailer [" << LVL1CTP::convertToHex(this->getTrailer()) << "]";

    return s.str();
  }

  const std::string CTPSLink::print(const bool longFormat) const
  {
    std::ostringstream s;

    s << "CTPSLink: header  [" << LVL1CTP::convertToHex(this->getHeader()) << "]" << std::endl;
    s << "CTPSLink: data content" << convert(this->getDataElements(), longFormat) << std::endl;
    s << "CTPSLink: trailer: [" << LVL1CTP::convertToHex(this->getTrailer()) << "]";

    return s.str();
  }

  const std::string CTPSLink::convert(std::vector<uint32_t> data, const bool longFormat) const
  {
    std::ostringstream s;

    // time
    for (size_t i(0); (i < m_ctpVersion->getNumberTimeWords()) && (i < data.size()); ++i) {
      if (i == 0 || longFormat) s << "\nTime";
      if (longFormat) s << std::setw(1) << i;
      s << " " << std::setw(8) << data[i];
      if (longFormat) s << std::endl;
    }

    // TIP
    for (size_t i(0), p(m_ctpVersion->getTIPpos());
	 (i < m_ctpVersion->getTIPwords()) && (p < data.size()); 
	 ++i, ++p) {
      if (i == 0 || longFormat) s << "\nTIP";
      if (longFormat) s << std::setw(1) << i;
      s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
      if (longFormat) s << std::endl;
    }
    
    // FPI
    //for (size_t i(0), p(m_ctpVersion->getFPIpos());
//         (i < m_ctpVersion->getFPIwords()) && (p < data.size()); 
//         ++i, ++p) {
//      if (i == 0 || longFormat) s << "\nFPI";
//      if (longFormat) s << std::setw(1) << i;
//      s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
//      if (longFormat) s << std::endl;
//    }

    // TBP
    for (size_t i(0), p(m_ctpVersion->getTBPpos());
	 (i < m_ctpVersion->getTBPwords()) && (p < data.size()); 
	 ++i, ++p) {
      if (i == 0 || longFormat) s << "\nTBP";
      if (longFormat) s << std::setw(1) << i;
      s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
      if (longFormat) s << std::endl;
    }

    // TAP
    for (size_t i(0), p(m_ctpVersion->getTAPpos());
	 (i < m_ctpVersion->getTAPwords()) && (p < data.size()); 
	 ++i, ++p) {
      if (i == 0 || longFormat) s << "\nTAP";
      if (longFormat) s << std::setw(1) << i;
      s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
      if (longFormat) s << std::endl;
    }

    // TAV
    for (size_t i(0), p(m_ctpVersion->getTAVpos());
	 (i < m_ctpVersion->getTAVwords()) && (p < data.size()); 
	 ++i, ++p) {
      if (i == 0 || longFormat) s << "\nTAV";
      if (longFormat) s << std::setw(1) << i;
      s << " 0x" << std::hex << std::setw(8) << std::setfill( '0' ) << data[p] << std::dec << std::setfill(' ');
      if (longFormat) s << std::endl;
    }
    
    return s.str();
  }


} // namesapce LVL1CTP
