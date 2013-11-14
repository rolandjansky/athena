/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyDumpMboyStoreTool.h"

#include "MboyAthEvt/MB_TrackStoreAth.h"
#include "MboyAthEvt/MB_SegmentStoreAth.h"

#include "MboyAthEvt/MB_TrackStoreTDS.h"
#include "MboyAthEvt/MB_SegmentStoreTDS.h"

MboyDumpMboyStoreTool::MboyDumpMboyStoreTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyDumpMboyStoreTool>(this);

}

MboyDumpMboyStoreTool::~MboyDumpMboyStoreTool(){}

// Initialize
StatusCode MboyDumpMboyStoreTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }
  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyDumpMboyStoreTool::finalize(){return StatusCode::SUCCESS;}

// Execute
StatusCode MboyDumpMboyStoreTool::dumpMboyStore(
   std::string SegmentStoreLocation              , 
   std::string TrackStoreLocation                ,
   std::ofstream*  pOutMuonboySegmentStore       ,
   std::ofstream*  pOutMuonboySegmentStoreChecks ,
   std::ofstream*  pOutMuonboyTrackStore         ,
   std::ofstream*  pOutMuonboyTrackStoreChecks  
){

  StatusCode sc = StatusCode::SUCCESS;

//Get pMB_TrackStoreAth from TDS
  const DataHandle<MB_TrackStoreTDS> pMB_TrackStoreTDS ;
  sc = evtStore()->retrieve(pMB_TrackStoreTDS,TrackStoreLocation);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "get pMB_TrackStoreTDS failed" ) ;
    return( StatusCode::FAILURE);
  }
  const MB_TrackStoreAth* pMB_TrackStoreAth = &(pMB_TrackStoreTDS->m_MB_TrackStore);


//Get pMB_SegmentStoreAth from TDS
  const DataHandle<MB_SegmentStoreTDS> pMB_SegmentStoreTDS ;
  sc = evtStore()->retrieve(pMB_SegmentStoreTDS,SegmentStoreLocation);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "get pMB_SegmentStoreTDS failed" ) ;
    return( StatusCode::FAILURE);
  }
  const MB_SegmentStoreAth* pMB_SegmentStoreAth = &(pMB_SegmentStoreTDS->m_MB_SegmentStore);


//Check event
  const DataHandle<EventInfo> TheEventInfo;
  sc = evtStore()->retrieve(TheEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " retrieve TheEventInfo failed" ) ;
    return( StatusCode::FAILURE );
  }

//Set Event Nber
  int EventNber = TheEventInfo->event_ID()->event_number();

//Dump pMB_TrackStoreAth
  if (pMB_TrackStoreAth) { 
    pMB_TrackStoreAth->PrintTracks(pOutMuonboyTrackStore,EventNber);
    PrintTracks(pMB_TrackStoreAth,pOutMuonboyTrackStoreChecks,EventNber);
  }

//Dump pMB_SegmentStoreAth
  if (pMB_SegmentStoreAth) {
    pMB_SegmentStoreAth->PrintSegments(pOutMuonboySegmentStore,EventNber);
    PrintSegments(pMB_SegmentStoreAth,pOutMuonboySegmentStoreChecks,EventNber);
  }                              

  return StatusCode::SUCCESS;

}
 
