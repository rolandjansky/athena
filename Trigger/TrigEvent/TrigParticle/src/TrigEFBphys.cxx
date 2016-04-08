// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigEFBphys.cxx

Description: To be created by hypothesis algorithms at end of Level 2 
             chains. To be used as a way to communicate EF decisions and 
             to keep information available only online so far.
  
Design: Must be light and easy to serialize AND to store in ESD; should 
             avoid ElementLinks and try to persistify pointers instead; 
             should avoid inheritance to be easy to serialize.

Authour: J.Kirk - Wed May 10

*/

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

// default constructor
TrigEFBphys::TrigEFBphys() :
  m_roiID(-1),  m_particleType(PHIKK), m_eta(0),  m_phi(0) ,
  m_mass(0), m_fitmass(-99.), m_fitchi2(-99.), m_fitndof(-99),
  m_fitx(-99.), m_fity(-99.), m_fitz(-99),
  m_secondaryDecay(), m_trackVector(0)
{}

// constructors

TrigEFBphys::TrigEFBphys(int roi, float eta, float phi, pType particleType,  
		     float mass):
  m_roiID(roi),
  m_particleType(particleType), 
  m_eta(eta),  m_phi(phi),  m_mass(mass),  
  m_fitmass(-99.), m_fitchi2(-99.), m_fitndof(-99),m_fitx(-99.),  m_fity(-99.),  m_fitz(-99.),
  m_secondaryDecay(), m_trackVector(0)
{}

TrigEFBphys::TrigEFBphys(int roi, float eta, float phi, pType particleType,  
		     float mass,  const ElementLink<TrigEFBphysContainer>& sDecay):
  m_roiID(roi),  
  m_particleType(particleType), 
  m_eta(eta),  m_phi(phi),  m_mass(mass),  
  m_fitmass(-99.), m_fitchi2(-99.), m_fitndof(-99),m_fitx(-99.),  m_fity(-99.),  m_fitz(-99.),
  m_secondaryDecay(sDecay), m_trackVector(0)
{}


// destructor
TrigEFBphys::~TrigEFBphys() 
{
}

void TrigEFBphys::addTrack(const ElementLink<Rec::TrackParticleContainer>& track)
{
   m_trackVector.push_back( track);
}

void TrigEFBphys::pSecondDecay(ElementLink<TrigEFBphysContainer> p_secondaryDecay)
{
  m_secondaryDecay=p_secondaryDecay;
}


void TrigEFBphys::fitmass(float FitMass)
{
  m_fitmass = FitMass;
}

void TrigEFBphys::fitchi2(float FitChi2)
{
  m_fitchi2 = FitChi2;
}

void TrigEFBphys::fitndof(int FitNdof)
{
  m_fitndof = FitNdof;
}

void TrigEFBphys::fitx(float FitX)
{
  m_fitx = FitX;
}
void TrigEFBphys::fity(float FitY)
{
  m_fity = FitY;
}
void TrigEFBphys::fitz(float FitZ)
{
  m_fitz = FitZ;
}

MsgStream& operator<< ( MsgStream& m, const TrigEFBphys& p )
{
  m << str(p);
  return m;
}

std::string str( const TrigEFBphys& p )
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

bool operator== ( const TrigEFBphys& a, const TrigEFBphys& b )
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

void diff( const TrigEFBphys& a, const TrigEFBphys& b, std::map<std::string, double>& variableChange )
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


/** accessor method: secondary decay particle */
const TrigEFBphys* TrigEFBphys::pSecondDecay() const { 
  if (m_secondaryDecay.isValid()) { 
    return *m_secondaryDecay; 
  } else {
    return NULL;
  }
}


const ElementLink< TrigEFBphysContainer>& TrigEFBphys::secondaryDecayLink() const
{
  return m_secondaryDecay;
}

