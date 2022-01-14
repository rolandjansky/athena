/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeometryDBSvc.h"
#include "TextFileDBReader.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include <string>
#include <sstream>
#include <stdexcept>

GeometryDBSvc::GeometryDBSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_textParameters(0)
{
  declareProperty("TextFileName",m_textFileName, "Text file name for overriding database.");
  declareProperty("PrintParameters",m_printParameters = true, "Print parameters read in from text file.");
  declareProperty("PrintNotUsed", m_printNotUsed = true, "Print parameters not used.");
  declareProperty("Sections", m_sections, "Sections considered for logging. Empty list means all.");
}

GeometryDBSvc::~GeometryDBSvc()
{
  delete m_textParameters;
}

StatusCode 
GeometryDBSvc::initialize()
{ 
  msg(MSG::INFO) << "GeometryDBSvc Initialized" << endmsg;
  setParameterFileName(m_textFileName);

  // Some printout
  if (m_textParameters && m_printParameters && msgLvl(MSG::INFO)) {
    if (m_sections.empty()) {
      // Print all parameters if no sections are requested
      msg(MSG::INFO) << "Parameters from text file:" << endmsg;
      m_textParameters->printParameters();
    } else {
      // Otherwise print only those parameters that belong to sections requested.
      bool printedUnnamed = false;
      for (std::vector<std::string>::const_iterator iter = m_sections.begin(); iter != m_sections.end(); ++iter) {
	msg(MSG::INFO) << "Parameters from text file from section: " << *iter << endmsg;
	if (m_textParameters->sectionPresent(*iter)) {
	  m_textParameters->printParameters(*iter);
	} else {
	  // If section not present print those in unnamed section (passing a section name that is not present will do this) 
	  // If we already printed those in the unnamed section don't do so again.
	  if (printedUnnamed) {
	    msg(MSG::INFO) << " Section not present. Parameters from text file from unnamed section already printed above." << endmsg;
	  } else {	    
	    msg(MSG::INFO) << " Section not present. Parameters from text file from unnamed section will be printed." << endmsg;
	    m_textParameters->printParameters(*iter);
	    printedUnnamed = true;
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;  
}


StatusCode 
GeometryDBSvc::finalize()
{
  
  if (m_textParameters && m_printNotUsed && msgLvl(MSG::INFO)) {
    if (m_sections.empty()) {
      // Consider all parameters if no sections are requested.
      msg(MSG::INFO) << "The following parameters were not used:" << endmsg;
      m_textParameters->printNotUsed();
    } else {
      // Otherwise consider only those parameters that belong to sections requested.
      bool printedUnnamed = false;
      for (std::vector<std::string>::const_iterator iter = m_sections.begin(); iter != m_sections.end(); ++iter) {
	msg(MSG::INFO) << "The following parameters were not used from section: " << *iter <<endmsg;
	if (m_textParameters->sectionPresent(*iter)) {
	  m_textParameters->printNotUsed(*iter);
	} else {
	  // If section not present consider those in unnamed section (passing a section name that is not present will do this) 
	  // If we already printed those in the unnamed section don't do so again.
	  if (printedUnnamed) {
	    msg(MSG::INFO) << " Section not present. Parameters from text file from unnamed section already printed above." << endmsg;
	  } else {	    
	    msg(MSG::INFO) << " Section not present. Parameters from text file from unnamed section will be printed." << endmsg;
	    m_textParameters->printNotUsed(*iter);
	    printedUnnamed = true;
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode
GeometryDBSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IGeometryDBSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IGeometryDBSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void 
GeometryDBSvc::setParameterFileName(const std::string & filename)
{
  if (!filename.empty()) {
    msg(MSG::INFO) << "Parameters overriden from text file: " << filename << endmsg; 
    msg(MSG::WARNING) << "Overriding from a text file is NOT recommended for production use." << endmsg; 
    if (!m_textParameters)  m_textParameters = new TextFileDBReader;
    bool status = m_textParameters->readFile(filename);
    if (!status) {
      msg(MSG::ERROR) << "Problem reading text file: " << filename << endmsg; 
    }
  }
}



std::string 
GeometryDBSvc::parameterKey(const std::string & recordSetName, const std::string & name, int index) const
{
  std::ostringstream ostr;
  if (!recordSetName.empty()) {
    ostr << recordSetName << "#" << index << ":" << name;
  } else {
    ostr << name;
  }
  return ostr.str();
}

double 
GeometryDBSvc::getDouble(IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  if (!recordSet.get()) return getDouble("",name,index);
  double tmpPar = 0;
  std::string recordSetName = recordSet->nodeName();
  if (getValue(recordSetName,name,index,tmpPar)) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Parameter " << parameterKey(recordSetName,name,index) << " from text file: " << tmpPar << endmsg; 
  } else {
    if (index >= (int)recordSet->size()) {
      msg(MSG::ERROR) << "Parameter " << parameterKey(recordSetName,name,index) << " not found. " << endmsg; 
    } else {
      tmpPar = (*recordSet)[index]->getDouble(name);
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Parameter " << parameterKey(recordSetName,name,index) << " from database: " << tmpPar << endmsg; 
    }
  } 
  return tmpPar;
}

double 
GeometryDBSvc::getDouble(const std::string & recordSetName, const std::string & name, int index) const
{
  double tmpPar = 0;
  if (getValue(recordSetName,name,index,tmpPar)) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Parameter " << parameterKey(recordSetName,name,index) << " from text file: " << tmpPar << endmsg; 
    return tmpPar;
  } else  {
    msg(MSG::ERROR) << "Parameter " << parameterKey(recordSetName,name,index) << " not found." << endmsg; 
    return 0;
  } 
}

int 
GeometryDBSvc::getInt(IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  if (!recordSet.get()) return getInt("",name,index);
  int tmpPar = 0;
  std::string recordSetName = recordSet->nodeName();
  if (getValue(recordSetName,name,index,tmpPar)) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Parameter " << parameterKey(recordSetName,name,index) << " from text file: " << tmpPar << endmsg; 
  } else {
    if (index >= (int)recordSet->size()) {
      msg(MSG::ERROR) << "Parameter " << parameterKey(recordSetName,name,index) << " not found. " << endmsg; 
    } else {
      tmpPar = (*recordSet)[index]->getInt(name);
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Parameter " << parameterKey(recordSetName,name,index) << " from database: " << tmpPar << endmsg; 
    }
  } 
  return tmpPar;
}

int 
GeometryDBSvc::getInt(const std::string & recordSetName, const std::string & name, int index) const 
{
  int tmpPar = 0;
  if (getValue(recordSetName,name,index,tmpPar)) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Parameter " << parameterKey(recordSetName,name,index) << " from text file: " << tmpPar << endmsg; 
    return tmpPar;
  } else  {
    msg(MSG::ERROR) << "Parameter " << parameterKey(recordSetName,name,index) << index << " not found." << endmsg; 
    return 0;
  } 
}

std::string 
GeometryDBSvc::getString(IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  if (!recordSet.get()) return getString("",name,index);
  std::string tmpPar;
  std::string recordSetName = recordSet->nodeName();
  if (getValue(recordSetName,name,index,tmpPar)) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Parameter " << parameterKey(recordSetName,name,index) << " from text file: " << tmpPar << endmsg; 
  } else {
    if (index >= (int)recordSet->size()) {
      msg(MSG::ERROR) << "Parameter " << parameterKey(recordSetName,name,index) << index << " not found. " << endmsg; 
    } else {
      tmpPar = (*recordSet)[index]->getString(name);
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Parameter " << parameterKey(recordSetName,name,index) << " from database: " << tmpPar << endmsg; 
    }
  } 
  return tmpPar;
}

std::string 
GeometryDBSvc::getString(const std::string & recordSetName, const std::string & name, int index) const
{
  std::string tmpPar;
  if (getValue(recordSetName,name,index,tmpPar)) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Parameter " << parameterKey(recordSetName,name,index) << " from text file: " << tmpPar << endmsg; 
    return tmpPar;
  } else  {
    msg(MSG::ERROR) << "Parameter " << parameterKey(recordSetName,name,index) << " not found." << endmsg; 
    return "";
  } 
}

// This is where we actually try to get the parameter from the text file. 
bool 
GeometryDBSvc::getValue(const std::string & recordSetName, const std::string & name, int index, std::string & var) const
{ 
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Looking for Parameter " << parameterKey(recordSetName,name,index) << endmsg; 
  if (recordSetName.empty() && index) {
    msg(MSG::WARNING) << "Non zero index for parameter with empry record set. Index will be ignored: " << index << endmsg; 
  }
  var = "";
  if (!m_textParameters) return false;
  std::string lookupKey = parameterKey(recordSetName,name,index);
  bool result = m_textParameters->find(parameterKey(recordSetName,name,index),var);
  if (!result && !recordSetName.empty()) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Not found. Looking for default entry with #ALL" << endmsg; 
    std::ostringstream keyalt;
    keyalt << recordSetName << "#ALL:" << name;
    result = m_textParameters->find(keyalt.str(),var);
  }
  return result;
}


bool 
GeometryDBSvc::getValue(const std::string & recordSetName, const std::string & name, int index, double & var) const
{
  std::string result;
  var = 0;
  // Get the parameter as a string first
  if (getValue(recordSetName, name, index, result)) {
    std::istringstream istr(result);
    istr >> var;
    if (!istr.eof()) { // Should have read the whole stream
      msg(MSG::ERROR) << "Error retrieving parameter " << parameterKey(recordSetName,name,index) << " as a double: " << result << endmsg; 
      return false;
    }
    return true;
  }
  return false;
}
 
bool 
GeometryDBSvc::getValue(const std::string & recordSetName, const std::string & name, int index, int & var) const
{
  std::string result;
  var = 0;
  // Get the parameter as a string first
  if (getValue(recordSetName, name, index, result)) {
    std::istringstream istr(result);
    istr >> var;
    if (!istr.eof()) {  // Should have read the whole stream
      msg(MSG::ERROR) << "Error retrieving parameter " << parameterKey(recordSetName,name,index) << " as an int: " << result << endmsg;
      return false;
    }
    return true;
  }
  return false;
}

bool 
GeometryDBSvc::testFieldTxt(IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  std::string recordSetName = recordSet->nodeName();
  return testField(recordSetName, name, index);
}

bool 
GeometryDBSvc::testField(IRDBRecordset_ptr recordSet, const std::string & name, int index) const 
{
  std::string recordSetName = recordSet->nodeName();
  if (testField(recordSetName, name, index)) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Parameter " << parameterKey(recordSetName,name,index) << " found in text file. " << endmsg; 
    return true;
  }
  bool result = false;
  if (index < (int)recordSet->size()) {
    try {
      result = !(*recordSet)[index]->isFieldNull(name);
    }
    catch(std::runtime_error& ex) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Exception caught: " << ex.what() << endmsg;
      result = false;
    }
  } 
  if (result){ 
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Parameter " << parameterKey(recordSetName,name,index) << " found in database. " << endmsg; 
  } else {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Parameter " << parameterKey(recordSetName,name,index) << " not found. " << endmsg; 
  }
  return result;
}

bool 
GeometryDBSvc::testField(const std::string & recordSetName, const std::string & name, int index) const 
{
  std::string tmpStr;
  return getValue(recordSetName, name, index, tmpStr);
}
 

unsigned int 
GeometryDBSvc::getTableSize(IRDBRecordset_ptr recordSet) const 
{
  int tmpPar = 0;
  unsigned int tmpParUnsigned = 0;
  std::string recordSetName = recordSet->nodeName();
  // Table size stored with key of form TableSize:TableName
  if (getTableSizeFromTextFile(recordSetName,tmpPar)) {
    if (tmpPar < 0) {
      msg(MSG::ERROR) << "Table " << recordSetName << " size from text file is negative: " << tmpPar << endmsg;  
      tmpPar = 0;
    } else {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Table " << recordSetName << " size from text file: " << tmpPar << endmsg;     
    }
    tmpParUnsigned = static_cast<unsigned int>(tmpPar);
  } else {
    // Get from database
    tmpParUnsigned = recordSet->size();
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Table " << recordSetName << " size from database: " << tmpParUnsigned << endmsg;       
  }
  return tmpParUnsigned;
}
    
unsigned int 
GeometryDBSvc::getTableSize(const std::string & recordSetName) const 
{
  int tmpPar = 0;
  unsigned int tmpParUnsigned = 0;
  // Table size stored with key of form TableSize:TableName
  if (getTableSizeFromTextFile(recordSetName,tmpPar)) {
    if (tmpPar < 0) {
      msg(MSG::ERROR) << "Table " << recordSetName << " size from text file is negative: " << tmpPar << endmsg;  
      tmpPar = 0;
    } else {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TEXTFILE: Table " << recordSetName << " size from text file: " << tmpPar << endmsg;     
    }
    tmpParUnsigned = static_cast<unsigned int>(tmpPar);
  } else {
    msg(MSG::ERROR) << "getTableSize: Table " << recordSetName << " does not exist in text file." << endmsg;  
  }
  return tmpParUnsigned;
}

// Get table size from the text file. 
bool 
GeometryDBSvc::getTableSizeFromTextFile(const std::string & recordSetName, int & var) const
{ 
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Looking for TableSize for " << recordSetName << endmsg; 
  std::string result;
  var = -1;
  if (!m_textParameters) return false;
  std::ostringstream key;
  key << "TableSize:" << recordSetName;
  if (m_textParameters->find(key.str(),result)) {
    // Convert to int
    std::istringstream istr(result);
    istr >> var;
    if (!istr.eof()) { // Should have read the whole stream
      msg(MSG::ERROR) << "Error retrieving parameter TableSize:" << recordSetName << " as an int: " << result << endmsg; 
      return false;
    }
    return true;
  } 
  return false;
}

void 
GeometryDBSvc::printParameters(const std::string & section) const
{
  m_textParameters->printParameters(section); 
}

void 
GeometryDBSvc::printNotUsed(const std::string & section) const
{
  m_textParameters->printNotUsed(section);   
}

