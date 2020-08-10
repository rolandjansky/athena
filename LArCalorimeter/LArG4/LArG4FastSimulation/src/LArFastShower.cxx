/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFastShower.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "LArG4Code/EnergySpot.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/IO_GenEvent.h"

#include <stdexcept>

#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4Neutron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4VSensitiveDetector.hh"
#include "G4EventManager.hh"
#include "LArG4ShowerLibSvc/ILArG4ShowerLibSvc.h"
#include "IFastSimDedicatedSD.h"

#undef _TRACE_FSM_
#undef _TRACE_DOIT_
#undef _TRACE_POSITION_
#undef _INFO_FSM_


LArFastShower::LArFastShower(const std::string& name, const FastShowerConfigStruct& config,
                             IFastSimDedicatedSD* fastSimDedicatedSD):
  G4VFastSimulationModel(name),
  m_configuration(config),
  m_fastSimDedicatedSD(fastSimDedicatedSD),
  m_showerLibSvc(nullptr),
  m_generate_starting_points(false),
  m_starting_points_file(),
  m_eventNum(0)
{
  enum DETECTOR { EMB=100000, EMEC=200000, FCAL1=300000, FCAL2=400000,
                  FCAL3=500000, HECLOC=600000, HEC=700000 };

  m_detmap["EMB"]=EMB;
  m_detmap["EMEC"]=EMEC;
  m_detmap["FCAL1"]=FCAL1;
  m_detmap["FCAL2"]=FCAL2;
  m_detmap["FCAL3"]=FCAL3;
  m_detmap["HECLOC"]=HECLOC;
  m_detmap["HEC"]=HEC;
}

IFastSimDedicatedSD* LArFastShower::fastShowerSD()
{
  if ( !m_fastSimDedicatedSD ) {
    throw std::runtime_error("LArFastShower: no pointer to IFastSimDedicatedSD!");
  }
  return m_fastSimDedicatedSD;
}


ILArG4ShowerLibSvc* LArFastShower::showerLibSvc()
{
  if (!m_showerLibSvc ) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc = svcLocator->service(m_configuration.m_showerLibSvcName, m_showerLibSvc);
    if (sc.isFailure()) {
      throw std::runtime_error("LArFastShower: cannot retrieve LArG4ShowerLibSvc");
    }
  }
  return m_showerLibSvc;
}


G4bool LArFastShower::IsApplicable(const G4ParticleDefinition& particleType)
{
#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::IsApplicable" << G4endl;
#endif

  /*
   * if ( flag to parameterize is set )
   * && ( (we want to record SPs) || (ShowerLibSvc has a library for this particle) )
   */
  if (m_applicableMap.find(particleType.GetPDGEncoding()) != m_applicableMap.end()) {
    return m_applicableMap.find(particleType.GetPDGEncoding())->second;
  }
  bool rez = false;
  if ( flagToShowerLib(particleType) &&
       ( m_generate_starting_points ||
         showerLibSvc()->checkLibrary( particleType.GetPDGEncoding(),
                                       m_configuration.m_detector_tag ) ))
    rez = true;
  m_applicableMap[particleType.GetPDGEncoding()] = rez;
  return rez;
}

