/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/AmdcsimrecAccess.h"
 
Amdcsimrec* AmdcsimrecAccess::p_Amdcsimrec = 0;

AmdcsimrecAccess::AmdcsimrecAccess(){}
AmdcsimrecAccess::~AmdcsimrecAccess(){}
 
// Get
Amdcsimrec* AmdcsimrecAccess::GetAmdcsimrec(){

//  if (!p_Amdcsimrec){
//   std::cout << " AmdcsimrecAccess::GetAmdcsimrec "
//             << " can not access Amdcsimrec "
//	     << std::endl;
//  }
  return p_Amdcsimrec;

}
 
// Set
void AmdcsimrecAccess::SetAmdcsimrec(Amdcsimrec* pAmdcsimrec){

//std::cout << " AmdcsimrecAccess::SetAmdcsimrec " << std::endl;
  p_Amdcsimrec = pAmdcsimrec;

}
 
// Reset
void AmdcsimrecAccess::ResetAmdcsimrec(){p_Amdcsimrec = 0;}
 
