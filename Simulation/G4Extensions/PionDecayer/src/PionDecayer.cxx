/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PionDecayer/PionDecayer.h"
#include "G4LorentzVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

#include <CLHEP/Random/RandFlat.h>

#include "MCTruth/PrimaryParticleInformation.h"

static PionDecayer filter("PionDecayer");

double PionDecayer::alenz=350.*CLHEP::cm;
double PionDecayer::cvrad=115.*CLHEP::cm;
double PionDecayer::ptmuon=1.*CLHEP::GeV;
bool   PionDecayer::m_useCustom=false;

PionDecayer::PionDecayer(std::string s): FADS::G4EventAnalyzer(s), 
  verboseLevel(1), 
  m_motherId(0)
{
  // 	TurnOn();
  m_childrenId = new std::vector<int>;
}

PionDecayer::~PionDecayer()
{
	if (m_childrenId) delete m_childrenId;
}
void PionDecayer::SetIDETlength(double l)
{
	alenz=l;
}
void PionDecayer::SetIDETradius(double r)
{
	cvrad=r;
}
void PionDecayer::SetPtmuon(double s)
{
	ptmuon=s;
}

void PionDecayer::UseCustomDecayMode()
{
        m_useCustom=true;
}

void PionDecayer::SetMother(int id)
{
        m_motherId = id;
}

void PionDecayer::AddDecayProduct(int id)
{
       m_childrenId->push_back(id);
}

void PionDecayer::EventInitialization()
{
	 std::cout << " Pion Decayer :EventInitialization() called here "<<std::endl;
}

void PionDecayer::EventAnalysis(std::vector<G4PrimaryVertex*>& /*vert*/, std::vector<G4PrimaryParticle*>& part) {
  std::cout <<" Pion Decayer :EventAnalysis called here "<<std::endl;
  std::cout <<" ptmuon cut set to "<<ptmuon<< " MeV "<<std::endl;
  std::cout <<" fiducial volume dimensions "<<alenz<<" "<<cvrad<<" mm "<<std::endl;

  std::vector<G4PrimaryParticle*> particles;
  std::vector<G4PrimaryParticle*> particles2;
  
  bool caseSelect=false;
  if (part.size()) {
    std::cout << "++++ number of particles in the event ++++ "<<part.size()<<std::endl;
    
    /// first loop to take care of those cases where the particle to decay was already
    /// chosen at the generator level
    if ( m_useCustom ) {
      std::cout << "++++ Selected decay channel ++++" << std::endl;
      std::cout <<"  Mother id: " << m_motherId << std::endl;
      for (unsigned int i=0 ; i<m_childrenId->size() ; i++ )
	std::cout <<"  Child id:  " << (*m_childrenId)[i] << std::endl;
      
      particles2 = customDecayMode( part );
      if (particles2.size()) caseSelect=true;
    }

    /// second loop
    /// if no particles were selected on the generator level, pick a particle to decay
    if (!caseSelect) {
      for (unsigned int i=0;i<part.size();i++) {
	G4PrimaryParticle *p=part[i];
	int pdgcode=p->GetPDGcode();
	double p_X=p->GetPx();
	double p_Y=p->GetPy();
	double ptmod=sqrt(p_X*p_X+p_Y*p_Y);
	if((ptmod>ptmuon) && ((abs(pdgcode) ==211) || (abs(pdgcode) ==321)))
	  particles.push_back(part[i]);	
      }
      if (!particles.size()) {
	std::cout << " no particles found that pass the threshold:  aborting the event!"<<std::endl;
	G4Event *ev=const_cast<G4Event *>(G4RunManager::GetRunManager()->GetCurrentEvent());
	ev->SetEventAborted();
	return;
      }
      std::cout << "++++ number of muons and kaons in the event ++++ "<<particles.size()<<std::endl;
      double rn=CLHEP::RandFlat::shoot();
      unsigned int i=int(rn*particles.size());
      particles2.push_back(particles[i]);
    }


    /// Do the decaying
    std::cout << "Number of particles to decay: " << particles2.size() << std::endl;
    for (unsigned int i=0;i<particles2.size();i++) {
      G4PrimaryParticle *pa=particles2[i];
   
      /// flag particle by setting status=9999
      HepMC::GenParticle *hPart=const_cast<HepMC::GenParticle *>(((PrimaryParticleInformation*)(pa->GetUserInformation()))->GetHepMCParticle());
      hPart->set_status(9999);
      
      double p_X=pa->GetPx();
      double p_Y=pa->GetPy();
      double p_Z=pa->GetPz();
      double mass=pa->GetG4code()->GetPDGMass();
      double pmod=sqrt(p_X*p_X+p_Y*p_Y+p_Z*p_Z);
      double ptmod=sqrt(p_X*p_X+p_Y*p_Y);
      double E=sqrt(p_X*p_X+p_Y*p_Y+p_Z*p_Z + mass*mass);
      G4LorentzVector vL(G4ThreeVector(p_X,p_Y,p_Z),E);
      double beta = vL.beta();
      double gamma = vL.gamma();
      double betaGamma=beta*gamma;
      double thetac=atan(cvrad/alenz);
      double thetap=atan(ptmod/fabs(p_Z));
      double decayL=std::min(cvrad/sin(thetap),alenz/cos(thetap));	
      double randNum=CLHEP::RandFlat::shoot();
      decayL = decayL*randNum;
      
      std::cout <<" mass "<<mass<<std::endl;
      std::cout <<" pmod "<<pmod<<std::endl;
      std::cout <<" decayL "<<decayL<<std::endl;
      std::cout <<" thetac "<<thetac<<" decayL "<<decayL<<" ptmod "<<ptmod<<std::endl;
      std::cout <<" ratio "<<mass/pmod<<std::endl;
      std::cout <<" beta,gamma,betaGamma "<<beta<<" "<<gamma<<" "<<betaGamma<<std::endl;
      std::cout <<" set c*tau to (decayL/betaGamma) "<<decayL/betaGamma<<std::endl;
      
      pa->SetProperTime(decayL/(betaGamma*CLHEP::c_light));
    }
  }
}
  


