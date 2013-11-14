/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"

#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyMooSegmentCombinationFinderTester.h"

MboyMooSegmentCombinationFinderTester::MboyMooSegmentCombinationFinderTester(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
 p_OwnEDMHelper                  ( "MboyAthToolHelper" ),
 p_IMboyGetPrepDataFromEventTool ( "MboyGetPrepDataFromEventTool" ) ,
 p_IMboyMooSegmentCombinationFinder ( "Muon::MboyMooSegmentCombinationFinder" ) 
{

  m_EvtTot = 0 ;

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

  declareProperty("MboyGetPrepDataFromEventTool" , p_IMboyGetPrepDataFromEventTool ) ;

  declareProperty("MboyMooSegmentCombinationFinder" , p_IMboyMooSegmentCombinationFinder ) ;

//Declare the properties

  declareProperty( "SegmentCollectionLocation"                , m_SegmentCollectionLocation                = "MboyMooSegmentCombinationFinderTester.ConvertedMBoySegments"                ) ;

  declareProperty( "DoDump"                                   , m_DoDump                                   = 0                                      ) ;      
  declareProperty( "MboyDumpSegmentCollectionOutputFile"      , m_MboyDumpSegmentCollectionOutputFile      = "Out.MboyMooSegmentCombinationFinderTester"        ) ;      

}
MboyMooSegmentCombinationFinderTester::~MboyMooSegmentCombinationFinderTester(){}


// Initialize
StatusCode MboyMooSegmentCombinationFinderTester::initialize(){

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;


  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );
  ATH_MSG_INFO( "= SegmentCollectionLocation                 " << m_SegmentCollectionLocation          );
  ATH_MSG_INFO( "= DoDump                                    " << m_DoDump                );
  ATH_MSG_INFO( "= MboyDumpSegmentCollectionOutputFile       " << m_MboyDumpSegmentCollectionOutputFile           );
  ATH_MSG_INFO( "================================" );


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;

//Retrieve p_IMboyGetPrepDataFromEventTool
  if ( p_IMboyGetPrepDataFromEventTool.retrieve().isFailure() ) {
   ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyGetPrepDataFromEventTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyGetPrepDataFromEventTool );
  
//Retrieve p_IMboyMooSegmentCombinationFinder
  if ( p_IMboyMooSegmentCombinationFinder.retrieve().isFailure() ) {
   ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyMooSegmentCombinationFinder );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyMooSegmentCombinationFinder );
  

//Set files for dumps for checks
  m_OutFile_MboyDumpSegmentCollectionOutputFile.open(m_MboyDumpSegmentCollectionOutputFile.c_str());
  m_OutFile_MboyDumpSegmentCollectionOutputFile << setiosflags(std::ios::fixed);


  m_EvtTot = 0 ;
  m_KounterSegmentCollection.clear();


  ATH_MSG_INFO( "Initialisation ended     " );


  return StatusCode::SUCCESS;

}

// Execute
StatusCode MboyMooSegmentCombinationFinderTester::execute(){

  StatusCode sc = StatusCode::SUCCESS;


//Collect PrepRawData
  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ; VectorOfpPrepData.clear();
  sc=p_IMboyGetPrepDataFromEventTool->CollectPrepData(VectorOfpPrepData);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " CollectPrepData failed" );
    return( StatusCode::FAILURE );
  }
  
  
  
  
//Find Segments
  std::vector<const Muon::MuonSegment*>* pVectorOfMuonSegment = p_IMboyMooSegmentCombinationFinder->findFromPrep(VectorOfpPrepData) ;




//Store SegmentCollection
  Trk::SegmentCollection* pSegmentCollection = new Trk::SegmentCollection;
  for(std::vector<const Muon::MuonSegment*>::const_iterator segIt = pVectorOfMuonSegment->begin();  segIt != pVectorOfMuonSegment->end(); ++segIt){
    const Muon::MuonSegment* pMuonSegment = *segIt;
    pSegmentCollection->push_back(const_cast <Muon::MuonSegment*>(pMuonSegment)); 
  }
  delete pVectorOfMuonSegment ;
 
  sc = evtStore()->record(pSegmentCollection, m_SegmentCollectionLocation);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not save SegmentCollection at " << m_SegmentCollectionLocation );
    return( StatusCode::FAILURE);
  }




//Containers count 
  Kount();

//Containers dump 
  if ( m_DoDump ==1  ) DumpContainers();



  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyMooSegmentCombinationFinderTester::finalize(){

//Print Containers count
  PrintKount();

  return StatusCode::SUCCESS;

}

void MboyMooSegmentCombinationFinderTester::Kount(){

//Count all evts
  m_EvtTot = m_EvtTot + 1 ;

//Access Segments
  unsigned SizeContainer = 0 ;
  const Trk::SegmentCollection* pSegmentCollection ;
  StatusCode sc = evtStore()->retrieve(pSegmentCollection, m_SegmentCollectionLocation);
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "pSegmentCollection not found at " << m_SegmentCollectionLocation );
  }else{
    SizeContainer = pSegmentCollection->size() ;
  }

//Count evts of given nber of Reconstructed segments
  if ( m_KounterSegmentCollection.size() < (SizeContainer+1) ){
    int ItemNberStart = m_KounterSegmentCollection.size() + 1;
    int ItemNberEnd   = SizeContainer + 1      ;
    for (int ItemNber=ItemNberStart; ItemNber<=ItemNberEnd ; ItemNber++){
      int IntToAdd = 0 ;
      m_KounterSegmentCollection.push_back(IntToAdd) ;
    }
  }
  m_KounterSegmentCollection[SizeContainer] = m_KounterSegmentCollection[SizeContainer] + 1 ;

}

void MboyMooSegmentCombinationFinderTester::PrintKount(){

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "*Total number of Evts: MboyEvtTot " << m_EvtTot );

//Print kounting of  SegmentCollection 
  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "*Nber of Trk::Segment in " << m_SegmentCollectionLocation); ;
  int ItemNberStart = 0 ;
  int ItemNberEnd	= m_KounterSegmentCollection.size() ;
  ATH_MSG_INFO( "   " ); ;
  ATH_MSG_INFO( "  Nber Of Segments/Nber Of Evts" ); ;
  for (int ItemNber=ItemNberStart; ItemNber<ItemNberEnd ; ItemNber++){
   if (m_KounterSegmentCollection[ItemNber] !=0 ){
     ATH_MSG_INFO( " " << ItemNber 
        << " " << m_KounterSegmentCollection[ItemNber]
        << " (" << (100.*m_KounterSegmentCollection[ItemNber])/m_EvtTot<< ")" 
        );
   }
  }

  ATH_MSG_INFO( "================================" );

}

void MboyMooSegmentCombinationFinderTester::DumpContainers(){

  StatusCode sc = p_OwnEDMHelper->MboyDumpSegmentCollectionToolDoDump(
                                               m_SegmentCollectionLocation,
                                               &m_OutFile_MboyDumpSegmentCollectionOutputFile
                                             );
  if ( sc.isFailure() ) ATH_MSG_WARNING( "DoDump failed " );

}
