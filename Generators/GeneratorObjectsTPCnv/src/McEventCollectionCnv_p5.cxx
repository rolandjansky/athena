///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McEventCollectionCnv_p5.cxx
// Implementation file for class McEventCollectionCnv_p5
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// STL includes
#include <utility>
#include <cmath>
#include <cfloat> // for DBL_EPSILON

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p5.h"
#include "HepMcDataPool.h"

#include "GenInterfaces/IHepMCWeightSvc.h"

namespace {
  // helper method to compute the number of particles and vertices in a
  // whole McEventCollection
  std::pair<unsigned int,unsigned int>
  nbrParticlesAndVertices( const McEventCollection* mcEvents ) {
    unsigned int nParts = 0;
    unsigned int nVerts = 0;
    const McEventCollection::const_iterator itrEnd = mcEvents->end();
    for ( McEventCollection::const_iterator itr = mcEvents->begin();
          itr != itrEnd;
          ++itr ) {
      nParts += (*itr)->particles_size();
      nVerts += (*itr)->vertices_size();
    }

    return std::make_pair( nParts, nVerts );
  }
}

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////

McEventCollectionCnv_p5::McEventCollectionCnv_p5() :
  Base_t( ),
  m_isPileup(false),m_hepMCWeightSvc("HepMCWeightSvc","McEventCollectionCnv_p5")
{}

McEventCollectionCnv_p5::McEventCollectionCnv_p5( const McEventCollectionCnv_p5& rhs ) :
  Base_t( rhs ),
  m_isPileup(false),m_hepMCWeightSvc("HepMCWeightSvc","McEventCollectionCnv_p5")
{}

McEventCollectionCnv_p5&
McEventCollectionCnv_p5::operator=( const McEventCollectionCnv_p5& rhs )
{
  if ( this != &rhs ) {
    Base_t::operator=( rhs );
    m_isPileup=rhs.m_isPileup;m_hepMCWeightSvc = rhs.m_hepMCWeightSvc;
  }
  return *this;
}

// Destructor
///////////////

McEventCollectionCnv_p5::~McEventCollectionCnv_p5()
{
}

///////////////////////////////////////////////////////////////////
// Const methods:
///////////////////////////////////////////////////////////////////