std::vector<G4PrimaryParticle*> PionDecayer::customDecayMode( std::vector<G4PrimaryParticle*>& part) {
  std::vector<G4PrimaryParticle*> selectedParticles;
  bool particleInList=false;

  for (unsigned int i=0;i<part.size();i++) {
    G4PrimaryParticle *p=part[i];

    /// Check if the current particle is in the list of children pdgIds
    int pdgId=((PrimaryParticleInformation*)(p->GetUserInformation()))->GetHepMCParticle()->pdg_id();
    for ( unsigned int j=0 ; j<m_childrenId->size() ; j++ )
      if ( pdgId==(*m_childrenId)[j] )   particleInList=true;


    if ( particleInList ) {
      const HepMC::GenVertex* vertex = ((PrimaryParticleInformation*)(p->GetUserInformation()))->GetHepMCParticle()->production_vertex();
      HepMC::GenVertex::particles_in_const_iterator mother = vertex->particles_in_const_begin();
      if ( *mother!=NULL ) {
	/// Check if the particle come from decay of the correct particle. 
	/// If it does, check that the rest of the decay products are the correct ones.
	int id = (*mother)->pdg_id();
	if (id==m_motherId) {
	  unsigned int nbrMatch  = 0;
	  HepMC::GenVertex::particles_out_const_iterator partItr  = vertex->particles_out_const_begin();
	  HepMC::GenVertex::particles_out_const_iterator partItrE = vertex->particles_out_const_end();
	  for ( ; partItr!=partItrE ; partItr++ ) {
	    for ( unsigned int j=0 ; j<m_childrenId->size() ; j++ ) {
	      if ( (*partItr)->pdg_id()==(*m_childrenId)[j] ) {
            nbrMatch++;
	      }
	    }
	  }
	  /// If all decay products are matched to specified children, this is the correct decay.
	  /// If this was a pion or a kaon, put the particle in the list of particles which should
	  /// be decayed by PionDecayer
	  if ( nbrMatch==m_childrenId->size() && ( std::abs(pdgId)==211 || std::abs(pdgId)==321 ) ) {
	    std::cout << "adding " << pdgId << " to decay list" << std::endl;
	    selectedParticles.push_back(part[i]);
	  }
	}
      }
    }
  }

  return selectedParticles;
}

