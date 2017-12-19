/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_TrackTimingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRTTRACKTIMINGTOOL_H
#define TRTTRACKTIMINGTOOL_H

#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetCosmicsEventPhase/ITRT_TrackTimingTool.h"
#include "TrkTrack/Track.h"
#include "Particle/TrackParticle.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSpaceTimePoint/SpaceTimePointBase.h"
#include "TrkSpaceTimePoint/SpaceTimePoint.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include "StoreGate/ReadHandleKey.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "InDetCosmicsEventPhase/IInDetCosmicsEventPhaseTool.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "xAODEventInfo/EventInfo.h"

class AtlasDetectorID;
class Identifier;

namespace Trk{
  class ITrackFitter;
}

/** @class TRT_TrackTimingTool is a wrapper that passes on the track time, as measured in TRT, to MuonCombinedTimingTools
      
    @author  Sasa Fratina, Brett Jackson
*/  

namespace InDet
{

  class IInDetCosmicsEventPhaseTool;

class TRT_TrackTimingTool : virtual public ITRT_TrackTimingTool, public AthAlgTool {
  public:
    TRT_TrackTimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor */
    virtual ~TRT_TrackTimingTool();
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();

      /** standard Athena-Algorithm method */
      virtual StatusCode finalize();

    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::Track& trk, const Trk::SegmentCollection *sgmColl = 0) const;
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::TrackParticleBase& trk) const; // calls the above
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(std::vector<const Trk::Segment*> segments) const; // will return empty vector!
    
       /** calculate the time from drift radius - when RDO hits are not available - for running on AODs */
        float getTrackTimeFromDriftRadius(const Trk::Track* track, int &nHits) const;
  
  private:
      
    /** tools - this is the tool that actually does the work */
    ToolHandle<Trk::ITrackFitter>    m_ITrackFitter; // refit the ID track if measurements on track are not available
    ToolHandle<InDet::IInDetCosmicsEventPhaseTool> m_eventPhaseTool; //<! Cosmics Event Phase tool

	ServiceHandle<ITRT_CalDbSvc> m_trtconddbsvc ;//!< TRT Calibration DB tool

    bool m_doEtaCorrection;
    bool m_debug;
    SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey;  //!< Current event


	
	/** eta-dependent corrections, called if m_doEtaCorrection == true */ 
	float etaCorrection(const Trk::Track& track) const;

	/** get basic track info to know which function needs to be called, and calculate the position */
	void trackInformation(const Trk::Track& track, Amg::Vector3D &position, int &nTRTdriftCircles, int &nMissingRDOs, int &nMissingTrackParameters, int &nUsedHits) const;	
		
    /** private debugging function for commissioning phase */
    void debugMissingMeasurements(const Trk::Track& trk) const;

    /** debug printing function */
    void print(const Trk::Track* track, float time) const;

  }; 

}

#endif 