void McEventCollectionCnv_p5::persToTrans( const McEventCollection_p5* persObj,
                                           McEventCollection* transObj,
                                           MsgStream& msg )
{
  msg << MSG::DEBUG << "Loading McEventCollection from persistent state..."
      << endreq;

  // elements are managed by DataPool
  if (!m_isPileup) {
    transObj->clear(SG::VIEW_ELEMENTS);
  }
  HepMC::DataPool datapools;
  const unsigned int nVertices = persObj->m_genVertices.size();
  if ( datapools.vtx.capacity() - datapools.vtx.allocated() < nVertices ) {
    datapools.vtx.reserve( datapools.vtx.allocated() + nVertices );
  }
  const unsigned int nParts = persObj->m_genParticles.size();
  if ( datapools.part.capacity() - datapools.part.allocated() < nParts ) {
    datapools.part.reserve( datapools.part.allocated() + nParts );
  }
  const unsigned int nEvts = persObj->m_genEvents.size();
  if ( datapools.evt.capacity() - datapools.evt.allocated() < nEvts ) {
    datapools.evt.reserve( datapools.evt.allocated() + nEvts );
  }
  DataPool<HepMC::GenEvent>& poolOfEvents = datapools.evt;

  transObj->reserve( nEvts );
  for ( std::vector<GenEvent_p5>::const_iterator
          itr = persObj->m_genEvents.begin(),
          itrEnd = persObj->m_genEvents.end();
        itr != itrEnd;
        ++itr ) {
    const GenEvent_p5& persEvt = *itr;
    HepMC::GenEvent * genEvt(0);
    if(m_isPileup) {
      genEvt = new HepMC::GenEvent();
    } else {
      genEvt        =  poolOfEvents.nextElementPtr();
    }
    genEvt->m_signal_process_id     = persEvt.m_signalProcessId;
    genEvt->m_event_number          = persEvt.m_eventNbr;
    genEvt->m_mpi                   = persEvt.m_mpi;
    genEvt->m_event_scale           = persEvt.m_eventScale;
    genEvt->m_alphaQCD              = persEvt.m_alphaQCD;
    genEvt->m_alphaQED              = persEvt.m_alphaQED;
    genEvt->m_signal_process_vertex = 0;
    genEvt->m_beam_particle_1       = 0;
    genEvt->m_beam_particle_2       = 0;
    genEvt->m_weights               = persEvt.m_weights;
    genEvt->m_random_states         = persEvt.m_randomStates;
    genEvt->m_vertex_barcodes.clear();
    genEvt->m_particle_barcodes.clear();
    genEvt->m_momentum_unit         = static_cast<HepMC::Units::MomentumUnit>(persEvt.m_momentumUnit);
    genEvt->m_position_unit         = static_cast<HepMC::Units::LengthUnit>(persEvt.m_lengthUnit);

    //restore weight names from the dedicated svc (which was keeping them in metadata for efficiency)
    genEvt->m_weights.m_names = m_hepMCWeightSvc->weightNames();

    // cross-section restore
    if( genEvt->m_cross_section )
      delete genEvt->m_cross_section;
    genEvt->m_cross_section = 0;

    genEvt->m_cross_section = new HepMC::GenCrossSection();
    if (!persEvt.m_crossSection.empty()) {
      const std::vector<double>& xsection = persEvt.m_crossSection;
      if( static_cast<bool>(xsection[0]) )
        genEvt->m_cross_section->set_cross_section(xsection[2],xsection[1]);
    }

    // heavyIon restore
    if(genEvt->m_heavy_ion )
      delete genEvt->m_heavy_ion;
    genEvt->m_heavy_ion = 0;
    if (!persEvt.m_heavyIon.empty()) {
      const std::vector<float>& hIon = persEvt.m_heavyIon;
      genEvt->m_heavy_ion = new HepMC::HeavyIon
        (
         static_cast<int>(hIon[12]), // Ncoll_hard
         static_cast<int>(hIon[11]), // Npart_proj
         static_cast<int>(hIon[10]), // Npart_targ
         static_cast<int>(hIon[9]), // Ncoll
         static_cast<int>(hIon[8]), // spectator_neutrons
         static_cast<int>(hIon[7]), // spectator_protons
         static_cast<int>(hIon[6]), // N_Nwounded_collisions
         static_cast<int>(hIon[5]), // Nwounded_N_collisions
         static_cast<int>(hIon[4]), // Nwounded_Nwounded_collisions
         hIon[3],                   // impact_parameter
         hIon[2],                  // event_plane_angle
         hIon[1],                  // eccentricity
         hIon[0]         );         // sigma_inel_NN
    }



    // pdfinfo restore
    if(genEvt->m_pdf_info)
      delete genEvt->m_pdf_info;
    genEvt->m_pdf_info = 0;
    if (!persEvt.m_pdfinfo.empty()) {
      const std::vector<double>& pdf = persEvt.m_pdfinfo;
      genEvt->m_pdf_info = new HepMC::PdfInfo
        (
         static_cast<int>(pdf[8]), // id1
         static_cast<int>(pdf[7]), // id2
         pdf[4],                   // x1
         pdf[3],                   // x2
         pdf[2],                   // scalePDF
         pdf[1],                   // pdf1
         pdf[0],                   // pdf2
         static_cast<int>(pdf[6]), // pdf_id1
         static_cast<int>(pdf[5])  // pdf_id2
         );
    }

    transObj->push_back( genEvt );

    // create a temporary map associating the barcode of an end-vtx to its
    // particle.
    // As not all particles are stable (d'oh!) we take 50% of the number of
    // particles as an initial size of the hash-map (to prevent re-hash)
    ParticlesMap_t partToEndVtx( (persEvt.m_particlesEnd-
                                  persEvt.m_particlesBegin)/2 );

    // create the vertices
    const unsigned int endVtx = persEvt.m_verticesEnd;
    for ( unsigned int iVtx= persEvt.m_verticesBegin; iVtx != endVtx; ++iVtx ) {
      genEvt->add_vertex( createGenVertex( *persObj,
                                           persObj->m_genVertices[iVtx],
                                           partToEndVtx,
                                           &datapools ) );
    } //> end loop over vertices

    // set the signal process vertex
    const int sigProcVtx = persEvt.m_signalProcessVtx;
    if ( sigProcVtx != 0 ) {
      genEvt->set_signal_process_vertex( genEvt->barcode_to_vertex( sigProcVtx ) );
    }

    // connect particles to their end vertices
    for ( ParticlesMap_t::iterator
            p = partToEndVtx.begin(),
            endItr = partToEndVtx.end();
          p != endItr;
          ++p ) {
      HepMC::GenVertex* decayVtx = genEvt->barcode_to_vertex( p->second );
      if ( decayVtx ) {
        decayVtx->add_particle_in( p->first );
      } else {
        msg << MSG::ERROR
            << "GenParticle points to null end vertex !!"
            << endreq;
      }
    }

    // set the beam particles
    const int beamPart1 = persEvt.m_beamParticle1;
    const int beamPart2 = persEvt.m_beamParticle2;
    if (  beamPart1 != 0  &&  beamPart2 !=0 ) {
      genEvt->set_beam_particles(genEvt->barcode_to_particle(beamPart1),
                                 genEvt->barcode_to_particle(beamPart2));
    }



  } //> end loop over m_genEvents

  msg << MSG::DEBUG << "Loaded McEventCollection from persistent state [OK]"
      << endreq;

  return;
}

