/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  : _physVol(physVol)
{
  physVol->ref();
}

/**
 ** Destructor
 **/
GeoModelExperiment::~GeoModelExperiment()  {
  // Unref all temporary volumes
  std::vector<GeoPhysVol*>::iterator it = _tmpVolumes.begin();
  for(; it!=_tmpVolumes.end(); it++) 
    (*it)->unref();

  _physVol->unref();
}

GeoPhysVol * GeoModelExperiment::getPhysVol() {
  return _physVol;
}


const GeoPhysVol *GeoModelExperiment::getPhysVol() const{
  return _physVol;
}

GeoModelExperiment::ConstIterator GeoModelExperiment::beginManager() const {
  return _managers.begin();
}

GeoModelExperiment::ConstIterator GeoModelExperiment::endManager() const {
  return _managers.end();
}

void GeoModelExperiment::addManager(const GeoVDetectorManager *mgr) {
  if (getManager(mgr->getName())) {
    std::ostringstream errorMessage;
    errorMessage
      << "A detector manager named" << mgr->getName() 
      << " has already been declared to GeoModelExperiment";
    
    throw std::runtime_error(errorMessage.str());
  }
  _managers.insert(mgr);
}

const GeoVDetectorManager *GeoModelExperiment::getManager(const std::string & name) const {
  
  const_iterator_type m = std::find_if(_managers.begin(),_managers.end(),NameEquals(name));
  if (m!=_managers.end()) {
    return *m;
  }
  else {
    return NULL;
  }
  
}

GeoModelExperiment::NameEquals::NameEquals(const std::string & name):_name(name) {
}

bool GeoModelExperiment::NameEquals::operator() (const value_type & m) const {
  return m->getName()==_name;
}

bool GeoModelExperiment::LexigraphicalOrder::operator () (const value_type & a, const value_type &b) const {
  return a->getName()< b->getName();
}

void GeoModelExperiment::addTmpVolume(GeoPhysVol* volume)
{
  _tmpVolumes.push_back(volume);
}
