/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file GeneratorObjectsTPCnv/test/HepMcParticleLinkCnv_p1_test.cxx
 * @brief Tests for HepMcParticleLinkCnv_p1.
 */


#undef NDEBUG
#include <cassert>
#include <vector>
#include <iostream>
// HepMC includes
#include "AtlasHepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "StoreGate/WriteHandle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TestTools/initGaudi.h"

void compare (const HepMcParticleLink& p1,
              const HepMcParticleLink& p2)
{
  assert ( p1.isValid() == p2.isValid() );
  assert ( p1.barcode() == p2.barcode() );
  assert ( p1.eventIndex() == p2.eventIndex() );
  assert ( p1.getEventCollectionAsChar() == p2.getEventCollectionAsChar() );
  assert ( p1.cptr() == p2.cptr() );
  assert ( p1 == p2 );
}

void populateGenEvent(HepMC::GenEvent & ge, int pdgid1, int pdgid2, std::vector<HepMC::GenParticlePtr>& genPartList)
{
  HepMC::FourVector myPos( 0.0, 0.0, 0.0, 0.0);
  HepMC::GenVertexPtr myVertex = HepMC::newGenVertexPtr( myPos, -1 );
  HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle1 = HepMC::newGenParticlePtr(fourMomentum1, pdgid1, 2);
  myVertex->add_particle_in(inParticle1);
  HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle2 = HepMC::newGenParticlePtr(fourMomentum2, pdgid2, 2);
  myVertex->add_particle_in(inParticle2);
  HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle3 = HepMC::newGenParticlePtr(fourMomentum3, pdgid1, 1);
  myVertex->add_particle_out(inParticle3);
  genPartList.push_back(inParticle3);
  HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle4 = HepMC::newGenParticlePtr(fourMomentum4, pdgid2, 1);
  myVertex->add_particle_out(inParticle4);
  genPartList.push_back(inParticle4);
  ge.add_vertex( myVertex );
  HepMC::set_signal_process_vertex(&ge, myVertex );
  ge.set_beam_particles(inParticle1,inParticle2);
}

void populateFilteredGenEvent(HepMC::GenEvent & ge, std::vector<HepMC::GenParticlePtr>& genPartList)
{
  //.......Create new particle (geantino) to link  hits from pileup
  HepMC::GenParticlePtr genPart=HepMC::newGenParticlePtr();
  genPart->set_pdg_id(999); //Geantino
  genPart->set_status(1); //!< set decay status
  HepMC::suggest_barcode(genPart, std::numeric_limits<int32_t>::max() );

  HepMC::GenVertexPtr genVertex=HepMC::newGenVertexPtr();
  genVertex->add_particle_out(genPart);
  genPartList.push_back(genPart);

  //to set geantino vertex as a truth primary vertex
  HepMC::GenVertexPtr hScatVx = HepMC::barcode_to_vertex(&ge,-3);
  if(hScatVx!=nullptr) {
    HepMC::FourVector pmvxpos=hScatVx->position();
    genVertex->set_position(pmvxpos);
    //to set geantino kinematic phi=eta=0, E=p=E_hard_scat
    HepMC::GenVertex::particles_in_const_iterator itrp =hScatVx->particles_in_const_begin();
    if (hScatVx->particles_in_size()==2){
      HepMC::FourVector mom1=(*itrp)->momentum();
      HepMC::FourVector mom2=(*(++itrp))->momentum();
      HepMC::FourVector vxmom;
      vxmom.setPx(mom1.e()+mom2.e());
      vxmom.setPy(0.);
      vxmom.setPz(0.);
      vxmom.setE(mom1.e()+mom2.e());

      genPart->set_momentum(vxmom);
    }
  }

  if(!ge.vertices_empty()){
    std::vector<HepMC::GenVertexPtr> vtxvec;
    HepMC::GenEvent::vertex_iterator itvtx = ge.vertices_begin();
    for (;itvtx != ge.vertices_end(); ++itvtx ) {
      ge.remove_vertex(*itvtx);
      vtxvec.push_back((*itvtx));
      //fix me: delete vertex pointer causes crash
      //delete (*itvtx);
    }
    for(unsigned int i=0;i<vtxvec.size();i++)  delete vtxvec[i];
  }

  //.....add new vertex with geantino
  ge.add_vertex(genVertex);
}

