/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalMotherMonTool.cxx
 * Implementation of inner detector global monitoring mother tool
 *
 * @author Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch>     
 *
 * $Id: InDetGlobalMotherMonTool.cxx,v 1.42 2009-04-23 14:31:54 kastanas Exp $
 *
****************************************************************************/

//Local
#include "InDetGlobalMotherMonTool.h"
//Root
#include "TH1.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
//Standard c++
#include <vector>
#include <string>

//---------------------------------------------------------
InDetGlobalMotherMonTool::InDetGlobalMotherMonTool(
    const std::string & type, 
    const std::string & name,
    const IInterface* parent)						   
    : ManagedMonitorToolBase(type, name, parent),
      m_isOnline(false),
      m_isCosmicsRun(false),
      m_checkRate(1000),
      m_trackMax(1000),
      m_trackBin(100),
      m_d0Max(10),
      m_z0Max(50),
      m_nBinsEta(50),
      m_nBinsPhi(50),
      m_perEvent(500),
      m_evt(0),
      m_LayersB(73),  
      m_LayersEC(10*16),
      m_sct_tracks(0),
      m_trt_tracks(0),
      m_pix_tracks(0),
      m_combined_tracks(0),
      m_trtID(0),
      m_sctID(0),
      m_pixelID(0),
      m_BCM_RDO(nullptr),
      m_pixRdoContainer(0),
      m_sctRdoContainer(0),
      m_driftCircleContainer(0),   
      m_pixLVL1IDContainer(0),
      m_sctLVL1IDContainer(0),
      m_trtLVL1IDContainer(0),
      m_pixBCIDContainer(0),
      m_sctBCIDContainer(0),
      m_trtBCIDContainer(0),
      m_trtPhase(0),
      m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool")
{   
  declareProperty("checkRate",m_checkRate);  
  declareProperty("m_isOnline",m_isOnline);
  declareProperty("trackMax", m_trackMax);
  declareProperty("trackBin", m_trackBin);
  declareProperty("d0Max", m_d0Max);
  declareProperty("z0Max", m_z0Max);
  declareProperty("nBinsEta", m_nBinsEta);
  declareProperty("nBinsPhi", m_nBinsPhi);
  declareProperty("perEvent", m_perEvent); // Check what this is
  declareProperty("isCosmicsRun", m_isCosmicsRun);
    
}

// //--------------------------------------------------------------------
// StatusCode InDetGlobalMotherMonTool::bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
// {
//   if ( isNewEventsBlock || isNewLumiBlock || isNewRun ){
//     m_evt = 0;
//   }
  
//   if(m_environment==AthenaMonManager::tier0Raw){
//     m_isOnline = false;
//   }
//   if(m_environment==AthenaMonManager::tier0ESD){
//     m_isOnline = false;
//   }
  
//   return StatusCode::SUCCESS;
// }

// //----------------------------------------------------------
// StatusCode InDetGlobalMotherMonTool::fillHistograms()
// {  
//   return StatusCode::SUCCESS;
// }

// //----------------------------------------------------------
// StatusCode InDetGlobalMotherMonTool::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
// {
//   if ( isEndOfEventsBlock || isEndOfLumiBlock || isEndOfRun ){ 
//     m_evt = 0;
//   }
  
//   return StatusCode::SUCCESS;
// }


//-------------------------------------------------------------------------
StatusCode InDetGlobalMotherMonTool::registerHist(
						  MonGroup & theGroup, 
						  TH1* h1)
{   
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH1F* h1 )
{   
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH1I* h1 )
{   
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}
StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH2F* h2 )
{   
  StatusCode rc = theGroup.regHist(h2) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH2I* h2 )
{   
  StatusCode rc = theGroup.regHist(h2) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TProfile* h1 )
{   
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}


StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH1I_LW* h1 )
{    
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH2I_LW* h1 )
{    
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH1F_LW* h1 )
{    
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TH2F_LW* h2 )
{    
  StatusCode rc = theGroup.regHist(h2) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}

StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TProfile_LW* h1 )
{    
  StatusCode rc = theGroup.regHist(h1) ;    
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}


StatusCode InDetGlobalMotherMonTool::registerHist( MonGroup & theGroup, 
						   TProfile2D* h2 )
{    
  StatusCode rc = theGroup.regHist(h2);
  if (rc != StatusCode::SUCCESS) 
    {  
      //std::string msgtext = "Could not register histogram ";  //fix this it did not like the variable
      //if (h1!=0) msg+=h1->GetName();
      if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Could not register histogram :" << endreq;
    }    
  return rc;
}
