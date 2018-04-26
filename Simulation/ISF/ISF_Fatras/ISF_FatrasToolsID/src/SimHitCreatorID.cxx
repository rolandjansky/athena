/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimHitCreatorID.cxx
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasToolsID/SimHitCreatorID.h"

// iFatras && ISF
#include "ISF_FatrasInterfaces/IHitCreator.h"
#include "ISF_Event/ISFParticle.h"
// Tracking
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
//CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
// Core
#include "StoreGate/StoreGateSvc.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"
// STL
#include <iomanip>
#include <sstream>
#include <iostream>
#include <math.h>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::SimHitCreatorID::SimHitCreatorID(const std::string& t, const std::string& n, const IInterface* p)
        : base_class(t,n,p),
          m_pixelHitCreator(""),
          m_sctHitCreator(""),
          m_trtHitCreator(""),
          m_idHelperName("AtlasID"),
          m_idHelper(0)
{
  // retrieve the Extrapolator  ------------------------------------------------
  declareProperty("PixelHitCreator"              , m_pixelHitCreator);
  declareProperty("SctHitCreator"                , m_sctHitCreator);
  declareProperty("TrtHitCreator"                , m_trtHitCreator);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::SimHitCreatorID::~SimHitCreatorID()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode iFatras::SimHitCreatorID::initialize()
{
  ATH_MSG_INFO( "[ idhit ] initialize()" );
  // Get Pixel / SCT / TRT hit creator tools
  if (!m_pixelHitCreator.empty() && m_pixelHitCreator.retrieve().isFailure()) {
    ATH_MSG_FATAL( "[ --- ] Could not retrieve " << m_pixelHitCreator );
    return StatusCode::FAILURE;
  }
  if (!m_sctHitCreator.empty() && m_sctHitCreator.retrieve().isFailure()) {
    ATH_MSG_FATAL( "[ --- ] Could not retrieve " << m_sctHitCreator );
    return StatusCode::FAILURE;
  }
  if (!m_trtHitCreator.empty() && m_trtHitCreator.retrieve().isFailure()) {
    ATH_MSG_FATAL( "[ --- ] Could not retrieve " << m_trtHitCreator );
    return StatusCode::FAILURE;
  }
  // Get ID Helper from detector store
  if (detStore()->retrieve(m_idHelper, m_idHelperName).isFailure()) { 
     ATH_MSG_ERROR ("[ --- ] Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE; 
  } 
  ATH_MSG_INFO( "[ idhit ] initialize() successful" );
  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode iFatras::SimHitCreatorID::finalize()
{

  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

//================ SimHit Creation Interface  =====================================
void iFatras::SimHitCreatorID::createHits(const ISF::ISFParticle& isp, const std::vector<Trk::HitInfo>& hitVector) const {
   // iterate and assign as well the layer
   std::vector<Trk::HitInfo>::const_iterator plIter    = hitVector.begin();
   std::vector<Trk::HitInfo>::const_iterator plIterEnd = hitVector.end();
   size_t nHits = 0;
   for ( ; plIter != plIterEnd; ++plIter ){
       // decide which HitCreator to take
       // get the informations from the Simulation
       const Trk::TrackParameters*  hitParameter  = (*plIter).trackParms;
       double time                                = (*plIter).time;
       // -------------------------------------------------------------------------
       const Trk::TrkDetElementBase* hitDetElement =  hitParameter->associatedSurface().associatedDetectorElement();
       if (hitDetElement){
           // initialize an unvalid one
           Identifier hitId = hitDetElement ?  hitDetElement->identify() : Identifier();
           if (m_idHelper->is_pixel(hitId)) {
             // -----------------------------------------------------------------------
             // HIT in Pixel Detector
             // -----------------------------------------------------------------------
             ATH_MSG_VERBOSE(  "[ sim ] Creating Pixel Cluster" );
             m_pixelHitCreator->createSimHit(isp,*hitParameter,time); ++nHits;
           }
           else if (m_idHelper->is_sct(hitId)) {
             // -----------------------------------------------------------------------
             // HIT in SCT Detector
             // -----------------------------------------------------------------------
               ATH_MSG_VERBOSE(  "[ sim ] Creating SCT Cluster" );
               m_sctHitCreator->createSimHit(isp,*hitParameter,time); ++nHits;
           }
           else if (m_idHelper->is_trt(hitId)) {
             // -----------------------------------------------------------------------
             // HIT in TRT Detector 
             // -----------------------------------------------------------------------
             ATH_MSG_VERBOSE(  "[ sim ] Creating TRT DriftCircle" );
             m_trtHitCreator->createSimHit(isp,*hitParameter,time); ++nHits;
         }
       }
   }
  ATH_MSG_VERBOSE(  "[ idtrack ] **** done, " << nHits << " hits created from this track." );    
}

