/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/Trig3Momentum.h"

TrigT2Jet::TrigT2Jet():
  m_e(), m_ehad0(), m_eem0(), m_eta(), m_phi(), m_grid(0), m_roiWord(), m_nLeadingCells(), m_hecf(), m_jetQuality(), m_emf(), m_jetTimeCells() 
{}
// initialise cleaining variables?

void TrigT2Jet::clearGrid(){
  std::vector<Trig3Momentum>::iterator i_begin;
  std::vector<Trig3Momentum>::iterator i_end;
  std::vector<Trig3Momentum>::iterator i_it;
  m_grid->begin() = i_begin;
  m_grid->end() = i_end;
  for( i_it = i_begin; i_it!=i_end; ++i_it){
     m_grid->erase(i_it);
  }
//  m_grid->clear();
  delete m_grid;
  m_grid = 0;
}

TrigT2Jet::~TrigT2Jet(){
  delete m_grid;
}

bool operator== ( const TrigT2Jet& a, const TrigT2Jet& b ) {

   const double DELTA=1e-3; // distance used to compare floats

  if ( std::abs(a.e() - b.e()) > DELTA)         return false;
  if ( std::abs(a.ehad0() - b.ehad0()) > DELTA) return false;
  if ( std::abs(a.eem0() - b.eem0()) > DELTA)   return false;
  if ( std::abs(a.eta() - b.eta()) > DELTA)     return false;
  if ( std::abs(a.phi() - b.phi()) > DELTA)     return false;
  if( a.RoIword() != b.RoIword() )          return false;
	// Cleaning:
  if( a.nLeadingCells() != b.nLeadingCells() )              return false;
  if( std::abs(a.hecf() - b.hecf()) > DELTA )                  return false;
  if( std::abs(a.jetQuality() - b.jetQuality()) > DELTA )      return false;
  if( std::abs(a.emf() - b.emf()) > DELTA )                    return false;
  if( std::abs(a.jetTimeCells() - b.jetTimeCells()) > DELTA )  return false;
   

   return true;
}

std::string str (const TrigT2Jet& a){

  std::string s="";
  char buff[1024];

  std::sprintf(buff,"L2 Jet E     = %10.2f MeV\n", a.e());     s += buff;
  std::sprintf(buff,"L2 Jet EHad0 = %10.2f MeV\n", a.ehad0()); s += buff;
  std::sprintf(buff,"L2 Jet EEM0  = %10.2f MeV\n", a.eem0());  s += buff;
  std::sprintf(buff,"L2 Jet Eta   = %10.2f \n",    a.eta());   s += buff;
  std::sprintf(buff,"L2 Jet Phi   = %10.2f \n",    a.phi());   s += buff;

  if(a.grid()) {
    std::sprintf(buff,"Grid Members = %4d\n", (int)a.grid()->size());  s += buff;
  }
  // Cleaning:
  // propose to not implement this at the moment.
/*  std::sprintf(buff,"L2 Jet nLeadingCells = %4d\n", a.nLeadingCells()) s+=buff;
  std::sprintf(buff,"L2 Jet hecf  = %10.2f \n",    a.hecf());  s+= buff;
  std::sprintf(buff,"L2 Jet jet Quality  = %10.2f \n",    a.jetQuality());  s+= buff;
  std::sprintf(buff,"L2 Jet emf  = %10.2f \n",    a.emf());  s+= buff;
  std::sprintf(buff,"L2 Jet jetTime  = %10.2f \n",    a.jetTimeCells());  s+= buff;
*/

  return s;

}


MsgStream& operator<< (MsgStream& m, const TrigT2Jet& d) {
  return ( m << str( d ) );
}

void diff(const TrigT2Jet& a, const TrigT2Jet& b,
	  std::map<std::string, double>& variableChange){
 const double DELTA=1e-3; // distance used to compare floats

  unsigned long aRoI = (unsigned long) a.RoIword();
  unsigned long bRoI = (unsigned long) b.RoIword();
  if( aRoI != bRoI )
    variableChange[ "RoIwordsXOR" ] = aRoI ^ bRoI;

  float aE = a.e();
  float bE = b.e();
  if( fabsf(aE - bE) > DELTA )
     variableChange[ "E" ] = aE - bE;

  float aEHad0 = a.ehad0();
  float bEHad0 = b.ehad0();
  if( fabsf(aEHad0 - bEHad0) > DELTA )
     variableChange[ "EHad0" ] = aEHad0 - bEHad0;

  float aEEM0 = a.eem0();
  float bEEM0 = b.eem0();
  if( fabsf(aEEM0 - bEEM0) > DELTA )
     variableChange[ "EEM0" ] = aEEM0 - bEEM0;

  float aEta = a.eta();
  float bEta = b.eta();
  if( fabsf(aEta - bEta) > DELTA )
     variableChange[ "Eta" ] = aEta - bEta;

  float aPhi = a.phi();
  float bPhi = b.phi();
  if( fabsf(aPhi - bPhi) > DELTA )
     variableChange[ "Phi" ] = aPhi - bPhi;
     
  // Cleaning:
  int diffLeadingCells = a.nLeadingCells() - b.nLeadingCells();
  if( diffLeadingCells != 0){
  	 variableChange[ "nLeadCells" ] = diffLeadingCells;
	}
	
  float diffhecf = a.hecf() - b.hecf();
  if( fabsf(diffhecf) > DELTA){
  	 variableChange[ "hecf" ] = diffhecf;
  }
  
  float diffjetQuality = a.jetQuality() - b.jetQuality();
  if( fabsf(diffjetQuality) > DELTA){
  	variableChange[ "jetQual" ] = diffjetQuality;
  }
  
  float diffemf = a.emf() - b.emf();
  if( fabsf(diffemf) > DELTA){
  	variableChange[ "emf" ] = diffemf;
  }
  
  float diffjetTime = a.jetTimeCells() - b.jetTimeCells();
  if( fabsf(diffjetTime) > DELTA ){
  	variableChange[ "jetTime" ] = diffjetTime;
  }
      

  return;
}
