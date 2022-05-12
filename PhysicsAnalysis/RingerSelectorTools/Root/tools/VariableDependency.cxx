/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "RingerSelectorTools/tools/VariableDependency.h"

#include "RingerSelectorTools/tools/IOHelperFcns.icc"

namespace Ringer {

// =============================================================================
std::string VariableDependency::type() const
{
  std::stringstream depVar;
  if (m_etaDependency){
    if (m_etDependency){
      depVar.precision(3);
      depVar << "[" << m_etaMin << "<=eta<" << m_etaMax << ", ";
      depVar.precision(2);
      depVar << m_etMin << "<=Et<" << m_etMax << "]";
    } else {
      depVar.precision(3);
      depVar << "[" << m_etaMin << "<=eta<" << m_etaMax << "]";
    }
  } else {
    if (m_etDependency){
      depVar.precision(2);
      depVar << "[" << m_etMin << "<=Et<" << m_etMax << "]";
    } else {
      return "[eta, Et Independent]";
    }
  }
  return depVar.str();
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

