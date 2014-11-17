/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBEvent/TBTriggerPatternUnit.h"

#include "TBRec/TBPatternUnitStreamerToolH8.h"

#include <string>

//#include <iomanip>
#include <ios>
//#include <iomanip>

TBPatternUnitStreamerToolH8::TBPatternUnitStreamerToolH8(
						     const std::string& name,
						     const std::string& type,
						     const IInterface* parent)
  : TBEventStreamerTool(name,type,parent)
    , m_patternUnitKey("TBTrigPat")
    , m_acceptPattern()
    , m_rejectPattern()
{
  declareProperty("PatternUnitName",m_patternUnitKey);
  declareProperty("AcceptPattern",  m_acceptPatternNames);
  declareProperty("RejectPattern",  m_rejectPatternNames);
  // pre-settings (to be moved to helper!)
  m_triggerBitMap["Physics"]     = 0x00000001;  // Physics accept
  m_triggerBitMap["Calibration"] = 0x00000002;  // Calib accept
  m_triggerBitMap["LED"]         = 0x00000002;  // LED (same as calib) accept
  m_triggerBitMap["Random"]      = 0x00000004;  // Random accept
  m_triggerBitMap["MonoCIS"]     = 0x00000008;  // MonoCIS accept
}

TBPatternUnitStreamerToolH8::~TBPatternUnitStreamerToolH8()
{ }

////////////////
// Initialize //
////////////////

StatusCode TBPatternUnitStreamerToolH8::initializeTool()
{ 
  // messages
  MsgStream report(msgSvc(),name());

  // form accept trigger pattern
  m_acceptPattern = 0x00000000;
  if ( (this->findPattern(m_acceptPatternNames,
			  m_triggerBitMap,
			  m_acceptPattern)).isFailure())
    {
      report << MSG::ERROR
	     << "cannot build accept pattern mask."
	     << endreq;
      return StatusCode::FAILURE;
    }

  // form reject trigger pattern
  m_rejectPattern = 0x00000000;
  if ( (this->findPattern(m_rejectPatternNames,
			  m_triggerBitMap,
			  m_rejectPattern)).isFailure() )
    {
      report << MSG::ERROR
	     << "cannot build reject pattern mask."
	     << endreq;
      return StatusCode::FAILURE;
    }

  // print pattern matrices
  report << MSG::INFO << " " << endreq;
  report << MSG::INFO << "                      ###################" << endreq;
  report << MSG::INFO << "                      # TriggerPatterns #" << endreq;
  report << MSG::INFO << "                      ###################" << endreq;
  report << MSG::INFO << " " << endreq;
  report << MSG::INFO << "         +-+-+-+-+-+-+" << endreq;
  report << MSG::INFO << "         |   | C |   |" << endreq;
  report << MSG::INFO << "         |   | a |   |" << endreq;
  report << MSG::INFO << "         |   | l |   |" << endreq;
  report << MSG::INFO << "         |   | i |   |" << endreq;
  report << MSG::INFO << "         | P | b |   |" << endreq;
  report << MSG::INFO << "         | h | r | R |" << endreq;
  report << MSG::INFO << "         | y | a | a |" << endreq;
  report << MSG::INFO << "         | s | t | n |" << endreq;
  report << MSG::INFO << "         | i | i | d |" << endreq;
  report << MSG::INFO << "         | c | o | o |" << endreq;
  report << MSG::INFO << "         | s | n | m |" << endreq;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;
  std::string decoded = this->decodeBinary(m_acceptPattern);
  report << MSG::INFO << "| accept | " ;
  for ( unsigned int i=0; i<std::min(decoded.length(),m_triggerBitMap.size());
	i++ )
    {
      report << MSG::INFO << decoded.substr(i,1) << " | ";
    }
//   report << MSG::INFO
// 	 << " (0x" << std::setw(8) << std::setfill('0') 
// 	 << std::setiosflags(std::ios_base::hex) << m_acceptPattern << ")" 
// 	 << std::setiosflags(std::ios_base::dec)
// 	 << endreq;      
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;
  report << MSG::INFO << "| reject | ";
  decoded = this->decodeBinary(m_rejectPattern);
  for ( unsigned int i=0; i<std::min(decoded.length(),m_triggerBitMap.size());
	i++ )
    {
      report << MSG::INFO << decoded.substr(i,1) << " | ";
    }
//   report << MSG::INFO
// 	 << " (0x" << std::setw(8) << std::setfill('0')
// 	 << std::setiosflags(std::ios_base::hex) << m_rejectPattern << ")" 
// 	 << std::setiosflags(std::ios_base::dec)
// 	 << endreq;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;
  report << MSG::INFO << "| ignore | ";
  bit_mask ignorePattern = ~( m_rejectPattern ^ m_acceptPattern );
  decoded = this->decodeBinary(ignorePattern);
  for ( unsigned int i=0; i<std::min(decoded.length(),m_triggerBitMap.size()); 
	i++ )
    {
      report << MSG::INFO << decoded.substr(i,1) << " | ";
    }
//   report << MSG::INFO
// 	 << " (0x" << std::setw(8) << std::setfill('0')
// 	 << std::setiosflags(std::ios_base::hex) << ignorePattern << ")" 
// 	 << std::setiosflags(std::ios_base::dec)
// 	 << endreq;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;

  return StatusCode::SUCCESS;
}

