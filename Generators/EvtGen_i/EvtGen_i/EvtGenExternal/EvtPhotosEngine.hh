#ifdef EVTGEN_PHOTOS
//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2011      University of Warwick, UK
//
// Module: EvtPhotosEngine
//
// Description: Interface to the PHOTOS external generator
//
// Modification history:
//
//    John Back       May 2011            Module created
//
//------------------------------------------------------------------------

#ifndef EVT_I_PHOTOSENGINE_HH
#define EVT_I_PHOTOSENGINE_HH

#include "EvtGenModels/EvtAbsExternalGen.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtVector4R.hh"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

#include <string>

class EvtPhotosEngine : public EvtAbsExternalGen {

public:

  EvtPhotosEngine(std::string photonType = "gamma", bool useEvtGenRandom = true);
  virtual ~EvtPhotosEngine();

  virtual bool doDecay(EvtParticle* theMother);

  virtual void initialise();

protected:

private:

  std::string m_photonType;
  EvtId m_gammaId;
  double m_mPhoton;
  bool m_initialised;

  HepMC::GenParticle* createGenParticle(EvtParticle* theParticle, bool incoming);

};

#endif

#endif
