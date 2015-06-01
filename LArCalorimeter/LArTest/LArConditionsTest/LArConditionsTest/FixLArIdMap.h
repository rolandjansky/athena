/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FIXLARIDMAP_H
#define FIXLARIDMAP_H

#include "GaudiKernel/Algorithm.h"
#include <string>

#include "LArTools/LArOnOffId_P.h"

class FixLArIdMap : public Algorithm
{
 public:
  FixLArIdMap(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~FixLArIdMap();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

  StatusCode fix1();  
  StatusCode fix2();  
  StatusCode fix3();  
  StatusCode fix4();  
  StatusCode fix5();  
  StatusCode fix6();  
  StatusCode fix7();  
  StatusCode fix8();  
  StatusCode fix9();  
  StatusCode fix10();  
  StatusCode fix11();  
  StatusCode fix12();  


  void dumpId(); 
 private: 
  int  m_fixFlag ; 
  bool m_dumpIds ;
  std::string m_febToRodMap; 

};

bool operator < (const LArOnOffId_P::LArOnOffId_P_t & t1,
		   const LArOnOffId_P::LArOnOffId_P_t & t2) ; 

#endif // FixLArIDMAP_H