void McEventCollectionCnv_p5::transToPers( const McEventCollection* transObj,
                                           McEventCollection_p5* persObj,
                                           MsgStream& msg )
{
  msg << MSG::DEBUG << "Creating persistent state of McEventCollection..."
      << endreq;
  persObj->m_genEvents.reserve( transObj->size() );

  const std::pair<unsigned int,unsigned int> stats = nbrParticlesAndVertices( transObj );
  persObj->m_genParticles.reserve( stats.first  );
  persObj->m_genVertices.reserve ( stats.second );

  const McEventCollection::const_iterator itrEnd = transObj->end();
  for ( McEventCollection::const_iterator itr = transObj->begin();
        itr != itrEnd;
        ++itr ) {
    const unsigned int nPersVtx   = persObj->m_genVertices.size();
    const unsigned int nPersParts = persObj->m_genParticles.size();
    const HepMC::GenEvent* genEvt = *itr;
    const int signalProcessVtx = genEvt->m_signal_process_vertex
      ? genEvt->m_signal_process_vertex->barcode()
      : 0;
    const int beamParticle1Barcode = genEvt->m_beam_particle_1
      ? genEvt->m_beam_particle_1->barcode()
      : 0;
    const int beamParticle2Barcode = genEvt->m_beam_particle_2
      ? genEvt->m_beam_particle_2->barcode()
      : 0;

   //save the weight names to metadata via the HepMCWeightSvc 
   m_hepMCWeightSvc->setWeightNames( genEvt->m_weights.m_names ).ignore();
   

    persObj->m_genEvents.
      push_back( GenEvent_p5( genEvt->m_signal_process_id,
                              genEvt->m_event_number,
                              genEvt->mpi(),              // number of multi particle interactions
                              genEvt->m_event_scale,
                              genEvt->m_alphaQCD,
                              genEvt->m_alphaQED,
                              signalProcessVtx,
                              beamParticle1Barcode,       // barcodes of beam particles
                              beamParticle2Barcode,
                              genEvt->m_weights.m_weights,
                              genEvt->m_random_states,
                              std::vector<double>(),      // cross section
                              std::vector<float>(),       // heavyion
                              std::vector<double>(),      // pdf info
                              genEvt->m_momentum_unit,
                              genEvt->m_position_unit,
                              nPersVtx,
                              nPersVtx + genEvt->vertices_size(),
                              nPersParts,
                              nPersParts + genEvt->particles_size() ) );
    //HepMC::GenCrossSection encoding
    if (genEvt->m_cross_section) {
      GenEvent_p5& persEvt = persObj->m_genEvents.back();
      std::vector<double>& crossSection = persEvt.m_crossSection;
      crossSection.resize(3);
      crossSection[2] = genEvt->m_cross_section->m_cross_section;
      crossSection[1] = genEvt->m_cross_section->m_cross_section_error;
      crossSection[0] = static_cast<double>(genEvt->m_cross_section->m_is_set);
    }

    //HepMC::HeavyIon encoding
    if (genEvt->m_heavy_ion) {
      GenEvent_p5& persEvt = persObj->m_genEvents.back();
      std::vector<float>& heavyIon = persEvt.m_heavyIon;
      heavyIon.resize(13);
      heavyIon[12]  = static_cast<float>(genEvt->m_heavy_ion->m_Ncoll_hard);
      heavyIon[11]  = static_cast<float>(genEvt->m_heavy_ion->m_Npart_proj);
      heavyIon[10]  = static_cast<float>(genEvt->m_heavy_ion->m_Npart_targ);
      heavyIon[9]   = static_cast<float>(genEvt->m_heavy_ion->m_Ncoll);
      heavyIon[8]   = static_cast<float>(genEvt->m_heavy_ion->m_spectator_neutrons);
      heavyIon[7]   = static_cast<float>(genEvt->m_heavy_ion->m_spectator_protons);
      heavyIon[6]   = static_cast<float>(genEvt->m_heavy_ion->m_N_Nwounded_collisions);
      heavyIon[5]   = static_cast<float>(genEvt->m_heavy_ion->m_Nwounded_N_collisions);
      heavyIon[4]   = static_cast<float>(genEvt->m_heavy_ion->m_Nwounded_Nwounded_collisions);
      heavyIon[3]   = genEvt->m_heavy_ion->m_impact_parameter;
      heavyIon[2]   = genEvt->m_heavy_ion->m_event_plane_angle;
      heavyIon[1]   = genEvt->m_heavy_ion->m_eccentricity;
      heavyIon[0]   = genEvt->m_heavy_ion->m_sigma_inel_NN;
    }

    //PdfInfo encoding
    if (genEvt->m_pdf_info) {
      GenEvent_p5& persEvt = persObj->m_genEvents.back();
      std::vector<double>& pdfinfo = persEvt.m_pdfinfo;
      pdfinfo.resize(9);
      pdfinfo[8] = static_cast<double>(genEvt->m_pdf_info->m_id1);
      pdfinfo[7] = static_cast<double>(genEvt->m_pdf_info->m_id2);
      pdfinfo[6] = static_cast<double>(genEvt->m_pdf_info->m_pdf_id1);
      pdfinfo[5] = static_cast<double>(genEvt->m_pdf_info->m_pdf_id2);
      pdfinfo[4] = genEvt->m_pdf_info->m_x1;
      pdfinfo[3] = genEvt->m_pdf_info->m_x2;
      pdfinfo[2] = genEvt->m_pdf_info->m_scalePDF;
      pdfinfo[1] = genEvt->m_pdf_info->m_pdf1;
      pdfinfo[0] = genEvt->m_pdf_info->m_pdf2;
    }

    // create vertices
    const HepMC::GenEvent::vertex_const_iterator endVtx=genEvt->vertices_end();
    for ( HepMC::GenEvent::vertex_const_iterator i = genEvt->vertices_begin();
          i != endVtx;
          ++i ) {
      writeGenVertex( **i, *persObj );
    }

  } //> end loop over GenEvents

  msg << MSG::DEBUG << "Created persistent state of HepMC::GenEvent [OK]"
      << endreq;
  return;
}