///////////////////
// Accept/Reject //
///////////////////

StatusCode TBPatternUnitStreamerToolH8::accept()
{
  // messaging
  MsgStream report(msgSvc(),name());
  
  // retrieve pattern unit
  const TBTriggerPatternUnit* theTrigger;
  ATH_CHECK( evtStore()->retrieve(theTrigger,m_patternUnitKey) );

  //
  bit_mask thePattern = theTrigger->getTriggerWord() & 0x00ffffff;

  // check acceptance (AND requirement)
  bool globalAccept =  m_acceptPattern != 0x00000000
    /// ? ( thePattern & m_acceptPattern ) == m_acceptPattern
    ? ( thePattern & m_acceptPattern ) != 0x00000000
    : true;

  // check rejection (OR requirement)
  globalAccept = m_rejectPattern != 0x0000  
    ? globalAccept & ( ( thePattern & m_rejectPattern ) == 0x0000 )
    : globalAccept;

  return globalAccept
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

////////////
// Helper //
////////////

std::string TBPatternUnitStreamerToolH8::decodeBinary(bit_mask& theTriggerWord)
{
  unsigned int nBits = sizeof(theTriggerWord) ;
  std::string thePattern;
  bit_mask testPattern = 0x00000001;
  
  for ( unsigned int i=0; i<nBits; i++ )
    {
      if ( ( theTriggerWord & testPattern ) == testPattern ) 
	{
	  thePattern = thePattern + "X";
	}
      else
	{ 
	  thePattern = thePattern + "O";
	}
      testPattern *= 2;
    }
  return thePattern;
}

StatusCode TBPatternUnitStreamerToolH8::findPattern(const pattern_store&
						  listOfPatterns,
						  const bit_mask_store&
						  definedPattern,
						  bit_mask& theMask)
{
  // messaging
  MsgStream report(msgSvc(),name());

  // build pattern
  pattern_store::const_iterator firstPattern = listOfPatterns.begin();
  pattern_store::const_iterator lastPattern  = listOfPatterns.end();
  for ( ; firstPattern != lastPattern; firstPattern++ )
    {
      bit_mask_store::const_iterator findIter = 
	definedPattern.find(*firstPattern);
      if ( findIter != definedPattern.end() )
	{
	  theMask = theMask | (*findIter).second;
	}
      else
	{
	  report << MSG::WARNING
		 << "requested pattern <"
		 << *firstPattern
		 << "> unknown, ignore..."
		 << endreq;
	}
    }
  return StatusCode::SUCCESS;
}


