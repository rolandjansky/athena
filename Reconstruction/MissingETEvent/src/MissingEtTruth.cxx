/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtTruth.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  P. Loch, S. Resconi
CREATED:  Oct 2004

PURPOSE:  
********************************************************************/
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "MissingETEvent/MissingEtRegions.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "TruthHelper/GenAccessIO.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace TruthHelper; 

MissingEtTruth::MissingEtTruth() : MissingET(MissingET::Truth)
{
  this->setup();
}

MissingEtTruth::MissingEtTruth(MissingEtRegions* aRegion)
  : MissingET(MissingET::Truth,aRegion)
{
  this->setup();
}

MissingEtTruth::MissingEtTruth(MissingET::Source aSource, 
			     MissingEtRegions* aRegion)
  : MissingET(aSource,aRegion)
{ 
  this->setup();
}

MissingEtTruth::~MissingEtTruth()
{ 
  this->setup();
}

void MissingEtTruth::addPart(HepMC::ConstGenParticlePtr Part, double etaFull)
{
  
  double weight = 1.;
  double exp  = (Part)->momentum().px();
  double eyp  = (Part)->momentum().py();
  double etp  = (Part)->momentum().perp();
  double etap = (Part)->momentum().pseudoRapidity();
  int idp    = (Part)->pdg_id();
 
  TruthIndex thePart = NotValid;
  IsGenNonInteracting nonint;
 
  if ( nonint.operator()(Part) )  // if IsGenNonInteracting returns true
  {  
    thePart = NonInt;    // all NON interacting particles (neutrinos + SUSY particles)
    m_exTruth[NonInt]	 += exp;    
    m_eyTruth[NonInt]	 += eyp;    
    m_etSumTruth[NonInt] += etp; 
    
  } else {                 
    if (abs(idp) != 13)  // all interacting particles and NO muons 
    {
      if( fabs(etap) <= 5.) {
        // update the global sums and eta regions in eta <= 5
        this->add(exp, eyp, etp, etap, weight);
         m_exTruth[Int]	   -= exp;    
         m_eyTruth[Int]	   -= eyp;    
         m_etSumTruth[Int] += etp;   
	//thePart = Int;
      }                          
      if( fabs(etap) <= etaFull)                    thePart = IntCentral;     
      if( fabs(etap) > etaFull && fabs(etap) <= 5.) thePart = IntFwd; 
      if( fabs(etap) > 5.)                          thePart = IntOutCover;		 
    } else {
      //if (etp > 6000. && fabs(etap) <= 2.5)   //  select  "measurable" muons
      //{
        thePart = Muons; //  all muons
      //}
    }
    
   if (thePart != NotValid) { // Can be == if etap is a nan !! Fixed by Scott Snyder
     m_exTruth[(size_t)thePart]	-= exp;    
     m_eyTruth[(size_t)thePart]	-= eyp;    
     m_etSumTruth[(size_t)thePart] += etp;
   }
  
  }  
  
     
    
}


//set methods

void MissingEtTruth::setExTruth(MissingEtTruth::TruthIndex theTruth, 
			      double theEx)
{
  if ( theTruth < Size ) m_exTruth[(size_t)theTruth] = theEx;
}


void MissingEtTruth::setEyTruth(MissingEtTruth::TruthIndex theTruth, 
			      double theEy)
{
  if ( theTruth < Size ) m_eyTruth[(size_t)theTruth] = theEy;
}


void MissingEtTruth::setEtSumTruth(MissingEtTruth::TruthIndex theTruth, 
			         double theEtSum)
{
  if ( theTruth < Size ) m_etSumTruth[(size_t)theTruth] = theEtSum;
  
}


void MissingEtTruth::setExTruthVec(std::vector<double>&& exTruthVec)
{
  assert (exTruthVec.size() == Size);
  m_exTruth = std::move (exTruthVec);
}


void MissingEtTruth::setEyTruthVec(std::vector<double>&& eyTruthVec)
{
  assert (eyTruthVec.size() == Size);
  m_eyTruth = std::move (eyTruthVec);
}


void MissingEtTruth::setEtSumTruthVec(std::vector<double>&& etSumTruthVec)
{
  assert (etSumTruthVec.size() == Size);
  m_etSumTruth = std::move (etSumTruthVec);
}


// get methods

double MissingEtTruth::exTruth(MissingEtTruth::TruthIndex theTruth) const
{
  return m_exTruth[theTruth] ;
}

double MissingEtTruth::eyTruth(MissingEtTruth::TruthIndex theTruth) const
{
  return m_eyTruth[theTruth] ;
}

double MissingEtTruth::etSumTruth(MissingEtTruth::TruthIndex theTruth) const
{
  return m_etSumTruth[theTruth] ;
}


const std::vector<double>& MissingEtTruth::exTruthVec() const
{
  return m_exTruth;
}


const std::vector<double>& MissingEtTruth::eyTruthVec() const
{
  return m_eyTruth;
}


const std::vector<double>& MissingEtTruth::etSumTruthVec() const
{
  return m_etSumTruth;
}


void MissingEtTruth::setup()
{
  // initialize data members
  
  m_exTruth.resize(MissingEtTruth::Size,0.);
  m_eyTruth.resize(MissingEtTruth::Size,0.);
  m_etSumTruth.resize(MissingEtTruth::Size,0.);
}

