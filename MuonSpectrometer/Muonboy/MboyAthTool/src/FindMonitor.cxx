/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackFindingEvent/MuPatCandidateBase.h"
#include "MuonTrackFindingEvent/MuPatSegment.h"
#include "MuonSegment/MuonSegment.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/FindMonitor.h"
#include "MboyAthToolInterfaces/IMboyAthToolHelper.h"

namespace Muon {

//---------------------------------------------------------------------------------------------------------------//
FindMonitor::FindMonitor(const std::string& t, 
                                       const std::string& n,
                                       const IInterface*  p ):AthAlgTool(t,n,p),
 p_OwnEDMHelper                  ( "MboyAthToolHelper" )
{

  declareInterface<IFindMonitor>(this);

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;
  declareProperty("MboyAthToolHelper"                 , p_OwnEDMHelper          ) ;

}

FindMonitor::~FindMonitor(){}

// Initialize
StatusCode FindMonitor::initialize(){

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

StatusCode FindMonitor::finalize(){return StatusCode::SUCCESS;}

void FindMonitor::Reset() 
{

  m_CalledConfig.clear() ;

}

bool FindMonitor::AlreadySuccessfullyTried ( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree)
{

  Muon::MuPatSegment* pMuPatSegmentFix = dynamic_cast<Muon::MuPatSegment*> (&candidate);
  Muon::MuPatTrack*   pMuPatTrackFix   = dynamic_cast<Muon::MuPatTrack*>   (&candidate);
  if ( pMuPatSegmentFix ==0 && pMuPatTrackFix ==0 ) {
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "IsRegistered  FAILURE  " ) ;
    ATH_MSG_DEBUG( "Candidate can not be cast into  MuPatSegment/MuPatTrack    " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return false ;
  }

  std::vector<const Muon::MuonSegment*> Vector_MuonSegment ;
  std::vector< Muon::MuPatSegment* >::const_iterator it_tg      = MuPatSegmentFree.begin() ;  
  std::vector< Muon::MuPatSegment* >::const_iterator it_tg_end  = MuPatSegmentFree.end()   ;
  for(; it_tg!=it_tg_end;++it_tg ){  Vector_MuonSegment.push_back( (*it_tg)->segment ) ; }
  if ( pMuPatTrackFix ) {
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd      = (pMuPatTrackFix->segments()).begin() ;  
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd_end  = (pMuPatTrackFix->segments()).end()   ;
    for(; it_cd!=it_cd_end;++it_cd ){  Vector_MuonSegment.push_back( (*it_cd)->segment ) ; }
  }
  if (pMuPatSegmentFix) Vector_MuonSegment.push_back( pMuPatSegmentFix->segment ) ;

  std::vector< std::vector<const Muon::MuonSegment*> >::iterator   it1       = m_CalledConfig.begin();
  std::vector< std::vector<const Muon::MuonSegment*> >::iterator   it1_end   = m_CalledConfig.end(); 

  int IsConfigurationFound = 0 ;
  for(; it1!=it1_end;++it1 ){
    if ( Vector_MuonSegment.size() == (*it1).size() && (*it1).size() == 3 ){
      int IsConfigurationDifferent = 0 ; 
      
      std::vector<const Muon::MuonSegment* >::iterator itin     = Vector_MuonSegment.begin();
      std::vector<const Muon::MuonSegment* >::iterator itin_end = Vector_MuonSegment.end();
      for(; itin!=itin_end;++itin ){ 
        int IsSegmentfound = 0 ;
        std::vector<const Muon::MuonSegment* >::iterator it2     = (*it1).begin();
        std::vector<const Muon::MuonSegment* >::iterator it2_end = (*it1).end();
        for(; it2!=it2_end;++it2 ){
          if ( (*itin) == (*it2) ){
            IsSegmentfound = 1 ;
            break ;
          }
        }
        if ( IsSegmentfound == 0 ) {
          IsConfigurationDifferent = 1 ;
          break ;
        }
      }
      
      if ( IsConfigurationDifferent == 0 ) {
        IsConfigurationFound = 1 ;
      }
      
    }
      
    if (IsConfigurationFound == 1 ){
      return true ;
    }

  }

  return false ;

}


void FindMonitor::RegisterIfSuccessful     ( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree, const std::vector<Muon::MuPatTrack*>* pVectorpMuPatTrack )
{
  
  Muon::MuPatSegment* pMuPatSegmentFix = dynamic_cast<Muon::MuPatSegment*> (&candidate);
  Muon::MuPatTrack*   pMuPatTrackFix   = dynamic_cast<Muon::MuPatTrack*>   (&candidate);
  if ( pMuPatSegmentFix ==0 && pMuPatTrackFix ==0 ) {
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "Register  FAILURE  " ) ;
    ATH_MSG_DEBUG( "Candidate can not be cast into  MuPatSegment/MuPatTrack    " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return  ;
  }

  if (pVectorpMuPatTrack==0) return;
  if (pVectorpMuPatTrack->size()==0) return;
  
  std::vector<const Muon::MuonSegment*> Vector_MuonSegment ;
  std::vector< Muon::MuPatSegment* >::const_iterator it_tg      = MuPatSegmentFree.begin() ;  
  std::vector< Muon::MuPatSegment* >::const_iterator it_tg_end  = MuPatSegmentFree.end()   ;
  for(; it_tg!=it_tg_end;++it_tg ){  Vector_MuonSegment.push_back( (*it_tg)->segment ) ; }
  if ( pMuPatTrackFix ) {
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd      = (pMuPatTrackFix->segments()).begin() ;  
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd_end  = (pMuPatTrackFix->segments()).end()   ;
    for(; it_cd!=it_cd_end;++it_cd ){  Vector_MuonSegment.push_back( (*it_cd)->segment ) ; }
  }
  if (pMuPatSegmentFix) Vector_MuonSegment.push_back( pMuPatSegmentFix->segment ) ;

  m_CalledConfig.push_back(Vector_MuonSegment) ;

}

void FindMonitor::PrintSegConfig ( Muon::MuPatCandidateBase& candidate, const std::vector<Muon::MuPatSegment*>& MuPatSegmentFree )
{

  Muon::MuPatSegment* pMuPatSegmentFix = dynamic_cast<Muon::MuPatSegment*> (&candidate);
  Muon::MuPatTrack*   pMuPatTrackFix   = dynamic_cast<Muon::MuPatTrack*>   (&candidate);
  if ( pMuPatSegmentFix ==0 && pMuPatTrackFix ==0 ) {
    ATH_MSG_DEBUG( "    " ) ;
    ATH_MSG_DEBUG( "Register  FAILURE  " ) ;
    ATH_MSG_DEBUG( "Candidate can not be cast into  MuPatSegment/MuPatTrack    " ) ;
    ATH_MSG_DEBUG( "    " ) ;
    return  ;
  }

  if ( pMuPatTrackFix ) {
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd      = (pMuPatTrackFix->segments()).begin() ;  
    std::vector< Muon::MuPatSegment* >::const_iterator it_cd_end  = (pMuPatTrackFix->segments()).end()   ;
    for(; it_cd!=it_cd_end;++it_cd ){  
      ATH_MSG_INFO( "|   " << p_OwnEDMHelper->SegmentAssociatedStationName( (*it_cd)->segment ) );
    }
  }
  if (pMuPatSegmentFix)  
      ATH_MSG_INFO( " |  " << p_OwnEDMHelper->SegmentAssociatedStationName(  pMuPatSegmentFix->segment ) ) ;

  std::vector<const Muon::MuonSegment*> Vector_MuonSegment ;
  std::vector< Muon::MuPatSegment* >::const_iterator it_tg      = MuPatSegmentFree.begin() ;  
  std::vector< Muon::MuPatSegment* >::const_iterator it_tg_end  = MuPatSegmentFree.end()   ;
  for(; it_tg!=it_tg_end;++it_tg ){  
      ATH_MSG_INFO( "   |" << p_OwnEDMHelper->SegmentAssociatedStationName( (*it_tg)->segment ) );
  }

}


}
