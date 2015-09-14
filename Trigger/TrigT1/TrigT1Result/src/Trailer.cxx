/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "TrigT1Result/Trailer.h"

// gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// STL includes
#include <sstream>
#include <iomanip>

// Local include(s):
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

namespace ROIB {

  Trailer::Trailer( const uint32_t i1, const uint32_t i2 ) {

    m_trailer.push_back( i2 );     // status element: error
    m_trailer.push_back( 0 );      // status element: info 
    m_trailer.push_back( 2 );      // number of status words
    m_trailer.push_back( i1 );     // number of data words 
    m_trailer.push_back( 1 );      // status block position

  }

  Trailer::Trailer( const uint32_t i ) {

    m_trailer.push_back( 0 );      // status element: error
    m_trailer.push_back( 0 );      // status element: info 
    m_trailer.push_back( 2 );      // number of status words
    m_trailer.push_back( i );      // number of data words 
    m_trailer.push_back( 1 );      // status block position

  }


  const std::string Trailer::dump() const
  {
    std::ostringstream s;

    for (size_t i(0); i < m_trailer.size(); ++i) {
      s << LVL1CTP::convertToHex(m_trailer[i]);
    }
    
    return s.str();
  }

  const std::string Trailer::print(const bool longFormat) const
  {
    std::ostringstream s;

    if (longFormat) {
      s << "Size: " << size() << std::endl;
      s << std::setw(2) << 0 << "ErrorStatus:" << errorStatus() << std::endl;
      s << std::setw(2) << 1 << "SatusInfo:" << statusInfo() << std::endl;
      s << std::setw(2) << 2 << "nStatusWords:" << numStatusWords() << std::endl;
      s << std::setw(2) << 3 << "nDataWords:" << numDataWords() << std::endl;
      s << std::setw(2) << 4 << "StatusPosition:" << statusPosition();
    } else {
      for (size_t i(0); i < m_trailer.size(); ++i) {
	s << LVL1CTP::convertToHex(m_trailer[i]);
      }
    }
    
    return s.str();
  }

  void Trailer::dumpData() const
  {
    IMessageSvc*  msgSvc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
    if ( sc.isFailure() ) {
      return;
    }
    MsgStream log(msgSvc, "ROIB::Header");
    dumpData(log);
  }
  
  void Trailer::dumpData(MsgStream& log) const
  {
    log << MSG::DEBUG << "*BEGIN* ROIB::Trailer" << endreq;
    log << MSG::DEBUG << "Trailer size            :  " << size() << endreq;
    log << MSG::DEBUG << "Error status            :  " << errorStatus() << endreq;
    log << MSG::DEBUG << "Status info             :  " << statusInfo() << endreq;
    log << MSG::DEBUG << "No data words           :  " << numDataWords() << endreq;    
    log << MSG::DEBUG << "*END* ROIB::Trailer" << endreq;
  }
} // namespace ROIB
