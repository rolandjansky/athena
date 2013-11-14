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
#include "MboyAthTool/MboyDumpNativeSegmentTool.h"

#include "MboyAthEvt/MB_SegmentStoreAth.h"
#include "MboyAthEvt/MB_SegmentStoreTDS.h"

#include "MboyEDMToolInterfaces/IMboyNativeHelper.h"

MboyDumpNativeSegmentTool::MboyDumpNativeSegmentTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper              ( "MboyNativeHelper" ) 
{

  declareInterface<IMboyDumpNativeSegmentTool>(this);

  declareProperty("MboyNativeHelper" , p_OwnEDMHelper ) ;

}

MboyDumpNativeSegmentTool::~MboyDumpNativeSegmentTool(){}

// Initialize
StatusCode MboyDumpNativeSegmentTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;


  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyDumpNativeSegmentTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyDumpNativeSegmentTool::dumpNativeSegments(
   int   LetCSCGo                     ,
   int   ApplyQualityFactorCut        , 
   float QualityFactorCut             ,
   int   ApplyNberofDigitsCut         ,
   int   NberofDigitsCut              ,
   int   ApplyNberofPrecisonDigitsCut ,
   int   NberofPrecisonDigitsCut      ,
   std::string SegmentStoreLocation   ,
   std::ofstream*      pOutFile
){

  StatusCode sc = StatusCode::SUCCESS;

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

  *pOutFile 
      << " " 
      <<std::endl;
  *pOutFile 
      << "* For the event : " 
      << TheEventInfo->event_ID()->event_number()
      << std::endl;

  int Kounter = 0 ;
  int NberOfSegments = pMB_SegmentStoreAth->NberOfMB_Segment();
  for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){
    MB_SegmentAth TheSegment = *(pMB_SegmentStoreAth->GetMB_SegmentAth(SegmentNber));
//  Select segments
    if( RecSeg_IsOk(
                    LetCSCGo                     ,
                    ApplyQualityFactorCut        , 
                    QualityFactorCut             ,
                    ApplyNberofDigitsCut         ,
                    NberofDigitsCut              ,
                    ApplyNberofPrecisonDigitsCut ,
                    NberofPrecisonDigitsCut      ,
                    TheSegment) != 0 
      ) {

      Kounter = Kounter + 1 ;

      *pOutFile 
         << " " 
         <<std::endl;
      *pOutFile 
         << "* Segment number  " 
         << Kounter 
         <<std::endl;
      *pOutFile 
         << "Segment Station Name   " 
         << " " << p_OwnEDMHelper->SegmentAssociatedStationName(&TheSegment)
         <<std::endl;
     *pOutFile 
         << "Fit Quality   " 
         << " " << std::setw(12)<<std::setprecision(5) << p_OwnEDMHelper->GetfitQuality(&TheSegment)
         <<std::endl;
     *pOutFile << "Nber of Els      " << " " << p_OwnEDMHelper->GetNberOfElements(&TheSegment) <<std::endl;
     *pOutFile << "Nber of Digits   " << " " << p_OwnEDMHelper->GetNberOfDigi(&TheSegment) <<std::endl;
     *pOutFile << "Nber of Digits MDT  "  << " " << p_OwnEDMHelper->GetNberOfDigiMDT    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Digits RPC  "  << " " << p_OwnEDMHelper->GetNberOfDigiRPC    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Digits TGC  "  << " " << p_OwnEDMHelper->GetNberOfDigiTGC    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Digits CSC  "  << " " << p_OwnEDMHelper->GetNberOfDigiCSC    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Digits CSCcl"  << " " << p_OwnEDMHelper->GetNberOfDigiCSCclu (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Digits CSCcl precise"  << " " << p_OwnEDMHelper->GetNberOfDigiCSCPrecise (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Holes  MDT  "  << " " << p_OwnEDMHelper->GetNberOfHoleMDT    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Holes  RPC  "  << " " << p_OwnEDMHelper->GetNberOfHoleRPC    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Holes  TGC  "  << " " << p_OwnEDMHelper->GetNberOfHoleTGC    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Holes  CSC  "  << " " << p_OwnEDMHelper->GetNberOfHoleCSC    (&TheSegment) <<std::endl;
     *pOutFile << "Nber of Holes  CSCcl"  << " " << p_OwnEDMHelper->GetNberOfHoleCSCclu (&TheSegment) <<std::endl;
     *pOutFile 
         << "Segment Sharing Status " 
         << " " << std::setw(12)<<std::setprecision(5) <<p_OwnEDMHelper->SharingState(&TheSegment)
         <<std::endl;
      sc = p_OwnEDMHelper->Dump_MB_SegmentAth(&TheSegment,pOutFile);
      if (sc.isFailure()) ATH_MSG_WARNING( "Dump_MB_SegmentAth failed" ) ;
      sc = p_OwnEDMHelper->Dump_MB_SegmentAthtElements(&TheSegment,pOutFile);
      if (sc.isFailure()) ATH_MSG_WARNING( "Dump_MB_SegmentAthtElements failed" ) ;

    }
  }

  return StatusCode::SUCCESS;

}

int  MboyDumpNativeSegmentTool::RecSeg_IsOk(
   int   LetCSCGo                     ,
   int   ApplyQualityFactorCut        , 
   float QualityFactorCut             ,
   int   ApplyNberofDigitsCut         ,
   int   NberofDigitsCut              ,
   int   ApplyNberofPrecisonDigitsCut ,
   int   NberofPrecisonDigitsCut      ,
   MB_SegmentAth& TheSegment
)
{

  if ( p_OwnEDMHelper->IsValid(&TheSegment) ==0 ) return 0;
  
  if( LetCSCGo==1 ){
    std::string StationName = TheSegment.GetStationName() ;
    if ( StationName == "CSS" ) return 1;
    if ( StationName == "CSL" ) return 1;
    if ( StationName == "CSC" ) return 1;
  }

  if( ApplyQualityFactorCut==1 ){
    double TheQualityFactor = TheSegment.GetQualityFactor() ;
    if (TheQualityFactor >= QualityFactorCut ) return 0;
  }

  if( ApplyNberofDigitsCut==1 ){
    int TheNberOfDig = TheSegment.GetNberOfDigi();
    if (TheNberOfDig < NberofDigitsCut ) return 0;
  }

  if( ApplyNberofPrecisonDigitsCut==1 ){
    int TheNberOfPrecisonDig = TheSegment.GetNberOfDigiMDT() 
    + TheSegment.GetNberOfDigiCSCPrecise() ;
    if (TheNberOfPrecisonDig < NberofPrecisonDigitsCut ) return 0;
  }

  return 1;

}
