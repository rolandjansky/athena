/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATHITTOOL_H
#define MUPATHITTOOL_H

#include <mutex>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuPatPrimitives/MuPatGarbage.h"
#include "MuPatPrimitives/MuPatHit.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"

namespace Trk {
    class IPropagator;
    class Track;
    class MeasurementBase;
    class IResidualPullCalculator;
}  // namespace Trk

namespace Muon {

    class MuonEDMPrinterTool;
    class MuonSegment;

    static const InterfaceID IID_MuPatHitTool("Muon::MuPatHitTool", 1, 0);

    class MuPatHitTool : public AthAlgTool {
    public:
        /** default AlgTool constructor */
        MuPatHitTool(const std::string&, const std::string&, const IInterface*);

        /** destructor */
        ~MuPatHitTool();

        /** initialize method, method taken from bass-class AlgTool */
        StatusCode initialize();

        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() { return IID_MuPatHitTool; }

        /** @brief insert a MuPatHit into a sorted list, ownership of the MuPatHit is taken by the routine
            @param hit the hit
            @param hitList the list
            @return true if insertion succeded
        */
        bool insert(MuPatHit* hit, MuPatHitList& hitList) const;

        /** @brief create a MCTBList from a MuonSegment
            @param seg the MuonSegment
            @param hitList the list to be filled
            @return true if creation succeded
        */
        bool create(const EventContext& ctx, const MuonSegment& seg, MuPatHitList& hitList, GarbageContainer& hitsToBeDeleted) const;

        /** @brief create a MuPatHitList from a Track
            @param track the input track
            @param hitList the list to be filled
            @return true if creation succeded
        */
        bool create(const Trk::Track& track, MuPatHitList& hitList, GarbageContainer& hitsToBeDeleted) const;

        /** @brief create a MuPatHitList from a Track
            @param pars the input parameters
            @param measVec the list of measurements
            @param hitList the list to be filled
            @return true if creation succeded
        */
        bool create(const EventContext& ctx, const Trk::TrackParameters& pars, const std::vector<const Trk::MeasurementBase*>& measVec, MuPatHitList& hitList,
                    GarbageContainer& hitsToBeDeleted) const;

        /** @brief merge two MuPatHitLists into a new one
            @param hitList1 the first  list
            @param hitList2 the second list
            @param outList  the resulting list
            @return true if merge succeded
        */
        bool merge(const EventContext& ctx, const MuPatHitList& hitList1, const MuPatHitList& hitList2, MuPatHitList& outList) const;

        /** @brief merge two MuPatHitLists into a new one. The first list will be added to the second
            @param hitList1 the first  list
            @param hitList2 the second list
            @return true if merge succeded
        */
        bool merge(const EventContext& ctx, const MuPatHitList& hitList1, MuPatHitList& hitList2) const;

        /** @brief extract a sorted vector of MeasurementBase objects
            @param hitList the input  list
            @param measVec the output vector
            @param usePreciseHits switch to select between precise and broad hits
            @param getReducedTrack switch to extract reduced track
            @return true if the extraction succeded
        */
        bool extract(const MuPatHitList& hitList, std::vector<const Trk::MeasurementBase*>& measVec, bool usePreciseHits = true,
                     bool getReducedTrack = false) const;

        /** @brief check whether the list is correctly sorted */
        bool isSorted(const MuPatHitList& hitList) const;

        /** print the list of hits, with optional parts of the printout (position,direction,momentum) */
        std::string print(const MuPatHitList& hitList, bool printPos = true, bool printDir = true, bool printMom = true) const;

        /** remove hit with a give Identifier */
        bool remove(const Identifier& id, MuPatHitList& hitList) const;

        /** remove hit containing give measurement (uses pointer comparison) */
        bool remove(const Trk::MeasurementBase& meas, MuPatHitList& hitList) const;

        /** update hit list for a give track */
        bool update(const Trk::Track& track, MuPatHitList& hitList) const;

        /** @brief print data part of Muon MeasurementBase to string.
         *
         * This really belongs in MuonEDMPrinterTool in package MuonRecHelperTools.
         * However, due to the tight tag collecting policy for release 15.5.0, this change
         * is not likely to get accepted there.
         * To be moved there as soon as release 16 is open
         */
        std::string printData(const Trk::MeasurementBase& measurement) const;

        /** @brief print identifier part of Muon MeasurementBase to string.
         *
         * This really belongs in MuonEDMPrinterTool in package MuonRecHelperTools.
         * However, due to the tight tag collecting policy for release 15.5.0, this change
         * is not likely to get accepted there.
         * To be moved there as soon as release 16 is open
         */
        std::string printId(const Trk::MeasurementBase& measurement) const;

    private:
        /** @brief get hit type */
        MuPatHit::Type getHitType(const Identifier& id) const;

        /** @brief get hit info */
        void getHitInfo(const Trk::MeasurementBase& meas, MuPatHit::Info& hitInfo) const;

        /** @brief calculate broad measurement for a give precise measurement */
        std::unique_ptr<const Trk::MeasurementBase> createBroadMeasurement(const Trk::MeasurementBase& preciseMeas,
                                                                           const MuPatHit::Info& hitInfo) const;

        /** @brief insert a hit into a sorted list
            @param  list the list into which the hit should be inserted
            @param  pos  an list iterator of the best estimate of the position where the hit should be inserted
            @param  hit  the actual hit that should be inserted
            @return      the position at which the hit was inserted
        */
        MuPatHitIt insert(MuPatHitList& list, MuPatHitIt& pos, MuPatHit* hit) const;

        ToolHandle<Trk::IPropagator> m_propagator{
            this,
            "AtlasRungeKuttaPropagator",
            "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator",
        };
        ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtRotCreator{
            this,
            "MdtRotCreator",
            "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator",
        };  //<! tool to calibrate MDT hits
        ToolHandle<IMuonClusterOnTrackCreator> m_cscRotCreator{
            this,
            "CscRotCreator",
            "Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator",
        };  //<! tool to calibrate CSC hits
        ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compClusterCreator{
            this,
            "TriggerChamberClusterOnTrackCreator",
            "Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator",
        };  //<! tool to create competing clusters on track
        ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator{
            this,
            "ResidualPullCalculator",
            "Trk::ResidualPullCalculator/ResidualPullCalculator",
        };  //<! tool to calculate residuals and pulls
        PublicToolHandle<MuonEDMPrinterTool> m_printer{
            this,
            "Printer",
            "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
        };  //<! tool to print EDM objects

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
            this,
            "MuonIdHelperSvc",
            "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
        };
        ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc{
            this,
            "edmHelper",
            "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
            "Handle to the service providing the IMuonEDMHelperSvc interface",
        };  //<! multipurpose helper tool

        Trk::MagneticFieldProperties m_magFieldProperties;  //!< magnetic field properties
    };

}  // namespace Muon

#endif
