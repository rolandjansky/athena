/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : UserAnalysisOverlapCheckingTool.cxx
Package : offline/PhysicsAnalysis/AnalysisCommon/UserAnalysisUtils
Author  : Ketevi A. Assamagan
Created : November 2007
Purpose : User Analysis Overlap Checking - see UserAnalysisOverlapCheckingTool.h for details
*****************************************************************************/

#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

// Accessing data:
#include "CLHEP/Units/PhysicalConstants.h"

// User Tools
#include "UserAnalysisUtils/UserAnalysisOverlapCheckingTool.h"

#include <sstream>
#include <iomanip>
#include <iostream>

using namespace Analysis;
using namespace Rec;
using namespace std;

//------------------------------------------------------------------------------
UserAnalysisOverlapCheckingTool::UserAnalysisOverlapCheckingTool( const std::string& type,
                                                                  const std::string& name, 
                                                                  const IInterface* parent )
  : AthAlgTool( type, name, parent ),
    m_analysisTools( "AnalysisTools", this ) {

  declareInterface<UserAnalysisOverlapCheckingTool>( this );

  declareProperty( "AnalysisTools", m_analysisTools );
  declareProperty("OverlapDeltaR",          m_deltaR=0.2);
  declareProperty("OverlapDeltaRWithJets",  m_deltaRWithJets=0.3);

}

//------------------------------------------------------------------------------
StatusCode UserAnalysisOverlapCheckingTool::initialize() {

  /// get a handle on the analysis tools
  StatusCode sc = m_analysisTools.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis tools");
    return sc;
  }

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode UserAnalysisOverlapCheckingTool::finalize() {

  ATH_MSG_DEBUG("in finalize()");
 

  return StatusCode::SUCCESS;
}




