/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcCore/AmdcsimrecAccess.h"
 
Amdcsimrec* AmdcsimrecAccess::s_Amdcsimrec = 0;

AmdcsimrecAccess::AmdcsimrecAccess(){}
AmdcsimrecAccess::~AmdcsimrecAccess(){}
 
// Get
Amdcsimrec* AmdcsimrecAccess::GetAmdcsimrec(){

//  if (!s_Amdcsimrec){
//   std::cout << " AmdcsimrecAccess::GetAmdcsimrec "
//             << " can not access Amdcsimrec "
//	     << std::endl;
//  }
  return s_Amdcsimrec;

}
 
// Set
void AmdcsimrecAccess::SetAmdcsimrec(Amdcsimrec* pAmdcsimrec){

//std::cout << " AmdcsimrecAccess::SetAmdcsimrec " << std::endl;
  s_Amdcsimrec = pAmdcsimrec;

}
 
// Reset
void AmdcsimrecAccess::ResetAmdcsimrec(){s_Amdcsimrec = 0;}
 