/** Dump pMB_TrackStoreAth */
void MboyDumpMboyStoreTool::PrintTracks
(const MB_TrackStoreAth* pMB_TrackStoreAth,
 std::ostream* out,int EventNber) const {

  *out << "Event Nber: " << EventNber << std::endl;

//Loop on all tracks
  int NberOfTracks = pMB_TrackStoreAth->NberOfMB_Track();
  *out << "* There are "
       << std::setw(5)<< NberOfTracks
       << " tracks "
       << std::endl;
  for (int TrackNber=0; TrackNber<NberOfTracks; TrackNber++){
  
    const MB_TrackAth* TheTrack = pMB_TrackStoreAth->GetMB_TrackAth(TrackNber);
    const MB_Track_MboyCyl* pMB_Track_MboyCyl_MS = TheTrack->pGetTrackAtSpectrometerEntrance();
    const MB_Track_MboyCyl* pMB_Track_MboyCyl_KA = TheTrack->pGetTrackAtCalorimeterEntrance();
    const MB_Track_Perigee* pMB_Track_Perigee_VT = TheTrack->pGetTrackAtPerigee();

  
    *out << "*   For track nber "
         << std::setw(4) << TrackNber << std::endl;
  
    *out << "*    Eloss = "
         << std::setw(12)<<std::setprecision(5)
         << pMB_Track_MboyCyl_KA->Momentum() - pMB_Track_MboyCyl_MS->Momentum()
         << std::endl;
  
    *out << "*     Track validities at "
         << std::endl;
    *out << "*     Muon System entrance:  " << std::setw(5)<< TheTrack->IsValidMS()
         << std::endl;
    *out << "*     Calorimeter entrance:  " << std::setw(5)<< TheTrack->IsValidKA()
         << std::endl;
    *out << "*     at vertex           :  " << std::setw(5)<< TheTrack->IsValidVT()
         << std::endl;

    *out << "*     Track parameters at "
         << std::endl;
  
    *out << "*     Muon System entrance:  "
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->ThetaOfCrossingPoint()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->PhiOfCrossingPoint()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->ThetaOfUnitVector()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->PhiOfUnitVector()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_MS->InverseMomentum()
         <<  std::setw(12)<<std::setprecision(5)<<1./pMB_Track_MboyCyl_MS->InverseMomentum()
         << std::endl;
    *out << "*                       +/-  "
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov11())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov22())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov33())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov44())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_MS->Cov55())
         <<  std::setw(12)<<std::setprecision(5)
         <<sqrt(pMB_Track_MboyCyl_MS->Cov55())/std::pow(pMB_Track_MboyCyl_MS->InverseMomentum(),2)
         << std::endl;
  
    *out << "*     Calorimeter entrance:  "
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->ThetaOfCrossingPoint()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->PhiOfCrossingPoint()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->ThetaOfUnitVector()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->PhiOfUnitVector()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_MboyCyl_KA->InverseMomentum()
         <<  std::setw(12)<<std::setprecision(5)<<1./pMB_Track_MboyCyl_KA->InverseMomentum()
         << std::endl;
    *out << "*                       +/-  "
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov11())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov22())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov33())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov44())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_MboyCyl_KA->Cov55())
         <<  std::setw(12)<<std::setprecision(5)
         <<sqrt(pMB_Track_MboyCyl_KA->Cov55())/std::pow(pMB_Track_MboyCyl_KA->InverseMomentum(),2)
         << std::endl;
  
    *out << "*     at vertex           :  "
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->ImpactParameterAtPerigee()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->ZAtPerigee()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->PhiAtPerigee()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->CotgAtPerigee()
         <<  std::setw(12)<<std::setprecision(5)<<pMB_Track_Perigee_VT->InverseTransverseMomentum()
         <<  std::setw(12)<<std::setprecision(5)<<1./pMB_Track_Perigee_VT->InverseTransverseMomentum()
         << std::endl;
    *out << "*                       +/-  "
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov11())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov22())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov33())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov44())
         <<  std::setw(12)<<std::setprecision(5)<<sqrt(pMB_Track_Perigee_VT->Cov55())
         << std::endl;
  
    *out << "*    Covariance matrice at "
         << std::endl;
  
    *out << "*     Muon System entrance:  "
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,1)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(1,1))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,2)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(2,2))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,3)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(3,3))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,4)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(4,4))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(1,5)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(1,1)*pMB_Track_MboyCyl_MS->Covij(5,5))
         << std::endl;
    for (int i1=2; i1<=5; i1++){
      *out << "*                            "
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,1)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(1,1))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,2)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(2,2))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,3)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(3,3))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,4)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(4,4))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_MS->Covij(i1,5)*100./sqrt(pMB_Track_MboyCyl_MS->Covij(i1,i1)*pMB_Track_MboyCyl_MS->Covij(5,5))
           << std::endl;
    }
  
    *out << "*     Calorimeter entrance:  "
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,1)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(1,1))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,2)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(2,2))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,3)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(3,3))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,4)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(4,4))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(1,5)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(1,1)*pMB_Track_MboyCyl_KA->Covij(5,5))
         << std::endl;
    for (int i1=2; i1<=5; i1++){
      *out << "*                            "
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,1)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(1,1))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,2)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(2,2))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,3)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(3,3))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,4)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(4,4))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_MboyCyl_KA->Covij(i1,5)*100./sqrt(pMB_Track_MboyCyl_KA->Covij(i1,i1)*pMB_Track_MboyCyl_KA->Covij(5,5))
           << std::endl;
    }
  
    *out << "*     at vertex           :  "
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,1)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(1,1))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,2)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(2,2))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,3)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(3,3))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,4)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(4,4))
         <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(1,5)*100./sqrt(pMB_Track_Perigee_VT->Covij(1,1)*pMB_Track_Perigee_VT->Covij(5,5))
         << std::endl;
    for (int i1=2; i1<=5; i1++){
      *out << "*                            "
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,1)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(1,1))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,2)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(2,2))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,3)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(3,3))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,4)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(4,4))
           <<  std::setw(12)<<std::setprecision(2)<<pMB_Track_Perigee_VT->Covij(i1,5)*100./sqrt(pMB_Track_Perigee_VT->Covij(i1,i1)*pMB_Track_Perigee_VT->Covij(5,5))
           << std::endl;
    }
