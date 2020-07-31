/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: MuonBackgroundConverter/MuonBackgroundConverter.cxx
// Description:
//
//    Reads cavern-background events from an ASCII file, converts 
//    them to HepMC.
//
//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
// 
// Author: A. Nairz, December 22, 2004
//         Based on work by W. Seligman, M. Shapiro, I. Hinchliffe;
//         S. Bentvelsen, N. van Eldik 
//
// Modifications:
//         - A.N. (Feb. 27, 2006): allow more flexible treatment of multiple 
//                                 hits with same energy (keep all/keep first)

//         - Ketevi A. Assamagan ( Jan 21, 2008): increase statistics with phi symmetry of Muon Spectrometer 
//                                                time distribution consistent with 25ns or 75ns or whatever
//                                                explicitly remove duplicates
// 

#include "CavernBkgGenerator/MuonBackgroundConverter.h"

// Other classes used by this class:
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/Polarization.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/GaudiException.h"

// For the Athena-based random numbers.
#include "CLHEP/Random/RandFlat.h"

#include <limits.h>

#include <cmath>
#include <vector>
#include <string>
#include <fstream>

MuonBackgroundConverter::MuonBackgroundConverter( const std::string& name, ISvcLocator* pSvcLocator ) 
  : GenModule( name, pSvcLocator ),
    m_nEvents(0),
    //m_smearPhi(false),
    m_used(0),
    m_rndmSvc("AtRndmGenSvc", name ),
    m_rndmEngine(0),
    m_rndmEngineName("SINGLE")
{
  declareProperty( "eventfile",    m_infile       = "partflux.fvpn21n.txt" );
  declareProperty( "safetyFactor", m_safetyFactor = 1 );
  declareProperty( "PhiSymN", m_phiN = 8 );
  //declareProperty( "smearPhi",     m_smearPhi     = true );
  declareProperty( "firstHitOnly", m_firstHitOnly = true );
  declareProperty( "BunchCrossingTime", m_t0      = 25.0*CLHEP::ns );
  declareProperty( "ctcut",        m_ctcut        = CLHEP::c_light * CLHEP::microsecond );
}

MuonBackgroundConverter::~MuonBackgroundConverter() 
{}


