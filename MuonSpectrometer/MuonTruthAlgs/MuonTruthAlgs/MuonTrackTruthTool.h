/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKTRUTHTOOL_H
#define MUON_MUONTRACKTRUTHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonTrackTruthTool.h"

#include "Identifier/Identifier.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "GeneratorObjects/McEventCollection.h"

#include <string>
#include <vector>
#include <map>
#include <utility>

class MsgStream;
class StoreGateSvc;
class TruthTrajectory;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;
  class MuonSegment;
}

namespace Trk {
  class Track;
  class MeasurementBase;
  class ITruthTrajectoryBuilder;
}

namespace Muon {
  

  /**
     @brief Tool to calculate track truth

  */
  class MuonTrackTruthTool : virtual public IMuonTrackTruthTool, public AthAlgTool {
  public:

    struct SortResultByMatchedHits {
      bool operator()(const MatchResult& r1,const MatchResult& r2) const;
      bool operator()(const SegmentMatchResult& r1,const SegmentMatchResult& r2) const;
      bool operator()(const MuonTrackTruth& r1,const MuonTrackTruth& r2) const;
    };

  public:
    /** @brief constructor */
    MuonTrackTruthTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonTrackTruthTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief perform truth matching for a given set of tracks */
    ResultVec match(const TrackCollection& tracks ) const;

    /** @brief perform truth matching for a given set of segments */
    SegmentResultVec match(const std::vector<const MuonSegment*>& segments ) const;

    /** @brief get track truth */
    MuonTrackTruth getTruth( const Trk::Track& track, bool restrictedTruth = false ) const;

    /** @brief get segment truth for a list of segments, the segments will be considered to belong to the same muon */
    MuonTrackTruth getTruth( const std::vector<const MuonSegment*>& segments, bool restrictedTruth = false ) const;

    /** @brief get segment truth */
    MuonTrackTruth getTruth( const Muon::MuonSegment& segment ) const;

    /** @brief get truth for a give set of hits. If restrictedTruth is set to true only missed hits 
	in chambers with hits will be counted.
    */
    MuonTrackTruth getTruth( const std::vector<const Trk::MeasurementBase*>& measurements, bool restrictedTruth = false ) const;

    /** create truth tree from sim data */
    const TruthTree& createTruthTree() const;

    /// Returns the mother particle of the particle with barcodeIn if it is found in the truth trajectory.
    /// It traces the decay chain until if finds the first particle that is different flavor from the starting one.
    const HepMC::GenParticle& getMother( const TruthTrajectory& traj, const int barcodeIn ) const;

    /// Returns the ancestor particle of the particle with barcodeIn if it is found in the truth trajectory.
    /// Ancestor here means the last particle at generator level that has a status code different from final state, e.g. Z
    const HepMC::GenParticle& getAncestor( const TruthTrajectory& traj, const int barcodeIn ) const;

    /// Returns the initial particle of the particle with barcodeIn if it is found in the truth trajectory.
    /// For example a mu undergoing a mubrem would create a second mu, in which case this method returns the mu prior to bremsstrahlung.
    /// This interface calls the method getInitialPair.
    const HepMC::GenParticle& getInitial( const TruthTrajectory& traj, const int barcodeIn ) const;

    /// Returns the number of steps a particle took while maintaining its PDG ID.
    /// This method calls getInitialPair for calculating this number.
    unsigned int getNumberOfScatters( const TruthTrajectory& traj, const int barcodeIn ) const;

  private:

    MuonTrackTruth getTruth( const std::vector<const Trk::MeasurementBase*>& measurements,
			     TruthTreeEntry& truthEntry, bool restrictedTruth ) const;

    void addSimDataToTree( const MuonSimDataCollection* simDataCol ) const;
    void addCscSimDataToTree( const CscSimDataCollection* simDataCol ) const;

    void addMdtTruth( MuonTechnologyTruth& trackTruth, const Identifier& id, const Trk::MeasurementBase& meas, 
		      const MuonSimDataCollection& simCol ) const;

    void addClusterTruth( MuonTechnologyTruth& trackTruth, const Identifier& id, const Trk::MeasurementBase& meas, 
			  const MuonSimDataCollection& simCol ) const;
    void addClusterTruth( MuonTechnologyTruth& trackTruth, const Identifier& id, const Trk::MeasurementBase& meas, 
			  const CscSimDataCollection& simCol ) const;

    void addMissedHits( MuonTechnologyTruth& truth, const std::set<Identifier>& ids, const std::set<Identifier>& chids, 
			const MuonSimDataCollection& simCol, bool restrictedTruth ) const;

    void addMissedHits( MuonTechnologyTruth& truth, const std::set<Identifier>& ids, const std::set<Identifier>& chids, 
			const CscSimDataCollection& simCol, bool restrictedTruth ) const;

    int manipulateBarCode( int barcode ) const;

    void clear() const;

    bool selectPdg( int pdg ) const { return m_selectedPdgs.count(pdg); }

    /// Returns the initial particle of the particle with barcodeIn if it is found in the truth trajectory.
    /// For example a mu undergoing a mubrem would create a second mu, in which case this method returns the mu prior to bremsstrahlung.
    /// The number of such scatters is returned in the .second.
    const std::pair<const HepMC::GenParticle*, unsigned int> getInitialPair( const TruthTrajectory& traj, const int barcodeIn ) const;
    
    const MuonGM::MuonDetectorManager*  m_detMgr;

    ToolHandle<Muon::MuonEDMPrinterTool>         m_printer;
    ToolHandle<Muon::MuonIdHelperTool>           m_idHelperTool;
    ToolHandle<Trk::ITruthTrajectoryBuilder>     m_truthTrajectoryBuilder;

    SG::ReadHandleKey<McEventCollection> m_mcEventColl{this,"McEventCollectionKey","TruthEvent","McEventCollection"};
    SG::ReadHandleKeyArray<MuonSimDataCollection> m_muonSimData{this,"MuonSimDataNames",{ "MDT_SDO", "RPC_SDO", "TGC_SDO" },"Muon SDO maps"};
    SG::ReadHandleKey<CscSimDataCollection> m_cscSimData{this,"CSC_SDO_Container","CSC_SDO","CSC SDO"};
    SG::ReadHandleKey<TrackRecordCollection> m_trackRecord{this,"TrackRecord","MuonEntryLayerFilter","Track Record Collection"};

    mutable TruthTree m_truthTree;
    mutable std::vector<TruthTrajectory*> m_truthTrajectoriesToBeDeleted;
    mutable std::map<int,int> m_barcodeMap; // map used to link barcode of TrackRecord particles/hits to 'final' state barcode
    bool m_manipulateBarCode;
    bool m_doSummary;
    unsigned int  m_minHits;
    bool m_matchAllParticles;
    
    IntegerArrayProperty m_pdgsToBeConsidered;
    std::set<int> m_selectedPdgs; // set storing particle PDG's considered for matching
  };

}

#endif // MUON_MDTONTRACKTOOL_H
