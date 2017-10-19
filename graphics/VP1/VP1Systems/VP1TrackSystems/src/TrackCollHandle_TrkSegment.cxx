/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandle_TrkSegment        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollHandle_TrkSegment.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1TrackSystems/TrackHandle_TrkSegment.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

//____________________________________________________________________
class TrackCollHandle_TrkSegment::Imp {
public:
  static bool alwaysShowAllTrackColls;
};


bool TrackCollHandle_TrkSegment::Imp::alwaysShowAllTrackColls
  = VP1QtUtils::environmentVariableIsOn("VP1_ALWAYS_SHOW_ALL_TRACK_COLLECTIONS");

//____________________________________________________________________
QStringList TrackCollHandle_TrkSegment::availableCollections(IVP1System*sys)
{
  //There must be at least one of the trackers available for this to make sense.
  if ( ! ( VP1JobConfigInfo::hasPixelGeometry()||VP1JobConfigInfo::hasSCTGeometry()
	   ||VP1JobConfigInfo::hasTRTGeometry()||VP1JobConfigInfo::hasMuonGeometry() ) ) {
    sys->messageDebug("TrackCollHandle_TrkSegment::availableCollections: Neither"
		      " Pixel, SCT, TRT or Muon geometry available. Won't"
		      " look in event store for Trk::Track collections");
    return QStringList();
  }

  QStringList keysInSG = VP1SGContentsHelper(sys).getKeys<Trk::SegmentCollection>();
  if (keysInSG.isEmpty()||Imp::alwaysShowAllTrackColls)
    return keysInSG;

  QRegExp needsMuonsPattern("Converted.*|.*MBoy.*|.*Moore.*|.*Mu.*|.*Staco.*");
  QRegExp needsAllIDPattern(".*Id.*|.*ID.*|.*InDet.*|.*Inner.*");
  QRegExp needsTRTPattern(".*Trt.*|.*TRT.*|.*trt.*");

  const bool jobcfgAllID
    = VP1JobConfigInfo::hasPixelGeometry()
    &&VP1JobConfigInfo::hasSCTGeometry()
    &&VP1JobConfigInfo::hasTRTGeometry();

  QStringList outkeys;

  foreach (QString key, keysInSG) {
    if (!VP1JobConfigInfo::hasMuonGeometry() && needsMuonsPattern.exactMatch(key)) {
      sys->messageDebug("TrackCollHandle_TrkSegment::availableCollections: Ignoring key '"
			+key+"' since muon geometry is not present in job.");
      continue;
    }
    if (!VP1JobConfigInfo::hasTRTGeometry() && needsTRTPattern.exactMatch(key)) {
      sys->messageDebug("TrackCollHandle_TrkSegment::availableCollections: Ignoring key '"
			+key+"' since TRT geometry is not present in job.");
      continue;
    }
    if (!jobcfgAllID && needsAllIDPattern.exactMatch(key)) {
      sys->messageDebug("TrackCollHandle_TrkSegment::availableCollections: Ignoring key '"
			+key+"' since pixel, sct and trt geometry are not all present in job.");
      continue;
    }
    outkeys<<key;
  }
  return outkeys;

}

//____________________________________________________________________
TrackCollHandle_TrkSegment::TrackCollHandle_TrkSegment(TrackSysCommonData * cd,
						       const QString& name)
  : TrackCollHandleBase(cd,name,TrackType::TrkSegment), m_d(new Imp)
{
}

//____________________________________________________________________
TrackCollHandle_TrkSegment::~TrackCollHandle_TrkSegment()
{
  delete m_d;
}

//____________________________________________________________________
void TrackCollHandle_TrkSegment::setupSettingsFromControllerSpecific(TrackSystemController*)
{

}

//____________________________________________________________________
bool TrackCollHandle_TrkSegment::load()
{
  //Get collection:
  const Trk::SegmentCollection *segmentColl(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(segmentColl, name())) {
    common()->systemBase()->message("Error: Could not retrieve segment collection with key="+name());
    return false;
  }

  //Make appropriate trk::segment handles:
  hintNumberOfTracksInEvent(segmentColl->size());
  int i(0);
  Trk::SegmentCollection::const_iterator segItr, segItrEnd = segmentColl->end();
  for ( segItr = segmentColl->begin() ; segItr != segItrEnd; ++segItr) {
    if (*segItr)
      addTrackHandle(new TrackHandle_TrkSegment(this,*segItr));
    else
      common()->systemBase()->messageDebug("WARNING: Ignoring null Trk::Segment pointer.");

    if (!(i++%100))
      common()->systemBase()->updateGUI();
  }

  return true;
}

//____________________________________________________________________
bool TrackCollHandle_TrkSegment::cut(TrackHandleBase* handle)
{
  if (!TrackCollHandleBase::cut(handle))
    return false;

  //fixme: more?
  return true;
}

//____________________________________________________________________
void TrackCollHandle_TrkSegment::visibleStateUpdated(TrackHandle_TrkSegment* handle)
{
  common()->visTrkSegmentsToMaterialHelper()->setState(handle->trkSegmentPointer(), (handle->visible() ? handle->currentMaterial() : 0 ));
}
