///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HistAlg.cxx 
// Implementation file for class HistAlg
// Author: Carsten Burgard <cburgard@cern.ch>
/////////////////////////////////////////////////////////////////// 

// HistogramUtils includes
#include "HistAlg.h"
#include "HistogramTool.h"

// STL includes
#include <iostream>

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "PATCore/IAthHistogramTool.h"

// ROOT includes
#include "TH1.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
HistAlg::HistAlg( const std::string& name, 
		  ISvcLocator* pSvcLocator ) : 
  ::AthHistogramAlgorithm( name, pSvcLocator ),
  m_histToolList(this)
{
  //
  // Property declaration
  // 
  //declareProperty( "HistToolList", m_histToolList, "List of histogram tools to be employed by this algorithm" );
  declareProperty( "HistToolList", m_histToolList, "Histogram tool list to be employed by this algorithm" );


}

// Destructor
///////////////
HistAlg::~HistAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode HistAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << " and retrieving " << m_histToolList.size() << " tools...");
  
  ATH_CHECK(m_histToolList.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode HistAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  ATH_CHECK(m_histToolList.release());

  return StatusCode::SUCCESS;
}

StatusCode HistAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  
  for(auto itr = this->m_histToolList.begin(); itr != this->m_histToolList.end(); itr++){
    ATH_CHECK((*itr)->fill(1.));
  }

  return StatusCode::SUCCESS;
}