//
    *out << "*    PerigeePoint            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->XPerigeePoint()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->YPerigeePoint()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->ZPerigeePoint()
         << std::endl;
    *out << "*    Momentum                "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->XUnitVector()*pMB_Track_Perigee_VT->Momentum()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->YUnitVector()*pMB_Track_Perigee_VT->Momentum()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_Perigee_VT->ZUnitVector()*pMB_Track_Perigee_VT->Momentum()
         << std::endl;
    *out << "*    MuonSysEntrancePoint    "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->XCrossingPoint()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->YCrossingPoint()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->ZCrossingPoint()
         << std::endl;
    *out << "*    Momentum                "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->XUnitVector()*pMB_Track_MboyCyl_MS->Momentum()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->YUnitVector()*pMB_Track_MboyCyl_MS->Momentum()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_MS->ZUnitVector()*pMB_Track_MboyCyl_MS->Momentum()
         << std::endl;
  
    *out << "*    CaloEntrancePoint       "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->XCrossingPoint()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->YCrossingPoint()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->ZCrossingPoint()
         << std::endl;
    *out << "*    Momentum                "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->XUnitVector()*pMB_Track_MboyCyl_KA->Momentum()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->YUnitVector()*pMB_Track_MboyCyl_KA->Momentum()
         << std::endl;
    *out << "*                            "
         <<  std::setw(20)<<std::setprecision(10)<<pMB_Track_MboyCyl_KA->ZUnitVector()*pMB_Track_MboyCyl_KA->Momentum()
         << std::endl;
  
    *out << "*     List of Digits           :  "
         << std::endl;
  
    int NberOfMDT    = TheTrack->GetNberOfDigiMDT   () ;
    int NberOfRPC    = TheTrack->GetNberOfDigiRPC   () ;
    int NberOfTGC    = TheTrack->GetNberOfDigiTGC   () ;
    int NberOfCSC    = TheTrack->GetNberOfDigiCSC   () ;
    int NberOfCSCclu = TheTrack->GetNberOfDigiCSCclu() ;
    int NberOfElements = TheTrack->GetNberOfElements() ;

    int NberOfHoleMDT    = TheTrack->GetNberOfHoleMDT   () ;
    int NberOfHoleRPC    = TheTrack->GetNberOfHoleRPC   () ;
    int NberOfHoleTGC    = TheTrack->GetNberOfHoleTGC   () ;
    int NberOfHoleCSC    = TheTrack->GetNberOfHoleCSC   () ;
    int NberOfHoleCSCclu = TheTrack->GetNberOfHoleCSCclu() ;
    int NberOfDigi       = TheTrack->GetNberOfDigi() ;

    *out << "*      First Coordinate "
         << std::endl;
  
    double Chi2Typ1 = TheTrack->GetChi2(1);
    double Xcrpoint_SpecEntr = pMB_Track_MboyCyl_MS->XCrossingPoint() ;
    double Ycrpoint_SpecEntr = pMB_Track_MboyCyl_MS->YCrossingPoint() ;
    double Zcrpoint_SpecEntr = pMB_Track_MboyCyl_MS->ZCrossingPoint() ;
    double Xunivect_SpecEntr = pMB_Track_MboyCyl_MS->XUnitVector()    ;
    double Yunivect_SpecEntr = pMB_Track_MboyCyl_MS->YUnitVector()    ;
    double Zunivect_SpecEntr = pMB_Track_MboyCyl_MS->ZUnitVector()    ;
    double Momentum_SpecEntr = pMB_Track_MboyCyl_MS->Momentum()       ;
    double Charge_SpecEntr   = 1.                       ;
    if (pMB_Track_MboyCyl_MS->InverseMomentum() <= 0. ) Charge_SpecEntr = -1. ;
  
    Xcrpoint_SpecEntr = Xcrpoint_SpecEntr/10.  ;
    Ycrpoint_SpecEntr = Ycrpoint_SpecEntr/10.  ;
    Zcrpoint_SpecEntr = Zcrpoint_SpecEntr/10.  ;
    Momentum_SpecEntr = Momentum_SpecEntr/1000.;
  
    *out << " Tra.------>"
         << std::setw(5)<< TrackNber+1
         << "     "
         << std::setw(5)<< NberOfMDT
         << std::setw(10)<<std::setprecision(4)<< Chi2Typ1
         << std::endl;
    *out << std::setw(5)<< NberOfMDT   
         << std::setw(5)<< NberOfRPC   
         << std::setw(5)<< NberOfTGC   
         << std::setw(5)<< NberOfCSC   
         << std::setw(5)<< NberOfCSCclu
         << std::endl;
    *out << std::setw(5)<< NberOfHoleMDT   
         << std::setw(5)<< NberOfHoleRPC   
         << std::setw(5)<< NberOfHoleTGC   
         << std::setw(5)<< NberOfHoleCSC   
         << std::setw(5)<< NberOfHoleCSCclu
         << std::setw(5)<< NberOfDigi      
         << std::endl;

    *out << std::setw(12)<<std::setprecision(4)<< Xcrpoint_SpecEntr
         << std::setw(12)<<std::setprecision(4)<< Ycrpoint_SpecEntr
         << std::setw(12)<<std::setprecision(4)<< Zcrpoint_SpecEntr
         << std::setw(12)<<std::setprecision(6)<< Xunivect_SpecEntr
         << std::setw(12)<<std::setprecision(6)<< Yunivect_SpecEntr
         << std::setw(12)<<std::setprecision(6)<< Zunivect_SpecEntr
         << std::setw(12)<<std::setprecision(3)<< Momentum_SpecEntr
         << std::setw(12)<<std::setprecision(1)<< Charge_SpecEntr
         << std::endl;
  
    int kountMbROTMDT = 0 ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::MdtPrepData* pMdtPrepData    = TheTrack->LiIdGetMdtPrepData       (ElementNber);
      if (pMdtPrepData) {
        const MbROTMDT* pMbROTMDT = TheTrack->LiIdGetMbROTMDT (ElementNber);
        kountMbROTMDT =kountMbROTMDT  + 1 ;
        *out << std::setw(5)<< kountMbROTMDT ;
        pMbROTMDT->Print(out);
      }
    }
  
    int kountMbROTRPC = kountMbROTMDT ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::RpcPrepData* pRpcPrepData    = TheTrack->LiIdGetRpcPrepData       (ElementNber);
      if (pRpcPrepData) {
        const MbROTRPC*    pMbROTRPC    = TheTrack->LiIdGetMbROTRPC    (ElementNber);
        kountMbROTRPC =kountMbROTRPC  + 1 ;
        *out << std::setw(5)<< kountMbROTRPC ;
        pMbROTRPC->Print(out);
      }
    }

    int kountMbROTTGC = kountMbROTRPC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::TgcPrepData* pTgcPrepData    = TheTrack->LiIdGetTgcPrepData       (ElementNber);
      if (pTgcPrepData) {
        const MbROTTGC*    pMbROTTGC    = TheTrack->LiIdGetMbROTTGC    (ElementNber);
        kountMbROTTGC =kountMbROTTGC  + 1 ;
        *out << std::setw(5)<< kountMbROTTGC ;
        pMbROTTGC->Print(out);
      }
    }
  
    int kountMbROTCSC = kountMbROTTGC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::CscStripPrepData* pCscStripPrepData    = TheTrack->LiIdGetCscStripPrepData       (ElementNber);
      if (pCscStripPrepData) {
        const MbROTCSC*    pMbROTCSC    = TheTrack->LiIdGetMbROTCSC    (ElementNber);
        kountMbROTCSC =kountMbROTCSC  + 1 ;
        *out << std::setw(5)<< kountMbROTCSC ;
        pMbROTCSC->Print(out);
      }
    }

    int kountMbROTCSCclu = kountMbROTCSC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::CscPrepData* pCscPrepDataClu = TheTrack->LiIdGetCscClusterPrepData(ElementNber);
      if (pCscPrepDataClu) {
        const MbROTCSCclu*    pMbROTCSCclu   = TheTrack->LiIdGetMbROTCSCclu    (ElementNber);
        kountMbROTCSCclu =kountMbROTCSCclu  + 1 ;
        *out << std::setw(5)<< kountMbROTCSCclu ;
        pMbROTCSCclu->Print(out);
      }
    }


    int kountMbHoleMDT = kountMbROTCSCclu ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleMDT* pMbHoleMDT = TheTrack->LiIdGetMbHoleMDT (ElementNber);
      if (pMbHoleMDT) {
        kountMbHoleMDT =kountMbHoleMDT  + 1 ;
        *out << std::setw(5)<< kountMbHoleMDT ;
        pMbHoleMDT->Print(out);
      }
    }
  
  
    int kountMbHoleRPC = kountMbHoleMDT ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleRPC*    pMbHoleRPC    = TheTrack->LiIdGetMbHoleRPC    (ElementNber);
      if (pMbHoleRPC) {
        kountMbHoleRPC =kountMbHoleRPC  + 1 ;
        *out << std::setw(5)<< kountMbHoleRPC ;
        pMbHoleRPC->Print(out);
      }
    }

    int kountMbHoleTGC = kountMbHoleRPC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleTGC*    pMbHoleTGC    = TheTrack->LiIdGetMbHoleTGC    (ElementNber);
      if (pMbHoleTGC) {
        kountMbHoleTGC =kountMbHoleTGC  + 1 ;
        *out << std::setw(5)<< kountMbHoleTGC ;
        pMbHoleTGC->Print(out);
      }
    }

    int kountMbHoleCSC = kountMbHoleTGC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleCSC*    pMbHoleCSC    = TheTrack->LiIdGetMbHoleCSC    (ElementNber);
      if (pMbHoleCSC) {
        kountMbHoleCSC =kountMbHoleCSC  + 1 ;
        *out << std::setw(5)<< kountMbHoleCSC ;
        pMbHoleCSC->Print(out);
      }
    }

    int kountMbHoleCSCclu = kountMbHoleCSC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleCSCclu*    pMbHoleCSCclu   = TheTrack->LiIdGetMbHoleCSCclu    (ElementNber);
      if (pMbHoleCSCclu) {
        kountMbHoleCSCclu =kountMbHoleCSCclu  + 1 ;
        *out << std::setw(5)<< kountMbHoleCSCclu ;
        pMbHoleCSCclu->Print(out);
      }
    }

    TheTrack->PrintSortedElements(out);
    
  }

}
  
