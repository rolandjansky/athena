/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TextFileDBReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>

TextFileDBReader::TextFileDBReader() 
  : m_numSections(0),
    m_currentSection(0)
{}

TextFileDBReader::TextFileDBReader(const std::string & filename) 
  : m_numSections(0),
    m_currentSection(0)
{
  readFile(filename);
}


bool
TextFileDBReader::readFile(const std::string & readFile) 
{
  std::ifstream ifile;
  ifile.open(readFile.c_str());
  if (!ifile) {
    //std::cout << "Error opening file: " << readFile << std::endl;;
    return false;
  }
  
  bool tableMode = false;
  std::string currentTable;
  int currentIndex = -1;
  std::vector<std::string> currentFields;
  m_currentSection = 0;

  while (ifile) {
    std::string line;
    std::getline(ifile,line);
    if (!ifile) break;
    // Skip blank or comment lines
    if (line.empty() || line[0] == '#' || line.substr(0,2) == "//" ) continue;
    std::istringstream istr(line);

    std::string key;

    istr >> key;

    if (key.empty()) continue;
    
    if (key == "Table") {
      std::string value;
      istr >> value;
      tableMode = true;
      currentTable = value;
      currentIndex = -1;
      currentFields.clear();
    } else if (key == "TableEnd") { 
      std::ostringstream ostr;
      ostr <<  "TableSize:" << currentTable;
      add(ostr.str(), currentIndex);
      tableMode = false;
      currentTable = "";
      currentIndex = -1;
      currentFields.clear();
    } else if (key == "Section") {
      std::string value;
      istr >> value;
      if (!value.empty()) {
	int & section = m_sections[value];
	// First section will number 1. Section 0 refers to unnamed section
	if (!section) section = ++m_numSections;
	m_currentSection = section;
      }
    } else if (key == "EndSection") {
	m_currentSection = 0;   
    } else {      
      if (!tableMode) {
	std::string value;
	istr >> value;
	// Make sure its in correct format 
	key = formatKey(key); 
	add(key,value);
      } else { // table mode
	// reset stream.
	std::istringstream istr2(line);

	if (currentIndex < 0) {
	  // Get the fields
	  while (!istr2.eof()) { 
	    std::string value;
	    istr2 >> value;
	    if (!value.empty()) currentFields.push_back(value);
	  }
	} else {
	  // Get row of values
	  for (unsigned int i=0; i < currentFields.size(); i++) { 
	    std::string value;
	    istr2 >> value;
	    if (istr2) {
	      std::ostringstream ostr;
	      ostr <<  currentTable << "#" << currentIndex << ":" << currentFields[i];
	      add(ostr.str(), value);
	    }
	  }
	}
	currentIndex++;
      }
    }
  }
  ifile.close();
  //printParameters();
  return true;
}

std::string 
TextFileDBReader::formatKey(const std::string & key) const
{
  //std::cout << "Key in:  " << key << std::endl;
  // Split into tableName, fieldName and rowNumber
  // and recreate
  std::string tableName;
  std::string fieldName;
  std::string rowNumber;
  bool foundRowNumber = false;
  std::string::size_type pos = key.find(":");
  if (pos != std::string::npos) {
    tableName = key.substr(0,pos);
    foundRowNumber = getRowNumber(tableName,rowNumber);
    fieldName = key.substr(++pos); 
  } else {
    fieldName = key;
  }
  std::string tmpRowNumber;
  if (getRowNumber(fieldName,tmpRowNumber)) {
    if (foundRowNumber) {
      // Already have rowNumber from before
      std::cout << "ERROR in format:" << key << std::endl;
    } else {
      rowNumber = tmpRowNumber;
    }
  }
  
  std::string newKey = key;
  if ((tableName.empty() && !rowNumber.empty()) || fieldName.empty()) {
    std::cout << "ERROR in format: " << key << std::endl;
  } else {
    if (rowNumber.empty()) rowNumber = "0";
    if (tableName.empty()) {
      newKey = fieldName;
    }else if (tableName == "TableSize") {
      newKey = "TableSize:"+fieldName;
    } else {
      newKey = tableName+"#"+rowNumber+":"+fieldName;
    }
  }
  //std::cout << "Key out: " << newKey << std::endl;
  return newKey;
}

bool
TextFileDBReader::getRowNumber(std::string & key, std::string & rowNumber) const
{
  std::string::size_type pos = key.find("#");
  if (pos != std::string::npos) {
    rowNumber = key.substr(pos+1);
    key = key.substr(0,pos);
    return true;
  } 
  return false;
}



void 
TextFileDBReader::add(const std::string & key, int value) {
  std::ostringstream ostr;
  ostr << value;
  add(key,ostr.str());
}


void 
TextFileDBReader::add(const std::string & key, const std::string & value)
{
  if (m_table.find(key) != m_table.end()) {
    std::cout << "WARNING! Overwriting exist entry with key: " << key << std::endl;
  } 
  m_table[key] = Data(value,m_currentSection);
}

bool 
TextFileDBReader::find(const std::string & key, std::string & result) const
{
  std::map<std::string,Data>::const_iterator iter = m_table.find(key);
  if (iter != m_table.end()) {
    result = iter->second.value;
    m_logger[key]++;
    return true;
  } else {
    result = "";
    return false;
  }
}

void
TextFileDBReader::printParameters(const std::string & section) const
{
  std::ios::fmtflags iosflags = std::cout.flags();
  std::cout << std::left;
  int sectionNum = 0;
  if (!section.empty()) {
    std::map<std::string,int>::const_iterator iterSect = m_sections.find(section);
    if (iterSect != m_sections.end()) sectionNum = iterSect->second;
    // If not found then prints those in unnamed section.(ie sectionNum = 0)
  } 
  for (std::map<std::string,Data>::const_iterator iter = m_table.begin();
       iter != m_table.end();
       ++iter) {
    if (section.empty() || iter->second.section == sectionNum) {
      std::cout << std::setw(35) << iter->first << " " << iter->second.value << std::endl;
    }
  }
  // reset flags to original state
  std::cout.flags(iosflags);
} 

// Print those variables that are not accessed
void
TextFileDBReader::printNotUsed(const std::string & section) const
{
  std::ios::fmtflags iosflags = std::cout.flags();
  std::cout << std::left;
  bool allused = true;
  int sectionNum = 0;
  if (!section.empty()) {
    std::map<std::string,int>::const_iterator iterSect = m_sections.find(section);
    if (iterSect != m_sections.end()) sectionNum = iterSect->second;
    // If not found then considers those in unnamed section (ie sectionNum = 0)
  }
  for (std::map<std::string,Data>::const_iterator iter = m_table.begin();
       iter != m_table.end();
       ++iter) {
    if ((section.empty() || iter->second.section == sectionNum) && m_logger.find(iter->first) == m_logger.end()) {
      std::cout << std::setw(35) << iter->first << " " << iter->second.value << std::endl;
      allused = false;
    }
  }
  if (allused) {
    std::cout << "All parameters used" << std::endl;
  }
  // reset flags to original state
  std::cout.flags(iosflags);
} 
 

bool 
TextFileDBReader::sectionPresent(const std::string & section) const
{
  return (m_sections.find(section) != m_sections.end());
}
