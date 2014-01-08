/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MB_TrackStoreMaker.h"

#include "MboyCoreEvt/MuonRecDigiStore.h"
#include "MboyCoreEvt/MB_TrackStore.h"

#include "MuonboyCore/FillerMB_Track.h"
#include "MuonboyCore/FillerMbIOObjectStore.h"
#include "MuonboyCore/MuonboyComcan.h"
 
MB_TrackStoreMaker::MB_TrackStoreMaker(){}
MB_TrackStoreMaker::~MB_TrackStoreMaker(){}

void MB_TrackStoreMaker::initialize(){}

void MB_TrackStoreMaker::execute  
  (const MuonRecDigiStore* pMuonRecDigiStore,
         MB_TrackStore*    pMB_TrackStore){

   if ( pMB_TrackStore == 0 ){
     std::cout << " MB_TrackStoreMaker::execute pMB_TrackStore is 0 " << std::endl; 
     return ;
   }
   if ( pMuonRecDigiStore == 0 ){
     std::cout << " MB_TrackStoreMaker::execute pMuonRecDigiStore is 0 " << std::endl; 
     return ;
   }  

// Take a MuonboyComcan 
   MuonboyComcan aMuonboyComcan;

// Loop on tracks candidates
   int NberOfTracks = aMuonboyComcan.NBCAN();
   for (int TrackNber=0; TrackNber<NberOfTracks; TrackNber++){
     if (aMuonboyComcan.CHICAN(TrackNber)<999.) {
     
       FillerMB_Track aFillerMB_Track ;

//     Create a MB_Track_SpecEntr
       MB_Track_SpecEntr  aMB_Track_SpecEntr ;
       aFillerMB_Track.FillMB_Track_SpecEntr(&aMB_Track_SpecEntr,TrackNber);

//     Create a MB_Track_CaloEntr
       MB_Track_CaloEntr aMB_Track_CaloEntr ;
       aFillerMB_Track.FillMB_Track_CaloEntr(&aMB_Track_CaloEntr,TrackNber);

//     Create a MB_Track_Perigee
       MB_Track_Perigee aMB_Track_Perigee ;
       aFillerMB_Track.FillMB_Track_Perigee(&aMB_Track_Perigee,TrackNber);

//     Collect indexes of digis
       MbIOObjectStore    aMbIOObjectStore ;
       aMbIOObjectStore.SetRunNber(pMuonRecDigiStore->GetRunNber());
       aMbIOObjectStore.SetEventNber(pMuonRecDigiStore->GetEventNber());

       FillerMbIOObjectStore aFillerMbIOObjectStore;
       aFillerMbIOObjectStore.FillMbIOObjectStore(
                                                 pMuonRecDigiStore,
                                                 &aMbIOObjectStore,
                                                 TrackNber
                                                );

//     Quality Factor, Nber of d.o.f and Chi2
       double QualityFactor = aMuonboyComcan.CHICAN(TrackNber);
       int    NberOfDOF     = aMuonboyComcan.NDFCAN(TrackNber);
       double Chi2Typ0      = aMuonboyComcan.CXXCAN(TrackNber,0);
       double Chi2Typ1      = aMuonboyComcan.CXXCAN(TrackNber,1);
       double Chi2Typ2      = aMuonboyComcan.CXXCAN(TrackNber,2);

//     Create a MB_Track and store it in the store
       MB_Track aMB_Track(
        aMB_Track_SpecEntr,aMB_Track_CaloEntr,aMB_Track_Perigee,
        QualityFactor,NberOfDOF,
        Chi2Typ0,Chi2Typ1,Chi2Typ2,
        aMbIOObjectStore
       );
       aMB_Track.Setenmsxx(aMuonboyComcan.ENMSXX(TrackNber)) ;
       
       pMB_TrackStore->Add(aMB_Track);
     }
   }

}

// Finalize
void MB_TrackStoreMaker::finalize(){}


