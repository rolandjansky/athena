/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
/**
 ** Constructor(s)
 **/
GeoModelExperiment::GeoModelExperiment( GeoPhysVol * physVol )
  : m_physVol(physVol)
{
  physVol->ref();
}

/**
 ** Destructor
 **/
GeoModelExperiment::~GeoModelExperiment()  {
  // Unref all temporary volumes
  std::vector<GeoPhysVol*>::iterator it = m_tmpVolumes.begin();
  for(; it!=m_tmpVolumes.end(); ++it) 
    (*it)->unref();

  m_physVol->unref();
}

GeoPhysVol * GeoModelExperiment::getPhysVol() {
  return m_physVol;
}


const GeoPhysVol *GeoModelExperiment::getPhysVol() const{
  return m_physVol;
}



GeoModelExperiment::ConstIterator GeoModelExperiment::beginManager() const {
  return m_managers.begin();
}

GeoModelExperiment::ConstIterator GeoModelExperiment::endManager() const {
  return m_managers.end();
}

void GeoModelExperiment::addManager(const GeoVDetectorManager *mgr) {
  if (getManager(mgr->getName())) {
    std::ostringstream errorMessage;
    errorMessage
      << "A detector manager named" << mgr->getName() 
      << " has already been declared to GeoModelExperiment";
    
    throw std::runtime_error(errorMessage.str());
  }
  m_managers.insert(mgr);
}

const GeoVDetectorManager *GeoModelExperiment::getManager(const std::string & name) const {
  
  const_iterator_type m = std::find_if(m_managers.begin(),m_managers.end(),NameEquals(name));
  if (m!=m_managers.end()) {
    return *m;
  }
  else {
    return NULL;
  }
  
}

std::vector<std::string> GeoModelExperiment::getListOfManagers() const {
  std::vector<std::string> managersList;
  for(auto manager : m_managers) {
    managersList.push_back(manager->getName());
  }
  return managersList;
}


GeoModelExperiment::NameEquals::NameEquals(const std::string & name):m_name(name) {
}

bool GeoModelExperiment::NameEquals::operator() (const value_type & m) const {
  return m->getName()==m_name;
}

bool GeoModelExperiment::LexigraphicalOrder::operator () (const value_type & a, const value_type &b) const {
  return a->getName()< b->getName();
}

void GeoModelExperiment::addTmpVolume(GeoPhysVol* volume)
{
  m_tmpVolumes.push_back(volume);
}
