//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtPHOTOS.cc
//
// Description: This routine takes the particle *p and applies
//              the PHOTOS package to generate final state radiation
//              on the produced mesons.
//
// Modification history:
//
//    RYD     October 1, 1997        Module created
//    JJB     May 2011               Modified to use new PHOTOS generator
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"

#include "EvtGen_i/EvtGenExternal/EvtPHOTOS.hh"
#include "EvtGen_i/EvtGenExternal/EvtExternalGenFactory.hh"

EvtPHOTOS::EvtPHOTOS() {

  m_photosEngine = 0;
  
}

EvtPHOTOS::~EvtPHOTOS() {

}

void EvtPHOTOS::doRadCorr(EvtParticle *p) {

  if (m_photosEngine == 0) {
    m_photosEngine = EvtExternalGenFactory::getInstance()->getGenerator(EvtExternalGenFactory::PhotosGenId);
  }

  if (m_photosEngine != 0) {
    m_photosEngine->doDecay(p);
  }
  
}

