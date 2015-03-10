/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/AlgTool.h"

#include "TBRec/TBXMLWriterToolBase.h"
#include "TBRec/TBXMLEventWriterTool.h"
#include "TBRec/TBXMLWriter.h"

#include <string>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream> 

//#include <map>
#include <vector>

TBXMLEventWriterTool::TBXMLEventWriterTool(const std::string& type, 
					   const std::string& name,
					   const IInterface* parent)
  : TBXMLWriterToolBase(type,name,parent)
{
  // retrieve pointer to mother
  m_mother = dynamic_cast<const TBXMLWriter*>(parent);
 }

TBXMLEventWriterTool::~TBXMLEventWriterTool()
{ }

StatusCode
TBXMLEventWriterTool::writeEvent(std::ostream& outStream,
				 const std::string&  entryTag)
{

  MsgStream log(msgSvc(),name());

  // check 
  if ( m_mother == 0 )
    {
      log << MSG::ERROR
	  << "not hanging off the TBXMLWriter algorithm."
	  << endreq;
      return StatusCode::FAILURE;
    }
  log << MSG::DEBUG << "EntryTag: "<< entryTag <<endreq;
      
  // access some data
  unsigned int theRun     = m_mother->getRunNumber(); 
  unsigned int theEvent   = m_mother->getEventNumber();
  unsigned int theType    = 0;

  //////////////////////////
  // Write XML Dictionary //
  //////////////////////////

  // Document type
  std::vector<std::string> theEventElements;
  theEventElements.push_back("EventHeader");
  theEventElements.push_back("(CaloCellContainer*)?");
  theEventElements.push_back("(BeamWireChambers*)?");
  theEventElements.push_back("(BeamProfileChambers*)?");
  theEventElements.push_back("(BeamScintillators*)?");
  theEventElements.push_back("(TailCatcher*)?");
  theEventElements.push_back("(PatternUnit*)?");
  this->openDictionary(outStream,"CTB_EC2Event",theEventElements);
  std::vector<std::string> theEventAttr;
  theEventAttr.push_back("name   CDATA   #REQUIRED");
  this->addAttributes(outStream,"CTB_EC2Event",theEventAttr);

  // EventHeader
  std::vector<std::string> theHeaderElements;
  //  theHeaderElements.push_back("TriggerWord?");
  //  theHeaderElements.push_back("TriggerPattern?");
  this->addDictElement(outStream,"EventHeader",theHeaderElements);
  std::vector<std::string> theHeaderAttr;
  theHeaderAttr.push_back("name  CDATA   #REQUIRED");
  theHeaderAttr.push_back("run   CDATA   #REQUIRED");
  theHeaderAttr.push_back("event CDATA   #REQUIRED");
  theHeaderAttr.push_back("type  CDATA   #REQUIRED");
  this->addAttributes(outStream,"EventHeader",theHeaderAttr);

  // Trigger etc
  //  std::vector<std::string> anyData;
  //  anyData.push_back("ANY");
  //  this->addDictElement(outStream,"TriggerWord",anyData);
  //  this->addDictElement(outStream,"TriggerPattern",anyData);

  // CaloCellContainer
  std::vector<std::string> anyData;
  anyData.push_back("ANY");
  this->addDictElement(outStream,"CaloCellContainer",anyData);
  std::vector<std::string> theCaloAttr;
  theCaloAttr.push_back("name    CDATA                           #REQUIRED");
  theCaloAttr.push_back("ncols   CDATA                           #REQUIRED");
  theCaloAttr.push_back("nrows   CDATA                           #REQUIRED"); 
  theCaloAttr.push_back("idtype  ( uint | int | short | ushort ) #REQUIRED");
  theCaloAttr.push_back("etype   ( float | double )              #REQUIRED");
  this->addAttributes(outStream,"CaloCellContainer",theCaloAttr);

  // BPC Container
  this->addDictElement(outStream,"BeamProfileChambers",anyData);
  std::vector<std::string> theBPCAttr;
  theBPCAttr.push_back("name     CDATA                           #REQUIRED");
  theBPCAttr.push_back("cols     CDATA                           #REQUIRED");
  theBPCAttr.push_back("idtype   ( uint | int | short | ushort ) #REQUIRED");
  theBPCAttr.push_back("xtype    ( float | double )              #REQUIRED");
  theBPCAttr.push_back("ytype    ( float | double )              #REQUIRED");

  // MWPC Container
  this->addDictElement(outStream,"BeamWireChambers",anyData);
  std::vector<std::string> theMWPCAttr;
  theMWPCAttr.push_back("name     CDATA                           #REQUIRED");
  theMWPCAttr.push_back("cols     CDATA                           #REQUIRED");
  theMWPCAttr.push_back("idtype   ( uint | int | short | ushort ) #REQUIRED");
  theMWPCAttr.push_back("xtype    ( float | double )              #REQUIRED");
  theMWPCAttr.push_back("ytype    ( float | double )              #REQUIRED");

  // Scintillator Container
  this->addDictElement(outStream,"BeamScinitllators",anyData);
  std::vector<std::string> theScintAttr;
  theScintAttr.push_back("name     CDATA                           #REQUIRED");
  theScintAttr.push_back("cols     CDATA                           #REQUIRED");
  theScintAttr.push_back("idtype   ( uint | int | short | ushort ) #REQUIRED");
  theScintAttr.push_back("signal   ( float | double )              #REQUIRED");

  // close dictionary
  this->closeDictionary(outStream);

  /////////////////
  // Write Event //
  /////////////////

  std::vector<std::string> theEventValues(theEventAttr.size());
  for ( unsigned int i=0; i<theEventAttr.size(); i++ )
    {
      if ( (theEventAttr[i]).find("name") != std::string::npos )
	{
	  theEventValues[i] = "EventInfo";
	}
      theEventAttr[i] = 
	theEventAttr[i].substr(0,theEventAttr[i].find_first_of(" "));
    }
  this->openElement(outStream,"EventInfo",theEventAttr,theEventValues);

  ///////////////////////
  // Write EventHeader //
  ///////////////////////

  std::vector<std::string> theHeaderValues(theHeaderAttr.size());
  for ( unsigned int i=0; i<theHeaderAttr.size(); i++ )
    {
      if ( (theHeaderAttr[i]).find("name") != std::string::npos )
  	{
	  theHeaderValues[i] = "EventInfo";
  	}
      else if ( (theHeaderAttr[i]).find("run") != std::string::npos )
  	{
  	  std::ostringstream theValue; theValue << theRun << std::ends;
	  theHeaderValues[i] = theValue.str();
	}
      else if ( (theHeaderAttr[i]).find("event") != std::string::npos )
	{
	  std::ostringstream theValue; theValue << theEvent << std::ends;
	  theHeaderValues[i] = theValue.str();
	}
      else if ( (theHeaderAttr[i]).find("type") != std::string::npos )
	{
	  std::ostringstream theValue; theValue << theType << std::ends;
	  theHeaderValues[i] = theValue.str();
	}
      else
	{
	  theHeaderValues[i] = "unknown";
	}
      theHeaderAttr[i]   = 
	theHeaderAttr[i].substr(0,theHeaderAttr[i].find_first_of(" "));
    }
  // -------------------------- EventHeader
  this->openElement(outStream,"EventHeader",
		    theHeaderAttr,theHeaderValues);
  // ----------------------------- TriggerWord
  //  std::vector<std::string> emptyStr;
  //  this->openElement(outStream,"TriggerWord",
  //		    emptyStr,emptyStr);
  //  unsigned short theTriggerWord = 
  //    (unsigned short) (m_data->getTriggerWord() & 0xffff);
  //  outStream << "    0x"<< setw(4) << setfill(48) << hex << theTriggerWord
  //	    << dec << std::endl;
  //  this->closeElement(outStream);
  // ----------------------------- TriggerPattern
  //  this->openElement(outStream,"TriggerPattern",emptyStr,emptyStr);
  //  outStream << "    ";
  //  std::map<std::string,unsigned int>::const_iterator 
  //    first = m_triggerBitMap.begin();
  //  std::map<std::string,unsigned int>::const_iterator 
  //    last  = m_triggerBitMap.end();
  //  for ( ; first != last; first++ )
  //   {
  //      if ( ( theTriggerWord & (*first).second ) == (*first).second )
  //	{
  //	  outStream << "\042" << (*first).first << "\042 ";
  //	}
  //   }
  outStream << std::endl;
  this->closeElement(outStream);

  //  this->closeElement(outStream,"EventHeader");
  return StatusCode::SUCCESS;
}

StatusCode
TBXMLEventWriterTool::writeRunFiles(const std::string& /* fileDir */,
				    unsigned int /* runNumber */ )
{
  return StatusCode::SUCCESS;
}
