/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/LArFastShower.h"

#include "LArG4FastSimulation/IFastSimDedicatedSD.h"
#include "LArG4Code/EnergySpot.h"

#include "LArG4FastSimSvc/ILArG4FastSimSvc.h"
#include "LArG4ShowerLibSvc/ILArG4ShowerLibSvc.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "G4VProcess.hh"
#include "G4ios.hh"
#include "G4VPhysicalVolume.hh"

#include "HepMC/GenEvent.h"

#include <stdexcept>
#include <sstream>

#include <stdlib.h>

#undef _TRACE_FSM_
#undef _TRACE_DOIT_
#undef _TRACE_POSITION_
#undef _INFO_FSM_


LArFastShower::LArFastShower(G4String name):
  FastSimModel(name),
  m_fastSimSvc(0),
  m_FastSimDedicatedSD(0),
  m_showerLibSvc(0),
  m_eventNum(0)
{
  std::cout << "Initializing " << GetName()
	    << " - package version " << PACKAGE_VERSION << std::endl;
}

LArFastShower::~LArFastShower()
{
	if (m_showerLibSvc) m_showerLibSvc->release();
	if (m_fastSimSvc) m_fastSimSvc->release();
}

ILArG4ShowerLibSvc* LArFastShower::showerLibSvc()
{
  // -----------------------------
  // get shower lib service
  // -----------------------------

  if ( 0 == m_showerLibSvc ) {
#ifdef _TRACE_FSM_  
    G4cout << "LArFastShower::showerLibSvc() - Getting Shower Lib Service" <<G4endl;
#endif 

    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc = svcLocator->service("LArG4ShowerLibSvc", m_showerLibSvc);
    if (sc.isFailure()) {
      throw std::runtime_error("LArFastShower: cannot retrieve LArG4ShowerLibSvc");
    }
  }

  return m_showerLibSvc;
}

G4bool LArFastShower::IsApplicable(const G4ParticleDefinition& particleType)
{
#ifdef _TRACE_FSM_  
  G4cout << "LArFastShower::IsApplicable" <<G4endl;
#endif
  /*
   * if ( flag to parameterize is set )
   * && ( (we want to record SPs) || (ShowerLibSvc has a library for this particle) )
   */
  if (m_applicableMap.find(particleType.GetPDGEncoding()) != m_applicableMap.end()) {
	  return m_applicableMap.find(particleType.GetPDGEncoding())->second;
  }
  bool rez = false;
  if (( fastSimSvc()->flagToShowerLib(particleType) )
   && ( (fastSimSvc()->generateFSStartingPoints()) || (showerLibSvc()->checkLibrary( particleType.GetPDGEncoding() , fastSimSvc()->DetectorTag() )) ))
	  rez = true;
  m_applicableMap[particleType.GetPDGEncoding()] = rez;
  return rez;
}

G4bool LArFastShower::ModelTrigger(const G4FastTrack& fastTrack)
{
/* ============================================================================================================
     Determine if the particle is to be returned to full Geant4 simulation. 
     In the event where the particle is EITHER killed and parameterised OR simply killed, this must be done
     in the appropriate LArFastShower DoIt method.
	 This method Checks: 1) Geometry; 2) Energy; 3) (for e+/e-) Containment
     ============================================================================================================ */
 
#ifdef _TRACE_FSM_  
  G4cout << "LArFastShower::commonTrigger" <<G4endl;
#endif
  
  // We are in a parameterized volume

  // Check if the particle is within energy bounds
  G4double  ParticleEnergy = fastTrack.GetPrimaryTrack()->GetKineticEnergy(); 
  G4ParticleDefinition& ParticleType = *(fastTrack.GetPrimaryTrack()->GetDefinition());

  if ( fastSimSvc()->flagToShowerLib(ParticleType) == true &&
	      ParticleEnergy > fastSimSvc()->minEneToShowerLib(ParticleType) &&
	      ParticleEnergy < fastSimSvc()->maxEneToShowerLib(ParticleType) ) {

#ifdef _TRACE_FSM_
    G4cout << "Particle has energy (" << ParticleEnergy << ") for shower lib and shower lib is on! Accept particle!" << G4endl;
#endif
  }  else {

#ifdef _TRACE_FSM_
    G4cout << "Particle has energy (" << ParticleEnergy << ") outside killing, shower lib and parametrisation "
	     << "or some features are switched off ... returning it to Geant  " << G4endl;
#endif

    return false;
  }

  if (ForcedAccept(fastTrack)) return true;
  if (ForcedDeny(fastTrack)) return false;
   
  if( CheckContainment(fastTrack)==false) {
#ifdef _TRACE_FSM_
    G4cout << "LArFastShower::ModelTrigger() particle failed CheckContainment()... will not be parameterised: " << G4endl;
#endif
    return false;
  }

#ifdef _TRACE_FSM_
    G4cout << "LArFastShower::ModelTrigger() direction: " << fastTrack.GetPrimaryTrackLocalDirection() << G4endl;
    G4cout << "LArFastShower::ModelTrigger() mom dir:   " << fastTrack.GetPrimaryTrack()->GetMomentumDirection() << G4endl;
#endif 

  return true;
}


