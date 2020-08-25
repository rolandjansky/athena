/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <iomanip>
#include <math.h>
#include "CosmicGenerator/CosmicGun.h"

//--------------------------------------------------------
//
// These are the fortran subroutines
//
extern "C"{
  void cosmic2_(void);
  void cosipr_(void);
  void cosgin_(void);
  void cosgen_(float* emin, float* emax, int* iacc);
}
//--------------------------------------------------------
//
// and the common blocks
//
struct genpar {
  float LBINWID, LEMIN, LEMAX;
  int   NBIN;
  float PROBE[100];
};
alignas(32) genpar genpar_;

struct coscut {
  float ctcut;
};
extern coscut coscut_;

struct cosevt{
  float ENER, COSTH, PHI, CHRG;
};
extern cosevt cosevt_;

struct flxout{
  float FLUX, FLUX2;
};
extern flxout flxout_;

//--------------------------------------------------------
//
// singleton pattern
//
CosmicGun* CosmicGun::s_mpointer = 0;

CosmicGun* CosmicGun::GetCosmicGun(void){
  if(!s_mpointer) s_mpointer = new CosmicGun();
  return s_mpointer;
}


//--------------------------------------------------------
//
// constructor with some default settings
//
// 9 August 2005, RMcP: remove initialization from constructor.
CosmicGun::CosmicGun(void){
  m_event    = 0;
  m_emin     = 50;
  m_emax     = 500;
  m_coscut   = 0.35;
  m_printevt = 20;
  m_printmod = 50;

  coscut_.ctcut = m_coscut;
  genpar_.LEMIN = std::log10(m_emin);
  genpar_.LEMAX = std::log10(m_emax);
  genpar_.NBIN  = 100;
  genpar_.LBINWID = (genpar_.LEMAX-genpar_.LEMIN)/genpar_.NBIN;

  // cosipr_();
  // cosgin_();

}

// Add separate generator initialization routine
// to avoid forced default initialization, RMcP 9 Aug 05.
float CosmicGun::InitializeGenerator()  {
  std::cout << " CosmicGun::InitializeGenerator: E(min,max)=(" << m_emin << "," << m_emax
            << ") GeV, and cos(ThetaCut)="<< m_coscut << std::endl;
  cosipr_();
  cosgin_();
  cosmic2_();
  return flxout_.FLUX2;
}

void CosmicGun::PrintLevel(int printevt,int printmod){
  if (printevt >= 0)
    {
      m_printevt = printevt;
    }
  else
    {
      std::cerr << "CosmicGun::PrintLevel - warning ignored input printevt = " << printevt << std::endl; 
    }
  if (printmod >= 1)
    {
      m_printmod = printmod;
    }
  else
    {
      std::cerr << "CosmicGun::PrintLevel - warning ignored input printmod = " << printmod << std::endl; 
    }
}

CLHEP::HepLorentzVector CosmicGun::GenerateEvent(void){
  int iacc = 0;

  while(iacc == 0){
    cosgen_(&m_emin, &m_emax, &iacc);
  }
  m_event++;
  
  float sinth = std::sqrt( 1-std::pow(cosevt_.COSTH,2) );
  float e  = cosevt_.ENER;
  float px = cosevt_.ENER * sinth * std::sin( cosevt_.PHI);
  float py = cosevt_.ENER * sinth * std::cos( cosevt_.PHI);
  float pz = cosevt_.ENER * cosevt_.COSTH;
  CLHEP::HepLorentzVector p(px,py,pz,e);

//   if(m_event < m_printevt || m_event%m_printmod == 0)
//     {
//       	std::cout << "CosmicGun::GenerateEvent: " << std::setw(4) << m_event 
//         	  << " muon charge " << std::setw(2) << cosevt_.CHRG << " with momentum : " << p << std::endl;
//     }

  return p;
}

int CosmicGun::GetMuonCharge(void){
  return (int)cosevt_.CHRG;
}

void CosmicGun::SetEnergyRange(float emin, float emax){
  if(emin >= emax || emin < 0 )
    {
      std::cout << "Error input energy range : (" << emin << " - " << emax << ") - ignored " << std::endl;
      return;
    }
  m_emin = emin;
  m_emax = emax;

  genpar_.LEMIN = std::log10(m_emin);
  genpar_.LEMAX = std::log10(m_emax);
  genpar_.NBIN  = 100;
  genpar_.LBINWID = (genpar_.LEMAX-genpar_.LEMIN)/genpar_.NBIN;

}

void CosmicGun::SetCosCut(float ctcut){
  m_coscut = ctcut;

  coscut_.ctcut = m_coscut;
}
  
