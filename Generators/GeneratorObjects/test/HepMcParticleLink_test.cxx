/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * @file test the special element link for GenParticle in the default
 * McEventCollection
 * @author Paolo Calafiura
 * $Id: HepMcParticleLink_test.cxx,v 1.5 2008-08-23 00:04:04 ianh Exp $
 ***************************************************************************/
/* #define GENP_DEBUG 1 */

#include "TestTools/initGaudi.h"

#include <iostream>
#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "StoreGate/StoreGateSvc.h"


using namespace Athena_test;
using namespace std;
using namespace HepMC;
using namespace CLHEP;

//cut-and-paste from HepMC/examples/example_BuildEventFromScratch.cxx
GenEvent* buildEvent() {
    //
    // In this example we will place the following event into HepMC "by hand"
    //
    //     name status pdg_id  parent Px       Py    Pz       Energy      Mass
    //  1  !p+!    3   2212    0,0    0.000    0.000 7000.000 7000.000    0.938
    //  2  !p+!    3   2212    0,0    0.000    0.000-7000.000 7000.000    0.938
    //=========================================================================
    //  3  !d!     3      1    1,1    0.750   -1.569   32.191   32.238    0.000
    //  4  !u~!    3     -2    2,2   -3.047  -19.000  -54.629   57.920    0.000
    //  5  !W-!    3    -24    1,2    1.517   -20.68  -20.605   85.925   80.799
    //  6  !gamma! 1     22    1,2   -3.813    0.113   -1.833    4.233    0.000
    //  7  !d!     1      1    5,5   -2.445   28.816    6.082   29.552    0.010
    //  8  !u~!    1     -2    5,5    3.962  -49.498  -26.687   56.373    0.006

    // // first we construct a ParticleDataTable with all the particles we need
    // ParticleDataTable pdt("my particle data table");
    // // create a particle data entry for the proton and add it to pdt at the
    // // same time
    // pdt.insert( new ParticleData( "p+", 2212,   +1, 0.938,  -1, .5 ) );
    // pdt.insert( new ParticleData( "d",  1,  -2./3., 0,      -1, .5 ) );
    // pdt.insert( new ParticleData( "u~", -2, -1./3., 0,      -1, .5 ) );
    // pdt.insert( new ParticleData( "W-", -24,    -1, 80.396,
	// 			  clifetime_from_width(2.06), 1 )    );
    // pdt.insert( new ParticleData( "gamma", 22,   0, 0,      -1, 1  ) );

    // // print out the GenParticle Data to the screen
    // //    pdt.print();

    // First create the event container, with Signal Process 20, event number 1
    GenEvent* evt = new GenEvent( 20, 1 );
    //
    // create vertex 1 and vertex 2, together with their inparticles
    GenVertex* v1 = new GenVertex();
    evt->add_vertex( v1 );
    v1->add_particle_in( new GenParticle( HepLorentzVector(0,0,7000,7000),
				       2212, 3 ) );
    GenVertex* v2 = new GenVertex();
    evt->add_vertex( v2 );
    v2->add_particle_in( new GenParticle( HepLorentzVector(0,0,-7000,7000),
				       2212, 3 ) );
    //
    // create the outgoing particles of v1 and v2
    GenParticle* p3 =
	new GenParticle( HepLorentzVector(.750,-1.569,32.191,32.238), 1, 3 );
    v1->add_particle_out( p3 );
    GenParticle* p4 =
	new GenParticle( HepLorentzVector(-3.047,-19.,-54.629,57.920), -2, 3 );
    v2->add_particle_out( p4 );
    //
    // create v3
    GenVertex* v3 = new GenVertex();
    evt->add_vertex( v3 );
    v3->add_particle_in( p3 );
    v3->add_particle_in( p4 );
    v3->add_particle_out(
	new GenParticle( HepLorentzVector(-3.813,0.113,-1.833,4.233 ), 22, 1 )
	);
    GenParticle* p5 =
	new GenParticle( HepLorentzVector(1.517,-20.68,-20.605,85.925), -24,3);
    v3->add_particle_out( p5 );
    //
    // create v4
    GenVertex* v4 = new GenVertex();
    evt->add_vertex( v4 );
    v4->add_particle_in( p5 );
    v4->add_particle_out(
	new GenParticle( HepLorentzVector(-2.445,28.816,6.082,29.552), 1,1 )
	);
    v4->add_particle_out(
	new GenParticle( HepLorentzVector(3.962,-49.498,-26.687,56.373), -2,1 )
	);
    //
    // tell the event which vertex is the signal process vertex
    evt->set_signal_process_vertex( v3 );
    // the event is complete, we now print it out to the screen
#ifdef GENP_DEBUG
    evt->print();
#endif

    // delete all particle data objects in the particle data table pdt
    //pdt.delete_all();
    return evt;
}

class is_photon {
public:
  bool operator() ( const GenParticle* p ) {
    return ( p && p->pdg_id() == 22 );
  }
};
#include <algorithm>
    // where is_photon() is a predicate like:
int main() {
  cout << "*** GenParticleLink_test starts ***" <<endl;
  ISvcLocator* pSvcLoc;
  if (!initGaudi("HepMcParticleLink_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }
  assert(pSvcLoc);

  StoreGateSvc* pStore(0);
  assert((pSvcLoc->service("StoreGateSvc", pStore)).isSuccess());
  assert(pStore);

  McEventCollection* pTruth = new McEventCollection();


  GenEvent* pEvent(buildEvent());
  pTruth->push_back(pEvent);
  pTruth->push_back(buildEvent());  //a copy
  const bool SETCONST(false);
  pStore->record(pTruth, HepMcParticleLink::s_DEFAULTKEY, SETCONST); //non-modifiable
  list<GenParticle*> theGammas;
  HepMC::copy_if( pEvent->particles_begin(), pEvent->particles_end(),
		  back_inserter(theGammas), is_photon() );
  assert(theGammas.size() == 1);
  const GenParticle* pGamma(theGammas.front());
#ifdef GENP_DEBUG
  pGamma->print();
  cout << "gamma barcode " << hex << pGamma->barcode() << endl;
#endif

  HepMcParticleLink gammaLink1(pGamma);
  HepMcParticleLink gammaLink2(pGamma->barcode());
  HepMcParticleLink gammaLink11(pGamma, 1);
  HepMcParticleLink gammaLink12(pGamma->barcode(), 1);

  cout << "Testing HepMcParticleLink streamer "
       << gammaLink1 << " --- " << gammaLink11 <<endl;


#ifdef GENP_DEBUG
  cout << "link barcode " << hex << gammaLink1.barcode() << endl;
  cout << "link index " << hex << gammaLink1.eventIndex() << endl;
  cout << "link1 barcode " << hex << gammaLink11.barcode() << endl;
  cout << "link1 index " << hex << gammaLink11.eventIndex() << endl;
#endif
  assert (*gammaLink1 == *gammaLink2);
  assert (gammaLink1 == gammaLink2);
  assert (gammaLink1.barcode() == pGamma->barcode());

  assert (*gammaLink11 == *gammaLink12);
  assert (gammaLink11 == gammaLink12);

  assert (*gammaLink1 == *gammaLink11);
  assert ( gammaLink1 != gammaLink11 ); //FIXME weird! Can't check ptr...
  assert ( gammaLink1 < gammaLink11 );  //FIXME weird! Can't check ptr...


#ifdef GENP_DEBUG
  cout << pStore->dump() << endl;
#endif
  pStore->clearStore();

  cout << "*** GenParticleLink_test OK ***" <<endl;
  return 0;
}