void LArFastShower::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep)
{
#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::Doit()" << G4endl;
#endif

  if ( fastSimSvc()->generateFSStartingPoints() == true ) {
	if ((float)rand()/RAND_MAX <= fastSimSvc()->generateFSStartingPointsRatio()) {
	 HepMC::GenEvent * ge = GetGenEvent(fastTrack);
	 fastSimSvc()->generateFSStartingPoint(ge);
	 delete ge;
	}
	KillParticle( fastTrack, fastStep );
	return;
  }
  this->UseShowerLib( fastTrack, fastStep );

#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::Doit() done" << G4endl;
#endif
  
}

void LArFastShower::KillParticle(const G4FastTrack& /*fastTrack*/, G4FastStep& fastStep)
{
  
#ifdef _TRACE_DOIT_
  G4cout << "Low energy particle is being killed: " << fastTrack.GetPrimaryTrack()->GetKineticEnergy() << G4endl;
#endif
  
  // Kill the particle
  fastStep.KillPrimaryTrack();
  fastStep.SetPrimaryTrackPathLength(0.0);

  return;
}

void LArFastShower::UseShowerLib(const G4FastTrack& fastTrack, G4FastStep& fastStep)
{
  try {
#ifdef _TRACE_DOIT_
    G4cout << "LArFastShower::UseShowerLib()" << G4endl;
#endif
    
    // kill the electron to be parametrised
    KillParticle(fastTrack, fastStep);

    // -----------------------------
    // Get Shower from ShowerLibSvc
    // -----------------------------	
    const std::vector<EnergySpot> shower = showerLibSvc()->getShower(fastTrack, fastSimSvc()->DetectorTag());
    
#ifdef _TRACE_DOIT_
    G4cout << "Got shower (" << shower.size() << ") from shower lib" << G4endl;
#endif
    
    // loop over hits in shower
    for (std::vector<EnergySpot>::const_iterator iter = shower.begin(); iter != shower.end(); ++iter) {
      
#ifdef _TRACE_DOIT_
      G4cout << "Make Spot: " << iter->GetPosition().x() << " " << iter->GetPosition().y() << " " << iter->GetPosition().z() 
	     << " " << iter->GetEnergy() << " " << iter->GetTime() << G4endl;
#endif
      const EnergySpot& a_spot = *iter;
      
      if (m_FastSimDedicatedSD != 0) {
        m_FastSimDedicatedSD->ProcessSpot(a_spot);
      } else {
        std::cout << "Error: no FastSimDedicatedSD" << std::endl;
      }
      
#ifdef _TRACE_DOIT_
      G4cout << "Made Spot" << G4endl;
#endif
      
    }
    
#ifdef _TRACE_FSM_
    G4cout << "LArFastShower::UseShowerLib() Done" << G4endl;
#endif
    
    return; 
  }
  catch (const std::exception & e) {
    std::cout << "Handling an exception in LArFastShower::" << e.what() << std::endl;
    return;
  }

}

