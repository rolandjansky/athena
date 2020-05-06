/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibTools/PatternNtupleMaker.h"

#include "MuonCalibNtuple/RootFileManager.h"
#include "MuonCalibNtuple/MuonCalibBranchNtuple.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"

namespace MuonCalib {
  
  PatternNtupleMaker::PatternNtupleMaker(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),m_ntupleName("PatternNtupleMaker"),m_printLevel(0),
    m_delayFinish(false),m_ntMaker(0), m_fillTruth(true)
  {
    declareInterface< IMuonCalibTool >(this) ;
    
    declareProperty("NtupleName",m_ntupleName);
    declareProperty("PrintLevel",m_printLevel);
    declareProperty("DelayFinish",m_delayFinish);
    declareProperty("FillTruth", m_fillTruth);
    declareProperty("DoRawTGC", m_doRawTgc=false);
  }

  StatusCode PatternNtupleMaker::initialize() 
  {
    ATH_MSG_DEBUG("initialize ");

    
    ATH_MSG_DEBUG("building MuonCalibBranchNtuple with name " << m_ntupleName);
    
    m_ntMaker = new MuonCalibBranchNtuple(m_ntupleName.c_str(), m_fillTruth, m_doRawTgc);
    ATH_MSG_DEBUG("Done building the MuonCalibBranchNtuple");
    m_ntMaker->setPrintLevel(m_printLevel);

    return StatusCode::SUCCESS;
  }

  StatusCode PatternNtupleMaker::finalize()
  {
    ATH_MSG_DEBUG("finalize ");
   if(m_ntMaker) delete m_ntMaker;     
    return StatusCode::SUCCESS;
  }
  
  
  bool PatternNtupleMaker::handleEventInfo( const MuonCalibEventInfo* info, const MuonCalibTriggerTimeInfo* trigTimeInfo )
  {

    
    ATH_MSG_DEBUG("handleEventInfo, "); 
    if (! info ) {
      ATH_MSG_INFO( "Invalid EventInfo pointer not filled ");
      return false;
    }else{
      ATH_MSG_DEBUG("tagged with " << info->tag());
      m_ntMaker->fillEventToTree( *info, *trigTimeInfo );
    }
    return true;
  }

  bool PatternNtupleMaker::handleTruthCollection( const MuonCalibTruthCollection* truthCollection ) 
  {
    if( !truthCollection ) return false;

    ATH_MSG_DEBUG("handleTruthCollection " << truthCollection->numberOfTruth());
    // loop over MuonCalibTruths

    MuonCalibTruthCollection::TruthVecCit truth_it     = truthCollection->truthCollectionBegin();
    MuonCalibTruthCollection::TruthVecCit truth_it_end = truthCollection->truthCollectionEnd();
    for( ;truth_it!=truth_it_end; ++truth_it ){
      if( !(*truth_it) ) {
	ATH_MSG_INFO( "Invalid MuonCalibTruth pointer not filled ");
	return false;
      }else{
	
	//Do not store the numerous photons/neutrons on ntuple.
	if( ( (*truth_it)->PDGCode() == 22) || ( std::abs( (*truth_it)->PDGCode()) == 2112) )  continue ;
	else m_ntMaker->fillTruthToTree( **truth_it );
      }
    }  

    ATH_MSG_DEBUG("filling MuonCalibMdtTruthHit ");
    MuonCalibTruthCollection::MdtTruthVecCit mdt_truth_it     = truthCollection->mdtTruthCollectionBegin();
    MuonCalibTruthCollection::MdtTruthVecCit mdt_truth_it_end = truthCollection->mdtTruthCollectionEnd();
    for( ;mdt_truth_it!=mdt_truth_it_end; ++mdt_truth_it ){
      if( !(*mdt_truth_it) ) {
	ATH_MSG_INFO( "Invalid MuonCalibMdtTruthHit pointer not filled ");
	return false;
      }else{
	m_ntMaker->fillMdtTruthToTree( **mdt_truth_it );
      }
    } 

    ATH_MSG_DEBUG("filling MuonCalibRpcTruthHit ");
    MuonCalibTruthCollection::RpcTruthVecCit rpc_truth_it     = truthCollection->rpcTruthCollectionBegin();
    MuonCalibTruthCollection::RpcTruthVecCit rpc_truth_it_end = truthCollection->rpcTruthCollectionEnd();
    for( ;rpc_truth_it!=rpc_truth_it_end; ++rpc_truth_it ){
      if( !(*rpc_truth_it) ) {
	ATH_MSG_INFO( "Invalid MuonCalibRpcTruthHit pointer not filled ");
	return false;
      }else{
	m_ntMaker->fillRpcTruthToTree( **rpc_truth_it );
      }
    } 

    ATH_MSG_DEBUG("filling MuonCalibTgcTruthHit ");
    MuonCalibTruthCollection::TgcTruthVecCit tgc_truth_it     = truthCollection->tgcTruthCollectionBegin();
    MuonCalibTruthCollection::TgcTruthVecCit tgc_truth_it_end = truthCollection->tgcTruthCollectionEnd();
    for( ;tgc_truth_it!=tgc_truth_it_end; ++tgc_truth_it ){
      if( !(*tgc_truth_it) ) {
	ATH_MSG_INFO( "Invalid MuonCalibTgcTruthHit pointer not filled ");
	return false;
      }else{
	m_ntMaker->fillTgcTruthToTree( **tgc_truth_it );
      }
    } 

    ATH_MSG_DEBUG("filling MuonCalibCscTruthHit ");
    MuonCalibTruthCollection::CscTruthVecCit csc_truth_it     = truthCollection->cscTruthCollectionBegin();
    MuonCalibTruthCollection::CscTruthVecCit csc_truth_it_end = truthCollection->cscTruthCollectionEnd();
    for( ;csc_truth_it!=csc_truth_it_end; ++csc_truth_it ){
      if( !(*csc_truth_it) ) {
	ATH_MSG_INFO( "Invalid MuonCalibCscTruthHit pointer not filled ");
	return false;
      }else{
	m_ntMaker->fillCscTruthToTree( **csc_truth_it );
      }
    } 
 
  
    ATH_MSG_DEBUG("handleTruthCollection Done ");
    
    return true;
  }

