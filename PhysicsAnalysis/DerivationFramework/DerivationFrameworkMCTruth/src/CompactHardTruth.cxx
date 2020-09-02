///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompactHardTruth.cxx 
// Implementation file for class CompactHardTruth
// Author: Frank Paige <paige@bnl.gov>
/////////////////////////////////////////////////////////////////// 

// McParticleTests includes
#include "DerivationFrameworkMCTruth/CompactHardTruth.h"

// STL includes
#include <list>

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// mc tools
//#include "McParticleKernel/IMcVtxFilterTool.h"
//#include "McParticleKernel/ITruthParticleCnvTool.h"
//#include "McParticleEvent/TruthParticle.h"
//#include "McParticleEvent/TruthParticleContainer.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
// Needed for FourVector
#include "AtlasHepMC/SimpleVector.h"

//#include "McParticleUtils/McVtxFilter.h"

//#include "TruthHelper/IsGenStable.h"
//#include "TruthHelper/IsGenNonInteracting.h"
//#include "TruthHelper/IsGenerator.h"

//#include "EventKernel/PdtPdg.h"

// ROOT includes
#include "TH1F.h"

namespace DerivationFramework {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
CompactHardTruth::CompactHardTruth( const std::string& name, 
                    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_mcEventsName ("GEN_AOD"),
  m_thinnedMcEventsName ("GEN_AODTHIN"),
  m_partonCut(10000.),
  m_hardCut(10000.),
  m_danglePtCut(0.),
  m_maxCount(0)
{
  //
  // Property declaration
  // 
  declareProperty("McEvent",
                  m_mcEventsName,
                  "input McEventCollection container name");
  declareProperty("McEventOut",
                  m_thinnedMcEventsName,
                  "output McEventCollection container name");
  declareProperty("ShowerMassCut",
                  m_partonCut,
                  "mass cut for thinning parton shower");
  declareProperty("SoftMtCut",
                  m_hardCut,
                  "mt cut for underlying event showers");
  declareProperty("DanglePtCut",
                   m_danglePtCut,
                   "maximum pt for dangling partons");

  declareProperty("MaxCount",
                  m_maxCount,
                  "maximum number of events to print");
}

// Destructor
///////////////
CompactHardTruth::~CompactHardTruth()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode CompactHardTruth::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  m_evtCount = -1;
  m_missCount = 0;
  m_dangleFound = 0;
  m_dangleRemoved = 0;
  m_danglePtMax = 0;

  m_thinParticles = 0;
  m_thinVertices = 0;

  // Print jobOption inputs
  ATH_MSG_INFO ("-------------------------------------------------" );
  ATH_MSG_INFO ("jobOption McEvent            " <<m_mcEventsName );
  ATH_MSG_INFO ("jobOption McEventOut         " <<m_thinnedMcEventsName );
  ATH_MSG_INFO ("jobOption ShowerMassCut      " <<m_partonCut );
  ATH_MSG_INFO ("jobOption SoftMtCut          " <<m_hardCut );
  ATH_MSG_INFO ("jobOption MaxCount           " <<m_maxCount );
  ATH_MSG_INFO ("-------------------------------------------------" );

  return StatusCode::SUCCESS;
}


StatusCode CompactHardTruth::finalize()
{

  ATH_MSG_INFO("Finalizing DerivationFramework::CompactHardTruth ");
  ATH_MSG_INFO("Missing items limiting reclustering " <<m_missCount);

  ATH_MSG_INFO("Dangling partons pt cut:  " <<m_danglePtCut);
  ATH_MSG_INFO("Dangling partons found:   " <<m_dangleFound);
  ATH_MSG_INFO("Dangling partons removed: " <<m_dangleRemoved);
  ATH_MSG_INFO("Dangling partons max pt:  " <<m_danglePtMax);

  ATH_MSG_INFO("CompactHardTruth total particles:  " <<m_thinParticles);
  ATH_MSG_INFO("CompactHardTruth total vertices:   " <<m_thinVertices);

  return StatusCode::SUCCESS;
}


StatusCode CompactHardTruth::execute() {  

  ++m_evtCount;
  //if( m_evtCount%100 == 0 ){
    ATH_MSG_INFO("Executing " <<name() <<" " <<m_evtCount);
  //}

  // Normally doPrint is used to print the first m_maxCount events
  // before and after thinning. 
  // doExtra adds extra intermediate event printouts.
  // doDebug allows debug printout for a range of events.
  bool doPrint = m_evtCount < m_maxCount;
  bool doDebug = false;
  bool doExtra = false;
  //doDebug = doPrint;
  //doExtra = doPrint;

  // Retrieve input data
  const McEventCollection* mcEvts = 0;
  if (!evtStore()->retrieve(mcEvts, m_mcEventsName).isSuccess() ||
      0 == mcEvts) {
    ATH_MSG_WARNING("could not retrieve mc collection at ["
                 << m_mcEventsName << "]!");
    return StatusCode::FAILURE;
  }

  if (mcEvts->empty()) {
    ATH_MSG_WARNING("empty McEventCollection at [" 
                    << m_mcEventsName << "]");
    return StatusCode::SUCCESS;
  }

  // Create output collection
  McEventCollection* thinnedMcEvts = new McEventCollection;
  if (!evtStore()->record(thinnedMcEvts, m_thinnedMcEventsName).isSuccess()){
    ATH_MSG_WARNING("Could not record thinned mc collection at ["
                 << m_thinnedMcEventsName << "]!");
    delete thinnedMcEvts; 
    thinnedMcEvts = 0;
    return StatusCode::FAILURE;
  }
  if (evtStore()->setConst(thinnedMcEvts).isFailure() ) {
    ATH_MSG_WARNING("Could not lock the McEventCollection at ["
                    << m_thinnedMcEventsName << "] !!");
  }

  // Signal event is first (only?) event; front() is from DataVector
  const HepMC::GenEvent* mcEvt = mcEvts->front();
  auto wtCont = mcEvt->weights();
  //double wt = 1;
  if( wtCont.size() != 0 ){
    //wt = wtCont[0];
  } else {
    ATH_MSG_WARNING("Weights not found for mc collection [" 
                    <<m_mcEventsName <<"]");
  }
  int inEvent = mcEvt->event_number();
  if( doDebug ) ATH_MSG_DEBUG("FETCHED count/event " <<m_evtCount <<" " 
                              <<inEvent);


  ///////////////////////////////
  // New event - copy of original
  ///////////////////////////////

  HepMC::GenEvent* thinEvt = new HepMC::GenEvent(*mcEvt);
  int nEvent = thinEvt->event_number();
  if( doPrint ) ATH_MSG_DEBUG("New event number = " <<nEvent);

  if( doPrint ){
    std::cout <<"========== BEGIN EVENT BEFORE THINNING ==========" <<std::endl;
    thinEvt->print();
    std::cout <<"========== END EVENT BEFORE THINNING ==========" <<std::endl;
  }


  /////////////////////////////////////////////////
  // Containers for manipulating particles/vertices
  /////////////////////////////////////////////////

  // Hadronization vertex must have at least two incoming partons to
  // conserve color. All outgoing must be hadrons; c.f. Pythia 2->1
  // color reconnection vertices.
  // Const iterators => must(?) save changes, then do them.
  // Always remove particles from vertices, then delete.
  // removePV:  remove particle from vertex
  // addinPV:   add particle in to vertex
  // addoutPV:  add particle out from vertex
  // removeV:   remove vertex from event
  // deleteP:   delete particle after all passes
  // deleteV:   delete vertex after all passes
  // HepMC ~GenVertex deletes particles, so remove them from ALL vertices

  typedef std::pair<HepMC::GenVertex*,HepMC::GenParticle*> vpPair;
  std::vector<vpPair> removePV;
  std::vector<vpPair> addinPV;
  std::vector<vpPair> addoutPV;
  std::vector<HepMC::GenVertex*> removeV;

  // Use list with sort/unique to insure no multiple deletion
  std::list<HepMC::GenParticle*> deleteP;
  std::list<HepMC::GenVertex*> deleteV;
  std::list<HepMC::GenParticle*>::iterator dpItr;
  std::list<HepMC::GenParticle*>::iterator dpItrE;
  std::list<HepMC::GenVertex*>::iterator dvItr;
  std::list<HepMC::GenVertex*>::iterator dvItrE;


  //////////////////////////////
  // Find hadronization vertices
  //////////////////////////////

  if( doDebug ) ATH_MSG_DEBUG("Find hadronization vertices");

  std::vector<HepMC::GenVertex*> hadVertices;

  HepMC::GenEvent::vertex_iterator hadv = thinEvt->vertices_begin();
  HepMC::GenEvent::vertex_iterator hadvB = thinEvt->vertices_begin();
  HepMC::GenEvent::vertex_iterator hadvE = thinEvt->vertices_end();

  for(; hadv!=hadvE; ++hadv){
    if( !(*hadv) ) continue;
    if( (*hadv)->particles_in_size() < 2 ) continue;
    if( (*hadv)->particles_out_size() < 1 ) continue;

    // Check hadronization vertex
    // isHad is true if at least one hadron
    // q qbar -> pi is allowed, but q qbar -> W... is not
    bool isHadVtx = true;
    bool isHadOut = false;
    HepMC::GenVertex::particles_in_const_iterator inp = 
      (*hadv)->particles_in_const_begin();
    HepMC::GenVertex::particles_in_const_iterator inpE = 
      (*hadv)->particles_in_const_end();
    for(; inp!=inpE; ++inp){
      if( !isParton(*inp) ) isHadVtx = false;
    }
        HepMC::GenVertex::particles_out_const_iterator vp = 
      (*hadv)->particles_out_const_begin();
    HepMC::GenVertex::particles_out_const_iterator vpE = 
      (*hadv)->particles_out_const_end();
    for(; vp!=vpE; ++vp){
      if( isParton(*vp) ) isHadVtx = false;      
      if( isHadron(*vp) ) isHadOut = true;
    }
    isHadVtx = isHadVtx && isHadOut;
    if( isHadVtx ) hadVertices.push_back(*hadv);
    if( doDebug && isHadVtx ) ATH_MSG_VERBOSE("Hadronization vertex "
                                              <<(*hadv)->barcode());
  }

  if( hadVertices.size() < 1 ){
    ATH_MSG_WARNING("No hadronization vertices for event " <<nEvent);
    ATH_MSG_WARNING("Exiting without changing event.");
    thinnedMcEvts->push_back(thinEvt);
    return StatusCode::SUCCESS;
  }


  //////////////////////////////////////////////////////////
  // Remove all incoming partons from hadronization vertices
  // Remove and delete all descendants
  //////////////////////////////////////////////////////////

  for(unsigned int iv=0; iv<hadVertices.size(); ++iv){
    HepMC::GenVertex* ivtx = hadVertices[iv];
    if( doDebug ) ATH_MSG_DEBUG("Removing partons from hadVertex "
                                <<ivtx->barcode());
    HepMC::GenVertex::particles_in_const_iterator pin =
     ivtx->particles_in_const_begin();
    HepMC::GenVertex::particles_in_const_iterator pinE =
     ivtx->particles_in_const_end();
    for(; pin!=pinE; ++pin){
      removePV.push_back(vpPair(ivtx,*pin));
    }
  }

  // Remove all descendant particles. Will remove empty vertices later.
  // Might have parton decays of hadrons - hence delete sort/unique
  for(unsigned int iv=0; iv<hadVertices.size(); ++iv){
    HepMC::GenVertex* ivtx = hadVertices[iv];
    HepMC::GenVertex::particle_iterator pout =
     ivtx->particles_begin(HepMC::descendants);
    HepMC::GenVertex::particle_iterator poutE =
     ivtx->particles_end(HepMC::descendants);
    for(; pout!=poutE; ++pout){
      HepMC::GenVertex* vpar = (*pout)->production_vertex();
      if( vpar ) removePV.push_back(vpPair(vpar,*pout));
      HepMC::GenVertex* vend = (*pout)->end_vertex();
      if( vend ) removePV.push_back(vpPair(vend,*pout));
      deleteP.push_back(*pout);
    }
  }

  // Remove empty vertices
  // Remove all particles from Geant vertices
  // Remove and delete Geant vertices and particles
  // All Geant particles should have Geant parent vertex

  static const int cutG4 = 200000;

  for(hadv=hadvB; hadv!=hadvE; ++hadv){

    // Empth vertices
    if( (*hadv)->particles_in_size()==0 && (*hadv)->particles_out_size()==0 ){
      removeV.push_back(*hadv);
      deleteV.push_back(*hadv);
    }

    // Geant vertices/particles
    if( (*hadv)->barcode() > -cutG4 ) continue;
    HepMC::GenVertex::particles_in_const_iterator pin =
     (*hadv)->particles_in_const_begin();
    HepMC::GenVertex::particles_in_const_iterator pinE =
     (*hadv)->particles_in_const_end();
    for(; pin!=pinE; ++pin){
      removePV.push_back(vpPair(*hadv,*pin));
      if( (*pin)->barcode() > cutG4 ){
        deleteP.push_back(*pin);
      }
    }
    HepMC::GenVertex::particles_out_const_iterator pout =
     (*hadv)->particles_out_const_begin();
    HepMC::GenVertex::particles_out_const_iterator poutE =
     (*hadv)->particles_out_const_end();
    for(; pout!=poutE; ++pout){
      removePV.push_back(vpPair(*hadv,*pout));
      if( (*pout)->barcode() > cutG4 ){
        deleteP.push_back(*pout);
      }
    }
    removeV.push_back(*hadv);
    deleteV.push_back(*hadv);
  }

  // Actually implement changes

  for(unsigned int i=0; i<removePV.size(); ++i){
    HepMC::GenVertex* v = removePV[i].first;
    HepMC::GenParticle* p = removePV[i].second;
    v->remove_particle(p);
  }

  for(unsigned int i=0; i<addoutPV.size(); ++i){
    HepMC::GenVertex* v = addoutPV[i].first;
    HepMC::GenParticle* p = addoutPV[i].second;
    v->add_particle_out(p);
  }

  for(unsigned int iv=1; iv<hadVertices.size(); ++iv){
    HepMC::GenVertex* v = hadVertices[iv];
    if( v->particles_in_size()!=0 || v->particles_out_size()!=0 ){
      ATH_MSG_WARNING("Removing vertex " <<v->barcode() 
      <<" for event " <<nEvent <<" with in/out particles " 
      <<v->particles_in_size() <<" " <<v->particles_out_size() );
    }
    if( !thinEvt->remove_vertex(hadVertices[iv]) ){
      ATH_MSG_WARNING("Error removing vertex " <<v->barcode()
      <<" for event " <<nEvent);
    }
  }

  // Delete removed particles/vertices

  if( doDebug ) ATH_MSG_DEBUG("Deleting hadronization vertices " 
                              <<deleteV.size());
  deleteV.sort();
  deleteV.unique();
  for(dvItr=deleteV.begin(); dvItr!=deleteV.end(); ++dvItr){
    if( doDebug ) ATH_MSG_VERBOSE("Deleting vertex " <<(*dvItr)->barcode());
    if( *dvItr) delete (*dvItr);
  }

  deleteP.sort();
  deleteP.unique();
  for(dpItr=deleteP.begin(); dpItr!=deleteP.end(); ++dpItr){
    if( doDebug ) ATH_MSG_VERBOSE("Deleting particle " <<(*dpItr)->barcode());
    if( *dpItr ) delete (*dpItr);
  }


  ////////////////////////
  // Cluster final partons
  ////////////////////////

  if( doDebug && doExtra ){
    std::cout <<"========== BEGIN EVENT BEFORE CLUSTER ==========" <<std::endl;
    thinEvt->print();
    std::cout <<"========== END EVENT BEFORE CLUSTER ==========" <<std::endl;
  }

  // Possible cases:
  // 1->1 branching:  Drop earlier 1 and vertex.
  // 2->1 connection: Drop 1 and vertex; leave 2 free
  //                  Note momentum is conserved for these.
  //                  Do not split 2->1 from initial 2->1->2 in Herwig.
  // 1->2 branching:  Set p1 = p2a+p2b, drop 2 and vertex.
  //                  Add 1 to first vertex.

  // Preserving color connections required merging vertices. Now no
  // longer needed.

  bool moreP = true;
  typedef std::pair<HepMC::GenVertex*,HepMC::GenParticle*> vpPair;
  //std::vector<vpPair> removePV;
  //std::vector<vpPair> addinPV;
  //std::vector<vpPair> addoutPV;
  //std::vector<HepMC::GenVertex*> removeV;
  //std::list<HepMC::GenParticle*> deleteP;
  //std::list<HepMC::GenVertex*> deleteV;
  removePV.clear();
  addinPV.clear();
  addoutPV.clear();
  removeV.clear();
  deleteP.clear();
  deleteV.clear();

  typedef std::pair<HepMC::GenParticle*,HepMC::FourVector> pkPair;
  std::vector<pkPair> changePK;

  if( doDebug ) ATH_MSG_DEBUG("Start parton thinning");
  while( moreP ){
    if( doDebug ) ATH_MSG_DEBUG("New parton pass " <<inEvent <<" "
                                <<thinEvt->particles_size() <<" " 
                                <<thinEvt->vertices_size());

    moreP = false;
    removePV.clear();
    addinPV.clear();
    addoutPV.clear();
    removeV.clear();
    changePK.clear();

    HepMC::GenEvent::particle_iterator finp = thinEvt->particles_begin();
    HepMC::GenEvent::particle_iterator finpB = thinEvt->particles_begin();
    HepMC::GenEvent::particle_iterator finpE = thinEvt->particles_end();

    // Find final partons
    for(finp=finpB; finp!=finpE; ++finp){
      int iCase = 0;

      HepMC::GenParticle* fp = *finp;
      if( !isFinalParton(fp) ) continue;
      if( doDebug ) ATH_MSG_DEBUG("Starting final parton " <<fp->barcode());

      // Production/end vertices
      HepMC::GenVertex* pvtx = fp->production_vertex();
      if( !pvtx ) {
        ATH_MSG_WARNING("Missing production for final parton "
                        <<fp->barcode());
        continue;
      }
      if( doDebug ) ATH_MSG_DEBUG("Final parton " <<pvtx->barcode() <<" "
                                  <<fp->barcode());

      ////////////
      // Case 1->1
      ////////////

      // One-particle decay; use final particle
      // ppvtx -> pp -> pvtx -> fp

      if( pvtx->particles_in_size()==1 && pvtx->particles_out_size()==1 ){
        // Incoming particle to parent vertex
        HepMC::GenVertex::particles_in_const_iterator pitr =
          pvtx->particles_in_const_begin();
        HepMC::GenParticle* pp = *pitr;
        if( !pp || pp->barcode()==0 ){
          ATH_MSG_DEBUG("1->1: missing pp for fp " <<fp->barcode());
          ++m_missCount;
          continue;
        }
        // Its parent vertex
        HepMC::GenVertex* ppvtx = pp->production_vertex();        
        if( !ppvtx || ppvtx->barcode()==0 ){
          ATH_MSG_DEBUG("1->1: missing ppvtx for fp " <<fp->barcode());
          ++m_missCount;
           continue;
        }
        moreP = true;
        iCase = 1;

        removePV.push_back(vpPair(ppvtx,pp));
        removePV.push_back(vpPair(pvtx,pp));
        deleteP.push_back(pp);
        removeV.push_back(pvtx);
        deleteV.push_back(pvtx);
        addoutPV.push_back(vpPair(ppvtx,fp));
        if( doDebug ){
          ATH_MSG_DEBUG("1->1: ppvtx,pp,pvtx,fp,evtx "
                        <<ppvtx->barcode() <<" " <<pp->barcode() <<" "
                        <<pvtx->barcode() <<" " <<fp->barcode() );
        }
      }

      ////////////
      // Case 2->1
      ////////////

      // Color recombination. Momentum is conserved so just keep 2.
      // Drop 1 and vertex. 
      // ppvtx1,ppvtx2 -> pp1,pp2 -> pvtx -> fp
      // Recombination should not affect hard physics!

      if( pvtx->particles_in_size()==2 && pvtx->particles_out_size()==1 ){
        // Incoming particles to parent vertex
        HepMC::GenVertex::particles_in_const_iterator pitr =
          pvtx->particles_in_const_begin();
        HepMC::GenParticle* pp1 = *pitr;
        ++pitr;
        HepMC::GenParticle* pp2 = *pitr;

        // Check for 2->1->2 initial state interactions in Herwig++
        // Initial partons have pt=0, use pt<0.001MeV
        if( fabs(pp1->momentum().perp())<1.e-3 ) continue;
        if( fabs(pp2->momentum().perp())<1.e-3 ) continue;
        // Their parent vertices
        HepMC::GenVertex* ppvtx1 = pp1->production_vertex();        
        HepMC::GenVertex* ppvtx2 = pp2->production_vertex();        
        if( !ppvtx1 || ppvtx1->barcode()==0 ){
          ATH_MSG_DEBUG("2->1: missing ppvtx1 for fp " <<fp->barcode());
          ++m_missCount;
          continue;
        }
       if( !ppvtx2 || ppvtx2->barcode()==0 ){
          ATH_MSG_DEBUG("2->1: missing ppvtx2 for fp " <<fp->barcode());
          ++m_missCount;
          continue;
        }
 
        moreP = true;
        iCase = 2;

        removePV.push_back(vpPair(pvtx,fp));
        removePV.push_back(vpPair(pvtx,pp1));
        removePV.push_back(vpPair(pvtx,pp2));
        deleteP.push_back(fp);
        removeV.push_back(pvtx);
        deleteV.push_back(pvtx);

        if( doDebug ){
          ATH_MSG_DEBUG("2->1: ppvtx1,pp1,ppvtx2,pp2,pvtx,fp "
                       <<ppvtx1->barcode() <<" " <<pp1->barcode() <<" "
                       <<ppvtx2->barcode() <<" " <<pp2->barcode() <<" "
                       <<pvtx->barcode() <<" " <<fp->barcode());
        }
      }

      ////////////
      // Case 1->2
      ////////////

      // Parton branching. Momentum not conserved; 2 momenta correct
      // Drop only if mass is below cut
      // ppvtx -> pp -> pvtx -> pout1,pout2/fp

      if( pvtx->particles_in_size()==1 && pvtx->particles_out_size()==2 ){
        HepMC::GenVertex::particles_out_const_iterator poutitr =
          pvtx->particles_out_const_begin();
        HepMC::GenParticle* pout1 = *poutitr;
        ++poutitr;
        HepMC::GenParticle* pout2 = *poutitr;

        // Require two final partons and avoid duplication
        if( fp == pout1 ) {
          if( !isFinalParton(pout2) ){
            if( doDebug ) ATH_MSG_DEBUG("1->2: not final " 
                                        <<pout2->barcode());
            continue;
          }
        } else if( fp == pout2 ) {
          if( !isFinalParton(pout1) ){
            if( doDebug ) ATH_MSG_DEBUG("1->2: not final " <<pout1->barcode());
            continue;
          }
        } else {
          ATH_MSG_WARNING("1->2: No match found for branching "
                          <<fp->barcode() <<" " <<pvtx->barcode() <<" "
                          <<pout1->barcode() <<" " <<pout2->barcode());
          continue;
        }
        if( fp != pout1 ) continue;
        // Incoming particle 
        HepMC::GenVertex::particles_in_const_iterator pitr =
          pvtx->particles_in_const_begin();
        HepMC::GenParticle* pp = *pitr;

        // Do not merge initial partons (pt<1MeV or m<-1MeV)
        if(pout1->momentum().m()<-1.0 || pout1->momentum().perp()<1.0) continue;
        if(pout2->momentum().m()<-1.0 || pout2->momentum().perp()<1.0) continue;

        // Parton pair mass cut
        HepMC::FourVector p12 = vtxOutMom(pvtx);
        double m12 = p12.m();
        if( m12<0 ){
          if( fabs(m12) > 10. + 1.0e-5*p12.e() ){
            ATH_MSG_WARNING("Spacelike mass^2 for parton sum " <<m12 <<" "
                            <<pp->barcode() <<" " <<pvtx->barcode() <<" "
                            <<pout1->barcode() <<" " <<pout2->barcode());
          }
          m12 = 0;
        }
        if( doDebug ) ATH_MSG_DEBUG("1->2: parton pair mass " <<m12);
        // If mass > cut, keep pair
        if( m12 > m_partonCut ) {
          if( doDebug ) ATH_MSG_DEBUG("Keeping 1->2: parton mass " <<m12);
          continue;
        }

        // Associated vertices
        HepMC::GenVertex* ppvtx = pp->production_vertex();
        if( !ppvtx || ppvtx->barcode() == 0 ){
          ATH_MSG_DEBUG("1->2: missing ppvtx for fp " <<fp->barcode());
          ++m_missCount;
          continue;
        }

        // Merge branching
        moreP = true;
        iCase = 3;
        if( doDebug ) ATH_MSG_DEBUG("Merging 1->2: mass " <<p12.m());

        changePK.push_back(pkPair(pp,p12));
        removePV.push_back(vpPair(pvtx,pp));
        removePV.push_back(vpPair(pvtx,pout1));
        removePV.push_back(vpPair(pvtx,pout2));

        deleteP.push_back(pout1);
        deleteP.push_back(pout2);
        removeV.push_back(pvtx);
        deleteV.push_back(pvtx);

        if( doDebug ){
          ATH_MSG_DEBUG("Merge 1->2: ppvtx,pp,pvtx,pout1,pout2,evtx "
                       <<ppvtx->barcode() <<" " <<pp->barcode() <<" "
                       <<pvtx->barcode() <<" " <<pout1->barcode() <<" "
                       <<pout2->barcode());
          ATH_MSG_DEBUG("Merge 1->2: id " <<pp->pdg_id() <<" "
                       <<pout1->pdg_id() <<" " <<pout2->pdg_id());
        }
      } //end 1->2 case


      /////////////////////////
      // Incoming proton vertex
      /////////////////////////

      // Do nothing
      if( pvtx->particles_in_size()==1 ){
        // Incoming particle to parent vertex
        HepMC::GenVertex::particles_in_const_iterator pitr =
          pvtx->particles_in_const_begin();
        HepMC::GenParticle* pp = *pitr;
        if( abs(pp->pdg_id()) == 2212 ) iCase = -1;
      }

      // Case not found
      // Need test for 2->2 in underlying event
      if( iCase==0 ){
        if( doDebug ) ATH_MSG_DEBUG("Case not found " <<pvtx->barcode() <<" " 
                                   <<fp->barcode() <<" " 
                                   <<pvtx->particles_in_size() <<" " 
                                   <<pvtx->particles_out_size() );
      }

    } //end final parton loop

    // Actually implement changes -- remove particles from vertices
    // Parton ends free, so no addinPV
    if( doDebug ) ATH_MSG_DEBUG("Actually removing particles " 
                                <<removePV.size());

    for(unsigned int i=0; i<removePV.size(); ++i){
      HepMC::GenVertex* v = removePV[i].first;
      HepMC::GenParticle* p = removePV[i].second;
      if( doDebug ) ATH_MSG_VERBOSE("Removing v,p " <<v->barcode() <<" " 
                                    <<p->barcode());
      v->remove_particle(p);
    }

    // Actually implement changes -- add particles to vertices
    if( doDebug ) ATH_MSG_DEBUG("Actually add particles in/out " 
                  <<addinPV.size() <<" " <<addoutPV.size() );
    for(unsigned int i=0; i<addoutPV.size(); ++i){
      HepMC::GenVertex* v = addoutPV[i].first;
      HepMC::GenParticle* p = addoutPV[i].second;
      if( doDebug ) ATH_MSG_VERBOSE("Adding v,p " <<v->barcode() <<" " 
                                    <<p->barcode());
      v->add_particle_out(p);
    }

    // Actually implement changes -- change momenta
    for(unsigned int i=0; i<changePK.size(); ++i){
      HepMC::GenParticle* pp = changePK[i].first;
      //!float eold = pp->momentum().e();
      pp->set_momentum(changePK[i].second);
    }

    // Actually implement changes -- remove vertices
    if( doDebug ) ATH_MSG_DEBUG("Actually remove vertices " <<removeV.size());
    for(unsigned int i=0; i<removeV.size(); ++i){
      int nv = thinEvt->vertices_size();
      if( thinEvt->remove_vertex(removeV[i]) ){
        if( doDebug ){
          ATH_MSG_VERBOSE("Removed vertex " <<removeV[i]->barcode() <<" "
                          <<nv <<" " <<thinEvt->vertices_size());
        }
      } else {
        ATH_MSG_WARNING("Failed to remove vertex " <<removeV[i]->barcode());
      }
    }
    if( doDebug ) ATH_MSG_DEBUG("End while(moreP) pass " <<moreP);

  } //end moreP

  // Delete removed particles/vertices
  if( doDebug ) ATH_MSG_DEBUG("Deleting vertices " <<deleteV.size());
  deleteV.sort();
  deleteV.unique();
  for(dvItr=deleteV.begin(); dvItr!=deleteV.end(); ++dvItr){
    if( doDebug ) ATH_MSG_VERBOSE("Deleting vertex " <<(*dvItr)->barcode());
    if( *dvItr) delete (*dvItr);
  }

  if( doDebug ) ATH_MSG_DEBUG("Deleting particles " <<deleteP.size());
  deleteP.sort();
  deleteP.unique();
  for(dpItr=deleteP.begin(); dpItr!=deleteP.end(); ++dpItr){
    if( doDebug ) ATH_MSG_VERBOSE("Deleting particle " <<(*dpItr)->barcode());
    if( *dpItr ) delete (*dpItr);
  }


  //////////////////////////////
  // Strip soft underlying stuff
  //////////////////////////////

  if( doDebug && doExtra ){
    std::cout <<"========== BEGIN EVENT BEFORE SOFT ==========" <<std::endl;
    thinEvt->print();
    std::cout <<"========== END EVENT BEFORE SOFT ==========" <<std::endl;
  }

  HepMC::GenParticle* beams[2];
  beams[0] = thinEvt->beam_particles().first;
  beams[1] = thinEvt->beam_particles().second;
  
  // Have deleted all hadronization particles
  // Find all particles connected to hard process(es) with m_T>10GeV
  std::list<HepMC::GenParticle*> pNotHad;
  std::list<HepMC::GenParticle*> pHard;

  HepMC::GenEvent::particle_iterator finp = thinEvt->particles_begin();
  HepMC::GenEvent::particle_iterator finpE = thinEvt->particles_end();

  for(; finp!=finpE; ++finp){ 
    HepMC::GenParticle* fp = *finp; 
    HepMC::GenVertex* pvtx = fp->production_vertex(); 
    if( !pvtx ) continue;

    double ep = fp->momentum().e();
    double pzp = fp->momentum().pz();
    double mtmax = (ep+pzp)*(ep-pzp);
    HepMC::GenVertex::particle_iterator gpar =
     fp->production_vertex()->particles_begin(HepMC::ancestors);
    HepMC::GenVertex::particle_iterator gparB = gpar;
    HepMC::GenVertex::particle_iterator gparE =
     fp->production_vertex()->particles_end(HepMC::ancestors);

    for(; gpar!=gparE; ++gpar){
      double e = (*gpar)->momentum().e();
      double pz = (*gpar)->momentum().pz();
      double mt = (e+pz)*(e-pz);
      if( mt > mtmax ) mtmax = mt;
    }

    // Keep hard particles and all ancestors
    pNotHad.push_back(fp);
    int ida = abs(fp->pdg_id());
    bool keepid = (ida>10 && ida<20) || (ida>1000000 && ida<9000000);
    if( mtmax > m_hardCut*m_hardCut || keepid ){
      pHard.push_back(fp);
      for(gpar=gparB; gpar!=gparE; ++gpar) pHard.push_back(*gpar);
    }

    // Also keep all descendants of interesting particles
    // Include leptons to get photons in Sherpa with no Z parent
    // All hard descendants would include soft initial radiation
    // Will remove duplicates with list sort/unique
    bool keepid2 = ida==6 || (ida>=11 && ida<=16) || (ida>=23 && ida<=37) ||
                   (ida>1000000 && ida<9000000);
    if( keepid2 && fp->end_vertex() ){
      HepMC::GenVertex::particle_iterator des =
       fp->end_vertex()->particles_begin(HepMC::descendants);
       HepMC::GenVertex::particle_iterator desE =
        fp->end_vertex()->particles_end(HepMC::descendants);
       for(; des!=desE; ++des) pHard.push_back(*des);
    }
  }

  // Sort/unique lists
  pNotHad.sort();
  pNotHad.unique();
  pHard.sort();
  pHard.unique();

  // Dump information
  if( doDebug ){
    std::list<HepMC::GenParticle*>::iterator hItr2 = pHard.begin();
    std::list<HepMC::GenParticle*>::iterator hItr2E = pHard.end();
    int nhard = 0;
    for(; hItr2!=hItr2E; ++hItr2){
      ++nhard;
      ATH_MSG_DEBUG("Hard GenParticles " <<(*hItr2)->barcode() <<" " 
                    <<(*hItr2)->pdg_id() <<" "
                    <<(*hItr2)->momentum().perp()/1000. <<" "
                    <<(*hItr2)->momentum().pz()/1000. );
    }
    if( doDebug ) ATH_MSG_DEBUG("Hard GenParticles total " <<nhard);
  }

  // Remove non-hadronization, non-hard GenParticles from vertices
  // and delete them using lists constructed above.
  // Any 1->1 vertices created will be removed below.

  std::list<HepMC::GenParticle*>::iterator pItr = pNotHad.begin();
  std::list<HepMC::GenParticle*>::iterator pItrE = pNotHad.end();

  std::list<HepMC::GenParticle*>::iterator hItr = pHard.begin();
  std::list<HepMC::GenParticle*>::iterator hItrB = pHard.begin();
  std::list<HepMC::GenParticle*>::iterator hItrE = pHard.end();

  for(; pItr!=pItrE; ++pItr){
    HepMC::GenParticle* p = *pItr;

    // Skip hard ones
    bool isHard = false;
    for(hItr=hItrB; hItr!=hItrE; ++hItr){
      if( p == (*hItr) ){
        isHard = true;
        break;
      }
    }
    if( doDebug ) ATH_MSG_DEBUG("Particle bc/isHard " <<p->barcode() <<" "
                                <<isHard);
    if( isHard ) continue;
    HepMC::GenVertex* pvtx = p->production_vertex();
    if( pvtx ) pvtx->remove_particle(p);
    HepMC::GenVertex* evtx = p->end_vertex();
    if( evtx ) evtx->remove_particle(p);
    delete p;
  }


  /////////////////////////////////////////////////////////
  // Remove and delete vertices with no remaining particles
  /////////////////////////////////////////////////////////

  removeV.clear();
  deleteV.clear();

  HepMC::GenEvent::vertex_iterator vtx = thinEvt->vertices_begin();
  HepMC::GenEvent::vertex_iterator vtxE = thinEvt->vertices_end();
  for(; vtx!=vtxE; ++vtx){
    if( (*vtx)->particles_in_size() != 0 ) continue;
    if( (*vtx)->particles_out_size() != 0 ) continue;
    removeV.push_back(*vtx);
    deleteV.push_back(*vtx);
  }

  if( doDebug ) ATH_MSG_DEBUG("Removing/deleting 0-particle vertices " 
                              <<removeV.size()  <<" " <<deleteV.size() );
  for(unsigned int i=0; i<removeV.size(); ++i){
    if( thinEvt->remove_vertex(removeV[i]) ){
      if( doDebug ) ATH_MSG_VERBOSE("Removed vertex " <<removeV[i]->barcode());
    } else {
      ATH_MSG_WARNING("Failed to remove vertex " <<removeV[i]->barcode());
    }
  }

  deleteV.sort();
  deleteV.unique();
  for(dvItr=deleteV.begin(); dvItr!=deleteV.end(); ++dvItr){
    if( doDebug ) ATH_MSG_VERBOSE("Deleting vertex " <<(*dvItr)->barcode());
    if( *dvItr) delete (*dvItr);
  }


  ////////////////////////////////
  // Remove remaining 1-1 vertices
  ////////////////////////////////

  if( doDebug && doExtra ){
    std::cout <<"========== BEGIN EVENT BEFORE 1-BODY ==========" <<std::endl;
    thinEvt->print();
    std::cout <<"========== END EVENT BEFORE 1-BODY ==========" <<std::endl;
  }

  // Not clear how to order sweep, so do it one at a time.
  // Always keep child
  // pvtx -> pin -> vtx11 -> pout -> evtx

  bool moreV1 = true;
  HepMC::GenVertex* vtx11;
  HepMC::GenParticle* pin;
  HepMC::GenParticle* pout;

  while( moreV1 ){
    moreV1 = false;
    
    HepMC::GenEvent::vertex_iterator v = thinEvt->vertices_begin();
    HepMC::GenEvent::vertex_iterator vE = thinEvt->vertices_end();

    // Find next 1->1 vertex
    for(; v!=vE; ++v){
      if( (*v)->particles_in_size() != 1 ) continue;
      if( (*v)->particles_out_size() != 1 ) continue;
      pin = *((*v)->particles_in_const_begin());
      pout = *((*v)->particles_out_const_begin());
      if( pin->pdg_id() != pout->pdg_id() ) continue;
      // Sherpa does 1-body decay of incoming protons :-(
      if( pin==beams[0] || pin==beams[1] ) continue;
      HepMC::GenVertex* pvtx = pin->production_vertex();
      if( !pvtx || pvtx->barcode()==0 ){
        ATH_MSG_DEBUG("1->1: missing pvtx for vertex " <<(*v)->barcode());
        ++m_missCount;
        continue;
      }

      moreV1 = true;
      vtx11 = (*v);
      if( doDebug ) ATH_MSG_DEBUG("One-body " <<pin->barcode() <<" " 
                                  <<vtx11->barcode()
                                  <<" " <<pout->barcode());
      break;
    }
    if( moreV1 ){
      HepMC::GenVertex* pvtx = pin->production_vertex();
      pvtx->remove_particle(pin);
      pvtx->add_particle_out(pout);
      vtx11->remove_particle(pin);
      vtx11->remove_particle(pout);
      thinEvt->remove_vertex(vtx11);
      delete pin;
      delete vtx11;
      if( doDebug ) ATH_MSG_DEBUG("One-body new pvtx " <<pvtx->barcode() <<" "
                                  <<pvtx->particles_in_size() <<" "
                                  <<pvtx->particles_out_size() );
    }
  }


  /////////////////////////////////////
  // Remove dangling particles/vertices
  /////////////////////////////////////

  // GEN_EVENT -> GEN_AOD conversion applies pt cut to partons, breaking
  // tree structure. Result is "dangling" partons with 1 -> 0 vertices.
  // FIXME!! Meanwhile discard these if pt < m_danglePtCut.

  if( m_danglePtCut > 0 ){

    removePV.clear();
    removeV.clear();
    deleteP.clear();
    deleteV.clear();

    HepMC::GenEvent::vertex_iterator badv = thinEvt->vertices_begin();
    HepMC::GenEvent::vertex_iterator badvE = thinEvt->vertices_end();

    for(; badv!=badvE; ++badv){
      if( !(*badv) ) continue;
      if( (*badv)->particles_in_size() != 1 ||
          (*badv)->particles_out_size() != 0 ) continue;
      HepMC::GenVertex::particles_in_const_iterator pitr =
        (*badv)->particles_in_const_begin();
      HepMC::GenParticle* pp = *pitr;
      if( pp->production_vertex() ) continue;
      double pt = pp->momentum().perp();
      if( pt > m_danglePtMax ) m_danglePtMax = pt;
      ++m_dangleFound;
      if( pt > m_danglePtCut ) continue;
      if( doDebug ) ATH_MSG_DEBUG("1->0: removing pp,badv,pt " 
                    <<pp->barcode() <<" "
                    <<(*badv)->barcode() <<" " <<pt);
      removePV.push_back(vpPair(*badv,pp));
      deleteP.push_back(pp);
      removeV.push_back(*badv);
      deleteV.push_back(*badv);
      ++m_dangleRemoved;
    }

    // Actually implement changes -- remove particles from vertices
    for(unsigned int i=0; i<removePV.size(); ++i){
      HepMC::GenVertex* v = removePV[i].first;
      HepMC::GenParticle* p = removePV[i].second;
      v->remove_particle(p);
    }

    // Actually implement changes -- remove vertices
    for(unsigned int i=0; i<removeV.size(); ++i){
      if( !thinEvt->remove_vertex(removeV[i]) ){
        ATH_MSG_WARNING("1->0: Failed to remove vertex "<<removeV[i]->barcode());
      }
    }

    // Delete removed particles/vertices
    deleteV.sort();
    deleteV.unique();
    for(dvItr=deleteV.begin(); dvItr!=deleteV.end(); ++dvItr){
      if( *dvItr) delete (*dvItr);
    }

    deleteP.sort();
    deleteP.unique();
    for(dpItr=deleteP.begin(); dpItr!=deleteP.end(); ++dpItr){
      if( *dpItr ) delete (*dpItr);
    }
  }// end m_danglePtCut



  /////////////////////////
  // Done - examine results
  /////////////////////////

  if( doPrint ){
    std::cout <<"========== BEGIN EVENT AFTER THINNING ==========" <<std::endl;
    thinEvt->print();
    std::cout <<"========== END EVENT AFTER THINNING ==========" <<std::endl;
  }

  m_thinParticles += thinEvt->particles_size();
  m_thinVertices +=  thinEvt->vertices_size();

  /////////////////////////////////////////
  // Save thinned event in output container
  /////////////////////////////////////////

  thinnedMcEvts->push_back(thinEvt);
  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

// Parton is quark or gluon
bool CompactHardTruth::isParton( const HepMC::GenParticle* p ) {
  int ida = abs(p->pdg_id());
  if( ida==21 || (ida>0 && ida<10) ) return true;
  // Diquarks too -- xx0x
  if( ida>1000 && ida<10000 && (ida/10)%10==0 ) return true;
  return false;
}

// Final parton is quark or gluon with no decay vertex.
// It should have been detached from hadronization vertex.
bool CompactHardTruth::isFinalParton( const HepMC::GenParticle* p ) {
  if( !isParton(p) ) return false;
  HepMC::GenVertex* endp = p->end_vertex();
  if( endp ) return false;
  return true;
}
  
// Hadron excludes leptons and BSM particles
// Includes clusters to find, e.g., partons->cluster vertices 
bool CompactHardTruth::isHadron( const HepMC::GenParticle* p ) {
  int ida = abs(p->pdg_id());
  if( (ida>=80 && ida<1000000) || ida>9000000 ) return true;
  return false;
}  

// Total cluster FourVectors

HepMC::FourVector CompactHardTruth::vtxInMom(HepMC::GenVertex* v){
  double px = 0;
  double py = 0;
  double pz = 0;
  double e = 0;
  HepMC::GenVertex::particles_in_const_iterator it = 
    v->particles_in_const_begin();
  HepMC::GenVertex::particles_in_const_iterator itE = 
    v->particles_in_const_end();
  for(; it!=itE; ++it){
    px += (*it)->momentum().px();
    py += (*it)->momentum().py();
    pz += (*it)->momentum().pz();
    e  += (*it)->momentum().e();
  }
  return HepMC::FourVector(px,py,pz,e);
}

HepMC::FourVector CompactHardTruth::vtxOutMom(HepMC::GenVertex* v){
  double px = 0;
  double py = 0;
  double pz = 0;
  double e = 0;
  HepMC::GenVertex::particles_out_const_iterator it = 
    v->particles_out_const_begin();
  HepMC::GenVertex::particles_out_const_iterator itE = 
    v->particles_out_const_end();
  for(; it!=itE; ++it){
    px += (*it)->momentum().px();
    py += (*it)->momentum().py();
    pz += (*it)->momentum().pz();
    e  += (*it)->momentum().e();
  }
  return HepMC::FourVector(px,py,pz,e);
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace DerivationFramework