///////////////////////////////////////////////////////////////////
// Non-const methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Protected methods:
///////////////////////////////////////////////////////////////////

HepMC::GenVertex*
McEventCollectionCnv_p5::createGenVertex( const McEventCollection_p5& persEvt,
                                          const GenVertex_p5& persVtx,
                                          ParticlesMap_t& partToEndVtx, HepMC::DataPool* datapools
                                          ) const
{
  DataPool<HepMC::GenVertex>& poolOfVertices = datapools->vtx;
  HepMC::GenVertex * vtx(0);
  if(m_isPileup) {
    vtx=new HepMC::GenVertex();
  } else {
    vtx = poolOfVertices.nextElementPtr();
  }
  vtx->m_position.setX( persVtx.m_x );
  vtx->m_position.setY( persVtx.m_y );
  vtx->m_position.setZ( persVtx.m_z );
  vtx->m_position.setT( persVtx.m_t );
  vtx->m_particles_in.clear();
  vtx->m_particles_out.clear();
  vtx->m_id      = persVtx.m_id;
  vtx->m_weights.m_weights.reserve( persVtx.m_weights.size() );
  vtx->m_weights.m_weights.assign ( persVtx.m_weights.begin(),
                                    persVtx.m_weights.end() );
  vtx->m_event   = 0;
  vtx->m_barcode = persVtx.m_barcode;

  // handle the in-going (orphans) particles
  const unsigned int nPartsIn = persVtx.m_particlesIn.size();
  for ( unsigned int i = 0; i != nPartsIn; ++i ) {
    createGenParticle( persEvt.m_genParticles[persVtx.m_particlesIn[i]],
                       partToEndVtx,
                       datapools );
  }

  // now handle the out-going particles
  const unsigned int nPartsOut = persVtx.m_particlesOut.size();
  for ( unsigned int i = 0; i != nPartsOut; ++i ) {
    vtx->add_particle_out( createGenParticle( persEvt.m_genParticles[persVtx.m_particlesOut[i]],
                                              partToEndVtx,
                                              datapools ) );
  }

  return vtx;
}

