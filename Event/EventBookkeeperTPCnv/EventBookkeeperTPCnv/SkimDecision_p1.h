/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Header file for class SkimDecision_p1
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef SKIMDECISION_P1_H 
#define SKIMDECISION_P1_H 

#include <stdint.h>
#include <string>

class SkimDecision_p1 {
 public: 
  
  ///////////////////////////////////////////////////////////////////////
  //  This class is the persistent form of SkimDecision
  ///////////////////////////////////////////////////////////////////////
  
  SkimDecision_p1();
  ~SkimDecision_p1() = default;

  std::string m_name;
  bool m_isAccepted = true;
  
}; 


#endif //> SKIMDECISION_P1_H
