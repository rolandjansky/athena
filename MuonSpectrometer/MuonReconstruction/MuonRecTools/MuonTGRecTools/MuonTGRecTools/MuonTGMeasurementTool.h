/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H
#define MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H

#include <fstream>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ConcurrencyFlags.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonTGHits.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"
#include "MuonTGSegments.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

namespace MuonGM {
    class MuonDetectorManager;
}

namespace Muon {

    /** @class MuonTGMeasurementTool

        contain application tools for (muon) tracking geometry

        @author Sarka.Todorova@cern.ch
        */

    class MuonTGMeasurementTool : public AthAlgTool, virtual public IMuonTGMeasTool {
    public:
        typedef Trk::Track Track;
        typedef std::pair<const Trk::Layer*, std::vector<Identifier>*> PairOfLayerID;
        typedef std::pair<const Trk::Layer*, std::vector<const Trk::PrepRawData*>*> PairOfLayerPrd;

        /** Constructor with AlgTool parameters */
        MuonTGMeasurementTool(const std::string& name, const std::string&, const IInterface*);

        virtual ~MuonTGMeasurementTool() = default;

        virtual StatusCode initialize() override;

        const Trk::TrackParameters* layerToDetEl(const Trk::Layer*, const Trk::TrackParameters*, Identifier) const override;
        const Trk::TrackParameters* detElToLayer(const Trk::Layer*, const Trk::TrackParameters*, Identifier) const override;
        const Trk::RIO_OnTrack* measToLayer(const Trk::Layer*, const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const override;
        double residual(const Trk::Layer*, const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const override;
        double residual(const Trk::Layer*, const Trk::TrackParameters*, Identifier) const override;
        double residual(const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const override;
        double residual(const Trk::TrackParameters*, Identifier&) const override;
        const Identifier nearestDetEl(const Trk::Layer*, const Trk::TrackParameters*, bool measPhi, double& pitch) const override;
        const Trk::Layer* associatedLayer(Identifier id, Amg::Vector3D& gp) const override;
        const Trk::Layer* associatedLayer(Identifier id, const Trk::TrackingVolume* vol) const override;
        const Trk::Layer* match(Identifier id, const Trk::Layer* lay) const override;

    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};

        ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc{this, "TrackingGeometrySvc",
                                                                       "TrackingGeometrySvc/AtlasTrackingGeometrySvc"};

        SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "",
                                                                               "Key of input TrackingGeometry"};

        const MuonGM::MuonDetectorManager* m_muonDetMgr;  // nominal MuonDetectorManager from DetectorStore (used if UseDSManager=true)

        // projection matrices
        std::unique_ptr<AmgMatrix(5, 5)> m_tgcProjEta;
        std::unique_ptr<AmgMatrix(5, 5)> m_tgcProjPhi;
        std::unique_ptr<AmgMatrix(5, 5)> m_rpcProjEta;
        std::unique_ptr<AmgMatrix(5, 5)> m_rpcProjPhi;

        // steering
        Gaudi::Property<bool> m_alignedMode{this, "AlignedMode", true};
        Gaudi::Property<bool> m_useDSManager{this, "UseDSManager", false};

        inline const Trk::TrackingGeometry* getGeometry() const {
            /// Good old way of retrieving the volume via the geometry service
            if (m_trackingGeometryReadKey.empty()) { return m_trackingGeometrySvc->trackingGeometry(); }
            SG::ReadCondHandle<Trk::TrackingGeometry> handle(m_trackingGeometryReadKey, Gaudi::Hive::currentContext());
            if (!handle.isValid()) {
                ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
                return nullptr;
            }
            return handle.cptr();
        }
    };

}  // namespace Muon

#endif  // MUONTGRECTOOLS_MUONTGMEASUREMENTTOOL_H
