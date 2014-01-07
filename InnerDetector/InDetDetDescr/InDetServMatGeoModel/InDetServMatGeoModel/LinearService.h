/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LinearService_H
#define LinearService_H

#include <string>
#include <vector>

class LinearService {
public:

  typedef std::pair< std::string, double> EntryType;

  LinearService() {}

  LinearService( const std::string& name, double lw, const std::vector< EntryType>& cmp) :
    m_name(name), m_length(lw), m_components(cmp) {}

  /// Constructor for elementary material (no components)
  LinearService( const std::string& name, double lw) :
    m_name(name), m_length(lw), m_components() {}

  const std::string& name() const {return m_name;}

  double length() const {return m_length;}

  const std::vector< EntryType>& components() const {return m_components;}

private:

  std::string             m_name;
  double                  m_length;
  std::vector< EntryType> m_components;

};

#endif
