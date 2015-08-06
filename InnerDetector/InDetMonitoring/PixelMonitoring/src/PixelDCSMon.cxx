/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill offline histograms showing status of modules
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TH1F.h"   
#include "TH1I.h"   
#include "TH2F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "TProfile2D.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "InDetIdentifier/PixelID.h"
#include <sstream>
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMonProfiles.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookPixelDCSMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Status" << endreq;  

   msg(MSG::INFO)  << "[BookPixelDCSMon]" << endreq;  

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;         
   return StatusCode::SUCCESS;
}  

StatusCode PixelMainMon::FillPixelDCSMon(void)
{
   msg(MSG::INFO)  << "[FillPixelDCSMon]" << endreq;  
   
   return StatusCode::SUCCESS;
}   

StatusCode PixelMainMon::ProcPixelDCSMon(void)
{
   msg(MSG::INFO)  << "[ProcPixelDCSMon]" << endreq;  
   return StatusCode::SUCCESS;                  
}                                               