  bool PatternNtupleMaker::analyse()
  {
    // write ntuple to file
    ATH_MSG_DEBUG("PatternNtupleMaker::analyse(), start writing tree ");
    m_ntMaker->writeTree(); 
    ATH_MSG_DEBUG("PatternNtupleMaker::analyse(), Done writing tree ");
    return true;
  }
  
  bool PatternNtupleMaker::handleEvent( const MuonCalibEvent* event )
  {
    bool handled = false;
    ATH_MSG_DEBUG("PatternNtupleMaker::handleEvent( MuonCalibEvent ) ");
   
    handled = handleEventInfo( &(event->eventInfo()), event->triggerTimeInfo() );
    handled = handleTruthCollection( event->calibTruthCollection() );

    MuonCalibEvent::MCPVecCit pat_it     = event->patternBegin();
    MuonCalibEvent::MCPVecCit pat_it_end = event->patternEnd(); 
    for(; pat_it != pat_it_end; ++pat_it){
      handlePattern( *pat_it );
    }
    handled = handleRawHits( event->rawHitCollection() );
    
    if( event->rawTriggerHitCollection() ){
      m_ntMaker->handleRawTriggerHits(*event->rawTriggerHitCollection());
    }

    if( event->rpcSectorLogicContainer() ){
      m_ntMaker->fillRpcSectorLogic(*event->rpcSectorLogicContainer());
    }

    //When using the ExtraTreeAlg, the finishEvent causes the event to be written.
    if(m_delayFinish==false)
      m_ntMaker->finishEvent();
      
    return handled;
  }

  bool PatternNtupleMaker::handlePattern( const MuonCalibPattern* pattern )
  {
    
    if( ! pattern ) {
      ATH_MSG_INFO( "Invalid Segment pointer not filled ");
    }else{
      m_ntMaker->handlePattern( *pattern );
    ATH_MSG_DEBUG("New pattern with segments " << pattern->muonSegments());
    }
    
    ATH_MSG_DEBUG("handlePattern Done ");
    return true;
  }
  
  bool PatternNtupleMaker::handleRawHits( const MuonCalibRawHitCollection* rawHits ){

    if( !rawHits )
      ATH_MSG_INFO( "Invalid MuonCalibRawHitCollection pointer: not filled ");
    else m_ntMaker->handleRawHits( *rawHits );

    return true;
  }
}