HepMC::GenParticle*
McEventCollectionCnv_p5::createGenParticle( const GenParticle_p5& persPart,
                                            ParticlesMap_t& partToEndVtx, HepMC::DataPool* datapools ) const
{
  using std::abs;
  using std::sqrt;
  using std::pow;

  DataPool<HepMC::GenParticle>& poolOfParticles = datapools->part;
  HepMC::GenParticle* p(0);
  if (m_isPileup) {
    p = new HepMC::GenParticle();
  } else {
    p    = poolOfParticles.nextElementPtr();
  }
  p->m_pdg_id              = persPart.m_pdgId;
  p->m_status              = persPart.m_status;
  p->m_polarization.m_theta= static_cast<double>(persPart.m_thetaPolarization);
  p->m_polarization.m_phi  = static_cast<double>(persPart.m_phiPolarization  );
  p->m_production_vertex   = 0;
  p->m_end_vertex          = 0;
  p->m_barcode             = persPart.m_barcode;
  p->m_generated_mass      = static_cast<double>(persPart.m_generated_mass);

  // Note: do the E calculation in extended (long double) precision.
  // That happens implicitly on x86 with optimization on; saying it
  // explicitly ensures that we get the same results with and without
  // optimization.  (If this is a performance issue for platforms
  // other than x86, one could change to double for those platforms.)
  if ( 0 == persPart.m_recoMethod ) {
    //    p->m_momentum.setVectM(Hep4Vector( persPart.m_px,
    //					persPart.m_py,
    //					persPart.m_pz ),
    //                            persPart.m_m );

    p->m_momentum.setPx( persPart.m_px);
    p->m_momentum.setPy( persPart.m_py);
    p->m_momentum.setPz( persPart.m_pz);
    double temp_e = sqrt( (long double)(persPart.m_px)*persPart.m_px +
                          (long double)(persPart.m_py)*persPart.m_py +
                          (long double)(persPart.m_pz)*persPart.m_pz +
                          (long double)(persPart.m_m) *persPart.m_m );
    p->m_momentum.setE( temp_e);
  } else {
    const int signM2 = ( persPart.m_m >= 0. ? 1 : -1 );
    const double persPart_ene =
      sqrt( abs((long double)(persPart.m_px)*persPart.m_px +
                (long double)(persPart.m_py)*persPart.m_py +
                (long double)(persPart.m_pz)*persPart.m_pz +
                signM2* (long double)(persPart.m_m)* persPart.m_m));
    const int signEne = ( persPart.m_recoMethod == 1 ? 1 : -1 );
    p->m_momentum.set( persPart.m_px,
                       persPart.m_py,
                       persPart.m_pz,
                       signEne * persPart_ene );
  }

  // setup flow
  const unsigned int nFlow = persPart.m_flow.size();
  p->m_flow.clear();
  for ( unsigned int iFlow= 0; iFlow != nFlow; ++iFlow ) {
    p->m_flow.set_icode( persPart.m_flow[iFlow].first,
                         persPart.m_flow[iFlow].second );
  }

  if ( persPart.m_endVtx != 0 ) {
    partToEndVtx[p] = persPart.m_endVtx;
  }

  return p;
}

