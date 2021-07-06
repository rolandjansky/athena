/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBPatternUnitStreamerToolH8.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include <string>
#include <ios>


TBPatternUnitStreamerToolH8::TBPatternUnitStreamerToolH8(
						     const std::string& type,
						     const std::string& name,
						     const IInterface* parent)
  : TBEventStreamerTool(type,name,parent)
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
	     << endmsg;
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
	     << endmsg;
      return StatusCode::FAILURE;
    }

  // print pattern matrices
  report << MSG::INFO << " " << endmsg;
  report << MSG::INFO << "                      ###################" << endmsg;
  report << MSG::INFO << "                      # TriggerPatterns #" << endmsg;
  report << MSG::INFO << "                      ###################" << endmsg;
  report << MSG::INFO << " " << endmsg;
  report << MSG::INFO << "         +-+-+-+-+-+-+" << endmsg;
  report << MSG::INFO << "         |   | C |   |" << endmsg;
  report << MSG::INFO << "         |   | a |   |" << endmsg;
  report << MSG::INFO << "         |   | l |   |" << endmsg;
  report << MSG::INFO << "         |   | i |   |" << endmsg;
  report << MSG::INFO << "         | P | b |   |" << endmsg;
  report << MSG::INFO << "         | h | r | R |" << endmsg;
  report << MSG::INFO << "         | y | a | a |" << endmsg;
  report << MSG::INFO << "         | s | t | n |" << endmsg;
  report << MSG::INFO << "         | i | i | d |" << endmsg;
  report << MSG::INFO << "         | c | o | o |" << endmsg;
  report << MSG::INFO << "         | s | n | m |" << endmsg;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endmsg;
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
// 	 << endmsg;      
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endmsg;
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
// 	 << endmsg;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endmsg;
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
// 	 << endmsg;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endmsg;

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
  for (const pattern& pat : listOfPatterns)
    {
      bit_mask_store::const_iterator findIter = 
	definedPattern.find(pat);
      if ( findIter != definedPattern.end() )
	{
	  theMask = theMask | (*findIter).second;
	}
      else
	{
	  report << MSG::WARNING
		 << "requested pattern <"
		 << pat
		 << "> unknown, ignore..."
		 << endmsg;
	}
    }
  return StatusCode::SUCCESS;
}


