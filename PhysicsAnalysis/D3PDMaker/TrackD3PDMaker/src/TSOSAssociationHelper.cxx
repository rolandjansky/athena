/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TSOSAssociationHelper.h"

#include "TrkTrack/Track.h"
#include <Particle/TrackParticle.h>
#include <TrkTrack/TrackStateOnSurface.h>
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"

namespace D3PD {


TSOSAssociationHelper::TSOSAssociationHelper()//:
  //  m_idHoleSearch("InDet::InDetTrackHoleSearchTool")// ,
  //   m_doHolesInDet(false)
  : m_idHelper(0),
    m_pixelId(0)
{
  m_getTypes["GetBLayerMeasurements"] = false;
  m_getTypes["GetPixelMeasurements"] = false;
  m_getTypes["GetSCTMeasurements"] = false;
  m_getTypes["GetTRTMeasurements"] = false;
  m_getTypes["GetMDTMeasurements"] = false;
  m_getTypes["GetCSCMeasurements"] = false;
  m_getTypes["GetRPCMeasurements"] = false;
  m_getTypes["GetTGCMeasurements"] = false;
  m_getTypes["GetBLayerOutliers"] = false;
  m_getTypes["GetPixelOutliers"] = false;
  m_getTypes["GetSCTOutliers"] = false;
  m_getTypes["GetTRTOutliers"] = false;
  m_getTypes["GetMDTOutliers"] = false;
  m_getTypes["GetCSCOutliers"] = false;
  m_getTypes["GetRPCOutliers"] = false;
  m_getTypes["GetTGCOutliers"] = false;
  m_getTypes["GetBLayerHoles"] = false;
  m_getTypes["GetPixelHoles"] = false;
  m_getTypes["GetSCTHoles"] = false;
  m_getTypes["GetTRTHoles"] = false;
  m_getTypes["GetMDTHoles"] = false;
  m_getTypes["GetCSCHoles"] = false;
  m_getTypes["GetRPCHoles"] = false;
  m_getTypes["GetTGCHoles"] = false;
}

TSOSAssociationHelper::~TSOSAssociationHelper(){

}

StatusCode TSOSAssociationHelper::doReset (const Trk::Track& p)
{

  const Trk::Track *track = &p;

  const DataVector<const Trk::TrackStateOnSurface> *TSOs = 0;

  //   static const Trk::Track *newTrack = 0;
  //   if(m_doHolesInDet){
  //     if(newTrack) delete newTrack;
  //     newTrack = m_idHoleSearch->getTrackWithHoles(*track, track->info().particleHypothesis());
  //     TSOs = newTrack->trackStateOnSurfaces();
  //   }else{
  TSOs = track->trackStateOnSurfaces();
  //   }

  if(!TSOs){
    m_TSOItr = m_TSOEnd;
    REPORT_MESSAGE (MSG::WARNING) << "Could not access track state on surfaces";
    return StatusCode::SUCCESS;
  }

  m_TSOItr = TSOs->begin();
  m_TSOEnd = TSOs->end();

  return StatusCode::SUCCESS;
}

StatusCode TSOSAssociationHelper::doReset (const Rec::TrackParticle& p)
{

  const Trk::Track *track = p.originalTrack();
  if(!track){
    m_TSOItr = m_TSOEnd;
    //REPORT_MESSAGE (MSG::DEBUG) << "Could not access original track";
    return StatusCode::SUCCESS;
  }

  const DataVector<const Trk::TrackStateOnSurface> *TSOs = 0;

  //   static const Trk::Track *newTrack = 0;
  //   if(m_doHolesInDet){
  //     if(newTrack) delete newTrack;
  //     newTrack = m_idHoleSearch->getTrackWithHoles(*track, track->info().particleHypothesis());
  //     TSOs = newTrack->trackStateOnSurfaces();
  //   }else{
  TSOs = track->trackStateOnSurfaces();
  //   }

  if(!TSOs){
    m_TSOItr = m_TSOEnd;
    REPORT_MESSAGE (MSG::WARNING) << "Could not access track state on surfaces";
    return StatusCode::SUCCESS;
  }

  m_TSOItr = TSOs->begin();
  m_TSOEnd = TSOs->end();

  return StatusCode::SUCCESS;
}

const Trk::TrackStateOnSurface* TSOSAssociationHelper::getNext(){

  // reached the end
  if(m_TSOItr == m_TSOEnd) return 0;

  const Trk::TrackStateOnSurface *TSO = (*m_TSOItr);
  m_TSOItr++;

  int detType = getDetectorType(TSO);

  const Trk::TrackStateOnSurface *ret = 0;

  if(TSO->types()[Trk::TrackStateOnSurface::Measurement]){
    if(Trk::TrackState::Pixel==detType){
      if(m_getTypes["GetPixelMeasurements"]){
        ret = TSO;
      }
      if(m_getTypes["GetBLayerMeasurements"] && isBLayer(TSO)){
        ret = TSO;
      }
    }
    if(Trk::TrackState::SCT==detType && m_getTypes["GetSCTMeasurements"]){
      ret = TSO;
    }
    if(Trk::TrackState::TRT==detType && m_getTypes["GetTRTMeasurements"]){
      ret = TSO;
    }
    if(Trk::TrackState::MDT==detType && m_getTypes["GetMDTMeasurements"]){
      ret = TSO;
    }
    if(Trk::TrackState::CSC==detType && m_getTypes["GetCSCMeasurements"]){
      ret = TSO;
    }
    if(Trk::TrackState::RPC==detType && m_getTypes["GetRPCMeasurements"]){
      ret = TSO;
    }
    if(Trk::TrackState::TGC==detType && m_getTypes["GetTGCMeasurements"]){
      ret = TSO;
    }
  }else if(TSO->types()[Trk::TrackStateOnSurface::Outlier]){
    if(Trk::TrackState::Pixel==detType){
      if(m_getTypes["GetPixelOutliers"]){
        ret = TSO;
      }
      if(m_getTypes["GetBLayerOutliers"] && isBLayer(TSO)){
        ret = TSO;
      }
    }
    if(Trk::TrackState::SCT==detType && m_getTypes["GetSCTOutliers"]){
      ret = TSO;
    }
    if(Trk::TrackState::TRT==detType && m_getTypes["GetTRTOutliers"]){
      ret = TSO;
    }
    if(Trk::TrackState::MDT==detType && m_getTypes["GetMDTOutliers"]){
      ret = TSO;
    }
    if(Trk::TrackState::CSC==detType && m_getTypes["GetCSCOutliers"]){
      ret = TSO;
    }
    if(Trk::TrackState::RPC==detType && m_getTypes["GetRPCOutliers"]){
      ret = TSO;
    }
    if(Trk::TrackState::TGC==detType && m_getTypes["GetTGCOutliers"]){
      ret = TSO;
    }
  }else if(TSO->types()[Trk::TrackStateOnSurface::Hole]){
    if(Trk::TrackState::Pixel==detType && m_getTypes["GetPixelHoles"]){
      if(m_getTypes["GetPixelHoles"]){
	ret = TSO;
      }
      if(m_getTypes["GetBLayerHoles"] && isBLayer(TSO)){
	ret = TSO;
      }
    }
    if(Trk::TrackState::SCT==detType && m_getTypes["GetSCTHoles"]){
      ret = TSO;
    }
    if(Trk::TrackState::TRT==detType && m_getTypes["GetTRTHoles"]){
      ret = TSO;
    }
    if(Trk::TrackState::MDT==detType && m_getTypes["GetMDTHoles"]){
      ret = TSO;
    }
    if(Trk::TrackState::CSC==detType && m_getTypes["GetCSCHoles"]){
      ret = TSO;
    }
    if(Trk::TrackState::RPC==detType && m_getTypes["GetRPCHoles"]){
      ret = TSO;
    }
    if(Trk::TrackState::TGC==detType && m_getTypes["GetTGCHoles"]){
      ret = TSO;
    }
  }

  if(!ret) ret = getNext();

  return ret;
}

int TSOSAssociationHelper::getDetectorType(const Trk::TrackStateOnSurface *TSO){

  int detType = Trk::TrackState::unidentified;

  if( TSO->types()[Trk::TrackStateOnSurface::Measurement] ||
      TSO->types()[Trk::TrackStateOnSurface::Outlier] ||
      TSO->types()[Trk::TrackStateOnSurface::Hole] ){

    const Trk::Surface& surface = TSO->surface();
    const Trk::TrkDetElementBase* detElement = surface.associatedDetectorElement();

    if(detElement){
      Identifier Id = detElement->identify();

      if (m_idHelper->is_pixel(Id) ) {
        detType = Trk::TrackState::Pixel;
      }else if (m_idHelper->is_sct(Id)) {
        detType = Trk::TrackState::SCT;
      }else if (m_idHelper->is_trt(Id)) {
        detType = Trk::TrackState::TRT;
      }  else if (m_idHelper->is_mdt(Id)) {
        detType = Trk::TrackState::MDT;
      }  else if (m_idHelper->is_csc(Id)) {
        detType = Trk::TrackState::CSC;
      }  else if (m_idHelper->is_rpc(Id)) {
        detType = Trk::TrackState::RPC;
      }  else if (m_idHelper->is_tgc(Id)) {
        detType = Trk::TrackState::TGC;
      }
    }
  }

  return detType;
}

bool TSOSAssociationHelper::isBLayer(const Trk::TrackStateOnSurface *TSO){

  if( TSO->types()[Trk::TrackStateOnSurface::Measurement] ||
      TSO->types()[Trk::TrackStateOnSurface::Outlier] ||
      TSO->types()[Trk::TrackStateOnSurface::Hole] ){

    const Trk::Surface& surface = TSO->surface();


    const Trk::TrkDetElementBase* detElement = surface.associatedDetectorElement();

    if(detElement){

      Identifier Id = detElement->identify();

      if(m_idHelper->is_pixel(Id)){

        if(m_pixelId->is_blayer(Id)){
          return true;
        }
      }
    }
  }

  return false;
}

} // namespace D3PD
