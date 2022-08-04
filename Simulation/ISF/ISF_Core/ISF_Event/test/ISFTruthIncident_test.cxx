/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "ISF_Event/ISFTruthIncident.h"

// inputs to truthincident
#include "ISF_Event/ISFParticle.h"

// children container typedefs
#include "ISF_Event/ISFParticleVector.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "AtlasDetDescr/AtlasRegion.h"
#include "BarcodeEvent/Barcode.h"


void testConstructors() {
  
  // generate ISFParticle parent and children 
  Amg::Vector3D pos(1., 0., 2.);
  Amg::Vector3D mom(5., 4., 3.);
  double mass    = 123.;
  double charge  = 987.;
  int    pdgCode = 675;
  double time    = 923.;
  int    bcid    = 123;
  const ISF::DetRegionSvcIDPair origin( AtlasDetDescr::fAtlasCalo, 2 );
  Barcode::ParticleBarcode partBC = Barcode::fUndefinedBarcode;
  ISF::TruthBinding *truth = 0;
  ISF::ISFParticle isp1( pos,
                         mom,
                         mass,
                         charge,
                         pdgCode,
                         time,
                         origin,
                         bcid,
                         partBC,
                         truth );
  
  ISF::ISFParticle isp2( pos,
                         mom,
                         mass,
                         charge,
                         pdgCode,
                         time,
                         isp1, // parent
                         partBC,
                         truth );
  
  Barcode::PhysicsProcessCode procBC = Barcode::fUndefinedBarcode;

  {
    // initialize with ISFParticleVector
    ISF::ISFParticleVector children {&isp2};
    ISF::ISFTruthIncident truthIncident(isp1,
                                        children,
                                        procBC,
                                        origin.first);

    assert(procBC == truthIncident.physicsProcessCode());
    assert(origin.first == truthIncident.geoID());

    assert(isp1.momentum().mag2() == truthIncident.parentP2());
    assert(isp1.pdgCode() == truthIncident.parentPdgCode());
    unsigned int nChildren = truthIncident.numberOfChildren();
    assert(1 == nChildren);
    assert(isp2.momentum().mag2() == truthIncident.childP2(0));
    assert(isp2.pdgCode() == truthIncident.childPdgCode(0));
  }
  
}


int main() {
  
  testConstructors();
  return 0;
}

