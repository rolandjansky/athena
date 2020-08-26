//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2011      University of Warwick, UK
//
// Module: EvtExternalGenFactory
//
// Description: A factory type method to create engines for external physics
// generators like Pythia.
//
// Modification history:
//
//    John Back       April 2011            Module created
//
//------------------------------------------------------------------------------
//

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGen_i/EvtGenExternal/EvtExternalGenFactory.hh"

#ifdef EVTGEN_PYTHIA
#include "EvtGen_i/EvtGenExternal/EvtPythiaEngine.hh"
#endif

#ifdef EVTGEN_PHOTOS
#include "EvtGen_i/EvtGenExternal/EvtPhotosEngine.hh"
#endif

#ifdef EVTGEN_TAUOLA
#include "EvtGen_i/EvtGenExternal/EvtTauolaEngine.hh"
#endif

#include <iostream>

EvtExternalGenFactory::EvtExternalGenFactory() {

  m_extGenMap.clear();

}

EvtExternalGenFactory::~EvtExternalGenFactory() {

  ExtGenMap::iterator iter;
  for (iter = m_extGenMap.begin(); iter != m_extGenMap.end(); ++iter) {

    EvtAbsExternalGen* theGenerator = iter->second;
    delete theGenerator;

  }
  
  m_extGenMap.clear();

}

EvtExternalGenFactory* EvtExternalGenFactory::getInstance() {

  static EvtExternalGenFactory* theFactory = 0;
  
  if (theFactory == 0) {
    theFactory = new EvtExternalGenFactory();
  }

  return theFactory;

}

#ifdef EVTGEN_PYTHIA
void EvtExternalGenFactory::definePythiaGenerator(std::string xmlDir, 
						  bool convertPhysCodes,
						  bool useEvtGenRandom) {

  // Only define the generator if we have the external ifdef variable set

  int genId = EvtExternalGenFactory::PythiaGenId;

  EvtGenReport(EVTGEN_INFO,"EvtGen")<<"Defining EvtPythiaEngine: data tables defined in "
		       <<xmlDir<<std::endl;

  if (convertPhysCodes == true) {
    EvtGenReport(EVTGEN_INFO,"EvtGen")<<"Pythia 6 codes in decay files will be converted to Pythia 8 codes"<<std::endl;
  } else {
    EvtGenReport(EVTGEN_INFO,"EvtGen")<<"Pythia 8 codes need to be used in decay files"<<std::endl;
  }

  if (useEvtGenRandom == true) {
    EvtGenReport(EVTGEN_INFO,"EvtGen")<<"Using EvtGen random engine for Pythia 8 as well"<<std::endl;
  }

  EvtAbsExternalGen* pythiaGenerator = new EvtPythiaEngine(xmlDir, convertPhysCodes, useEvtGenRandom);
  m_extGenMap[genId] = pythiaGenerator;
}
#else
void EvtExternalGenFactory::definePythiaGenerator(std::string , 
						  bool ,
						  bool )
{}
#endif



#ifdef EVTGEN_PHOTOS
void EvtExternalGenFactory::definePhotosGenerator(std::string photonType, bool useEvtGenRandom) {

  int genId = EvtExternalGenFactory::PhotosGenId;
  EvtGenReport(EVTGEN_INFO,"EvtGen")<<"Defining EvtPhotosEngine using photonType = "<<photonType<<std::endl;
  EvtAbsExternalGen* photosGenerator = new EvtPhotosEngine(photonType, useEvtGenRandom);
  m_extGenMap[genId] = photosGenerator;
}
#else
void EvtExternalGenFactory::definePhotosGenerator(std::string, bool)
{}
#endif



#ifdef EVTGEN_TAUOLA
void EvtExternalGenFactory::defineTauolaGenerator(bool useEvtGenRandom) {
  int genId = EvtExternalGenFactory::TauolaGenId;
  EvtGenReport(EVTGEN_INFO,"EvtGen")<<"Defining EvtTauolaEngine."<<std::endl;
  EvtAbsExternalGen* tauolaGenerator = new EvtTauolaEngine(useEvtGenRandom);
  m_extGenMap[genId] = tauolaGenerator;
}
#else
void EvtExternalGenFactory::defineTauolaGenerator(bool) {}
#endif


EvtAbsExternalGen* EvtExternalGenFactory::getGenerator(int genId) {

  EvtAbsExternalGen* theGenerator(0);

  ExtGenMap::iterator iter;

  if ((iter = m_extGenMap.find(genId)) != m_extGenMap.end()) {

    // Retrieve the external generator engine
    theGenerator = iter->second;

  }

  return theGenerator;

}

void EvtExternalGenFactory::initialiseAllGenerators() {

  ExtGenMap::iterator iter;
  for (iter = m_extGenMap.begin(); iter != m_extGenMap.end(); ++iter) {

    EvtAbsExternalGen* theGenerator = iter->second;
    if (theGenerator != 0) {
      theGenerator->initialise();
    }

  }
  
}
