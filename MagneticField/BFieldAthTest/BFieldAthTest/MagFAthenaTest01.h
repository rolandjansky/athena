/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  30/03/00
//
// The MagFAthenaTest01 class
// is an Algorithm class to test MagFieldAthenaSvc
//
#ifndef MAGFATHENATEST01
#define MAGFATHENATEST01

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "BFieldAth/MagFieldAthena.h"


class IMagFieldAthenaSvc;
class MagFAthenaTest01:public Algorithm {
public:
    MagFAthenaTest01(const std::string& name, ISvcLocator* pSvcLocator);

public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////

// Dumpy01
   int  DoneDumpy01 ;
   void Dumpy01();

//Data
// Pointer On MagFieldAthenaSvc
   ServiceHandle<IMagFieldAthenaSvc> p_MagFieldAthenaSvc;
   int    m_ca    ;
   int    m_nx    ;
   int    m_ny    ;
   int    m_nz    ;
   int    m_nr    ;
   int    m_nphi  ;
   int    m_nstx  ;
   int    m_nsty  ;
   int    m_nstz  ;
   int    m_nstr  ;
   double m_xmax  ;
   double m_xmin  ;
   double m_ymax  ;
   double m_ymin  ;
   double m_zmax  ;
   double m_zmin  ;
   double m_rmax  ;
   double m_rmin  ;
   double m_phimax;
   double m_phimin;    

};

#endif

