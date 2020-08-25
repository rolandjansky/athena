///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Hist.cxx 
// Implementation file for class AthEx::Hist
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthExHistNtup includes
#include "Hist.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

// ROOT includes
#include "TH1F.h"

// event includes
#include "xAODEventInfo/EventInfo.h"

using namespace AthEx;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
Hist::Hist( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_ntsvc("THistSvc/THistSvc", name),
  m_hist (0)
{
  //
  // Property declaration
  // 

  declareProperty( "EvtInfo", 
                   m_evtInfoName = "",
                   "key to the event-info instance to histogram-ize.");

}

// Destructor
///////////////
Hist::~Hist()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode Hist::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (!m_ntsvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve handle to ntuple svc");
    return StatusCode::FAILURE;
  }

  // register our histogram with the svc
  m_hist = new TH1F("h1", "histogram title", 100,0.,100.);
  if (!m_ntsvc->regHist("/stat/simple1D/h1", m_hist).isSuccess()) {
    ATH_MSG_ERROR("could not register histogram [h1]");
    delete m_hist; m_hist = 0;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode Hist::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode Hist::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // get event data...
  const xAOD::EventInfo* evt = 0;
  if (!evtStore()->retrieve(evt, m_evtInfoName).isSuccess() ||
      0==evt) {
    ATH_MSG_ERROR("could not get event-info at [" << m_evtInfoName << "]");
    return StatusCode::FAILURE;
  }

  int event = evt->eventNumber();

  // fill the histogram
  m_hist->Fill( float(event), 1.);

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


