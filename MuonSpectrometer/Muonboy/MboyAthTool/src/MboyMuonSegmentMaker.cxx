/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkRoad/TrackRoad.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MboyMuonSegmentMaker.h"

#include "MboyCoreEvt/MboyVarByName.h"
#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

#include "MboyAthEvt/MboyROAStoreAth.h"

namespace Muon {

//---------------------------------------------------------------------------------------------------------------//
MboyMuonSegmentMaker::MboyMuonSegmentMaker(const std::string& t, 
                                       const std::string& n,
                                       const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper                  ( "MboyAthToolHelper" )
{

  declareInterface<IMuonSegmentMaker>(this);
  declareInterface<IMboyMuonSegmentMaker>(this);

//Create a Variables carrier
  m_NberOfCtrlPpty = 0 ;
  p_MboyVarByName = 0 ;

//Define Proprieties
  declareControledProperty("LevelOfRecons" , 1 , "At what 'level' must MuonBoy stop ................................. " );
  declareControledProperty("TagadaTagada" , -100 );

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

  declareProperty("Clean"          , m_Clean          = 1                           ) ;

  declareProperty( "UseNewSegmentFinding", m_UseNewSegmentFinding           = 0 ) ;
}

MboyMuonSegmentMaker::~MboyMuonSegmentMaker(){
 delete p_MboyVarByName;
}

// Initialize
StatusCode MboyMuonSegmentMaker::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  ATH_MSG_INFO( "= ToolName                               " << m_ToolName                               ) ;
  ATH_MSG_INFO( "= Clean                                  " << m_Clean                                  ) ;
  ATH_MSG_INFO( "= UseNewSegmentFinding                   " << m_UseNewSegmentFinding                   ) ;
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

StatusCode MboyMuonSegmentMaker::finalize(){return StatusCode::SUCCESS;}

//---------------------------------------------------------------------------------------------------------------//
// Get Segments from PrepRawData

std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MboyROAStoreAth* pMboyROAExternalStoreAth){

  if (!p_MboyVarByName ) p_MboyVarByName = p_OwnEDMHelper->SetSettingsForCall(
                                                 m_ToolName          ,  
                                                 m_NberOfCtrlPpty    , 
                                                 m_NameOfCtrlPpty    , 
                                                 m_ValueOfCtrlPpty   , 
                                                 m_CommentOfCtrlPpty   
                                                ) ;
  return p_OwnEDMHelper->findFromPrep( p_MboyVarByName , VectorOfpPrepData, pMboyROAExternalStoreAth , m_Clean); 

}
std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData){
  MboyROAStoreAth* pMboyROAExternalStoreAth = 0 ;
  return findFromPrep(VectorOfpPrepData,pMboyROAExternalStoreAth);
}

//---------------------------------------------------------------------------------------------------------------//
// Get Segments vector from RIO_OnTrack

std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::find( const std::vector<const Trk::RIO_OnTrack*>& rios)
{

  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;
  p_OwnEDMHelper->CvRIO_OnTrack( VectorOfpPrepData , rios); 
  return findFromPrep(VectorOfpPrepData);

}

std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::find( const std::vector<const Trk::RIO_OnTrack*>& rios1,
 	                                                const std::vector<const Trk::RIO_OnTrack*>& rios2 )
{

  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;
  p_OwnEDMHelper->CvRIO_OnTrack( VectorOfpPrepData , rios1); 
  p_OwnEDMHelper->CvRIO_OnTrack( VectorOfpPrepData , rios2); 
  return findFromPrep(VectorOfpPrepData);

}

std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::find( const std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts,
 	                                                const std::vector<const Muon::MuonClusterOnTrack*>&  clusters)
{

  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;
  p_OwnEDMHelper->CvMdtDriftCircleOnTrack( VectorOfpPrepData , mdts); 
  p_OwnEDMHelper->CvMuonClusterOnTrack( VectorOfpPrepData , clusters);
  return findFromPrep(VectorOfpPrepData);

}

std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::find( const std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts,
 	                                                const std::vector<const Muon::MuonClusterOnTrack*>&  clusters,
                                                        MboyROAStoreAth* pMboyROAExternalStoreAth)
{

  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;
  p_OwnEDMHelper->CvMdtDriftCircleOnTrack( VectorOfpPrepData , mdts); 
  p_OwnEDMHelper->CvMuonClusterOnTrack( VectorOfpPrepData , clusters);
  return findFromPrep(VectorOfpPrepData,pMboyROAExternalStoreAth);

}

std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::find( const Amg::Vector3D& gpos , const Amg::Vector3D& gdir ,
                                                        const std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts,
                                                        const std::vector<const Muon::MuonClusterOnTrack*>&  clusters,
                                                        bool /* updatePhi */ , double /* momentum */)
{

  MboyROAStoreAth aMboyROAExternalStoreAth ;
  aMboyROAExternalStoreAth.Add(gpos,gdir); 
  return find(mdts,clusters,&aMboyROAExternalStoreAth);

}

std::vector<const Muon::MuonSegment*>* MboyMuonSegmentMaker::find( const Trk::TrackRoad& road ,
                                                        const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts,
                                                        const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >&  clusters,
                                                        bool /* updatePhi */ , double /* momentum */)
{

  MboyROAStoreAth aMboyROAExternalStoreAth ;
  aMboyROAExternalStoreAth.Add(road); 
  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;
  p_OwnEDMHelper->CvMdtDriftCircleOnTrack( VectorOfpPrepData , mdts) ;
  p_OwnEDMHelper->CvMuonClusterOnTrack( VectorOfpPrepData , clusters) ;
  return findFromPrep(VectorOfpPrepData,&aMboyROAExternalStoreAth);

}

//---------------------------------------------------------------------------------------------------------------//
// Controled proprieties

void MboyMuonSegmentMaker::declareControledProperty( std::string Name1, double Value, std::string Comment)
{
  m_NameOfCtrlPpty.push_back(Name1);
  m_ValueOfCtrlPpty[m_NberOfCtrlPpty] = Value ;
  m_CommentOfCtrlPpty.push_back(Comment);
  declareProperty(Name1, m_ValueOfCtrlPpty[m_NberOfCtrlPpty]);
  m_NberOfCtrlPpty = m_NberOfCtrlPpty + 1;
}
void MboyMuonSegmentMaker::PrintControledProperty()
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