void McEventCollectionCnv_p5::writeGenVertex( const HepMC::GenVertex& vtx,
                                              McEventCollection_p5& persEvt ) const
{
  const HepMC::FourVector& position = vtx.m_position;
  persEvt.m_genVertices.push_back(
                                  GenVertex_p5( position.x(),
                                                position.y(),
                                                position.z(),
                                                position.t(),
                                                vtx.m_id,
                                                vtx.m_weights.m_weights.begin(),
                                                vtx.m_weights.m_weights.end(),
                                                vtx.m_barcode ) );
  GenVertex_p5& persVtx = persEvt.m_genVertices.back();

  // we write only the orphans in-coming particles
  const std::vector<HepMC::GenParticle*>::const_iterator endInVtx = vtx.m_particles_in.end();
  persVtx.m_particlesIn.reserve(vtx.m_particles_in.size());
  for ( std::vector<HepMC::GenParticle*>::const_iterator p = vtx.m_particles_in.begin();
        p != endInVtx;
        ++p ) {
    if ( 0 == (*p)->production_vertex() ) {
      persVtx.m_particlesIn.push_back( writeGenParticle( **p, persEvt ) );
    }
  }

  const std::vector<HepMC::GenParticle*>::const_iterator endOutVtx = vtx.m_particles_out.end();
  persVtx.m_particlesOut.reserve(vtx.m_particles_out.size());
  for ( std::vector<HepMC::GenParticle*>::const_iterator p = vtx.m_particles_out.begin();
        p != endOutVtx;
        ++p ) {
    persVtx.m_particlesOut.push_back( writeGenParticle( **p, persEvt ) );
  }

  return;
}

int McEventCollectionCnv_p5::writeGenParticle( const HepMC::GenParticle& p,
                                               McEventCollection_p5& persEvt ) const
{
  using std::abs;
  const HepMC::FourVector& mom = p.m_momentum;
  const double ene = mom.e();
  const double m2  = mom.m2();

  // Definitions of Bool isTimeLilike, isSpacelike and isLightlike according to HepLorentzVector definition
  const bool useP2M2 = !(m2 > 0) &&   // !isTimelike
    (m2 < 0) &&   //  isSpacelike
    !(abs(m2) < 2.0*DBL_EPSILON*ene*ene); // !isLightlike

  //  const bool useP2M2 = !isTimelike () &&
  //                        mom.isSpacelike() &&
  //                       !mom.isLightlike();
  const short recoMethod = ( !useP2M2
                             ? 0
                             : ( ene >= 0. //*GeV
                                 ? 1
                                 : 2 ) );


  persEvt.m_genParticles.
    push_back( GenParticle_p5( mom.px(),
                               mom.py(),
                               mom.pz(),
                               mom.m(),
                               p.m_pdg_id,
                               p.m_status,
                               p.m_flow.size(),
                               p.m_polarization.theta(),
                               p.m_polarization.phi(),
                               p.m_production_vertex
                               ? p.m_production_vertex->barcode()
                               : 0,
                               p.m_end_vertex
                               ? p.m_end_vertex->barcode()
                               : 0,
                               p.m_barcode,
                               p.m_generated_mass,
                               recoMethod ) );
  persEvt.m_genParticles.back().m_flow.assign( p.m_flow.begin(),
                                               p.m_flow.end() );

  // we return the index of the particle in the big vector of particles
  // (contained by the persistent GenEvent)
  return (persEvt.m_genParticles.size() - 1);
}

void McEventCollectionCnv_p5::setPileup() {
  m_isPileup = true;
}
