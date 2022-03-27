/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERCOND_H
#define MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERCOND_H

// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonTrackingGeometry/MuonStationTypeBuilder.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
#include "TrkDetDescrInterfaces/IDetachedTrackingVolumeBuilderCond.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/TrackingVolume.h"

namespace Trk {
    class MaterialProperties;
}

namespace MuonGM {
    class MuonDetectorManager;
}

namespace Muon {

    /** @class MuonStationBuilderCond

        The Muon::MuonStationBuilderCond retrieves muon stations from Muon Geometry Tree
        prototypes built with help of Muon::MuonStationTypeBuilder

        by Sarka.Todorova@cern.ch
      */

    class MuonStationBuilderCond : public AthAlgTool, virtual public Trk::IDetachedTrackingVolumeBuilderCond {
    public:
        MuonStationBuilderCond(const std::string&, const std::string&, const IInterface*);
        virtual ~MuonStationBuilderCond() = default;
        StatusCode initialize() override;

        std::pair<EventIDRange, std::unique_ptr<const std::vector<std::unique_ptr<const Trk::DetachedTrackingVolume>>>>
        buildDetachedTrackingVolumes(const EventContext& ctx, bool blend = false) const override;

    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        std::vector<std::unique_ptr<Trk::DetachedTrackingVolume>> buildDetachedTrackingVolumeTypes(
                                                                    bool blend, const MuonGM::MuonDetectorManager* muonMgr) const;

        void glueComponents(const Trk::DetachedTrackingVolume*) const;
        void encloseLayers(const Trk::DetachedTrackingVolume*) const;
        void identifyLayers(const Trk::DetachedTrackingVolume*, int, int, const MuonGM::MuonDetectorManager*) const;
        void identifyPrototype(const Trk::TrackingVolume*, int, int, const Amg::Transform3D&, const MuonGM::MuonDetectorManager*) const;
        void getNSWStationsForTranslation(
            const GeoVPhysVol* pv, const std::string& name, const Amg::Transform3D&,
            std::vector<std::pair<std::pair<const GeoLogVol*, Trk::MaterialProperties*>, std::vector<Amg::Transform3D>>>& vols,
            std::vector<std::string>& volNames) const;

        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muonMgrReadKey{this, "MuonMgrReadKey", "MuonDetectorManager",
                                                                            "Key of input MuonMgr"};

        ToolHandle<Muon::MuonStationTypeBuilder> m_muonStationTypeBuilder{
            this, "StationTypeBuilder",
            "Muon::MuonStationTypeBuilder/MuonStationTypeBuilder"};  //!< Helper Tool to create TrackingVolume Arrays
        ToolHandle<Trk::ITrackingVolumeHelper> m_trackingVolumeHelper{
            this, "TrackingVolumeHelper", "Trk::TrackingVolumeHelper/TrackingVolumeHelper"};  //!< Helper Tool to create TrackingVolumes

        Trk::Material m_muonMaterial;                                    //!< the material
        std::unique_ptr<Trk::GeoShapeConverter> m_geoShapeConverter;     //!< shape converter
        std::unique_ptr<Trk::GeoMaterialConverter> m_materialConverter;  //!< material converter
        Gaudi::Property<bool> m_buildBarrel{this, "BuildBarrelStations", true};
        Gaudi::Property<bool> m_buildEndcap{this, "BuildEndcapStations", true};
        Gaudi::Property<bool> m_buildCsc{this, "BuildCSCStations", true};
        Gaudi::Property<bool> m_buildTgc{this, "BuildTGCStations", true};
    };

}  // namespace Muon

#endif  // MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERCOND_H
