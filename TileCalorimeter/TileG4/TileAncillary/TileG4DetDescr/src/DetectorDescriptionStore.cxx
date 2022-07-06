/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include "TileG4DetDescr/DetectorDescription.h"
//#include "FadsDetDescription/DescriptionMessenger.h"

#include <iostream>

namespace FADS {

  DetectorDescriptionStore::DetectorDescriptionStore() {
//	theMessenger=new DescriptionMessenger(this);
    m_detDescMap = new DetDescMap();
    m_nameSpaces["std"] = m_detDescMap;
    m_oldNameSpace = m_currentNameSpace = "std";
  }

  DetectorDescriptionStore& DetectorDescriptionStore::GetDetectorDescriptionStore ATLAS_NOT_THREAD_SAFE () {
    static DetectorDescriptionStore instance;
    return instance;
  }

  void DetectorDescriptionStore::UseNameSpace(std::string s) {
    if (s == m_currentNameSpace) return;
    m_oldNameSpace = m_currentNameSpace;
    if (m_nameSpaces.find(s) != m_nameSpaces.end()) {
      m_currentNameSpace = s;
      m_detDescMap = m_nameSpaces[s];
    } else {
      m_detDescMap = new DetDescMap();
      m_nameSpaces[s] = m_detDescMap;
      m_currentNameSpace = s;
    }
  }

  void DetectorDescriptionStore::ResetNameSpace() {
    m_currentNameSpace = m_oldNameSpace;
    m_detDescMap = m_nameSpaces[m_currentNameSpace];
    std::cout << " NameSpace reset to be " << m_currentNameSpace << std::endl;
  }

  void DetectorDescriptionStore::AddDetectorDescription(DetectorDescription *d) {
    std::string name = d->GetName();
    std::string::size_type i = name.find("::");
    std::string nspace;
    if (i != std::string::npos) {
      nspace = name.substr(0, i);
      name = name.substr(i + 2, name.size() - i + 2);
    }

    if (!nspace.empty()) UseNameSpace(nspace);
    if (m_detDescMap->find(name) != m_detDescMap->end()) std::cout << "Detector Description name " << name
                                                                   << " already exists in namespace "
                                                                   << m_currentNameSpace << ": nothing done"
                                                                   << std::endl;
    else (*m_detDescMap)[name] = d;
    if (!nspace.empty()) ResetNameSpace();
  }

  void DetectorDescriptionStore::RemoveDetectorDescription(std::string nam) {
    std::string::size_type i = nam.find("::");
    std::string nspace;
    if (i != std::string::npos) {
      nspace = nam.substr(0, i);
      nam = nam.substr(i + 2, nam.size() - i + 2);
    }

    if (!nspace.empty()) UseNameSpace(nspace);

    if (m_detDescMap->find(nam) != m_detDescMap->end()) m_detDescMap->erase(nam);

    if (!nspace.empty()) ResetNameSpace();
  }

  void DetectorDescriptionStore::ReplaceDetectorDescription(std::string nam, const DetectorDescription *d) {
    std::string::size_type i = nam.find("::");
    std::string nspace;
    if (i != std::string::npos) {
      nspace = nam.substr(0, i);
      nam = nam.substr(i + 2, nam.size() - i + 2);
    }

    if (!nspace.empty()) UseNameSpace(nspace);
    DetDescMap::iterator it;
    if ( (it = m_detDescMap->find(nam)) != m_detDescMap->end()) (*it).second = d;
    if (!nspace.empty()) ResetNameSpace();
  }

  bool DetectorDescriptionStore::FindDetectorDescription(std::string name) {
    return (m_detDescMap->find(name) != m_detDescMap->end());
  }

  const DetectorDescription* DetectorDescriptionStore::GetDetectorDescription(std::string name) {
    std::string::size_type i = name.find("::");
    std::string nspace;
    if (i != std::string::npos) {
      nspace = name.substr(0, i);
      name = name.substr(i + 2, name.size() - i + 2);
    }

    if (!nspace.empty()) UseNameSpace(nspace);
    const DetectorDescription *dd = 0;
    if (m_detDescMap->find(name) == m_detDescMap->end()) dd = 0;
//		std::cout<<"Detector Description name "<<name<<
//			   "not found! return 0"<<std::endl;
    else dd = (*m_detDescMap)[name];
    if (!nspace.empty()) ResetNameSpace();
    return dd;
  }

  void DetectorDescriptionStore::PrintDetectorDescription(std::string name) {
    std::string::size_type i = name.find("::");
    std::string nspace;
    if (i != std::string::npos) {
      nspace = name.substr(0, i);
      name = name.substr(i + 2, name.size() - i + 2);
    }

    if (!nspace.empty()) UseNameSpace(nspace);
    if (m_detDescMap->find(name) == m_detDescMap->end()) std::cout << "Detector Description name " << name
                                                                   << "not found! " << std::endl;
    else (*m_detDescMap)[name]->print();
    if (!nspace.empty()) ResetNameSpace();
  }

  DetDescIterator DetectorDescriptionStore::DetDescBegin() {
    return m_detDescMap->begin();
  }

  DetDescIterator DetectorDescriptionStore::DetDescEnd() {
    return m_detDescMap->end();
  }

  void DetectorDescriptionStore::PrintAll() {
    DetDescIterator it;
    NameSpaces::const_iterator ns;
    std::cout << std::endl;
    std::cout << "  Listing all detector description objects currently defined" << std::endl;
    for (ns = m_nameSpaces.begin(); ns != m_nameSpaces.end(); ns++) {
      std::cout << std::endl << "- NameSpace " << (*ns).first << std::endl << std::endl;
      for (it = ( (*ns).second)->begin(); it != ( (*ns).second)->end(); it++) {
        std::cout << "--->  ";
        if ( (*it).second->IsPointed()) std::cout << "\t\t";
        else std::cout << " * ";
        std::cout << (*it).first << std::endl;
      }
    }
  }

}	// end namespace