void createMcEventCollectionInStoreGate(std::vector<HepMC::GenParticlePtr>& genPartList)
{
  // create dummy input McEventCollection with a name that
  // HepMcParticleLink knows about
  SG::WriteHandle<McEventCollection> inputTestDataHandle{"TruthEvent"};
  inputTestDataHandle = std::make_unique<McEventCollection>();

  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext( SG::CurrentEventStore::store() ) );
  Gaudi::Hive::setCurrentContext( ctx );

  // Add a dummy GenEvent
  const int process_id1(20);
  const int event_number1(2147483647);
  // 2147483647 = 2^31 -1 is the largest supported event number by
  // HepMcParticleLink as the 32nd bit of the unsigned int used to
  // hold the eventIndex is used to flag whether the it represents the
  // position of the GenEvent in the McEventCollection or the
  // GenEvent::event_number.
  const int event_number2(std::numeric_limits<unsigned short>::max());
  // 2^16 -1 is the largest event number supported by
  // HepMcParticleLink_p2. A workaround is used to suppport larger
  // values for the first event in the McEventCollection.
  const int event_number3(64);
  const int event_number4(89);
  inputTestDataHandle->push_back(HepMC::newGenEvent(process_id1, event_number1));
  HepMC::GenEvent& ge1 = *(inputTestDataHandle->at(0));
  populateGenEvent(ge1,-11,11,genPartList);
  populateGenEvent(ge1,-13,13,genPartList);
  inputTestDataHandle->push_back(HepMC::newGenEvent(process_id1, event_number2));
  HepMC::GenEvent& ge2 = *(inputTestDataHandle->at(1));
  populateGenEvent(ge2,-11,11,genPartList);
  populateGenEvent(ge2,-13,13,genPartList);
  inputTestDataHandle->push_back(HepMC::newGenEvent(process_id1, event_number3));
  HepMC::GenEvent& ge3 = *(inputTestDataHandle->at(2));
  populateGenEvent(ge3,-11,11,genPartList);
  populateGenEvent(ge3,22,22,genPartList);
  inputTestDataHandle->push_back(new HepMC::GenEvent(ge1));
  HepMC::GenEvent& ge4 = *(inputTestDataHandle->at(3));
  ge4.set_event_number(event_number4);
  populateFilteredGenEvent(ge4,genPartList);
  return;
}

void testit (const HepMcParticleLink& trans1)
{
  MsgStream log (0, "test");
  HepMcParticleLinkCnv_p1 cnv;
  HepMcParticleLink_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  HepMcParticleLink trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}

void test1()
{
  std::cout << "test1\n";
  std::vector<HepMC::GenParticlePtr> genPartList;
  createMcEventCollectionInStoreGate(genPartList);
  assert ( genPartList.size() == 13 );
  // HepMcParticleLinks pointing at GenParticles in the first GenEvent in the McEventCollection
  // By event_number
  const HepMC::GenParticlePtr particle1 = genPartList.at(0);
  HepMcParticleLink trans1a(HepMC::barcode(particle1),particle1->parent_event()->event_number());
  testit (trans1a);
  // By position
  HepMcParticleLink trans1b(HepMC::barcode(particle1),0,EBC_MAINEVCOLL,HepMcParticleLink::IS_POSITION);
  testit (trans1b);
  // HepMcParticleLinks pointing at GenParticles in other GenEvents in the McEventCollection
  const HepMC::GenParticlePtr particle2 = genPartList.at(7);
  HepMcParticleLink trans2(HepMC::barcode(particle2),particle2->parent_event()->event_number());
  testit (trans2);
  const HepMC::GenParticlePtr particle3 = genPartList.at(8);
  HepMcParticleLink trans3(HepMC::barcode(particle3),particle3->parent_event()->event_number());
  testit (trans3);
  // HepMcParticleLinks pointing at filtered pileup truth
  const HepMC::GenParticlePtr particle4 = genPartList.at(12);
  HepMcParticleLink trans4(HepMC::barcode(particle4),particle4->parent_event()->event_number());
  testit (trans4);
  // HepMcParticleLinks pointing at delta-ray (barcode=0 - not recorded in McEventCollection) using event_number
  int deltaRayBarcode(0);
  HepMcParticleLink trans5a(deltaRayBarcode,particle1->parent_event()->event_number());
  testit (trans5a);
  // HepMcParticleLinks pointing at delta-ray (barcode=0 - not recorded in McEventCollection) using position
  HepMcParticleLink trans5b(deltaRayBarcode,0,EBC_MAINEVCOLL,HepMcParticleLink::IS_POSITION);
  testit (trans5b);
  // Link to a GenParticle which was not recorded to the
  // McEventCollection, even though other parts of the same GenEvent
  // were recorded.
  const HepMcParticleLink::index_type refEvtNum1 = static_cast<HepMcParticleLink::index_type>(particle1->parent_event()->event_number());
  const int cutBarcode(210001);
  HepMcParticleLink trans6a(cutBarcode, refEvtNum1);
  testit (trans6a);
  // Link to a GenEvent which was not recorded to the McEventCollection
  const int event_number5(460);
  const HepMcParticleLink::index_type refEvtNum5 = static_cast<HepMcParticleLink::index_type>(event_number5);
  HepMcParticleLink trans6b(cutBarcode, refEvtNum5);
  testit (trans6b);
}


int main()
{
  ISvcLocator* pSvcLoc = nullptr;
  if (!Athena_test::initGaudi(pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1();
  return 0;
}
