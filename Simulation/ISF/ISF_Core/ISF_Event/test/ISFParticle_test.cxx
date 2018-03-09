/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "ISF_Event/ISFParticle.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "AtlasDetDescr/AtlasRegion.h"
#include "BarcodeEvent/Barcode.h"

void testConstructors() {
    Amg::Vector3D pos(1., 0., 2.);
    Amg::Vector3D mom(5., 4., 3.);
    double mass    = 123.;
    double charge  = 987.;
    int    pdgCode = 675;
    double time    = 923.;
    int    bcid    = 123;
    const ISF::DetRegionSvcIDPair origin( AtlasDetDescr::fAtlasCalo, 2 );
    Barcode::ParticleBarcode bc = Barcode::fUndefinedBarcode;
    ISF::TruthBinding *truth = 0;

    //
    // Amg & origin
    //
    ISF::ISFParticle isp1( pos,
                           mom,
                           mass,
                           charge,
                           pdgCode,
                           time,
                           origin,
                           bcid,
                           bc,
                           truth );

    assert( pos     == isp1.position()     );
    assert( mom     == isp1.momentum()     );
    assert( mass    == isp1.mass()         );
    assert( charge  == isp1.charge()       );
    assert( pdgCode == isp1.pdgCode()      );
    assert( time    == isp1.timeStamp()    );
    const ISF::ParticleHistory &history1 = isp1.history();
    assert( 1       == history1.size()     );
    assert( origin  == history1[0]         );
    assert( bcid    == isp1.getBCID()      );
    assert( bc      == isp1.barcode()      );
    assert( truth   == isp1.getTruthBinding() );

    //
    // Amg & parent
    //
    ISF::ISFParticle isp2( pos,
                           mom,
                           mass,
                           charge,
                           pdgCode,
                           time,
                           isp1, // parent
                           bc,
                           truth );

    assert( pos     == isp2.position()     );
    assert( mom     == isp2.momentum()     );
    assert( mass    == isp2.mass()         );
    assert( charge  == isp2.charge()       );
    assert( pdgCode == isp2.pdgCode()      );
    assert( time    == isp2.timeStamp()    );
    const ISF::ParticleHistory &history2 = isp2.history();
    assert( 1       == history2.size()     );
    assert( origin  == history2[0]         );
    assert( bc      == isp2.barcode()      );
    assert( truth   == isp2.getTruthBinding() );

    //
    // HepGeom & parent
    //
    HepGeom::Point3D<double>  hepPos( pos.x(), pos.y(), pos.z() );
    HepGeom::Vector3D<double> hepMom( mom.x(), mom.y(), mom.z() );
    ISF::ISFParticle isp3( hepPos,
                           hepMom,
                           mass,
                           charge,
                           pdgCode,
                           time,
                           isp2, // parent
                           bc,
                           truth );

    assert( pos     == isp3.position()     );
    assert( mom     == isp3.momentum()     );
    assert( mass    == isp3.mass()         );
    assert( charge  == isp3.charge()       );
    assert( pdgCode == isp3.pdgCode()      );
    assert( time    == isp3.timeStamp()    );
    const ISF::ParticleHistory &history3 = isp3.history();
    assert( 1       == history3.size()     );
    assert( origin  == history3[0]         );
    assert( bc      == isp3.barcode()      );
    assert( truth   == isp3.getTruthBinding() );
}


int main() {
    testConstructors();
    return 0;
}