/** Dump pMB_SegmentStoreAth */
void MboyDumpMboyStoreTool::PrintSegments
(const MB_SegmentStoreAth* pMB_SegmentStoreAth,
 std::ostream* out,int EventNber) const{

  int NberOfSegments = pMB_SegmentStoreAth->NberOfMB_Segment();

  *out << "--------------------------------"
       << std::endl;
  *out << "Evt.Seg.Tra."
       << std::setw(10)<< EventNber
       << std::setw(5)<< NberOfSegments
       << std::endl;

  for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){
    const MB_SegmentAth* TheSegment = pMB_SegmentStoreAth->GetMB_SegmentAth(SegmentNber);
  
    int NberOfMDT    = TheSegment->GetNberOfDigiMDT   () ;
    int NberOfRPC    = TheSegment->GetNberOfDigiRPC   () ;
    int NberOfTGC    = TheSegment->GetNberOfDigiTGC   () ;
    int NberOfCSC    = TheSegment->GetNberOfDigiCSC   () ;
    int NberOfCSCclu = TheSegment->GetNberOfDigiCSCclu() ;
    int NberOfElements = TheSegment->GetNberOfElements() ;

    int NberOfHoleMDT    = TheSegment->GetNberOfHoleMDT   () ;
    int NberOfHoleRPC    = TheSegment->GetNberOfHoleRPC   () ;
    int NberOfHoleTGC    = TheSegment->GetNberOfHoleTGC   () ;
    int NberOfHoleCSC    = TheSegment->GetNberOfHoleCSC   () ;
    int NberOfHoleCSCclu = TheSegment->GetNberOfHoleCSCclu() ;
    int NberOfDigi       = TheSegment->GetNberOfDigi() ;

    double Xpt     =0.;
    double Ypt     =0.;
    double Zpt     =0.;
    TheSegment->GetPoint(Xpt,Ypt,Zpt); 
    double Xvec    =0.;
    double Yvec    =0.;
    double Zvec    =0.;
    TheSegment->GetDirection(Xvec,Yvec,Zvec) ;
    double      QualityFactor    = TheSegment->GetQualityFactor() ;
    int         NbreOfMultiLayer = TheSegment->GetNberOfMultiLayer() ;
    int         TrackSharing = TheSegment->GetTrackSharing() ;
    std::string StaName          = TheSegment->GetStationName();
  
    double X_dx  =0.;
    double Y_dx  =0.;
    double Z_dx  =0.;
    TheSegment->Getdx(X_dx,Y_dx,Z_dx);
    double X_dy  =0.;
    double Y_dy  =0.;
    double Z_dy  =0.;
    TheSegment->Getdy(X_dy,Y_dy,Z_dy);
    int    E_Valid   = 0  ;
    double E_dx      = 0. ;
    double E_Axz     = 0. ;
    double E_dxAxz   = 0. ;
    double E_dy      = 0. ; 
    double E_Ayz     = 0. ; 
    double E_dyAyz   = 0. ; 
    TheSegment->GetErrorsComponents(
     E_Valid,
     E_dx, 
     E_Axz , 
     E_dxAxz,
     E_dy, 
     E_Ayz , 
     E_dyAyz);

    Xpt     =Xpt/10.;
    Ypt     =Ypt/10.;
    Zpt     =Zpt/10.;
  
    *out << " Seg.------>"
         << std::setw(5)<< SegmentNber+1
         << std::setw(5)<< NbreOfMultiLayer
         << std::setw(5)<< NberOfMDT
         << std::setw(5)<< NberOfRPC
         << std::setw(5)<< TrackSharing
         << std::setw(10)<<std::setprecision(4)<< QualityFactor
         << "  " << std::setw(3)<< StaName
         << std::endl;
    *out << std::setw(5)<< NberOfMDT   
         << std::setw(5)<< NberOfRPC   
         << std::setw(5)<< NberOfTGC   
         << std::setw(5)<< NberOfCSC   
         << std::setw(5)<< NberOfCSCclu
         << std::endl;
    *out << std::setw(5)<< NberOfHoleMDT   
         << std::setw(5)<< NberOfHoleRPC   
         << std::setw(5)<< NberOfHoleTGC   
         << std::setw(5)<< NberOfHoleCSC   
         << std::setw(5)<< NberOfHoleCSCclu
         << std::setw(5)<< NberOfDigi      
         << std::endl;

    *out << std::setw(12)<<std::setprecision(4)<< Xpt
         << std::setw(12)<<std::setprecision(4)<< Ypt
         << std::setw(12)<<std::setprecision(4)<< Zpt
         << std::setw(12)<<std::setprecision(6)<< Xvec
         << std::setw(12)<<std::setprecision(6)<< Yvec
         << std::setw(12)<<std::setprecision(6)<< Zvec
         << std::endl;
    *out << " Error validity " << std::setw(12)<< E_Valid
	 << std::endl;
    if ( E_Valid !=0 ) {
    *out << "dx Axz mm/mrad         "
         << std::setw(12)<<std::setprecision(4)<< 1./sqrt(E_dx)
         << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(E_Axz)
         << std::setw(12)<<std::setprecision(4)<< E_dxAxz/(sqrt(E_dx)*sqrt(E_Axz))
	 << std::endl;
    *out << "dy Axz micron/microrad "
         << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(E_dy)
         << std::setw(12)<<std::setprecision(4)<< 1000000./sqrt(E_Ayz)
         << std::setw(12)<<std::setprecision(4)<< E_dyAyz/(sqrt(E_dy)*sqrt(E_Ayz))
	 << std::endl;
    }
  
    int kountMbROTMDT = 0 ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::MdtPrepData* pMdtPrepData    = TheSegment->LiIdGetMdtPrepData       (ElementNber);
      if (pMdtPrepData) {
        const MbROTMDT* pMbROTMDT = TheSegment->LiIdGetMbROTMDT (ElementNber);
        kountMbROTMDT =kountMbROTMDT  + 1 ;
        *out << std::setw(5)<< kountMbROTMDT ;
        pMbROTMDT->Print(out);
      }
    }
  
    int kountMbROTRPC = kountMbROTMDT ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::RpcPrepData* pRpcPrepData    = TheSegment->LiIdGetRpcPrepData       (ElementNber);
      if (pRpcPrepData) {
        const MbROTRPC*    pMbROTRPC    = TheSegment->LiIdGetMbROTRPC    (ElementNber);
        kountMbROTRPC =kountMbROTRPC  + 1 ;
        *out << std::setw(5)<< kountMbROTRPC ;
        pMbROTRPC->Print(out);
      }
    }

    int kountMbROTTGC = kountMbROTRPC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::TgcPrepData* pTgcPrepData    = TheSegment->LiIdGetTgcPrepData       (ElementNber);
      if (pTgcPrepData) {
        const MbROTTGC*    pMbROTTGC    = TheSegment->LiIdGetMbROTTGC    (ElementNber);
        kountMbROTTGC =kountMbROTTGC  + 1 ;
        *out << std::setw(5)<< kountMbROTTGC ;
        pMbROTTGC->Print(out);
      }
    }

    int kountMbROTCSC = kountMbROTTGC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::CscStripPrepData* pCscStripPrepData    = TheSegment->LiIdGetCscStripPrepData       (ElementNber);
      if (pCscStripPrepData) {
        const MbROTCSC*    pMbROTCSC    = TheSegment->LiIdGetMbROTCSC    (ElementNber);
        kountMbROTCSC =kountMbROTCSC  + 1 ;
        *out << std::setw(5)<< kountMbROTCSC ;
        pMbROTCSC->Print(out);
      }
    }

    int kountMbROTCSCclu = kountMbROTCSC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const Muon::CscPrepData* pCscPrepDataClu = TheSegment->LiIdGetCscClusterPrepData(ElementNber);
      if (pCscPrepDataClu) {
        const MbROTCSCclu*    pMbROTCSCclu   = TheSegment->LiIdGetMbROTCSCclu    (ElementNber);
        kountMbROTCSCclu =kountMbROTCSCclu  + 1 ;
        *out << std::setw(5)<< kountMbROTCSCclu ;
        pMbROTCSCclu->Print(out);
      }
    }


    int kountMbHoleMDT = kountMbROTCSCclu ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleMDT* pMbHoleMDT = TheSegment->LiIdGetMbHoleMDT (ElementNber);
      if (pMbHoleMDT) {
        kountMbHoleMDT =kountMbHoleMDT  + 1 ;
        *out << std::setw(5)<< kountMbHoleMDT ;
        pMbHoleMDT->Print(out);
      }
    }
  
    int kountMbHoleRPC = kountMbHoleMDT ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleRPC*    pMbHoleRPC    = TheSegment->LiIdGetMbHoleRPC    (ElementNber);
      if (pMbHoleRPC) {
        kountMbHoleRPC =kountMbHoleRPC  + 1 ;
        *out << std::setw(5)<< kountMbHoleRPC ;
        pMbHoleRPC->Print(out);
      }
    }

    int kountMbHoleTGC = kountMbHoleRPC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleTGC*    pMbHoleTGC    = TheSegment->LiIdGetMbHoleTGC    (ElementNber);
      if (pMbHoleTGC) {
        kountMbHoleTGC =kountMbHoleTGC  + 1 ;
        *out << std::setw(5)<< kountMbHoleTGC ;
        pMbHoleTGC->Print(out);
      }
    }

    int kountMbHoleCSC = kountMbHoleTGC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleCSC*    pMbHoleCSC    = TheSegment->LiIdGetMbHoleCSC    (ElementNber);
      if (pMbHoleCSC) {
        kountMbHoleCSC =kountMbHoleCSC  + 1 ;
        *out << std::setw(5)<< kountMbHoleCSC ;
        pMbHoleCSC->Print(out);
      }
    }

    int kountMbHoleCSCclu = kountMbHoleCSC ;
    for (int ElementNber=0; ElementNber<NberOfElements; ElementNber++){
      const MbHoleCSCclu*    pMbHoleCSCclu   = TheSegment->LiIdGetMbHoleCSCclu    (ElementNber);
      if (pMbHoleCSCclu) {
        kountMbHoleCSCclu =kountMbHoleCSCclu  + 1 ;
        *out << std::setw(5)<< kountMbHoleCSCclu ;
        pMbHoleCSCclu->Print(out);
      }
    }

    TheSegment->PrintSortedElements(out);

  }

}  
