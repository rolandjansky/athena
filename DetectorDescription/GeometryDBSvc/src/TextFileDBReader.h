/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TextFileDBReader_H
#define TextFileDBReader_H

/// Class to read in a text file and allow easy retrieval of parameters
#include <string>
#include <map>
//for thread-safety checker
#include "CxxUtils/checker_macros.h"
//use thread-safe atomic_flag to replace m_logger
#include <atomic> 
#include <unordered_map>

class TextFileDBReader
{							
public:
  TextFileDBReader(const std::string & filename);
  TextFileDBReader();
  bool readFile(const std::string & filename);
  bool find(const std::string & key, std::string & result) const;
  void printParameters(const std::string & section = "") const;
  void printNotUsed(const std::string & section = "") const;
  bool sectionPresent(const std::string & section) const;

private:
  class Data 
  {
  public:
    Data() : section(0) {}
    Data(const std::string & v, int s) 
      : value(v),
	section(s)
    {}
    std::string value;
    int section;
    //add a atomic_flag to replace the m_logger
    mutable std::atomic_bool flag = ATOMIC_VAR_INIT(false);
  };


  std::string formatKey(const std::string & key) const;
  bool getRowNumber(std::string & key, std::string & rowNumber) const;
  void add(const std::string & key, const std::string & value);
  void add(const std::string & key, int);
  std::unordered_map<std::string, Data> m_table;
  std::unordered_map<std::string, int> m_sections;
  int m_numSections;
  int m_currentSection;
  std::string m_name;

};

#endif // PixelGeoModel_TextFileDBReader_H
