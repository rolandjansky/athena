/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef DETAILEDMUONPATTERNTRUTHBUILDER_H
#define DETAILEDMUONPATTERNTRUTHBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "DataModel/ElementLink.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonPattern/DetailedMuonPatternTruthCollection.h"
#include "MuonPattern/MuonPatternCollection.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonRecToolInterfaces/IDetailedMuonPatternTruthBuilder.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventUtils/InverseMultiMap.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkToolInterfaces/ITruthTrajectoryBuilder.h"
#include "TrkTrack/Track.h"
#include "TrkTruthData/DetailedSegmentTruth.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/SubDetHitStatistics.h"
#include "TrkTruthData/TruthTrajectory.h"

// Include any other potentially useful packages
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include <vector>

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "TrackRecord/TrackRecord.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"

namespace Trk {

    class DetailedMuonPatternTruthBuilder : virtual public IDetailedMuonPatternTruthBuilder, public AthAlgTool {
    public:
        DetailedMuonPatternTruthBuilder(const std::string& type, const std::string& name, const IInterface* parent);

        virtual StatusCode initialize();

        /** See description for IDetailedMuonPatternTruthBuilder::buildDetailedTrackTruth() */
        virtual void buildDetailedMuonPatternTruth(DetailedMuonPatternTruthCollection* output,
                                                   const MuonPatternCombinationCollection& tracks,
                                                   const std::vector<const PRD_MultiTruthCollection*>& prdTruth);

        virtual void buildDetailedTrackTruth(std::vector<DetailedTrackTruth>* output, const Muon::MuonPatternCombination& pattern,
                                             const std::vector<const PRD_MultiTruthCollection*>& prdTruth);

        void buildDetailedTrackTruthFromSegments(std::vector<DetailedSegmentTruth>* output, const Muon::MuonSegment& segment,
                                                 const std::vector<const PRD_MultiTruthCollection*>& prdTruth);

    private:
        typedef InverseMultiMap<PRD_MultiTruthCollection> PRD_InverseTruth;

        const AtlasDetectorID* m_idHelper;
        ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;

        ToolHandle<Trk::ITruthTrajectoryBuilder> m_truthTrajBuilder;
        ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtCreator;
        ToolHandle<Muon::IMuonClusterOnTrackCreator> m_muonClusterCreator;
        ToolHandle<Muon::IMuonClusterOnTrackCreator> p_IMuonClusterOnTrackCreatorCSCCluster;

        SubDetHitStatistics::SubDetType findSubDetType(Identifier id);

        const MuonSimData::Deposit* getDeposit(const MuonSimDataCollection& simCol, const HepMC::GenParticle& genPart,
                                               const Identifier& id);
        const MuonSimDataCollection* retrieveTruthCollection(std::string colName);

        void addTrack(DetailedMuonPatternTruthCollection* output, const ElementLink<DataVector<Muon::MuonPatternCombination> >& track,
                      const std::vector<const PRD_MultiTruthCollection*>& orderedPRD_Truth, const PRD_InverseTruth& inverseTruth);

        void addDetailedTrackTruth(std::vector<DetailedTrackTruth>* output, const Muon::MuonPatternCombination& pattern,
                                   const std::vector<const PRD_MultiTruthCollection*>& orderedPRD_Truth,
                                   const PRD_InverseTruth& inverseTruth);

        SubDetHitStatistics countPRDsOnTruth(const TruthTrajectory& traj, const PRD_InverseTruth& inverseTruth,
                                             std::set<Muon::MuonStationIndex::ChIndex> chIndices);

        Amg::Vector3D getPRDTruthPosition(const Muon::MuonSegment& segment, std::list<const HepMC::GenParticle*> genPartList, int truthPos,
                                          std::set<Muon::MuonStationIndex::ChIndex> chIndices);

        void addDetailedTrackTruthFromSegment(std::vector<DetailedSegmentTruth>* output, const Muon::MuonSegment& segment,
                                              const std::vector<const PRD_MultiTruthCollection*>& orderedPRD_Truth,
                                              const PRD_InverseTruth& inverseTruth);
    };

}  // end namespace Trk

#endif /*DETAILEDMUONPATTERNTRUTHBUILDER_H*/
