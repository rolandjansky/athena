/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: TrackRecordGenerator/TrackRecordGenerator.cxx
//   Mostly adopted from the cosmic generator

#include "TrackRecordGenerator/TrackRecordGenerator.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Random/RandFlat.h"

#include "TrackRecord/TrackRecordCollection.h"

#include <limits>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>

//--------------------------------------------------------------------------
TrackRecordGenerator::TrackRecordGenerator(const std::string& name, ISvcLocator* pSvcLocator)
   : GenModule(name,pSvcLocator)
   , m_events(0)
{
  // Input collection name
  declareProperty("TRCollection" , m_recordName = "CosmicRecord" );

  // Smearing of the initial position for tracks
  declareProperty("TRSmearing", m_smearTR=-1, "Smear the initial position of the track by up to this amount");
  declareProperty("TRPSmearing", m_smearTRp=-1, "Smear the momentum of the track by up to this amount");
  declareProperty("StopParticles", m_stopParticles=false, "Stop the particles and make them decay within 25 ns");
  declareProperty("stopped_tminus", m_stopped_tminus =-25. );
  declareProperty("stopped_tplus", m_stopped_tplus =25. );
  declareProperty("Add_cL", m_add_cL=true, "For stopped particles, shift the time by c times the decay rho");
}

//--------------------------------------------------------------------------
 TrackRecordGenerator::~TrackRecordGenerator()
{}

