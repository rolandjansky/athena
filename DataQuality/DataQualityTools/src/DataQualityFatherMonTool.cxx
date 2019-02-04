/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DataQualityMonTool.cxx
// PACKAGE:  DataQualityTools
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
// 
// July 2006	
// ********************************************************************

#include "DataQualityTools/DataQualityFatherMonTool.h"

//----------------------------------------------------------------------------------
DataQualityFatherMonTool::DataQualityFatherMonTool(const std::string & type, 
						   const std::string & name,
						   const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent)
  , m_detStore(detStore().operator->())
 //----------------------------------------------------------------------------------
{
  declareInterface<IMonitorToolBase>(this);

  //declareProperty("histoPathBase", m_path = "DataQuality");
  declareProperty("doRunCosmics", m_doRunCosmics = 1);
  declareProperty("doRunBeam", m_doRunBeam = 1);
  declareProperty("doOfflineHists", m_doOfflineHists = 1);
  declareProperty("doOnlineHists", m_doOnlineHists = 1);

  // property histoPathBase in MonitorToolBase
  //m_stream = "/SHIFT";
}

//----------------------------------------------------------------------------------
DataQualityFatherMonTool::~DataQualityFatherMonTool()
//----------------------------------------------------------------------------------
{
}

/*---------------------------------------------------------*/
StatusCode DataQualityFatherMonTool:: initialize()
/*---------------------------------------------------------*/
{

  StatusCode sc;
  //m_stem=m_THistSvc_streamname+m_path;
  //sc = service( "StoreGateSvc", m_storeGate);
  //if( sc.isFailure() )
  //  {
  //    log << MSG::FATAL << name() << ": Unable to locate Service StoreGateSvc" << endmsg;
  //    return sc;
  //  }

  /*  
  sc = service("DetectorStore", m_detStore);
  if( sc.isFailure() )
    {
      log << MSG::ERROR << "Unable to get pointer to DetectorStore Service" << endmsg;
      return sc;
    }
  */

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure())  {
    ATH_MSG_ERROR( "Unable to Initialize ManagedMonitoringToolBase::initialize" );
    return sc;
  }

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::bookHistogramsRecurrent( )
//----------------------------------------------------------------------------------
{ 

  if ( newLumiBlockFlag() || newRunFlag() ) {

    ATH_MSG_DEBUG( "in bookHists()" );
    ATH_MSG_DEBUG( "Using base path " << m_path );
  }
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::bookHistograms( )
//----------------------------------------------------------------------------------
{ 

  ATH_MSG_DEBUG( "in bookHists()" );
  ATH_MSG_DEBUG( "Using base path " << m_path );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::fillHistograms()
//----------------------------------------------------------------------------------
{ 

  ATH_MSG_DEBUG( "in fillHists()" );
    
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::procHistograms( )
//----------------------------------------------------------------------------------
{ 

  ATH_MSG_DEBUG( "in procHistograms()" );
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------

{ 
  ATH_MSG_DEBUG( "in checkHists()" );
    
  return StatusCode::SUCCESS;
}


////----------------------------------------------------------------------------------
//StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH1F_LW* h1, LevelOfDetail_t level, Interval_t interval,  std::string dqmAlgorithm)
////----------------------------------------------------------------------------------
//{
//  if(ManagedMonitorToolBase::regHist(h1, path, level, interval, dqmAlgorithm) != StatusCode::SUCCESS) {
//    MsgStream log(msgSvc(), name());
//    log << MSG::WARNING << "Could not register histogram : " 
//	<< levelOfDetailEnumToString(level)+"/"+path+"/"+h1->GetName() << endmsg;
//    return StatusCode::FAILURE;
//  }
//  return StatusCode::SUCCESS;
//}
//
////----------------------------------------------------------------------------------
//StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH1I_LW* h1, LevelOfDetail_t level, Interval_t interval,  std::string dqmAlgorithm)
////----------------------------------------------------------------------------------
//{
//  if(ManagedMonitorToolBase::regHist(h1, path, level, interval, dqmAlgorithm) != StatusCode::SUCCESS) {
//    MsgStream log(msgSvc(), name());
//    log << MSG::WARNING << "Could not register histogram : " 
//        << levelOfDetailEnumToString(level)+"/"+path+"/"+h1->GetName() << endmsg;
//    return StatusCode::FAILURE;
//  }
//  return StatusCode::SUCCESS;
//}
//
//
////----------------------------------------------------------------------------------
//StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH2F_LW* h1, LevelOfDetail_t level, Interval_t interval,  std::string dqmAlgorithm)
////----------------------------------------------------------------------------------
//{
//  if(ManagedMonitorToolBase::regHist(h1, path, level, interval, dqmAlgorithm) != StatusCode::SUCCESS) {
//    MsgStream log(msgSvc(), name());
//    log << MSG::WARNING << "Could not register histogram : " 
//	<< levelOfDetailEnumToString(level)+"/"+path+"/"+h1->GetName() << endmsg;
//    return StatusCode::FAILURE;
//  }
//  return StatusCode::SUCCESS;
//}
//
////----------------------------------------------------------------------------------
//StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH2I_LW* h1, LevelOfDetail_t level, Interval_t interval,  std::string dqmAlgorithm)
////----------------------------------------------------------------------------------
//{
//  if(ManagedMonitorToolBase::regHist(h1, path, level, interval, dqmAlgorithm) != StatusCode::SUCCESS) {
//    MsgStream log(msgSvc(), name());
//    log << MSG::WARNING << "Could not register histogram : " 
//	<< levelOfDetailEnumToString(level)+"/"+path+"/"+h1->GetName() << endmsg;
//    return StatusCode::FAILURE;
//  }
//  return StatusCode::SUCCESS;
//}
//
////----------------------------------------------------------------------------------
//StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TProfile_LW* h1, LevelOfDetail_t level, Interval_t interval,  std::string dqmAlgorithm)
////----------------------------------------------------------------------------------
//{
//  if(ManagedMonitorToolBase::regHist(h1, path, level, interval, dqmAlgorithm) != StatusCode::SUCCESS) {
//    MsgStream log(msgSvc(), name());
//    log << MSG::WARNING << "Could not register histogram : "
//        << levelOfDetailEnumToString(level)+"/"+path+"/"+h1->GetName() << endmsg;
//    return StatusCode::FAILURE;
//  }
//  return StatusCode::SUCCESS;
//}
//
////----------------------------------------------------------------------------------
//StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TProfile2D_LW* h1, LevelOfDetail_t level, Interval_t interval,  std::string dqmAlgorithm)
////----------------------------------------------------------------------------------
//{
//  if(ManagedMonitorToolBase::regHist(h1, path, level, interval, dqmAlgorithm) != StatusCode::SUCCESS) {
//    MsgStream log(msgSvc(), name());
//    log << MSG::WARNING << "Could not register histogram : "
//        << levelOfDetailEnumToString(level)+"/"+path+"/"+h1->GetName() << endmsg;
//    return StatusCode::FAILURE;
//  }
//  return StatusCode::SUCCESS;
//}

//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH2I_LW* h1, Interval_t interval, MgmtAttr_t histo_mgmt)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regHist(h1, path, interval, histo_mgmt) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH1I_LW* h1, Interval_t interval, MgmtAttr_t histo_mgmt)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regHist(h1, path, interval, histo_mgmt) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}



//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH2F_LW* h1, Interval_t interval, MgmtAttr_t histo_mgmt)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regHist(h1, path, interval, histo_mgmt) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH1F_LW* h1, Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& merge_algo)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regHist(h1, path, interval, histo_mgmt, "", merge_algo) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TH1* h1, Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& merge_algo)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regHist(h1, path, interval, histo_mgmt, "", merge_algo) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TProfile_LW* h1, Interval_t interval, MgmtAttr_t histo_mgmt)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regHist(h1, path, interval, histo_mgmt) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TProfile2D_LW* h1, Interval_t interval, MgmtAttr_t histo_mgmt)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regHist(h1, path, interval, histo_mgmt) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DataQualityFatherMonTool::registerHist(const std::string& path, TGraph* h1, Interval_t interval, MgmtAttr_t histo_mgmt)
//----------------------------------------------------------------------------------
{
  if(ManagedMonitorToolBase::regGraph(h1, path, interval, histo_mgmt) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register histogram : " 
		     << "/"+path+"/"+h1->GetName() );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

