/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MuonStationTypeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRACKINGGEOMETRY_MUONSTATIONTYPEBUILDER_H
#define MUONTRACKINGGEOMETRY_MUONSTATIONTYPEBUILDER_H
// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/TrackingVolume.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoVPhysVol.h"

namespace Trk {
    class Volume;
    class Layer;
    class CuboidVolumeBounds;
    class TrapezoidVolumeBounds;
    class DoubleTrapezoidVolumeBounds;
    class PlaneLayer;
}  // namespace Trk

namespace MuonGM {
    class MuonDetectorManager;
    class MuonStation;
}  // namespace MuonGM

namespace Muon {

    typedef std::pair<Trk::SharedObject<const Trk::Layer>, const Amg::Transform3D*> LayTr;

    /** @class MuonStationTypeBuilder

        The Muon::MuonStationTypeBuilder retrieves components of muon stations from
       Muon Geometry Tree, builds 'prototype' object (TrackingVolume with NameType)

        by Sarka.Todorova@cern.ch
      */

    class MuonStationTypeBuilder : public AthAlgTool {
    public:
        struct Cache {
            std::unique_ptr<Trk::MaterialProperties> m_mdtTubeMat{};
            std::unique_ptr<Trk::MaterialProperties> m_rpcLayer{};
            std::vector<std::unique_ptr<Trk::MaterialProperties>> m_mdtFoamMat{};
            std::unique_ptr<Trk::MaterialProperties> m_rpc46{};
            std::vector<std::unique_ptr<Trk::MaterialProperties>> m_rpcDed{};
            std::unique_ptr<Trk::MaterialProperties> m_rpcExtPanel{};
            std::unique_ptr<Trk::MaterialProperties> m_rpcMidPanel{};
            std::unique_ptr<Trk::MaterialProperties> m_matCSC01{};
            std::unique_ptr<Trk::MaterialProperties> m_matCSCspacer1{};
            std::unique_ptr<Trk::MaterialProperties> m_matCSC02{};
            std::unique_ptr<Trk::MaterialProperties> m_matCSCspacer2{};
            std::unique_ptr<Trk::MaterialProperties> m_matTGC01{};
            std::unique_ptr<Trk::MaterialProperties> m_matTGC06{};
        };
        /** Constructor */
        MuonStationTypeBuilder(const std::string&, const std::string&, const IInterface*);
        /** Destructor */
        virtual ~MuonStationTypeBuilder() = default;
        /** AlgTool initailize method.*/
        StatusCode initialize();
        /** AlgTool finalize method */
        StatusCode finalize();
        /** Interface methode */
        static const InterfaceID& interfaceID();
        /** steering routine */
        Trk::TrackingVolumeArray* processBoxStationComponents(const GeoVPhysVol* cv, Trk::CuboidVolumeBounds* envBounds,
                                                              Cache&) const;

        Trk::TrackingVolumeArray* processTrdStationComponents(const GeoVPhysVol* cv, Trk::TrapezoidVolumeBounds* envBounds,
                                                              Cache&) const;

        Trk::TrackingVolume* processCscStation(const GeoVPhysVol* cv, const std::string& name, Cache&) const;

        std::vector<Trk::TrackingVolume*> processTgcStation(const GeoVPhysVol* cv, Cache&) const;

        /** components */
        const Trk::TrackingVolume* processMdtBox(Trk::Volume*&, const GeoVPhysVol*&, Amg::Transform3D*, double, Cache&) const;

        const Trk::TrackingVolume* processMdtTrd(Trk::Volume*&, const GeoVPhysVol*&, Amg::Transform3D*, Cache&) const;

        const Trk::TrackingVolume* processRpc(Trk::Volume*&, std::vector<const GeoVPhysVol*>, std::vector<Amg::Transform3D>, Cache&) const;

        const Trk::TrackingVolume* processSpacer(Trk::Volume&, std::vector<const GeoVPhysVol*>, std::vector<Amg::Transform3D>) const;

        const Trk::TrackingVolume* processNSW(const MuonGM::MuonDetectorManager* muonDetMgr, const std::vector<const Trk::Layer*>&) const;

        Trk::LayerArray* processCSCTrdComponent(const GeoVPhysVol*&, Trk::TrapezoidVolumeBounds*&, Amg::Transform3D*&, Cache&) const;

        Trk::LayerArray* processCSCDiamondComponent(const GeoVPhysVol*&, Trk::DoubleTrapezoidVolumeBounds*&, Amg::Transform3D*&,
                                                    Cache&) const;

        Trk::LayerArray* processTGCComponent(const GeoVPhysVol*&, Trk::TrapezoidVolumeBounds*&, Amg::Transform3D*&, Cache&) const;

        std::pair<const Trk::Layer*, const std::vector<const Trk::Layer*>*> createLayerRepresentation(
            const Trk::TrackingVolume* trVol) const;

        Trk::Layer* createLayer(const MuonGM::MuonDetectorManager* detMgr, const Trk::TrackingVolume* trVol, Trk::MaterialProperties*,
                                Amg::Transform3D&) const;

        static Identifier identifyNSW(const MuonGM::MuonDetectorManager* muonDetMgr, const std::string&, const Amg::Transform3D&) ;

        void printChildren(const GeoVPhysVol*) const;
        // used to be private ..
        double get_x_size(const GeoVPhysVol*) const;
        double decodeX(const GeoShape*) const;
        double getVolume(const GeoShape*) const;
        Trk::MaterialProperties getAveragedLayerMaterial(const GeoVPhysVol*, double, double) const;
        void collectMaterial(const GeoVPhysVol*, Trk::MaterialProperties&, double) const;
        Trk::MaterialProperties collectStationMaterial(const Trk::TrackingVolume* trVol, double) const;

    private:
        /** Private method to fill default material */
        // void fillDefaultServiceMaterial();

        Gaudi::Property<bool> m_multilayerRepresentation{this, "BuildMultilayerRepresentation", true};
        Gaudi::Property<bool> m_resolveSpacer{this, "ResolveSpacerBeams", false};

        ToolHandle<Trk::ITrackingVolumeArrayCreator> m_trackingVolumeArrayCreator{
            this, "TrackingVolumeArrayCreator",
            "Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"};  //!< Helper Tool to create TrackingVolume Arrays

        std::unique_ptr<Trk::Material> m_muonMaterial;  //!< the material
        std::unique_ptr<Trk::GeoMaterialConverter> m_materialConverter;
    };

}  // namespace Muon

#endif  // MUONTRACKINGGEOMETRY_MUONSTATIONTYPEBUILDER_H