StatusCode MuonBackgroundConverter::genInitialize() 
{

  // Initialize event count.
  m_nEvents = 0;

  // Clear vectors with PDG id, vertex, kinematic variables
  m_pdgCode.clear();
  m_fourPos.clear();
  m_fourMom.clear();
  m_polarization.clear();  

  // Open input file
  if ( m_infile == "" )
  {
    ATH_MSG_FATAL("No input file specified - stop!");
    return StatusCode::FAILURE;
  }
  else
  {
    ATH_MSG_INFO("Events are read from file "  <<  m_infile);
    m_ffile.open( m_infile.c_str() );
    if ( !m_ffile )
    {
      ATH_MSG_FATAL("Could not open input file - stop! ");
      return StatusCode::FAILURE;
    }
  }

  m_used = 0;
 
  // Check if safety factor is meaningful
  if ( m_safetyFactor < 1 )
  {
    ATH_MSG_ERROR("Safety factor " << m_safetyFactor << " not allowed. Set to 1.");
    m_safetyFactor = 1;
  }

  //random number initialization
  if (!m_rndmSvc.retrieve().isSuccess())
  {
    ATH_MSG_FATAL(" Could not initialize Random Number Service");
    return StatusCode::FAILURE;
  }      
    
  // getting our random numbers stream
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_FATAL("Could not find RndmEngine : " << m_rndmEngineName);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

StatusCode MuonBackgroundConverter::callGenerator() 
{

  ATH_MSG_DEBUG("In callGenerator()");

  ++m_nEvents;
  //++m_used;

  // Clear vectors with PDG id, vertex, kinematic variables
  m_pdgCode.clear();
  m_fourPos.clear();
  m_fourMom.clear();
  m_polarization.clear();  

  int phiSymmetry = (int) m_phiN;
  if ( phiSymmetry <=0 ) phiSymmetry=1;

  const double tolerance = 1.0 * CLHEP::keV;   // needed to avoid double counting
  
  if ( m_used == (phiSymmetry-1) ) m_used=0;

  while ( m_pdgCode.size() == 0 ) {

    if ( !m_ffile.eof() )
    {
       int nPart   = 0;  // number of particles per event
       int nMerged = 0;  // number of already merged events
       while ( nMerged < m_safetyFactor ) 
       {
         /** get a new evetn or keep using th eprevious event */
         //if ( m_used == 1 ) {
         //	 std::cout << "m_used=1read in " << m_nEvents << std::endl;
         ATH_MSG_DEBUG("Starting on a new event from the txt file ..."); 
         if ( nMerged == 0 ) {
	       m_evt.clear();
	     }
         MuonBackgroundEventParser particle;
         m_ffile >> particle;   // read in first line
         while ( particle.getString() != "END"  &&  particle.getString() != "" )
         {          
           if ( particle.getPdgID() )  // don't select (rare!) Deuteron, He2, He3, Alpha, ...
           {
             m_evt.push_back( particle );
           }
           //    read in next line
           m_ffile >> particle;
         }
         //}

         // increment number of already merged events
         ++nMerged;
       }
       
       /** process the list of particle */
       double p_prev  = 0.0;
       for ( unsigned int i=0; i<m_evt.size(); ++i ) {
         CLHEP::HepLorentzVector momentum = m_evt.at(i).getMomentum();
         CLHEP::Hep3Vector       threemom = momentum.vect();
         double           p_curr   = threemom.mag();
            
         // avoid double counting if m_firstHitOnly == true 
         // (there might be multiple entries from the same particle in the input file)
         // also remove muons because they are most-likely  prompt
         int id = abs (m_evt.at(i).getPdgID() );
         
         bool select = ( (!m_firstHitOnly) || 
                         (m_firstHitOnly && std::abs(p_curr-p_prev)>tolerance) ) && ( id != 13 ); 
         if ( select )
         {
             //std::cout  <<  evt  <<  std::endl;
 
             // polarisation 
             HepMC::Polarization pol(0.0,0.0);
             m_polarization.push_back( pol );
          
             // PDG id, vertex, kinematic variables
             m_pdgCode.push_back( m_evt.at(i).getPdgID() );
                        
             // For neutrals (neutron, gamma, pi0, K0_L): assign new time within [0,25] CLHEP::ns window
             int    pdgID   = std::abs( m_evt.at(i).getPdgID() );
             double newTime = m_evt.at(i).getVertex().t() * CLHEP::second;
             if ( pdgID == 2112  ||  pdgID == 22  ||  pdgID == 111  ||  pdgID == 130 )
             {
                newTime = CLHEP::RandFlat::shoot( m_rndmEngine, 0, m_t0 );
             }
             ATH_MSG_DEBUG("   PDG code = "              << pdgID 
                           << "   old time [ns] = "      << m_evt.at(i).getVertex().t() * CLHEP::second
                           << "   new time [ns] = "      << newTime 
                           << "   newVertex.t() [mm] = " << newTime * CLHEP::c_light);
             CLHEP::HepLorentzVector oldVertex = m_evt.at(i).getVertex();
             CLHEP::HepLorentzVector newVertex = oldVertex; 
             newVertex.setT( newTime * CLHEP::c_light );

             double rT     = ( oldVertex.vect() ).perp();
             double oldPhi = oldVertex.phi();
        
             // Smear azimuthal angle phi if requested
             CLHEP::HepLorentzVector oldMomentum = m_evt.at(i).getMomentum();
             CLHEP::HepLorentzVector newMomentum = oldMomentum; 
             //if ( m_smearPhi )
             //{
             //double minPhi   = (sector-1)*CLHEP::twopi/phiSymmetry;
             //double maxPhi   = sector*CLHEP::twopi/phiSymmetry;
             //double deltaPhi = CLHEP::RandFlat::shoot( m_rndmEngine, minPhi, maxPhi );
             double deltaPhi  = m_used * ( CLHEP::twopi/phiSymmetry + CLHEP::RandFlat::shoot( m_rndmEngine, 0, CLHEP::twopi ) );
             double newPhi    = oldPhi + deltaPhi;
             if ( newPhi > CLHEP::twopi ) newPhi = newPhi-CLHEP::twopi;
             ATH_MSG_DEBUG("   old phi = "  << oldPhi 
                           << "   deltaPhi = " << deltaPhi 
                           << "   newPhi = "   << newPhi 
                           << "   phiSector = " << m_used);
             newVertex.setX( rT * cos( oldPhi + deltaPhi ) );
             newVertex.setY( rT * sin( oldPhi + deltaPhi ) );
             ATH_MSG_DEBUG("   old vertex = " << oldVertex.x() << " " << oldVertex.y() << " " << oldVertex.z()  
                           << "   new vertex = " << newVertex.x() << " " << newVertex.y() << " " << newVertex.z());
             newMomentum.setPx( oldMomentum.px() * cos(deltaPhi) - oldMomentum.py() * sin(deltaPhi) );
             newMomentum.setPy( oldMomentum.px() * sin(deltaPhi) + oldMomentum.py() * cos(deltaPhi) );
             ATH_MSG_DEBUG("   old 3-momentum = " << oldMomentum.px() << " " << oldMomentum.py() << " " << oldMomentum.pz() 
                           << "   new 3-momentum = " << newMomentum.px() << " " << newMomentum.py() << " " << newMomentum.pz() 
                                                );
             //}   
         
             m_fourPos.push_back( newVertex );
             m_fourMom.push_back( newMomentum );

             ++nPart;
             // to avoid double counting 
             p_prev = p_curr;
          }
       }
      
       /** reset recycling */
       // std::cout << "m_evt=" << m_evt.size() << " in " << m_nEvents << std::endl;
       // std::cout << "m_fmom=" << m_fourMom.size() << " in " << m_nEvents << std::endl;
       // std::cout << "m_fpos=" << m_fourPos.size() << " in " << m_nEvents << std::endl;
       // std::cout << "m_fpos=" << m_pdgCode.size() << " in " << m_nEvents << std::endl;
       // std::cout << "m_npart=" << nPart << " in " << m_nEvents << std::endl;
       // if ( m_used == (phiSymmetry-1) ) m_used=0;
 
       ATH_MSG_DEBUG("   Number of particles = " << nPart);
    }
    else
    {
       ATH_MSG_INFO("   End of input file reached - will go back to the begin of file ...");
       m_used++;
       m_ffile.clear();
       m_ffile.seekg (0, std::ios::beg);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode MuonBackgroundConverter::genFinalize() 
{
  return StatusCode::SUCCESS;
}


StatusCode MuonBackgroundConverter::fillEvt( HepMC::GenEvent* event ) 
{

  ATH_MSG_DEBUG("In fillEvt()");

  // Note: The vertex and particle are owned by the event, so the
  // event is responsible for those pointers.

  // Create all the particles in the event, and specify their polarization


  ATH_MSG_INFO("the size of the event = " << m_pdgCode.size());
  if ( m_pdgCode.size()==0 ) ATH_MSG_WARNING("Empty event");


  int nPart = m_pdgCode.size();
  for ( int i = 0; i < nPart; i++ )
  {
     HepMC::GenParticlePtr particle = HepMC::newGenParticlePtr( HepMC::FourVector(m_fourMom[i].x(),m_fourMom[i].y(),m_fourMom[i].z(),m_fourMom[i].e()), m_pdgCode[i], 1 );
     HepMC::set_polarization(particle, m_polarization[i] );

     // Create the vertex, and add the particle to the vertex
     HepMC::GenVertexPtr vertex = HepMC::newGenVertexPtr( HepMC::FourVector(m_fourPos[i].x(),m_fourPos[i].y(),m_fourPos[i].z(),m_fourPos[i].t()) );
     vertex->add_particle_out( particle );

     // Add the vertex to the event
     event->add_vertex( vertex );
  }
  
  // Set the event number
  event->set_event_number( m_nEvents ); 
 
  return StatusCode::SUCCESS;
}

