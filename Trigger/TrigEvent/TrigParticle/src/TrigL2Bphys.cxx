// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigL2Bphys.cxx

Description: To be created by hypothesis algorithms at end of Level 2 
             chains. To be used as a way to communicate L2 decisions and 
             to keep information available only online so far.
  
Design: Must be light and easy to serialize AND to store in ESD; should 
             avoid ElementLinks and try to persistify pointers instead; 
             should avoid inheritance to be easy to serialize.

Authour: J.Kirk - Wed May 10

*/

#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"


// default constructor
TrigL2Bphys::TrigL2Bphys() :
  m_roiID(-1), m_particleType(PHIKK), m_eta(0),  m_phi(0), 
  m_mass(0), m_fitmass(-99.), m_fitchi2(-99.), m_fitndof(-99),
  m_fitx(-99.), m_fity(-99.), m_fitz(-99),
  m_secondaryDecay(), m_trackVector()
{}


TrigL2Bphys::TrigL2Bphys(int roi, float eta, float phi, pType particleType,  
		     float mass, const ElementLink<TrigL2BphysContainer>& sDecay):
  m_roiID(roi),  
  m_particleType(particleType), 
  m_eta(eta),  m_phi(phi),   m_mass(mass),   
  m_fitmass(-99.), m_fitchi2(-99.), m_fitndof(-99),m_fitx(-99.),  m_fity(-99.),  m_fitz(-99.),
  m_secondaryDecay(sDecay),
  m_trackVector()
{}


TrigL2Bphys::TrigL2Bphys(int roi, float eta, float phi, pType particleType,  float mass):
  m_roiID(roi),  
  m_particleType(particleType), 
  m_eta(eta),  m_phi(phi),   
  m_mass(mass),   m_fitmass(-99.), m_fitchi2(-99.), m_fitndof(-99), m_fitx(-99.),  
  m_fity(-99.),  m_fitz(-99.),  m_secondaryDecay(), m_trackVector()
{}



// copy constructor
TrigL2Bphys::TrigL2Bphys(const TrigL2Bphys& te) 
{
  m_roiID      = te.m_roiID;
  m_particleType = te.m_particleType;
  m_eta        = te.m_eta;
  m_phi        = te.m_phi;
  m_mass      = te.m_mass;
  m_fitmass      = te.m_fitmass;
  m_fitchi2      = te.m_fitchi2;
  m_fitndof      = te.m_fitndof;
  m_fitx      = te.m_fitx;
  m_fity      = te.m_fity;
  m_fitz      = te.m_fitz;
    
  for (unsigned int i=0; i<(te.m_trackVector).size(); ++i){
    m_trackVector.push_back( (te.m_trackVector).at(i));
  }
  m_secondaryDecay     = te.m_secondaryDecay;   
}

// destructor
TrigL2Bphys::~TrigL2Bphys() 
{

}


void TrigL2Bphys::addTrack(const ElementLink<TrigInDetTrackCollection>& track)
{
   m_trackVector.push_back( track);
}

void TrigL2Bphys::pSecondDecay(ElementLink<TrigL2BphysContainer> p_secondaryDecay)
{
  m_secondaryDecay=p_secondaryDecay;
}

const ElementLink<TrigL2BphysContainer>& TrigL2Bphys::secondaryDecayLink() const
{
  return m_secondaryDecay;
}


void TrigL2Bphys::fitmass(float FitMass)
{
  m_fitmass = FitMass;
}

void TrigL2Bphys::fitchi2(float FitChi2)
{
  m_fitchi2 = FitChi2;
}

void TrigL2Bphys::fitndof(int FitNdof)
{
  m_fitndof = FitNdof;
}

void TrigL2Bphys::fitx(float FitX)
{
  m_fitx = FitX;
}
void TrigL2Bphys::fity(float FitY)
{
  m_fity = FitY;
}
void TrigL2Bphys::fitz(float FitZ)
{
  m_fitz = FitZ;
}


std::string str( const TrigL2Bphys& p )
{
  std::stringstream ss;

  ss << "RoI_ID: "<<p.roiId()
     << "Particle type: "<<p.particleType()
     << "eta: "<<p.eta()
     << "phi: "<<p.phi()
     << "mass: "<<p.mass()
     << "mass after fit: "<<p.fitmass()
     << "fit chi2: "<<p.fitchi2()
     << "fit ndof: "<<p.fitndof()
     << "x: "<<p.fitx()
     << "y: "<<p.fity()
     << "z: "<<p.fitz();
   return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigL2Bphys& p )
{
  m << str(p);
  return m;
}

bool operator== ( const TrigL2Bphys& a, const TrigL2Bphys& b )
{
  const double epsilon = 1e-8;

  int aHasSD = (a.pSecondDecay()!=NULL)?1:0;
  int bHasSD = (b.pSecondDecay()!=NULL)?1:0;
  if((aHasSD+bHasSD)==1) 
    return false;

  if(a.trackVector().size()!=b.trackVector().size())
    return false;

  if(a.roiId() != b.roiId()) return false;
  if(a.particleType() != b.particleType()) return false;
  if(a.fitndof() != b.fitndof()) return false;

  if(fabs(a.eta()-b.eta())>epsilon) return false;
  if(fabs(a.phi()-b.phi())>epsilon) return false;
  if(fabs(a.mass()-b.mass())>epsilon) return false;
  if(fabs(a.fitmass()-b.fitmass())>epsilon) return false;
  if(fabs(a.fitchi2()-b.fitchi2())>epsilon) return false;
  if(fabs(a.fitx()-b.fitx())>epsilon) return false;
  if(fabs(a.fity()-b.fity())>epsilon) return false;
  if(fabs(a.fitz()-b.fitz())>epsilon) return false;

  if((aHasSD+bHasSD)==2)
    {
      if (!((*a.pSecondDecay())==(*b.pSecondDecay()))) 
	return false;
    }
  return true;
}

void diff( const TrigL2Bphys& a, const TrigL2Bphys& b, std::map<std::string, double>& variableChange )
{
  variableChange["eta"] = a.eta() - b.eta();
  variableChange["phi"] = a.phi() - b.phi();
  variableChange["mass"] = a.mass() - b.mass();
  variableChange["fitmass"] = a.fitmass() - b.fitmass();
  variableChange["fitchi2"] = a.fitchi2() - b.fitchi2();
  variableChange["x"] = a.fitx() - b.fitx();
  variableChange["y"] = a.fity() - b.fity();
  variableChange["z"] = a.fitz() - b.fitz();
}


const TrigL2Bphys* TrigL2Bphys::pSecondDecay() const 
{
  if (m_secondaryDecay.isValid()) { 
    return *m_secondaryDecay; 
  } else {
    return NULL;
  }
}
