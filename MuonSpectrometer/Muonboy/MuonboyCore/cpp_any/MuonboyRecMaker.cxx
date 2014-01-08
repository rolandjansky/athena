/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyRecMaker.h"
   
#include "MboyCoreEvt/MuonRecDigiStore.h"
#include "MboyCoreEvt/MB_TrackStore.h"
#include "MboyCoreEvt/MB_SegmentStore.h"
#include "MboyCoreEvt/MuonboyBFAbstract.h"
#include "MboyCoreEvt/MuonboyRTAbstract.h"

#include "MuonboyCore/MB_TrackStoreMaker.h"
#include "MuonboyCore/MB_SegmentStoreMaker.h"

#include "MuonboyCore/MuonboyBFAccess.h"
#include "MuonboyCore/MuonboyRTAccess.h"
#include "MuonboyCore/MuonboyRTc.h"

#include "AmdcStand/loadamdcagddfromstring.h"
#include "inirunendmuonboy.h"

MuonboyRecMaker::MuonboyRecMaker(){
 p_MB_SegmentStoreMaker = 0 ;
 p_MB_TrackStoreMaker = 0 ;
}

MuonboyRecMaker::~MuonboyRecMaker(){}

void MuonboyRecMaker::initialize(     
     std::string AmdcString,
     std::string AgddString,
     MuonboyBFAbstract* pMuonboyBFAbstract
){

 SetFromString(AmdcString,AgddString);
 initialize(pMuonboyBFAbstract);

}

void MuonboyRecMaker::initialize(     
     MuonboyBFAbstract* pMuonboyBFAbstract
){

 MuonboyBFAccess::SetMuonboyBFAbstract(pMuonboyBFAbstract);
 initialize();

}

void MuonboyRecMaker::initialize(){

// std::cout << "MuonboyRecMaker::initialize " << std::endl;

//*Initialize the Muonboy Algorithm
   initialisemuonboy_();

//*Create p_MB_TrackStoreMaker and Initialize it
   p_MB_TrackStoreMaker = new MB_TrackStoreMaker() ;
   p_MB_TrackStoreMaker->initialize();

//*Create p_MB_SegmentStoreMaker and Initialize it
   p_MB_SegmentStoreMaker = new MB_SegmentStoreMaker() ;
   p_MB_SegmentStoreMaker->initialize();     

}

void MuonboyRecMaker::executeTshift(     
     double TShift ,
     MuonboyBFAbstract* pMuonboyBFAbstract,
     MuonboyRTAbstract* pMuonboyRTAbstract,
     const MuonRecDigiStore* pMuonRecDigiStore,
     MB_TrackStore*          pMB_TrackStore,
     MB_SegmentStore*        pMB_SegmentStore
){
     
 MuonboyBFAccess::SetMuonboyBFAbstract(pMuonboyBFAbstract);
 MuonboyRTAccess::SetMuonboyRTAbstract(pMuonboyRTAbstract);

 resetsettshift_(TShift);

 execute(     
         pMuonRecDigiStore,
         pMB_TrackStore,
         pMB_SegmentStore
        );
        
}

void MuonboyRecMaker::execute(     
     MuonboyBFAbstract* pMuonboyBFAbstract,
     MuonboyRTAbstract* pMuonboyRTAbstract,
     const MuonRecDigiStore* pMuonRecDigiStore,
     MB_TrackStore*          pMB_TrackStore,
     MB_SegmentStore*        pMB_SegmentStore
){
     
 MuonboyBFAccess::SetMuonboyBFAbstract(pMuonboyBFAbstract);
 MuonboyRTAccess::SetMuonboyRTAbstract(pMuonboyRTAbstract);
 execute(     
         pMuonRecDigiStore,
         pMB_TrackStore,
         pMB_SegmentStore
        );
     
}

void MuonboyRecMaker::execute(     
     const MuonRecDigiStore* pMuonRecDigiStore,
     MB_TrackStore*          pMB_TrackStore,
     MB_SegmentStore*        pMB_SegmentStore
){

// std::cout << "MuonboyRecMaker::execute " << std::endl;

//*Reconstruct Event
   runmuonboy_();

//*Execute p_MB_TrackStoreMaker
   p_MB_TrackStoreMaker->execute(pMuonRecDigiStore,pMB_TrackStore);

//*Execute p_MB_SegmentStoreMaker
   p_MB_SegmentStoreMaker->execute(pMuonRecDigiStore,pMB_SegmentStore);

}

void MuonboyRecMaker::finalize(){

// std::cout << "MuonboyRecMaker::finalize " << std::endl;

//*Terminate the Muonboy Algorithm
   endmuonboy_();

//*Finalize p_MB_TrackStoreMaker and Delete it
   p_MB_TrackStoreMaker->finalize();
   delete p_MB_TrackStoreMaker ;

//*Finalize p_MB_SegmentStoreMaker and Delete it
   p_MB_SegmentStoreMaker->finalize();
   delete p_MB_SegmentStoreMaker ;

}

void MuonboyRecMaker::SetFromString(std::string AmdcString,std::string AgddString){

   int   NAmdc = AmdcString.size();
   char* CAmdc = new char[NAmdc];
   for (int i= 0; i <NAmdc; i++){CAmdc[i]=AmdcString[i];}
   int   NAgdd = AgddString.size();
   char* CAgdd = new char[NAgdd];
   for (int i= 0; i <NAgdd; i++){CAgdd[i]=AgddString[i];}
   loadamdcagddfromstring_(NAmdc,CAmdc,NAgdd,CAgdd);
   delete [] CAmdc ;
   delete [] CAgdd ;

} 

