/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBEventInfo.h"

#include "TBRec/TBPatternUnitStreamerTool.h"

#include <string>

//#include <iomanip>
#include <ios>
//#include <iomanip>

TBPatternUnitStreamerTool::TBPatternUnitStreamerTool(const std::string& name,
						     const std::string& type,
						     const IInterface* parent)
  : TBEventStreamerTool(name,type,parent)
    , m_patternUnitKey("TBTrigPat")
    , m_acceptPattern()
    , m_rejectPattern()
{
  declareInterface<TBEventStreamerTool>(this);
  declareProperty("PatternUnitName",m_patternUnitKey);
  declareProperty("AcceptPattern",  m_acceptPatternNames);
  declareProperty("RejectPattern",  m_rejectPatternNames);
  // pre-settings (to be moved to helper!)
  m_triggerBitMap["S1"]          = 0x00000001;  // S1 accept
  m_triggerBitMap["S2"]          = 0x00000002;  // S2 accept
  m_triggerBitMap["S3"]          = 0x00000004;  // S3 accept
  m_triggerBitMap["B2"]          = 0x00000008;  // B2 accept
  m_triggerBitMap["VetoLeft"]    = 0x00000010;  // VetoLeft 
  m_triggerBitMap["VetoRight"]   = 0x00000020;  // VetoRight
  m_triggerBitMap["VetoDown"]    = 0x00000040;  // VetoUp
  m_triggerBitMap["VetoUp"]      = 0x00000080;  // VetoDown
  m_triggerBitMap["Mu1Left"]     = 0x00000100;  // Mu 1 Left
  m_triggerBitMap["Mu1Right"]    = 0x00000200;  // Mu 1 Right
  m_triggerBitMap["Mu2Left"]     = 0x00000400;  // Mu 2 Left
  m_triggerBitMap["EarlyPU"]     = 0x00000800;  // Early PileUp
  m_triggerBitMap["LatePU"]      = 0x00001000;  // Late PileUp
  m_triggerBitMap["InSpill"]     = 0x00002000;  // In Spill Tag
  m_triggerBitMap["OffSpill"]    = 0x00004000;  // Off Spill Tag
  m_triggerBitMap["Cedar6of8"]   = 0x00008000;  // Cedar 7 of 8 segments
  m_triggerBitMap["Mu2Right"]    = 0x00010000;  // Mu 2 Right
  m_triggerBitMap["HoleVeto"]    = 0x00020000;  // Hole Counter
  m_triggerBitMap["W1*W2"]       = 0x00040000;  // W1*W2 coincidence
  m_triggerBitMap["B1"]          = 0x00080000;  // B2 accept
  m_triggerBitMap["ScalerReset"] = 0x00100000;  // Scaler Reset Tag
  m_triggerBitMap["Cedar7of8"]   = 0x08000000;  // Cedar 7 of 8 segments
  m_triggerBitMap["Cedar8of8"]   = 0x10000000;  // Cedar 8 of 8 segments
}

TBPatternUnitStreamerTool::~TBPatternUnitStreamerTool()
{ }

////////////////
// Initialize //
////////////////

StatusCode TBPatternUnitStreamerTool::initializeTool()
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
  report << MSG::INFO << "         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;
  report << MSG::INFO << "         | | | | | | | | | | | | | | | | | | | | |S| |" << endreq;
  report << MSG::INFO << "         | | | | | | | | | | | | | | | | | | | | |c| |" << endreq;
  report << MSG::INFO << "         | | | | | |V| | | | | | | | | |C| | | | |a|C|" << endreq;
  report << MSG::INFO << "         | | | | |V|e| |V| |M| | | | |O|e|M|H| | |l|e|" << endreq;
  report << MSG::INFO << "         | | | | |e|t| |e|M|u|M|E| |I|f|d|u|o| | |e|d|" << endreq;
  report << MSG::INFO << "         | | | | |t|o|V|t|u|1|u|a|L|n|f|a|2|l| | |r|a|" << endreq;
  report << MSG::INFO << "         | | | | |o|R|e|o|1|R|2|r|a|S|S|r|R|e|W| |R|r|" << endreq;
  report << MSG::INFO << "         | | | | |L|i|t|D|L|i|L|l|t|p|p|7|i|V|1| |e|8|" << endreq;
  report << MSG::INFO << "         | | | | |e|g|o|o|e|g|e|y|e|i|i|o|g|e|*| |s|o|" << endreq;
  report << MSG::INFO << "         |S|S|S|B|f|h|U|w|f|h|f|P|P|l|l|f|h|t|W|B|e|f|" << endreq;
  report << MSG::INFO << "         |1|2|3|2|t|t|p|n|t|t|t|U|U|l|l|8|t|o|2|1|t|8|" << endreq;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;
  report << MSG::INFO << "| accept |";
  std::string decoded = this->decodeBinary(m_acceptPattern);
  for ( unsigned int i=0; i<std::min(decoded.length(),m_triggerBitMap.size());
	i++ )
    {
      report << MSG::INFO << decoded.substr(i,1) << "|";
    }
//   report << MSG::INFO
// 	 << " (0x" << std::setw(8) << std::setfill('0') 
// 	 << std::setiosflags(std::ios_base::hex) << m_acceptPattern << ")" 
// 	 << std::setiosflags(std::ios_base::dec)
// 	 << endreq;      
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;
  report << MSG::INFO << "| reject |";
  decoded = this->decodeBinary(m_rejectPattern);
  for ( unsigned int i=0; i<std::min(decoded.length(),m_triggerBitMap.size());
	i++ )
    {
      report << MSG::INFO << decoded.substr(i,1) << "|";
    }
//   report << MSG::INFO
// 	 << " (0x" << std::setw(8) << std::setfill('0')
// 	 << std::setiosflags(std::ios_base::hex) << m_rejectPattern << ")" 
// 	 << std::setiosflags(std::ios_base::dec)
// 	 << endreq;
  report << MSG::INFO << "+--------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endreq;
  report << MSG::INFO << "| ignore |";
  bit_mask ignorePattern = ~( m_rejectPattern ^ m_acceptPattern );
  decoded = this->decodeBinary(ignorePattern);
  for ( unsigned int i=0; i<std::min(decoded.length(),m_triggerBitMap.size()); 
	i++ )
    {
      report << MSG::INFO << decoded.substr(i,1) << "|";
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

StatusCode TBPatternUnitStreamerTool::accept()
{
  // messaging
  MsgStream report(msgSvc(),name());
  
  // Always accept non-physics events
  const TBEventInfo* theEventInfo;
  ATH_CHECK( evtStore()->retrieve(theEventInfo,"TBEventInfo") );

  unsigned short evType = theEventInfo->getEventType();
  report << MSG::DEBUG << "Event Type found " << evType << endreq;
  if (evType != 1) return StatusCode::SUCCESS;

  // retrieve pattern unit
  const TBTriggerPatternUnit* theTrigger;
  ATH_CHECK( evtStore()->retrieve(theTrigger,m_patternUnitKey) );

  //
  bit_mask thePattern = theTrigger->getTriggerWord() & 0x00ffffff;

  // check acceptance (AND requirement)
  bool globalAccept =  m_acceptPattern != 0x00000000
    ? ( thePattern & m_acceptPattern ) == m_acceptPattern
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

std::string TBPatternUnitStreamerTool::decodeBinary(bit_mask& theTriggerWord)
{
  unsigned int nBits = sizeof(theTriggerWord) * 8;
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

StatusCode TBPatternUnitStreamerTool::findPattern(const pattern_store&
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


