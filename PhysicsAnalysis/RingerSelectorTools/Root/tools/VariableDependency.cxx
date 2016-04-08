/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VariableDependency.cxx 667899 2015-05-18 18:37:25Z wsfreund $
#include "RingerSelectorTools/tools/VariableDependency.h"

#include "RingerSelectorTools/tools/IOHelperFcns.icc"

namespace Ringer {

// =============================================================================
const char* VariableDependency::type() const
{
  std::stringstream depVar;
  depVar.precision(2);
  if (m_etaDependency){
    if (m_etDependency){
      depVar << "[" << m_etaMin << "<=eta<" << m_etaMax << ", " 
             << m_etMin << "<=Et<" << m_etMax << "]";
    } else {
      depVar << "[" << m_etaMin << "<=eta<" << m_etaMax << "]";
    }
  } else {
    if (m_etDependency){
      depVar << "[" << m_etMin << "<=Et<" << m_etMax << "]";
    } else {
      return "[eta, Et Independent]";
    }
  }
  const_cast<std::string&>(m_type) = depVar.str();
  return m_type.c_str();
}

// =============================================================================
void VariableDependency::read(VariableDependency *varDep, 
    TDirectory *configDir, 
    unsigned /*writtenVersion*/)
{
  // Read unsigned int and transform it to EtDependency:
  IOHelperFcns::readVar<EtaDependency, unsigned int>(configDir, 
      "etaDependency", 
      varDep->m_etaDependency ); 
  // Read unsigned int and transform it to EtDependency:
  IOHelperFcns::readVar<EtDependency, unsigned int>(configDir, 
      "etDependency",  
      varDep->m_etDependency  ); 
  IOHelperFcns::readVar(configDir, "etaMin",        varDep->m_etaMin        ); 
  IOHelperFcns::readVar(configDir, "etaMax",        varDep->m_etaMax        ); 
  IOHelperFcns::readVar(configDir, "etMin",         varDep->m_etMin         ); 
  IOHelperFcns::readVar(configDir, "etMax",         varDep->m_etMax         ); 
}

// =============================================================================
void VariableDependency::writeDepInfo(TDirectory *configDir) const
{
  // Write EtaDependency enumeration as unsigned int:
  IOHelperFcns::writeVar< const EtaDependency, const unsigned int>( configDir, 
      "etaDependency", 
      m_etaDependency );
  // Write EtDependency enumeration as unsigned int:
  IOHelperFcns::writeVar< const EtDependency, const unsigned int>( configDir, 
      "etDependency",  
      m_etDependency  );
  IOHelperFcns::writeVar(configDir, "etaMin",        m_etaMin        );
  IOHelperFcns::writeVar(configDir, "etaMax",        m_etaMax        );
  IOHelperFcns::writeVar(configDir, "etMin",         m_etMin         );
  IOHelperFcns::writeVar(configDir, "etMax",         m_etMax         );
}

} // namespace Ringer

