/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCLUSTERREC_LARTOWERSEG_H
#define LARCLUSTERREC_LARTOWERSEG_H
/**

@class      LArTowerSeg
@brief    Store segmentation for the tower configuration.

          Constrcutor takes segmentation parameters. 
	  Public methods for accessing the limits, and tower sizes,
	  and check bounds. 
	  
	  All methods implemented inline. 


PACKAGE:  offline/LArCalorimeter/LArClusterRec

@author   H. Ma, S. Rajagopalan
@date     Sept, 2000 


Updated  Nov 29,2000  (SR)
          add index to real space conversion for eta phi

Updated   Jan 3, 2001  (HM)
          QA. 


********************************************************************/

// INCLUDE HEADER FILES:


class LArTowerSeg 
{
  public:

  // Constructor 
  LArTowerSeg(int neta, int nphi, float etamin, float etamax,
        float phimin=0, float phimax=6.2831854):
       m_neta(neta),m_nphi(nphi),m_etamin(etamin),m_etamax(etamax),
       m_phimin(phimin),m_phimax(phimax) 
  {
          m_deta = (etamax-etamin)/neta; 
          m_dphi = (phimax-phimin)/nphi; 
  }; 

  // Destructor 
  ~LArTowerSeg ()
    { 
    
    };
 
  // Segmentation parameters. 
  int neta() const;
  int nphi() const;
  float deta() const;
  float dphi() const;
  float etamin() const; 
  float etamax() const; 
  float phimin() const; 
  float phimax() const; 

  // check bounds
  bool  inbound(int eta,   int phi) const; 
  bool  inbound(float eta, float phi) const; 

  // convert to a combined index 
  int etaphi(int,int) const;  

  // return eta in real space 
  double  eta(int) const;    
  // return phi in real space 
  double  phi(int) const;    
  

 private:
  // --------------------------
  // Segmentation parameters 
  // ------------------------

  // number of towers 
  int   m_neta;    
  int   m_nphi; 

  // tower size
  float m_deta ;  
  float m_dphi ;

  // limits 
  float m_etamin; 
  float m_etamax;
  float m_phimin;
  float m_phimax;
  
  
};

// ----------------------------------------
// returns parameters
// ----------------------------------------
inline int LArTowerSeg::neta() const
{
	return m_neta; 
}

inline int LArTowerSeg::nphi() const
{
	return m_nphi; 
}

inline float LArTowerSeg::deta() const
{
  return m_deta;
}

inline float LArTowerSeg::dphi() const
{
  return m_dphi;
}

inline float LArTowerSeg::etamin() const 
{
  return m_etamin;
}

inline float LArTowerSeg::etamax() const 
{
  return m_etamax;
}

inline float LArTowerSeg::phimin() const 
{
  return m_phimin;
}

inline float LArTowerSeg::phimax() const 
{
  return m_phimax;
}

inline bool LArTowerSeg::inbound(int eta, int phi) const 
{
     if( eta>0 && eta<=m_neta && phi>0 && phi<=m_nphi) return true; 
     return false; 
}

inline bool LArTowerSeg::inbound(float eta, float phi) const 
{
     if(eta>=m_etamin && eta<=m_etamax && phi>=m_phimin && phi<=m_phimax)
         return true; 
     return false; 
}


// combined index 
inline int LArTowerSeg::etaphi(int eta,int phi) const
{
	return eta+(phi-1)*m_neta -1; 
}

// index to real space conversion.
inline double LArTowerSeg::eta(int ieta) const 
{
 return    m_etamin + (ieta-0.5)*m_deta ; 
}

inline double LArTowerSeg::phi(int  iphi) const 
{
 return    m_phimin + (iphi-0.5)*m_dphi ; 
}


#endif 