G4bool LArFastShower::ModelTrigger(const G4FastTrack& fastTrack)
{
  /* ==========================================================================
     Determine if the particle is to be returned to full Geant4 simulation.
     In the event where the particle is EITHER killed and parameterised OR
     simply killed, this must be done in the appropriate LArFastShower DoIt method.
     This method Checks: 1) Geometry; 2) Energy; 3) (for e+/e-) Containment
     ========================================================================== */

#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::commonTrigger" << G4endl;
#endif

  // We are in a parameterized volume

  // Check if the particle is within energy bounds
  G4double particleEnergy = fastTrack.GetPrimaryTrack()->GetKineticEnergy();
  const G4ParticleDefinition& particleType = *(fastTrack.GetPrimaryTrack()->GetDefinition());

  if ( flagToShowerLib(particleType) == true &&
       particleEnergy > minEneToShowerLib(particleType) &&
       particleEnergy < maxEneToShowerLib(particleType) ) {

#ifdef _TRACE_FSM_
    G4cout << "Particle has energy (" << particleEnergy << ") for shower lib and shower lib is on! Accept particle!" << G4endl;
#endif
  }  else {

#ifdef _TRACE_FSM_
    G4cout << "Particle has energy (" << particleEnergy << ") outside killing, shower lib and parametrisation "
           << "or some features are switched off ... returning it to Geant  " << G4endl;
#endif

    return false;
  }

  if (ForcedAccept(fastTrack)) return true;
  if (ForcedDeny(fastTrack)) return false;

  if (CheckContainment(fastTrack)==false) {
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

  if ( m_generate_starting_points ) {
    if ((float)rand()/static_cast<float>(RAND_MAX) <= m_configuration.m_generated_starting_points_ratio) {
      HepMC::GenEvent * ge = GetGenEvent(fastTrack);
      generateFSStartingPoint(ge);
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

#ifdef _TRACE_DOIT_
void LArFastShower::KillParticle(const G4FastTrack& fastTrack, G4FastStep& fastStep)
#else
void LArFastShower::KillParticle(const G4FastTrack&, G4FastStep& fastStep)
#endif
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
    const std::vector<EnergySpot> shower =
      showerLibSvc()->getShower(fastTrack, m_configuration.m_detector_tag);

#ifdef _TRACE_DOIT_
    G4cout << "Got shower (" << shower.size() << ") from shower lib" << G4endl;
#endif

    // loop over hits in shower
    for (const auto& a_spot : shower) {

#ifdef _TRACE_DOIT_
      G4cout << "Make Spot: " << a_spot.GetPosition().x() << " "
             << a_spot.GetPosition().y() << " " << a_spot.GetPosition().z()
             << " " << a_spot.GetEnergy() << " " << a_spot.GetTime() << G4endl;
#endif
      fastShowerSD()->ProcessSpot(a_spot);

#ifdef _TRACE_DOIT_
      G4cout << "Made Spot" << G4endl;
#endif

    }

#ifdef _TRACE_FSM_
    G4cout << "LArFastShower::UseShowerLib() Done" << G4endl;
#endif

    return;
  }

  // FIXME: Catching all exceptions and suppressing them? That's awful!!
  catch (const std::exception & e) {
    G4cout << "FastShower::UseShowerLib ERROR Handling an exception in LArFastShower::" << e.what() << G4endl;
    return;
  }

}

G4bool LArFastShower::CheckContainment(const G4FastTrack &fastTrack)
{

  G4ThreeVector showerDirection = fastTrack.GetPrimaryTrack()->GetMomentumDirection();
  G4ThreeVector initialShowerPosition = fastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector orthoShower = showerDirection.orthogonal();
  G4ThreeVector crossShower = showerDirection.cross(orthoShower);

#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::CheckContainment() orthoShower: " << orthoShower << G4endl;
  G4cout << "LArFastShower::CheckContainment() crossShower: " << crossShower << G4endl;
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
  G4double Z = showerLibSvc()->getContainmentZ(fastTrack,m_configuration.m_detector_tag);
  G4double R = showerLibSvc()->getContainmentR(fastTrack,m_configuration.m_detector_tag);

  if (Z == 0.0 && R == 0.0) {
    // no containment check
    return true;
  }

  // Here is OUR magic number. Looking on the hit distribution plot,
  // it seems that that way most of hits will be inside
  G4double Zmx = Z / 3;

  G4int cosPhi[4] = {1,0,-1,0};
  G4int sinPhi[4] = {0,1,0,-1};

#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::CheckContainment() R =          " << R   << G4endl;
  G4cout << "LArFastShower::CheckContainment() Z =          " << Z   << G4endl;
#endif

  G4ThreeVector position;

  G4VSolid* caloSolid = fastTrack.GetEnvelopeSolid();
  const G4AffineTransform* affineTransformation = fastTrack.GetAffineTransformation();

  //Startpoint
  position = initialShowerPosition;
  affineTransformation->ApplyPointTransform(position);
  if(caloSolid->Inside(position) == kOutside)
    return false;

  //Longitudinal Endpoint
  position = initialShowerPosition + Z*showerDirection;
  affineTransformation->ApplyPointTransform(position);
  if(caloSolid->Inside(position) == kOutside)
    return false;

  //Lateral Spread
  for(int i=0; i<4 ;i++)
    {
      position = initialShowerPosition + Zmx*showerDirection +
                 R*cosPhi[i]*orthoShower + R*sinPhi[i]*crossShower;
      affineTransformation->ApplyPointTransform(position);
      if(caloSolid->Inside(position) == kOutside)
        return false;
    }

#ifdef _TRACE_FSM_
  G4cout << "LArFastShower::CheckContainment()  Shower is contained " << G4endl;
#endif

  return true;

}

HepMC::GenEvent * LArFastShower::GetGenEvent(const G4FastTrack &fastTrack)
{
  G4ThreeVector showerPos = fastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector showerMom = fastTrack.GetPrimaryTrack()->GetMomentum();

  G4double energy = fastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4int pdgcode = fastTrack.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();
  if (pdgcode < 0) pdgcode = -pdgcode; // hack for positrons. let it be electrons.

  // new event. Signal processing = 0, event number "next"
  HepMC::GenEvent* ge = new HepMC::GenEvent( 0, ++m_eventNum);
  // vertex. Position of the shower, time = 0
  HepMC::GenVertexPtr gv = HepMC::newGenVertexPtr(
      HepMC::FourVector(showerPos.x(), showerPos.y(), showerPos.z(), 0) );
  ge->add_vertex(gv);
  // particle. FourVector of the shower, pdgcode, status = 1
  HepMC::GenParticlePtr gp = HepMC::newGenParticlePtr(
      HepMC::FourVector(showerMom.x(), showerMom.y(), showerMom.z(), energy),
      pdgcode, 1 );
  gv->add_particle_out(gp);

  // return auto_pointer. will be deleted automatically
  return ge;
}

// Helper methods
bool   LArFastShower::flagToShowerLib( const G4ParticleDefinition& particleType )   const
{
  if ( &particleType == G4Electron::ElectronDefinition() ||
       &particleType == G4Positron::PositronDefinition() ) {
    return m_configuration.m_e_FlagShowerLib;
  } else if ( &particleType == G4Gamma::GammaDefinition() ) {
    return m_configuration.m_g_FlagShowerLib;
  } else if ( &particleType == G4Neutron::NeutronDefinition() ) {
    return m_configuration.m_Neut_FlagShowerLib;
  } else if ( &particleType == G4PionPlus::PionPlusDefinition() ||
              &particleType == G4PionMinus::PionMinusDefinition() ) {
    return m_configuration.m_Pion_FlagShowerLib;
  }
  return false;
}
double LArFastShower::minEneToShowerLib( const G4ParticleDefinition& particleType ) const
{
  if ( &particleType == G4Electron::ElectronDefinition() ||
       &particleType == G4Positron::PositronDefinition() ) {
    return m_configuration.m_e_MinEneShowerLib;
  } else if ( &particleType == G4Gamma::GammaDefinition() ) {
    return m_configuration.m_g_MinEneShowerLib;
  } else if ( &particleType == G4Neutron::NeutronDefinition() ) {
    return m_configuration.m_Neut_MinEneShowerLib;
  } else if ( &particleType == G4PionPlus::PionPlusDefinition() ||
              &particleType == G4PionMinus::PionMinusDefinition() ) {
    return m_configuration.m_Pion_MinEneShowerLib;
  }
  return 0.0;
}

double LArFastShower::maxEneToShowerLib( const G4ParticleDefinition& particleType ) const
{
  if ( &particleType == G4Electron::ElectronDefinition() ||
       &particleType == G4Positron::PositronDefinition() ) {
    return m_configuration.m_e_MaxEneShowerLib;
  } else if ( &particleType == G4Gamma::GammaDefinition() ) {
    return m_configuration.m_g_MaxEneShowerLib;
  } else if ( &particleType == G4Neutron::NeutronDefinition() ) {
    return m_configuration.m_Neut_MaxEneShowerLib;
  } else if ( &particleType == G4PionPlus::PionPlusDefinition() ||
              &particleType == G4PionMinus::PionMinusDefinition() ) {
    return m_configuration.m_Pion_MaxEneShowerLib;
  }
  return 0.0;
}
bool LArFastShower::generateFSStartingPoint( const HepMC::GenEvent * ge ) const
{
  if (!m_generate_starting_points)
    return false;
  m_starting_points_file->write_event(ge);
  return true;
}
G4bool LArFastShower::ForcedAccept(const G4FastTrack & fastTrack)
{
  G4ThreeVector initialShowerPosition = fastTrack.GetPrimaryTrack()->GetPosition();

  // if ( !m_configuration.m_containHigh &&
  //     ( initialShowerPosition.eta()>=m_configuration.m_absHighEta ||
  //       initialShowerPosition.eta()<=-m_configuration.m_absHighEta ) ) return true;

  if ( !m_configuration.m_containHigh &&
      ( initialShowerPosition.eta()>m_configuration.m_absHighEta ||
        initialShowerPosition.eta()<-m_configuration.m_absHighEta ) ) return true;

  if ( !m_configuration.m_containCrack &&
      ( ( initialShowerPosition.eta()>m_configuration.m_absCrackEta1 &&
          initialShowerPosition.eta()<m_configuration.m_absCrackEta2 ) ||
        ( initialShowerPosition.eta()<-m_configuration.m_absCrackEta1 &&
          initialShowerPosition.eta()>-m_configuration.m_absCrackEta2 ) ) ) return true;

  if ( !m_configuration.m_containLow &&
      ( initialShowerPosition.eta()<m_configuration.m_absLowEta ||
        initialShowerPosition.eta()>-m_configuration.m_absLowEta ) ) return true;
  return false;
}

G4bool LArFastShower::ForcedDeny  (const G4FastTrack &)
{
  return false;
}
