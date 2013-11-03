/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackInCaloTimingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CALOTOFCALOTIMEOFFLIGHTTOOL_H
#define CALOTOFCALOTIMEOFFLIGHTTOOL_H

#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TrackInCaloTimingTools/ITrackInCaloTimingTool.h"
#include "ITrackInCaloTools/ITrackInCaloTools.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkSpaceTimePoint/SpaceTimePoint.h"

class AtlasDetectorID;
class Identifier;

/** @class TrackInCaloTimingTool 

    TrackInCaloTimingTool calculates a beta estimate for a track based on 
    calorimeter timing information from the cells that it crosses. It also
    provides a list of time and position measurements for interfacing
    with other time-of-flight algorithms and tools, e.g. for cleaning out 
    cosmic rays from collision events.
      
    @author  Christian Ohm <christian.ohm@cern.ch>
*/  

namespace Rec {
  class TrackInCaloTimingTool : virtual public ITrackInCaloTimingTool, public AthAlgTool {
  public:
    TrackInCaloTimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor */
    virtual ~TrackInCaloTimingTool();
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();

    /** standard Athena-Algorithm method */
    virtual StatusCode finalize();

    virtual float calorimeterBeta(const Trk::Track& trk) const;
    virtual float calorimeterBeta(const Trk::TrackParticleBase& trk) const;
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::Track& trk, const Trk::SegmentCollection* = NULL) const;
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const Trk::TrackParticleBase& trk) const;
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(std::vector<const Trk::Segment*> segments) const; // will return empty vector!

    /** internal functions not seen from outside this tool */
    // TODO: add function for compensating cell times for eta and phi dependence
    // TODO: add function for getting rid of cells that we don't trust the timing
    // info from, should use some bad channel db or something...
      
  private:
      
    /** tools */
    ToolHandle<ITrackInCaloTools>  m_trackInCaloTool;

    /** member functions */

    // cell-energy dependent resolution for Tile
    // extracted from scraping events (L. Fiorini, A. Succurro, 
    // see slide 17 of http://indico.cern.ch/getFile.py/access?contribId=0&resId=0&materialId=slides&confId=91644)
    float tileCellTimeError(float cellEnergy) const;

    // helper function that calculates a weight based on known resolution, 
    // as 1/pow(uncertainty,2)
    float cellTimeWeight(float error) const;

    /** member variables for algorithm properties: */
    std::vector<Trk::SpaceTimePointBase*> m_points;
      
    // the following can be configured via job options
    float m_minCellEnergy;
    CaloCell_ID::CaloSample m_minSampling;
    CaloCell_ID::CaloSample m_maxSampling;
    bool m_doEtaAndPhiCompensation;
    bool m_doBadCellCleaning;
  }; 
}

#endif 
