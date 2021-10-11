/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackHandle_TrkSegment            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackHandle_TrkSegment.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/TrackCollHandle_TrkSegment.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1Msg.h"
#include "VP1PRDSystems/MuonChamberProjectionHelper.h"
#include "TrkSegment/Segment.h"
#include "TrkSurfaces/Surface.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "VP1TrackSystems/VP1TrackSummary.h"


//____________________________________________________________________
TrackHandle_TrkSegment::TrackHandle_TrkSegment(TrackCollHandleBase* ch, const Trk::Segment* segment)
: TrackHandleBase(ch),
  m_nhits_pixel(0), m_nhits_sct(0), m_nhits_trt(0), m_nhits_muon_phi(0), m_nhits_rpc(0), m_nhits_mdt(0), 
  m_nhits_tgc(0), m_nhits_csc(0), m_hitInfoStatus(-1),
  m_segment(segment), m_points(nullptr)
{
}
//____________________________________________________________________
TrackHandle_TrkSegment::~TrackHandle_TrkSegment()
{
  delete m_points;
}

//____________________________________________________________________
QStringList TrackHandle_TrkSegment::clicked() const
{
  QStringList l;
  l << "Trk::Segment";//fixme - more info
  l << TrackHandleBase::baseInfo();
  
  const Muon::MuonSegment* muonSegment = dynamic_cast<const Muon::MuonSegment*>(m_segment);
  if (muonSegment && common()->controller()->muonEDMPrinterTool()) {
    std::string temp = common()->controller()->muonEDMPrinterTool()->print(*muonSegment);      
    l << QString(temp.c_str());
  } 
  
  if (m_segment&&common()->controller()->printVerboseInfoOnSingleSelection()) {
    l << " ===== dump =====";
    std::ostringstream s;
    s << *m_segment;
    l << QString(s.str().c_str()).split('\n');
  }
  return l;
}

//____________________________________________________________________
void TrackHandle_TrkSegment::ensureTouchedMuonChambersInitialised() const
{
  if (!VP1JobConfigInfo::hasMuonGeometry()) {
    if (VP1Msg::verbose())
      collHandle()->messageVerbose("");
    return;
  }
  const Trk::MeasurementBase* meas;
  const MuonGM::MuonReadoutElement* muonDetEl;
  std::vector < const Trk::MeasurementBase * >::const_iterator tsos_iter, tsos_end(m_segment->containedMeasurements().end());
  for (tsos_iter = m_segment->containedMeasurements().begin(); tsos_iter != tsos_end; ++tsos_iter) {
    meas = *tsos_iter;
    muonDetEl = meas ? dynamic_cast<const MuonGM::MuonReadoutElement*>(meas->associatedSurface().associatedDetectorElement() ) : nullptr;
    if (muonDetEl)
      registerTouchedMuonChamber(muonDetEl->parentStationPV());
  }
}

//____________________________________________________________________
void TrackHandle_TrkSegment::visibleStateChanged()
{
  TrackHandleBase::visibleStateChanged();
  static_cast<TrackCollHandle_TrkSegment*>(collHandle())->visibleStateUpdated(this);
}

//____________________________________________________________________
void TrackHandle_TrkSegment::currentMaterialChanged()
{
  if (visible())
    static_cast<TrackCollHandle_TrkSegment*>(collHandle())->visibleStateUpdated(this);
}

//____________________________________________________________________
void TrackHandle_TrkSegment::ensureInitSubSysHitInfo() const
{
  if (m_hitInfoStatus==-1) 
    m_hitInfoStatus = VP1TrackSummary::countHits( m_segment, m_nhits_pixel, m_nhits_sct, m_nhits_trt, m_nhits_muon_phi, m_nhits_rpc, m_nhits_mdt, m_nhits_tgc, m_nhits_csc) ? 1 : 0;
}

//____________________________________________________________________
const std::vector< Amg::Vector3D > * TrackHandle_TrkSegment::provide_pathInfoPoints() const
{
  if (!m_points) {
    m_points = new std::vector< Amg::Vector3D >;
    std::set<GeoPVConstLink>::iterator it(touchedMuonChambers().begin()), itE(touchedMuonChambers().end());
    const bool muonsegment = it!=itE;
    
    ////////////////////////////////////////////
    // Find the two endpoints of the segment: //
    ////////////////////////////////////////////
    double radius = ( muonsegment ? 1000.0 : 150.0 );//A bit arbitrary for ID segments.
    
    double xOffset(0),yOffset(0),zOffset(0);
    if (muonsegment) {
      //Fixme: jobconfig safety.
      double phi   = m_segment->localParameters()[Trk::phi0];
      double theta = m_segment->localParameters()[Trk::theta];
      xOffset = radius*cos(phi)*sin(theta);
      yOffset = radius*sin(phi)*sin(theta);
      zOffset = radius*cos(theta);
    }
    
    //TK: Fixme: globalPosition can cause a crash!
    
    Amg::Vector2D localPos(m_segment->localParameters()[Trk::loc1], m_segment->localParameters()[Trk::loc2]);
    const Amg::Vector3D globalPos = m_segment->associatedSurface().localToGlobal(localPos);
    Amg::Vector3D pointA(globalPos.x()-xOffset,globalPos.y()-yOffset,globalPos.z()-zOffset);
    Amg::Vector3D pointB(globalPos.x()+xOffset,globalPos.y()+yOffset,globalPos.z()+zOffset);
    
    bool hasnonmdtchamber(false);
    bool outsidechamber, success;
    if (muonsegment) {
      for (;it!=itE;++it) {
        if (true||/*FIXME: TEMPORARY DISABLE CLIPPING DUE TO BUG IN clipLineSegmentToMDTChamber*/
            !common()->muonChamberProjectionHelper()->isKnownMDTChamber(*it)) {
          hasnonmdtchamber = true;
          break;
        }
        Amg::Vector3D a(pointA),b(pointB);
        success = common()->muonChamberProjectionHelper()->clipLineSegmentToMDTChamber( *it,a,b, outsidechamber,200.0 );
        if (!success) {
          collHandle()->message("Error: Something we wrong with clipLineSegmentToMDTChamber");
          continue;
        }
        if (outsidechamber) {
          collHandle()->messageDebug("WARNING: Segment is outside a chamber that it is touching!");
          continue;
        }
        m_points->push_back(a);
        m_points->push_back(b);
      }
      if (hasnonmdtchamber) {
        m_points->clear();//Fixme: ok?
        m_points->push_back(pointA);
        m_points->push_back(pointB);
      } else if (m_points->size()>2) {
        if (VP1Msg::verbose())
          collHandle()->messageVerbose("Combining segment points into a single line!");
        (*m_points)[1] = m_points->back();
        m_points->resize(2);
      }
    } else {
      //Fixme: Do we need something better for ID segments?
      m_points->push_back(pointA);
      m_points->push_back(pointB);
    }
    
  }
  return m_points;
}