//---------------------------------------------------------------------------
StatusCode TrackRecordGenerator::callGenerator() {

  ++m_events;
  ATH_MSG_DEBUG( "Event #" << m_events);

  // clear up the vectors
  m_fourPos.clear();
  m_fourMom.clear();
  m_polarization.clear();
  m_pdgCode.clear();

  //const DataHandle <TimedTrackRecordCollection> coll;
  const DataHandle <TrackRecordCollection> coll;
  CHECK( evtStore()->retrieve(coll,m_recordName) );

  ATH_MSG_INFO("retrieved "<<coll->size()<<" TTR hits; will smear position by "<< (m_smearTR>0?m_smearTR:0.) <<" mm and momentum by "<< (m_smearTRp>0?m_smearTRp:0.) <<" radians");

  for (auto iterTTR : *coll) {

    const HepPDT::ParticleData* particle = particleData(abs(iterTTR.GetPDGCode()));
    double mass = particle->mass().value();
    double en = std::sqrt(mass*mass+iterTTR.GetMomentum().mag2());

    ATH_MSG_VERBOSE("Reading back TTR:\n pos is "<<iterTTR.GetPosition()
                  <<"\n mom is "<<iterTTR.GetMomentum()
                  <<"\n pdg is "<<iterTTR.GetPDGCode() );

    CLHEP::HepLorentzVector particle4Position( iterTTR.GetPosition(), iterTTR.GetTime());

    ATH_MSG_DEBUG( "Smearing position by up to " << m_smearTR << " mm and momentum by up to " << m_smearTRp << " radians" );
    if (m_smearTR>0){
      ATH_MSG_DEBUG( "Track record started at " << particle4Position );

      // if Z is maximal, move in X and Y; otherwise move in Z and "phi"
      if ( particle4Position.z() == 22031 || particle4Position.z() == -22031 ){ //FIXME Hardcoded limits!
        particle4Position.setX( particle4Position.x() + CLHEP::RandFlat::shoot(&randomEngine(), -m_smearTR, m_smearTR) );
        particle4Position.setY( particle4Position.y() + CLHEP::RandFlat::shoot(&randomEngine(), -m_smearTR, m_smearTR) );
      } else {
        particle4Position.setZ( particle4Position.z() + CLHEP::RandFlat::shoot(&randomEngine(), -m_smearTR, m_smearTR) );
        double R = std::sqrt( std::pow( particle4Position.x(),2 ) + std::pow(particle4Position.y(),2 ) );
        double dPhi = std::atan2( m_smearTR, R );
        dPhi = CLHEP::RandFlat::shoot( &randomEngine(), -dPhi, dPhi );
        double theta = std::atan2( particle4Position.x() , particle4Position.y() );
        particle4Position.setX( R*sin( theta + dPhi ) );
        particle4Position.setY( R*cos( theta + dPhi ) );
      }
      ATH_MSG_DEBUG( "Shifted track record to " << particle4Position );
    }
    CLHEP::HepLorentzVector particle4Momentum( iterTTR.GetMomentum(), en );
    if (m_smearTRp>0){
      ATH_MSG_DEBUG( "Track record momentum was " << particle4Momentum );

      // Keep p - smear an angle, and then randomly spin that change in (0,2PI)
      double dTheta = CLHEP::RandFlat::shoot(&randomEngine(), 0, m_smearTRp);
      double dPhi   = CLHEP::RandFlat::shoot(&randomEngine(), 0, 2*M_PI);

      // Need a perpendicular vector...
      CLHEP::HepLorentzVector perpendicularMomentum( 1 , 0 , 0 , 0);
      if ( particle4Momentum.x() != 0 ){
        if (particle4Momentum.y() == 0){
          perpendicularMomentum.setX( 0 );
          perpendicularMomentum.setY( 1 );
        } else {
          perpendicularMomentum.setX( particle4Momentum.y() );
          perpendicularMomentum.setY( particle4Momentum.x() );
        }
      }

      // Now scale it based on dTheta
      double tempP = pow(particle4Momentum.x(),2)+pow(particle4Momentum.y(),2)+pow(particle4Momentum.z(),2);
      if ( tempP==0 ) {
        ATH_MSG_DEBUG("Our initial momentum had zero magnitude!!");
        perpendicularMomentum.setX(0);
        perpendicularMomentum.setY(0);
      } else if ( tan(dTheta) == 0 ){
        ATH_MSG_DEBUG("Randomly deciding to keep the vector's direction...");
        perpendicularMomentum.setX(0);
        perpendicularMomentum.setY(0);
      } else {
        double scale = ( tempP ) * sin(dTheta) / ( pow(perpendicularMomentum.x(),2)+pow(perpendicularMomentum.y(),2) );
        perpendicularMomentum.setX( perpendicularMomentum.x() * scale );
        perpendicularMomentum.setY( perpendicularMomentum.y() * scale );

        // Rotate perpendicularMomentum by dPhi around particle4Momentum
        perpendicularMomentum.rotate( dPhi , particle4Momentum.vect() );
      }
      particle4Momentum.setX( particle4Momentum.x() + perpendicularMomentum.x() );
      particle4Momentum.setY( particle4Momentum.y() + perpendicularMomentum.y() );
      particle4Momentum.setZ( particle4Momentum.z() + perpendicularMomentum.z() );

      // Rescale (very small effect, but want to include it)
      double scale2 = tempP==0? 1 : (pow(particle4Momentum.x(),2)+pow(particle4Momentum.y(),2)+pow(particle4Momentum.z(),2)) / tempP;
      particle4Momentum.setX( particle4Momentum.x() * scale2 );
      particle4Momentum.setY( particle4Momentum.y() * scale2 );
      particle4Momentum.setZ( particle4Momentum.z() * scale2 );
      ATH_MSG_DEBUG( "Rotated the vector by " << perpendicularMomentum );
      ATH_MSG_DEBUG( "And resulting momentum is " << particle4Momentum );
    }
    if (m_stopParticles){
      ATH_MSG_DEBUG( "Will stop the track record where it is and give it mass " << mass );
      particle4Momentum.setX(0);
      particle4Momentum.setY(0);
      particle4Momentum.setZ(0);
      particle4Momentum.setT(mass);

      double settime=CLHEP::RandFlat::shoot(&randomEngine(),m_stopped_tminus, m_stopped_tplus);
      ATH_MSG_DEBUG( "Setting particle time to something uniform between "<<m_stopped_tminus<<" and "<<m_stopped_tplus<<" ns : " << settime );
      if (m_add_cL){
        settime += particle4Position.rho()/CLHEP::c_light;
      }
      particle4Position.setT(settime*CLHEP::c_light); // ct in mm
    }

    m_fourPos.push_back( particle4Position );
    m_fourMom.push_back( particle4Momentum );

    m_pdgCode.push_back(iterTTR.GetPDGCode());
    HepMC::Polarization thePolarization(0.0,0.0);
    m_polarization.push_back(thePolarization);

    if (m_stopParticles){
      ATH_MSG_DEBUG( "Only one per event!!" );
      break;
    }
  } // Loop through the track record collection

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TrackRecordGenerator::fillEvt(HepMC::GenEvent* event) {

  if(m_fourMom.size()==m_fourPos.size()&&m_fourMom.size()==m_polarization.size()){

    for(std::size_t v=0;v<m_fourMom.size();++v){
      // Note: The vertex and particle are owned by the event, so the
      // event is responsible for those pointers.

      // Create the particle, and specify its polarization.
      HepMC::GenParticle* particle = new HepMC::GenParticle( m_fourMom[v], m_pdgCode[v], 1);
      particle->set_polarization( m_polarization[v] );

      // Create the vertex, and add the particle to the vertex.
      HepMC::GenVertex* vertex = new HepMC::GenVertex(m_fourPos[v]);
      vertex->add_particle_out( particle );

      // Add the vertex to the event.
      event->add_vertex( vertex );
    } // Loop over the particles

    event->set_event_number(m_events); // Set the event number
    if (event->weights().empty()){
      event->weights().push_back(1.0);
    }
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_ERROR("Wrong different number of vertexes/momenta/polaritazions!");
    return StatusCode::FAILURE;
  }

}

