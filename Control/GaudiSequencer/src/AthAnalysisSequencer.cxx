///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisSequencer.cxx 
// Implementation file for class AthAnalysisSequencer
// Author: Carsten Burgard
/////////////////////////////////////////////////////////////////// 

// GaudiSequencer includes
#include "AthAnalysisSequencer.h"

// STL includes

// FrameWork includes
//#include "GaudiKernel/Property.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthAnalysisSequencer::AthAnalysisSequencer( const std::string& name, 
                                            ISvcLocator* pSvcLocator ) : 
  ::AthSequencer( name, pSvcLocator )
{
}

// Destructor
///////////////
AthAnalysisSequencer::~AthAnalysisSequencer()
{}
