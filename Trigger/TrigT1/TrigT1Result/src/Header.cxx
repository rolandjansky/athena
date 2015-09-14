/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "TrigT1Result/Header.h"

// STL includes
#include <sstream>
#include <iomanip>

// Local include(s):
#include "TrigT1Interfaces/TrigT1CTPDefs.h"

// gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "eformat/eformat.h"

namespace ROIB {

  Header::Header( const uint32_t i1, const uint32_t i2, const uint32_t i3 ) {

    m_header.push_back( eformat::ROD );   // start of header marker	     
    m_header.push_back( 8 );              // header size xxx needs update to adjusted
    m_header.push_back( i3 );		  // format version number	     
    m_header.push_back( i1 );		  // source identifier            
    m_header.push_back( i2 );             // extended level 1 ID	
    m_header.push_back( 0 );              // bunch crossing ID 
    m_header.push_back( 0 );		  // level 1 trigger type	
    m_header.push_back( 0 );		  // detector event type       

    // m_header.push_back( 0 );		  // run number xxx needs to be added for eformat v3
  }

  Header::Header( const uint32_t i1, const uint32_t i2 ) {

    m_header.push_back( eformat::ROD );              // start of header marker	     
    m_header.push_back( 8 ); 			     // header size xxx needs update 
    m_header.push_back( eformat::DEFAULT_VERSION );  // format version number	     
    m_header.push_back( i1 );			     // source identifier            
    m_header.push_back( i2 );                        // extended level 1 ID	
    m_header.push_back( 0 );                         // bunch crossing ID
    m_header.push_back( 0 );                         // level 1 trigger type	
    m_header.push_back( 0 );		             // detector event type       

    // m_header.push_back( 0 );		  // run number xxx needs to be added for eformat v3
  }

  //! Constructor specifying subdet ID only 
  Header::Header( const uint32_t i ) {

    m_header.push_back( eformat::ROD );              // start of header marker
    m_header.push_back( 8 );                         // header size xxx needs update
    m_header.push_back( eformat::DEFAULT_VERSION );  // format version number
    m_header.push_back( i );                         // source identifier            
    m_header.push_back( 0 );                         // extended level 1 ID
    m_header.push_back( 0 );                         // bunch crossing ID
    m_header.push_back( 0 );                         // level 1 trigger type
    m_header.push_back( 0 );                         // detector event type       

    // m_header.push_back( 0 );		  // run number xxx needs to be added for eformat v3
  }

  const std::string Header::dump() const
  {
    std::ostringstream s;

    for (std::vector< uint32_t >::size_type i(0); i < m_header.size(); ++i) {
      s << LVL1CTP::convertToHex(m_header[i]);
    }
    
    return s.str();
  }

  const std::string Header::print(const bool longFormat) const
  {
    std::ostringstream s;

    if (longFormat) {
      s << "Size: " << size() << std::endl;
      s << std::setw(2) << 0 << "Marker:" << headerMarker() << std::endl;
      s << std::setw(2) << 1 << "Size:" << headerSize() << std::endl;
      s << std::setw(2) << 2 << "Version:" << formatVersion() << std::endl;
      s << std::setw(2) << 3 << "SourceID:" << sourceID() << std::endl;
      s << std::setw(2) << 4 << "L1ID:" << L1ID() << std::endl;
      s << std::setw(2) << 5 << "BCID:" << BCID() << std::endl;
      s << std::setw(2) << 6 << "TriggerType:" << triggerType() << std::endl;
      s << std::setw(2) << 7 << "EventType:" << eventType() << std::endl;
    } else {
      for (size_t i(0); i < m_header.size(); ++i) {
	s << LVL1CTP::convertToHex(m_header[i]);
      }
    }
    
    return s.str();
  }

  void Header::dumpData() const
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
  
  void Header::dumpData(MsgStream& log) const
  {
    log << MSG::DEBUG << "*BEGIN* ROIB::Header" << endreq;
    log << MSG::DEBUG << "Header marker           :  " << MSG::hex << headerMarker() << MSG::dec << endreq;
    log << MSG::DEBUG << "Header size             :  " << headerSize() << endreq;
    log << MSG::DEBUG << "Source ID               :  0x" << MSG::hex << sourceID() << MSG::dec << endreq;
    log << MSG::DEBUG << "Ext. LVL1 ID            :  " << L1ID() << endreq;
    log << MSG::DEBUG << "BCID                    :  " << BCID() << endreq;
    log << MSG::DEBUG << "Trigger type            :  " << triggerType() << endreq;
    log << MSG::DEBUG << "Det. event type         :  " << eventType() << endreq;
    log << MSG::DEBUG << "*END* ROIB::Header" << endreq;
  }
} // namespace ROIB
