/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TextFileDBReader_H
#define TextFileDBReader_H

/// Class to read in a text file and allow easy retrieval of parameters
#include <string>
#include <map>

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
  };


  std::string formatKey(const std::string & key) const;
  bool getRowNumber(std::string & key, std::string & rowNumber) const;
  void add(const std::string & key, const std::string & value);
  void add(const std::string & key, int);
  std::map<std::string, Data> m_table;
  std::map<std::string, int> m_sections;
  mutable std::map<std::string, int> m_logger;
  int m_numSections;
  int m_currentSection;
  std::string m_name;
};

#endif // PixelGeoModel_TextFileDBReader_H
