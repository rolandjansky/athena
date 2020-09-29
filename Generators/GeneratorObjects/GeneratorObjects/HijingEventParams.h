/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTSHIJINGEVENTPARAMS_H
#define GENERATOROBJECTSHIJINGEVENTPARAMS_H 1
#include "iostream"

// --------------------------------------------------
//
// File:  GeneratorObjects/HijingEventParams.h
// Description:
//   This defines the HijingEventParams, a class to store 
//   properties of Hijing MC event 
//
// AuthorList:
//         A. Olszewski:  Initial Code December 2005


#include "AthenaKernel/CLASS_DEF.h"

class HijingEventParams
{
 public:
  HijingEventParams (void);
  HijingEventParams (int np, int nt, int n0, int n01, int n10, int n11,
		     int natt, int jatt, float b, float bphi);
  ~HijingEventParams (void);

  int   get_np()   const;
  int   get_nt()   const;
  int   get_n0()   const;
  int   get_n01()  const;
  int   get_n10()  const;
  int   get_n11()  const;
  int   get_natt()  const;
  int   get_jatt()  const;
  float get_b()    const;
  float get_bphi() const;


  float get_psi(int n) const;
  void  set_psi(int ihar,float psi);


private:
  int m_np = 0;
  int m_nt = 0;
  int m_n0 = 0;
  int m_n01 = 0;
  int m_n10 = 0;
  int m_n11 = 0;
  int m_natt = 0;
  int m_jatt = 0;
  float m_b = 0;
  float m_bphi = 0; 

  float m_psi_1 = 0,m_psi_2 = 0,m_psi_3 = 0,m_psi_4 = 0,m_psi_5 = 0,m_psi_6 = 0;
};

inline
HijingEventParams::HijingEventParams (void)
{
 // std::cout<<"BBBBBBBBBBBBBBBBBB  "<<this<<std::endl;  
}

inline
HijingEventParams::HijingEventParams (int np, int nt, 
				      int n0, int n01, int n10, int n11,
				      int natt, int jatt, float b, float bphi)
{ 
  m_np = np; m_nt = nt; m_n0 = n0; m_n01 = n01; m_n10 = n10; m_n11 = n11;
  m_natt = natt; m_jatt = jatt; m_b = b; m_bphi = bphi;

  m_psi_1=0;m_psi_2=0;m_psi_3=0;m_psi_4=0;m_psi_5=0;m_psi_6=0;
  //std::cout<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA "<<this<<std::endl;  
}

inline
HijingEventParams::~HijingEventParams (void)
{}

inline  int HijingEventParams::get_np() const { return m_np; }
inline  int HijingEventParams::get_nt() const { return m_nt; }
inline  int HijingEventParams::get_n0() const { return m_n0; }
inline  int HijingEventParams::get_n01() const { return m_n01; }
inline  int HijingEventParams::get_n10() const { return m_n10; }
inline  int HijingEventParams::get_n11() const { return m_n11; }
inline  int HijingEventParams::get_natt() const { return m_natt; }
inline  int HijingEventParams::get_jatt() const { return m_jatt; }
inline  float HijingEventParams::get_b() const { return m_b; }
inline  float HijingEventParams::get_bphi() const { return m_bphi; }


inline  float HijingEventParams::get_psi(int ihar) const 
{ 
  //std::cout<<"1111111111111111111111111  "<<this<<std::endl;  
    if( ihar==1) return m_psi_1;
    if( ihar==2) return m_psi_2;
    if( ihar==3) return m_psi_3;
    if( ihar==4) return m_psi_4;
    if( ihar==5) return m_psi_5;
    if( ihar==6) return m_psi_6;
    return 0; 
}

inline  void HijingEventParams::set_psi(int ihar, float psi)
{ 
    if(ihar==1) m_psi_1=psi;
    if(ihar==2) m_psi_2=psi;
    if(ihar==3) m_psi_3=psi;
    if(ihar==4) m_psi_4=psi;
    if(ihar==5) m_psi_5=psi;
    if(ihar==6) m_psi_6=psi;
}

CLASS_DEF(HijingEventParams, 54790518, 2)

#endif
