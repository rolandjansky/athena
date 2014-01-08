/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyExtROALoader.h"

#include "MboyCoreEvt/MboyROAStore.h"

//*Wrapped Fortran routine
extern "C" void resetextroa_();
extern "C" void loadextroa_(double& ExtROATheIN, double& ExtROAPhiIN, double& ExtROAR3dIN );


MuonboyExtROALoader::MuonboyExtROALoader(){}

MuonboyExtROALoader::~MuonboyExtROALoader(){}

void MuonboyExtROALoader::initialize(){}


void MuonboyExtROALoader::execute(MboyROAStore* pMboyROAExternalStore)
{

 Reset();
 if (!pMboyROAExternalStore) return ;
 int itemMax = pMboyROAExternalStore->NberOfObjects() ;
 for (int item= 0 ; item <itemMax ; item++){
   const MboyROA* pMboyROA = pMboyROAExternalStore->GetMboyROA(itemMax) ;
   double ExtROATheIN = pMboyROA->Get_ROAThe() ;
   double ExtROAPhiIN = pMboyROA->Get_ROAPhi() ;
   double ExtROAR3dIN = pMboyROA->Get_ROAR3d() ;
   loadextroa_(ExtROATheIN, ExtROAPhiIN, ExtROAR3dIN );
 }
 
}     
void MuonboyExtROALoader::Reset()
{
 resetextroa_();
}     

void MuonboyExtROALoader::finalize(){}
