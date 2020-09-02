/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "../TrigCostAnalysis/TableEntry.h"

#include <iomanip>
#include <sstream>

TableEntry::TableEntry() : 
  m_msgStream(nullptr, "TableEntry"),
  m_columnName(),
  m_columnHeader(),
  m_columnTooltip(),
  m_floats(),
  m_ints(),
  m_strings()
{}


void TableEntry::normaliseEntry(const std::string& name, const float denominator) {
  auto mapIt = m_floats.find(name);
  if (mapIt != m_floats.end()) {
    if (fabs(denominator) < 1e-10) {
      msg() << MSG::ERROR << "TableEntry::normaliseEntry: Normalise denominator is zero. Setting " << name << " to zero." << endmsg;
      mapIt->second = 0;
    } else {
      mapIt->second /= denominator;
    }
  } else {
    msg() << MSG::ERROR << "TableEntry::normaliseEntry: No floating point value with name " << name << endmsg;
  }
}


void TableEntry::addColumn(const std::string& name, const std::string& header, const std::string& tooltip) {
  std::string nameCopy(name);
  std::string headerCopy(header);
  std::string tooltipCopy(tooltip);
  nameCopy.erase(std::remove(nameCopy.begin(), nameCopy.end(), '\''), nameCopy.end());
  headerCopy.erase(std::remove(headerCopy.begin(), headerCopy.end(), '\''), headerCopy.end());
  tooltipCopy.erase(std::remove(tooltipCopy.begin(), tooltipCopy.end(), '\''), tooltipCopy.end());
  m_columnName.push_back(nameCopy);
  m_columnHeader.push_back(headerCopy);
  m_columnTooltip.push_back(tooltipCopy);
}


void TableEntry::setEntry(const std::string& name, const double value) {
  m_floats.emplace( name, static_cast<float>(value) );
}


void TableEntry::setEntry(const std::string& name, const std::string& value) {
  m_strings.emplace( name, value );
}


void TableEntry::setEntry(const std::string& name, const int value) {
  m_ints.emplace( name, value );
}


void TableEntry::setEntry(const std::string& name, const float value) {
  m_floats.emplace( name, value );
}


float TableEntry::getEntryFloat(const std::string& name) const {
  const auto mapIt = m_floats.find(name);
  if (mapIt != m_floats.end()) {
    return mapIt->second;
  }
  msg() << MSG::ERROR << "TableEntry::getEntryFloat: No floating point value with name " << name << endmsg;
  return 0.;
}


TString TableEntry::getHeaderRow() const {
  std::stringstream ss;
  for (const std::string& s : m_columnHeader) {
    ss << s;
    if (s != m_columnHeader.back()) {
      ss << ",";
    }
  }
  ss << std::endl;
  for (const std::string& s : m_columnTooltip) {
    ss << s;
    if (s != m_columnTooltip.back()) {
      ss << ",";
    }
  }
  ss << std::endl;
  return ss.str();
}


TString TableEntry::getRow() const {
  std::stringstream ss;
  ss << std::setprecision(4);
  for (const std::string& s : m_columnName) {
    const auto mapItS = m_strings.find(s);
    const auto mapItF = m_floats.find(s);
    const auto mapItI = m_ints.find(s);
    if (mapItS != m_strings.end()) {
      ss << mapItS->second;
    } else if (mapItF != m_floats.end()) {
      ss << mapItF->second;
    } else if (mapItI != m_ints.end()) {
      ss << mapItI->second;
    } else {
      msg() << MSG::ERROR << "TableEntry::getRow: No entry for column " << s << endmsg;
      ss << "???";
    }
    if (s != m_columnHeader.back()) {
      ss << ",";
    }
  }
  ss << std::endl;
  return ss.str();
}


MsgStream& TableEntry::msg() const {
  return m_msgStream;
}
