/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MuonSegment/MuonSegment.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyMooSegmentCombinationFinder.h"

#include "MboyCoreEvt/MboyVarByName.h"
#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

namespace Muon {

//---------------------------------------------------------------------------------------------------------------//
MboyMooSegmentCombinationFinder::MboyMooSegmentCombinationFinder(const std::string& t, 
                                       const std::string& n,
                                       const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper                  ( "MboyAthToolHelper" )
{

  declareInterface<IMooSegmentCombinationFinder>(this);
  declareInterface<IMboyMooSegmentCombinationFinder>(this);

//Create a Variables carrier
  m_NberOfCtrlPpty = 0 ;
  p_MboyVarByName = 0 ;

//Define Proprieties
  declareControledProperty("LevelOfRecons" , 1 , "At what 'level' must MuonBoy stop ................................. " );
  declareControledProperty("TsoinTsoin"    , -100 );

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

  declareProperty("Clean"          , m_Clean          = 1                           ) ;

}

MboyMooSegmentCombinationFinder::~MboyMooSegmentCombinationFinder(){
 delete p_MboyVarByName;
}

// Initialize
StatusCode MboyMooSegmentCombinationFinder::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  ATH_MSG_INFO( "= ToolName                               " << m_ToolName                               ) ;
  ATH_MSG_INFO( "= Clean                                  " << m_Clean                                  ) ;
  PrintControledProperty() ;
  ATH_MSG_INFO( "================================" ) ;


//Retrieve p_OwnEDMHelper
  if ( p_OwnEDMHelper.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_OwnEDMHelper ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_OwnEDMHelper ) ;


  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyMooSegmentCombinationFinder::finalize(){return StatusCode::SUCCESS;}

//---------------------------------------------------------------------------------------------------------------//
// Get Segments from PrepRawData

std::vector<const Muon::MuonSegment*>* MboyMooSegmentCombinationFinder::findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MboyROAStoreAth* pMboyROAExternalStoreAth ){

  if (!p_MboyVarByName ) p_MboyVarByName = p_OwnEDMHelper->SetSettingsForCall(
                                                 m_ToolName          ,  
                                                 m_NberOfCtrlPpty    , 
                                                 m_NameOfCtrlPpty    , 
                                                 m_ValueOfCtrlPpty   , 
                                                 m_CommentOfCtrlPpty   
                                                ) ;
  return p_OwnEDMHelper->findFromPrep( p_MboyVarByName , VectorOfpPrepData, pMboyROAExternalStoreAth , m_Clean); 

}
std::vector<const Muon::MuonSegment*>* MboyMooSegmentCombinationFinder::findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData){
  MboyROAStoreAth* pMboyROAExternalStoreAth = 0 ;
  return findFromPrep(VectorOfpPrepData,pMboyROAExternalStoreAth);
}

//---------------------------------------------------------------------------------------------------------------//
// Get Segments vector from Prep collections

Muon::IMooSegmentCombinationFinder::Output* MboyMooSegmentCombinationFinder::MboyMooSegmentCombinationFinder::findSegments( 
                      const std::vector<const MdtPrepDataCollection*>& mdtCols, 
                      const std::vector<const CscPrepDataCollection*>& cscCols, 
                      const std::vector<const TgcPrepDataCollection*>& tgcCols, 
                      const std::vector<const RpcPrepDataCollection*>& rpcCols )
{

  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;

  p_OwnEDMHelper->CvMdtPrepDataCollection( VectorOfpPrepData , mdtCols ) ;
  p_OwnEDMHelper->CvCscPrepDataCollection( VectorOfpPrepData , cscCols ) ;
  p_OwnEDMHelper->CvTgcPrepDataCollection( VectorOfpPrepData , tgcCols ) ;
  p_OwnEDMHelper->CvRpcPrepDataCollection( VectorOfpPrepData , rpcCols ) ;

  std::vector<const Muon::MuonSegment*>* pVectorOfMuonSegment = findFromPrep(VectorOfpPrepData);
  
  Trk::SegmentCollection* pSegmentCollection = new Trk::SegmentCollection ;
  std::vector<const Muon::MuonSegment*>::const_iterator it     = pVectorOfMuonSegment->begin() ;
  std::vector<const Muon::MuonSegment*>::const_iterator it_end = pVectorOfMuonSegment->end()   ;
  for( ;  it != it_end; ++it){
    const Muon::MuonSegment* pMuonSegment = *it;
    pSegmentCollection->push_back(const_cast <Muon::MuonSegment*>(pMuonSegment)); 
  }
  delete pVectorOfMuonSegment ;
  
  Muon::IMooSegmentCombinationFinder::Output* output = new Muon::IMooSegmentCombinationFinder::Output();
  output->segmentCollection = pSegmentCollection  ; 
  
  return output;
}

//---------------------------------------------------------------------------------------------------------------//
// Controled proprieties

void MboyMooSegmentCombinationFinder::declareControledProperty( std::string Name1, double Value, std::string Comment)
{
  m_NameOfCtrlPpty.push_back(Name1);
  m_ValueOfCtrlPpty[m_NberOfCtrlPpty] = Value ;
  m_CommentOfCtrlPpty.push_back(Comment);
  declareProperty(Name1, m_ValueOfCtrlPpty[m_NberOfCtrlPpty]);
  m_NberOfCtrlPpty = m_NberOfCtrlPpty + 1;
}
void MboyMooSegmentCombinationFinder::PrintControledProperty()
{
  int itemMax = m_NberOfCtrlPpty;
  for (int item = 0 ; item < itemMax ; item++){    
    std::string Name1   = m_NameOfCtrlPpty[item];
    double      Value   = m_ValueOfCtrlPpty[item] ;
    std::string Comment = m_CommentOfCtrlPpty[item];
    Name1.resize(30,' ');
    Comment.resize(70,' ');
    ATH_MSG_INFO( "= " << Name1  << " " << Comment << " " << Value ) ;
  }
}

}