G4bool LArFastShower::CheckContainment(const G4FastTrack &fastTrack)
{

  G4ThreeVector DirectionShower = fastTrack.GetPrimaryTrack()->GetMomentumDirection();
  G4ThreeVector InitialPositionShower = fastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector OrthoShower = DirectionShower.orthogonal();
  G4ThreeVector CrossShower = DirectionShower.cross(OrthoShower);
  
#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::CheckContainment() OrthoShower: " << OrthoShower << G4endl; 
  G4cout << "LArFastShower::CheckContainment() CrossShower: " << CrossShower << G4endl; 
#endif
  
  //Build 5 points at the shower max. edges and far end
  //this picks the points where 99% of the particle energy was already deposited 
  /* Short history of containment check:
   * First, it was magic numbers from ShowerParams class
  G4double Zmx    = myParameterisation()->GetAveZmx();
  G4double  R     = myParameterisation()->GetAveR90();	// Easy - just multiply
  G4double  Z     = myParameterisation()->GetAveZ90();	// More convoluted
   * Then, after ShowerParams class was sent to code heaven, the magic numbers were saved from it
   * and stored right here:
  G4double Zmx = 22*mm;
  G4double R   = 1.5*4*cm;
  G4double Z   = 22*2.5*mm;
   * Now, we take it directly from the library
  */
  G4double Z = showerLibSvc()->getContainmentZ(fastTrack,fastSimSvc()->DetectorTag());
  G4double R = showerLibSvc()->getContainmentR(fastTrack,fastSimSvc()->DetectorTag());

  if (Z == 0.0 && R == 0.0) {
	  //no containment check
	  return true;
  }
  G4double Zmx = Z / 3; //<-here is OUR magic number. looking on the hit distribution plot, it seems that that way most of hits will be inside

  G4int CosPhi[4] = {1,0,-1,0};
  G4int SinPhi[4] = {0,1,0,-1};
  
#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::CheckContainment() R =          " << R   << G4endl; 
  G4cout << "LArFastShower::CheckContainment() Z =          " << Z   << G4endl; 
#endif
  
  G4ThreeVector Position;
  
  G4VSolid* SolidCalo = fastTrack.GetEnvelopeSolid();
  const G4AffineTransform* AffineTransformation = fastTrack.GetAffineTransformation();

  //Startpoint
  Position = InitialPositionShower;
  AffineTransformation->ApplyPointTransform(Position);
  if(SolidCalo->Inside(Position) == kOutside)
    return false;

  //Longitudinal Endpoint
  Position = InitialPositionShower + Z*DirectionShower;
  AffineTransformation->ApplyPointTransform(Position);
  if(SolidCalo->Inside(Position) == kOutside)
    return false;

  //Lateral Spread
  for(int i=0; i<4 ;i++)
    {
      Position = InitialPositionShower + Zmx*DirectionShower + R*CosPhi[i]*OrthoShower + R*SinPhi[i]*CrossShower;
      AffineTransformation->ApplyPointTransform(Position);
      if(SolidCalo->Inside(Position) == kOutside) 
         return false;
    }

#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::CheckContainment()  Shower is contained " << G4endl;
#endif

  return true;

}

HepMC::GenEvent * LArFastShower::GetGenEvent(const G4FastTrack &fastTrack)
{
  G4ThreeVector PositionShower = fastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector MomentumShower = fastTrack.GetPrimaryTrack()->GetMomentum();

  G4double energy = fastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4int pdgcode = fastTrack.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();
  if (pdgcode < 0) pdgcode = -pdgcode; // hack for positrons. let it be electrons.

  // new event. Signal processing = 0, event number "next"
  HepMC::GenEvent * ge = new HepMC::GenEvent( 0, ++m_eventNum);
  // vertex. Position of the shower, time = 0
  HepMC::GenVertex * gv = new HepMC::GenVertex(HepMC::FourVector(PositionShower.x(),PositionShower.y(),PositionShower.z(),0));
  ge->add_vertex(gv);
  // particle. FourVector of the shower, pdgcode, status = 1
  HepMC::GenParticle * gp = new HepMC::GenParticle(HepMC::FourVector(MomentumShower.x(),MomentumShower.y(),MomentumShower.z(),energy), pdgcode,1);
  gv->add_particle_out(gp);

  // return auto_pointer. will be deleted automatically
  return ge;
}
