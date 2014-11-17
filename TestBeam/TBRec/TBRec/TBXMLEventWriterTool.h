/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBXMLEVENTWRITERTOOL_H
#define TBREC_TBXMLEVENTWRITERTOOL_H
///////////////////////////////////////////////////////////////////////////////
/// \brief writes out event header
///////////////////////////////////////////////////////////////////////////////

#include "TBRec/TBXMLWriterToolBase.h"

#include <iostream>
#include <string>
#include <map>

class TBXMLWriter;

class TBXMLEventWriterTool : public TBXMLWriterToolBase
{

 public:

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBXMLEventWriterTool(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);

  ~TBXMLEventWriterTool();

  ////////////
  // Action //
  ////////////

  // virtual StatusCode initialize();

 protected:

  virtual StatusCode writeRunFiles(const std::string& /* fileDir */,
				   unsigned int /*runNumber*/ );

  virtual StatusCode writeEvent( std::ostream& outFile,
				 const std::string& entryTag);

 private:

  const TBXMLWriter* m_mother;
};
#endif
