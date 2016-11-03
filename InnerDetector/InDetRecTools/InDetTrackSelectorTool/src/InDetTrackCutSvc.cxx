/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackCutSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
 
#include "InDetTrackSelectorTool/InDetTrackCutSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
 
/** Constructor **/
InDet::InDetTrackCutSvc::InDetTrackCutSvc(const std::string& name,ISvcLocator* svc) :
    AthService(name,svc)

{
  // the geometry builder -------------------------------------------------------- hmmm ?
  declareProperty( "MaxD0",         m_trackCuts.maxD0);
  declareProperty( "MaxZ0",         m_trackCuts.maxZ0);
  declareProperty( "MaxZ0sinTheta", m_trackCuts.maxZ0sinTheta);
  declareProperty( "MaxEta",	    m_trackCuts.maxEta);
  declareProperty( "MinSiHits",     m_trackCuts.minSiHits);
  declareProperty( "MinPixelHits",  m_trackCuts.minPixelHits);
  declareProperty( "MinSCTHits",    m_trackCuts.minSCTHits);

}
 
/** Destructor **/
InDet::InDetTrackCutSvc::~InDetTrackCutSvc()
{}
 
/** Initialize Service */
StatusCode InDet::InDetTrackCutSvc::initialize()
{
  ATH_MSG_INFO( "Dedicated ITk 1.6 TrackCutSvc." );
  ATH_MSG_INFO( "initialize() successful! " );
  return StatusCode::SUCCESS;
}
 
 
/** Finalize Service */
StatusCode InDet::InDetTrackCutSvc::finalize()
{
    ATH_MSG_INFO( "finalize() successful." );
    return StatusCode::SUCCESS;
}
 
/** Query the interfaces. */
StatusCode InDet::InDetTrackCutSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( Trk::IID_ITrackCutSvc == riid )
    *ppvInterface = (Trk::ITrackCutSvc*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
