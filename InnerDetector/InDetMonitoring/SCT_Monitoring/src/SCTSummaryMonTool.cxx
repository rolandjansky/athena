/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTSummaryMonTool.cxx
*
*    @author Elias Coniavitis based on code from Luca Fiorini,
*    Shaun Roe, Manuel Diaz, Rob McPherson & Richard Batley
*    Modified by Yuta
*/
#include "SCT_Monitoring/SCTSummaryMonTool.h"
#include "deletePointers.h"
#include "SCT_NameFormatter.h"
#include "boost/lexical_cast.hpp"
#include <cmath>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TF1.h"
#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"


using namespace std;
using namespace Rec;
using namespace SCT_Monitoring;


//====================================================================================================
/** Constructor, calls base class constructor with parameters
*
*  several properties are "declared" here, allowing selection
*  of the filepath for histograms, the first and second plane
*  numbers to be used, and the timebin.
*/
//====================================================================================================
SCTSummaryMonTool::SCTSummaryMonTool(const string & type,const string & name,const IInterface* parent):SCTMotherTrigMonTool(type, name, parent)
{
  m_numberOfEvents=0;
}

//====================================================================================================
//====================================================================================================
SCTSummaryMonTool::~SCTSummaryMonTool(){
  //nada
}

//====================================================================================================
//                       SCTSummaryMonTool :: bookHistograms
//====================================================================================================
//StatusCode SCTSummaryMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun */)//suppress 'unused' compiler warning
StatusCode SCTSummaryMonTool::bookHistograms()
{ 
  return StatusCode::SUCCESS;
}

StatusCode SCTSummaryMonTool::bookHistogramsRecurrent()
{ 
  return StatusCode::SUCCESS;
}

//====================================================================================================
//                        SCTSummaryMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
//====================================================================================================
StatusCode SCTSummaryMonTool::fillHistograms(){
    return StatusCode::SUCCESS;
  }


//====================================================================================================
//                             SCTSummaryMonTool :: procHistograms
//====================================================================================================
//  StatusCode  SCTSummaryMonTool::procHistograms(bool /*isEndOfEventsBlock*/, bool /*isEndOfLumiBlock*/, bool /*isEndOfRun*/){//suppress 'unused' compiler warning
  StatusCode  SCTSummaryMonTool::procHistograms(){
  
  return StatusCode::SUCCESS;
}



