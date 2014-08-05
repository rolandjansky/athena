/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBXMLWRITERTOOLBASE_H
#define TBREC_TBXMLWRITERTOOLBASE_H
///////////////////////////////////////////////////////////////////////////////
/// \brief base class for XML writer tools for 2004 event display
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/AlgTool.h"

#include "StoreGate/StoreGateSvc.h"

#include <iostream>

#include <vector>
#include <string>

class TBXMLWriterToolBase : public AlgTool
{
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

 protected:

  TBXMLWriterToolBase(const std::string& type, const std::string& name,
		      const IInterface* parent) 
    : AlgTool( type, name, parent ),
      m_storeGate(0)
    { }

 public:

  virtual ~TBXMLWriterToolBase()
    { } 

  // initialize
  virtual StatusCode initialize()
    {
      StatusCode checkOut = service("StoreGateSvc",m_storeGate);
      if ( checkOut.isFailure() )
	{
	  MsgStream log(msgSvc(),name());
	  log << MSG::ERROR
	      << "cannot allocate StoreGate service."
	      << endreq;
	  return StatusCode::FAILURE;
	}
      return checkOut;
    }

  /////////////
  // Actions //
  /////////////

  //  template<typename DATA>
  virtual StatusCode writeOut( std::ostream& outStream, 
			       const std::string& entryTag = " " )
    {
      return this->writeEvent(outStream,entryTag);
    }

  virtual StatusCode finalize() 
    { 
      return StatusCode::SUCCESS;
    }

  virtual StatusCode finalize(std::ostream& outStream) 
   {
     this->closeAllElements(outStream);
     return this->finalize();
   }

 protected:

  ////////////
  // Stores //
  ////////////

  StoreGateSvc* m_storeGate;

  //  const TBXMLWriter* m_mother;
 
  ///////////////////////////
  // Client Implementation //
  ///////////////////////////

  virtual StatusCode writeRunFiles(const std::string& fileDir,
				   unsigned int runNumber) = 0;
  virtual StatusCode writeEvent(std::ostream& theFile,
				const std::string& entryTag) = 0;

  /////////////
  // Helpers //
  /////////////

  // dictionary management 
  virtual void openDictionary(std::ostream& outStream,
			      const std::string& dictName,
			      const std::vector<std::string>& listOfElements);
  virtual void openDictionary(std::ostream& outStream,
			      const std::string& dictName);
  virtual void closeDictionary(std::ostream& outStream);

  virtual void addDictElement(std::ostream& outStream,
			      const std::string& theElement,
			      const std::vector<std::string>& listOfElements);
  virtual void addAttributes(std::ostream& outStream,
			     const std::string& theElement,
			     const std::vector<std::string>& listOfAttributes);

  // element handling
  virtual void openElement(std::ostream& outStream,
			   const std::string& theElement,
			   const std::vector<std::string>& listOfAttr,
			   const std::vector<std::string>& listOfValues);
  virtual void closeElement(std::ostream& outStream,
			    const std::string& theElement);
  virtual void closeElement(std::ostream& outStream);
  virtual void closeAllElements(std::ostream& outStream);
  
  ///////////////////
  // Internal Data //
  ///////////////////

  std::vector<std::string> m_elementList;
  std::string              m_dictionary;

  static std::string       m_doubleQuote;
};
#endif
