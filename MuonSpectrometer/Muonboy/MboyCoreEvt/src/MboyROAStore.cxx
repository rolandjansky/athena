/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyROAStore.h"

MboyROAStore::MboyROAStore(){}
MboyROAStore::~MboyROAStore(){}

// Reset the Store
void MboyROAStore::Reset(){MboyROASet.clear();}
 
// Add a MboyROASet
void MboyROAStore::AddROA(MboyROA& ToBeAdded){MboyROASet.push_back(ToBeAdded);}

void MboyROAStore::AddROA(
            const double& ROAThe ,
            const double& ROAPhi ,
            const double& ROAR3d 
){
  MboyROA aMboyROA(
                   ROAThe , 
                   ROAPhi ,
                   ROAR3d 
                  );
  AddROA(aMboyROA);
}
void MboyROAStore::AddROA(
         const double& Point_x ,
         const double& Point_y ,
         const double& Point_z ,
         const double& Direc_x ,
         const double& Direc_y ,
         const double& Direc_z ,
         const double& DeltPhi ,
         const double& DeltEta  
){
  MboyROA aMboyROA(
                   Point_x , 
                   Point_y ,
                   Point_z ,
                   Direc_x ,
                   Direc_y ,
                   Direc_z ,
                   DeltPhi ,
                   DeltEta  
                  );
  AddROA(aMboyROA);
}

void MboyROAStore::AddROA(
         const double& Point_x ,
         const double& Point_y ,
         const double& Point_z ,
         const double& Direc_x ,
         const double& Direc_y ,
         const double& Direc_z 
){
  MboyROA aMboyROA(
                   Point_x , 
                   Point_y ,
                   Point_z ,
                   Direc_x ,
                   Direc_y ,
                   Direc_z 
                  );
  AddROA(aMboyROA);
}
 
// Nber of MboyROASet
int MboyROAStore::NberOfObjects() const{return MboyROASet.size();}
 
// Get a Pointer on a MboyROASet of Nber MboyROASetNber
const MboyROA* MboyROAStore::GetMboyROA(int MboyROASetNber) const{
 if (MboyROASetNber < 0 || MboyROASetNber > NberOfObjects()) return 0;
 return &MboyROASet[MboyROASetNber];
}
