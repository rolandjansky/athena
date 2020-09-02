/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRACKTRUTHTOOL_H
#define MUON_IMUONTRACKTRUTHTOOL_H
 
#include "GaudiKernel/IAlgTool.h"

#include "Identifier/Identifier.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "GeneratorObjects/McEventCollection.h"

#include <set>
#include <vector>
#include <map>


static const InterfaceID IID_IMuonTrackTruthTool("Muon::IMuonTrackTruthTool",1,0);

class TruthTrajectory;

namespace Muon {
  class MuonSegment;
}

namespace Muon {

  
  class MuonTechnologyTruth {
  public:
    std::set<Identifier> matchedHits;
    std::set<Identifier> matchedChambers;
    std::set<Identifier> missedHits;
    std::set<Identifier> missedChambers;
    std::set<Identifier> wrongHits;
    std::set<Identifier> wrongChambers;
  };

  class MuonTrackTruth {
  public:
    const TrackRecord* truthTrack;
    const TruthTrajectory* truthTrajectory;
    bool isHitMatched;
    bool isParameterMatched;
    MuonTechnologyTruth mdts;
    MuonTechnologyTruth cscs;
    MuonTechnologyTruth rpcs;
    MuonTechnologyTruth tgcs;
    MuonTechnologyTruth stgcs;
    MuonTechnologyTruth mms;
    unsigned int numberOfMatchedHits() const {
      return mdts.matchedHits.size() + tgcs.matchedHits.size() + rpcs.matchedHits.size() + cscs.matchedHits.size() + stgcs.matchedHits.size() + mms.matchedHits.size() ;
    }
    MuonTrackTruth() : truthTrack(0),truthTrajectory(0),isHitMatched(false),isParameterMatched(false) {}
  };

  class MuonSegmentTruth {
  public:
    const TrackRecord* truthTrack;
    bool isHitMatched;
    MuonTechnologyTruth mdts;
    MuonTechnologyTruth cscs;
    MuonTechnologyTruth rpcs;
    MuonTechnologyTruth tgcs;
    MuonTechnologyTruth stgcs;
    MuonTechnologyTruth mm;
    MuonSegmentTruth() : truthTrack(0),isHitMatched(false) {}

  };

  /** @brief The IMuonTrackTruthTool is a pure virtual interface for tools that recover hole on a track */
  class IMuonTrackTruthTool : virtual public IAlgTool 
  {      
    public:
    struct TruthTreeEntry {
      const TrackRecord* truthTrack;
      const TruthTrajectory* truthTrajectory;
      MuonSimDataCollection mdtHits;
      CscSimDataCollection  cscHits;
      MuonSimDataCollection rpcHits;
      MuonSimDataCollection tgcHits;
      MuonSimDataCollection stgcHits;
      MuonSimDataCollection mmHits; 
    };
    

    typedef std::pair<const Trk::Track*,MuonTrackTruth> MatchResult;
    typedef std::vector<MatchResult>                    ResultVec;

    typedef std::pair<const Muon::MuonSegment*,MuonTrackTruth> SegmentMatchResult;
    typedef std::vector<SegmentMatchResult>                    SegmentResultVec;

    
    // collect track record entries per barcode
    typedef std::map<int,TruthTreeEntry> TruthTree;
    typedef TruthTree::iterator          TruthTreeIt;
  
  public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();


    /** @brief perform truth matching for a given set of tracks */
    virtual ResultVec match(const TrackCollection& tracks ) const = 0;

    /** @brief perform truth matching for a given set of segments */
    virtual SegmentResultVec match(const std::vector<const MuonSegment*>& segments ) const = 0;

    /** create truth tree from sim data */
    virtual const TruthTree createTruthTree(const TrackRecordCollection* truthTrackCol, const McEventCollection* mcEventCollection,
					    std::vector<const MuonSimDataCollection*> muonSimData, const CscSimDataCollection* cscSimDataMap) const = 0;

    /** @brief get track truth */
    virtual MuonTrackTruth getTruth( const Trk::Track& track, bool restrictedTruth = false ) const = 0;

    /** @brief get segment truth for a list of segments, the segments will be considered to belong to the same muon */
    virtual MuonTrackTruth getTruth( const std::vector<const MuonSegment*>& segments, bool restrictedTruth = false ) const = 0;

    /** @brief get segment truth */
    virtual MuonTrackTruth getTruth( const Muon::MuonSegment& segment ) const = 0;

    /** @brief get truth for a give set of hits. If restrictedTruth is set to true only missed hits 
	in chambers with hits will be counted.
    */
    virtual MuonTrackTruth getTruth( const std::vector<const Trk::MeasurementBase*>& measurements, bool restrictedTruth = false ) const = 0;

    /// returns the mother particle of the particle with barcodeIn if it is found in the truth trajectory
    /// It traces the decay chain until if finds the first particle that is different flavor from the starting one.
    virtual const HepMC::GenParticle* getMother( const TruthTrajectory& traj, const int barcodeIn ) const = 0;

    /// Returns the ancestor particle of the particle with barcodeIn if it is found in the truth trajectory.
    /// Ancestor here means the last particle at generator level that has a status code different from final state, e.g. Z
    virtual const HepMC::GenParticle* getAncestor( const TruthTrajectory& traj, const int barcodeIn ) const = 0;
    
    /// Returns the initial particle of the particle with barcodeIn if it is found in the truth trajectory.
    /// For example a mu undergoing a mubrem would create a second mu, in which case this method returns the mu prior to bremsstrahlung.
    virtual const HepMC::GenParticle* getInitial( const TruthTrajectory& traj, const int barcodeIn ) const = 0;

    /// Returns the number of steps a particle took while maintaining its PDG ID
    virtual unsigned int getNumberOfScatters( const TruthTrajectory& traj, const int barcodeIn ) const = 0;
  };
  
  inline const InterfaceID& IMuonTrackTruthTool::interfaceID()
  {
    return IID_IMuonTrackTruthTool;
  }

} // end of name space

#endif 

