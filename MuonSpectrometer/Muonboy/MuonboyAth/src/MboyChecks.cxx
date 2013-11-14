/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
 
#include "MboyAthToolInterfaces/IMboyDumpMboyStoreTool.h"

/////////////////////////////////////////////////////////
#include "MuonboyAth/MboyChecks.h"
 
MboyChecks::MboyChecks(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
p_IMboyDumpMboyStoreTool            ( "MboyDumpMboyStoreTool"          ) 
{

  declareProperty("MboyDumpMboyStoreTool"            , p_IMboyDumpMboyStoreTool            ) ;

//Declare the properties

  declareProperty( "TrackStoreLocation"                  , m_TrackStoreLocation                  = "Muonboy_TrackStore"      ) ;
  declareProperty( "SegmentStoreLocation"                , m_SegmentStoreLocation                = "Muonboy_SegmentStore"    ) ;
  declareProperty( "NameOfMuonboyTrackStoreOutputFile"   , m_NameOfMuonboyTrackStoreOutputFile   = "Out.MuonboyTrackStore"   ) ;
  declareProperty( "NameOfMuonboySegmentStoreOutputFile" , m_NameOfMuonboySegmentStoreOutputFile = "Out.MuonboySegmentStore" ) ;

}
MboyChecks::~MboyChecks(){}
 
// Initialize
StatusCode MboyChecks::initialize(){

  ATH_MSG_INFO(  "Initialisation started     " ) ;

  ATH_MSG_INFO(  "================================" ) ;
  ATH_MSG_INFO(  "=Proprieties are     " ) ;
  ATH_MSG_INFO(  "= TrackStoreLocation                   " << m_TrackStoreLocation   ) ;
  ATH_MSG_INFO(  "= SegmentStoreLocation                 " << m_SegmentStoreLocation ) ;
  ATH_MSG_INFO(  "= NameOfMuonboyTrackStoreOutputFile    " << m_NameOfMuonboyTrackStoreOutputFile ) ;
  ATH_MSG_INFO(  "= NameOfMuonboySegmentStoreOutputFile  " << m_NameOfMuonboySegmentStoreOutputFile ) ;
  ATH_MSG_INFO(  "================================" ) ;

//Retrieve p_IMboyDumpMboyStoreTool
  if ( p_IMboyDumpMboyStoreTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDumpMboyStoreTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_IMboyDumpMboyStoreTool ) ;


//Set the file to dump the tracks
  m_OutMuonboyTrackStore.open(m_NameOfMuonboyTrackStoreOutputFile.c_str());
  m_OutMuonboyTrackStore << setiosflags(std::ios::fixed);
  std::string NameOfMboyViewOutputFileChecks = m_NameOfMuonboyTrackStoreOutputFile +"Checks" ;
  m_OutMuonboyTrackStoreChecks.open(NameOfMboyViewOutputFileChecks.c_str());
  m_OutMuonboyTrackStoreChecks << setiosflags(std::ios::fixed);


//Set the file to dump the Segment Store
  m_OutMuonboySegmentStore.open(m_NameOfMuonboySegmentStoreOutputFile.c_str());
  m_OutMuonboySegmentStore << setiosflags(std::ios::fixed);
  std::string NameOfMuonboySegmentStoreOutputFileChecks = m_NameOfMuonboySegmentStoreOutputFile +"Checks" ;
  m_OutMuonboySegmentStoreChecks.open(NameOfMuonboySegmentStoreOutputFileChecks.c_str());
  m_OutMuonboySegmentStoreChecks << setiosflags(std::ios::fixed);


  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}
 
// Finalize
StatusCode MboyChecks::finalize(){return StatusCode::SUCCESS;}
 
// Execute
StatusCode MboyChecks::execute()
{
 return p_IMboyDumpMboyStoreTool->dumpMboyStore(
                                    m_SegmentStoreLocation          , 
                                    m_TrackStoreLocation            ,
                                    &m_OutMuonboySegmentStore       ,
                                    &m_OutMuonboySegmentStoreChecks ,
                                    &m_OutMuonboyTrackStore         ,
                                    &m_OutMuonboyTrackStoreChecks  
                         );
                         
}
 
