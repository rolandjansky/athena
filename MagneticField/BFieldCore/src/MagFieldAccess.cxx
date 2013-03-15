/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  17/04/00
//
// MagFieldAccess is a monostate class providing an static access
// a AbstractMagneticField
//
#include "BFieldCore/MagFieldAccess.h"
 
AbstractMagneticField* MagFieldAccess::p_AbstractMagneticField = 0;
MagFieldAccess::MagFieldAccess(){}
 
MagFieldAccess::~MagFieldAccess(){}
 
// Get
AbstractMagneticField* MagFieldAccess::GetAbstractMagneticField(){
//
  if (!p_AbstractMagneticField){
   std::cout << " MagFieldAccess::GetAbstractMagneticField "
             << " can not access AbstractMagneticField "
	     << std::endl;
  }
  return p_AbstractMagneticField;
//
}
 
// Set
void MagFieldAccess::SetAbstractMagneticField(AbstractMagneticField* pAbstractMagneticField){
//
//std::cout << " MagFieldAccess::SetAbstractMagneticField " << std::endl;
  p_AbstractMagneticField = pAbstractMagneticField;
//
}
 
// Reset
void MagFieldAccess::ResetAbstractMagneticField(){p_AbstractMagneticField = 0;}
 
