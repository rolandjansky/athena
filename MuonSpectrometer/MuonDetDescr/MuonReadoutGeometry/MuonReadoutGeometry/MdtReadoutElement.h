/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREADOUTGEOMETRY_MDTREADOUTELEMENT_H
#define MUONREADOUTGEOMETRY_MDTREADOUTELEMENT_H

#include "CxxUtils/CachedUniquePtr.h"
#include "CxxUtils/CachedValue.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "TrkDistortedSurfaces/SaggedLineSurface.h"

class BLinePar;

namespace Trk {
    class CylinderBounds;
    class SurfaceBounds;
}  // namespace Trk

namespace Muon {
    class MdtAlignModule;
    class CombinedMuonAlignModule;
}  // namespace Muon

namespace MuonGM {

    /**
       An MdtReadoutElement corresponds to a single MDT multilayer; therefore
       typicaly a MDT chamber consists of two MdtReadoutElements, each identified
       by StationName, StationEta, StationPhi, Technology=0, and Multilayer.

       Pointers to all MdtReadoutElements are created in the build() method of
       the MuonChamber class, and are held in arrays by the MuonDetectorManager,
       which is responsible for storing, deleting and providing access to these
       objects.

       An MdtReadoutElement holds properties related to its internal structure
       (i.e. wire pitch) and general geometrical properties (size);  it
       implements tracking interfaces and provides access to typical
       readout-geometry information: i.e. number of tubes, position of the tubes,
       distance of a point to the readout side, etc.

       The globalToLocalCoords and globalToLocalTransform methods (+ their opposite)
       define the link between the ATLAS global reference frame and the internal
       (geo-model defined) local reference frame of any drift tube (which is the
       frame where local coordinates of SimHits, in output from G4, are expressed).
    */

    constexpr int maxnlayers = 4;
    constexpr int maxnsteps = 10;

    class MdtReadoutElement final : public MuonReadoutElement {
        friend class Muon::MdtAlignModule;
        friend class Muon::CombinedMuonAlignModule;
        friend class MuonChamber;

    public:
        MdtReadoutElement(GeoVFullPhysVol* pv, const std::string& stName, int zi, int fi, bool is_mirrored, MuonDetectorManager* mgr);

        virtual ~MdtReadoutElement() = default;

        // Id set/get methods
        inline int getMultilayer() const;
        inline int getNLayers() const;
        inline int getNtubesperlayer() const;
        inline int getNtubesinastep() const;
        inline void setMultilayer(const int ml);
        inline void setNLayers(const int nl);
        inline bool isInBarrel() const;
        void setIdentifier(const Identifier& id);
        bool getWireFirstLocalCoordAlongZ(int tubeLayer, double& coord) const;
        bool getWireFirstLocalCoordAlongR(int tubeLayer, double& coord) const;

        virtual bool containsId(const Identifier& id) const override;

        // detector specific
        inline double getTubeLength(const int tubeLayer, const int tube) const;
        inline double getActiveTubeLength(const int tubeLayer, const int tube) const;
        inline double getGasLength(const int tubeLayer, const int tube) const;
        inline double getWireLength(const int tubeLayer, const int tube) const;
        double tubeLength(const Identifier& id) const;
        double innerTubeRadius() const;
        double outerTubeRadius() const;
        inline double tubePitch() const;

        // local(tube frame) to global coord.
        Amg::Vector3D localToGlobalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D localToGlobalTransf(const Identifier& id) const;
        Amg::Transform3D localToGlobalTransf(const int tubeLayer, const int tube) const;
        Amg::Vector3D nodeform_localToGlobalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D nodeform_localToGlobalTransf(const Identifier& id) const;
        // global to local(tube frame) coord.
        Amg::Vector3D globalToLocalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D globalToLocalTransf(const Identifier& id) const;
        Amg::Vector3D nodeform_globalToLocalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D nodeform_globalToLocalTransf(const Identifier& id) const;
        // local(tube frame) to multilayer coord.
        Amg::Vector3D tubeToMultilayerCoords(Amg::Vector3D x, const Identifier& id) const;
        Amg::Transform3D tubeToMultilayerTransf(const Identifier& id) const;
        Amg::Vector3D nodeform_tubeToMultilayerCoords(Amg::Vector3D x, const Identifier& id) const;
        Amg::Transform3D nodeform_tubeToMultilayerTransf(const Identifier& id) const;
        // multilayer to local (tube frame) coords
        Amg::Vector3D multilayerToTubeCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D multilayerToTubeTransf(const Identifier& id) const;
        Amg::Vector3D nodeform_multilayerToTubeCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D nodeform_multilayerToTubeTransf(const Identifier& id) const;

        // in the native MDT reference system
        Amg::Vector3D localTubePos(const Identifier& id) const;
        Amg::Vector3D localTubePos(const IdentifierHash& id) const;
        Amg::Vector3D localTubePos(const int multilayer, const int tubelayer, const int tube) const;
        Amg::Vector3D nodeform_localTubePos(const Identifier& id) const;
        Amg::Vector3D nodeform_localTubePos(const IdentifierHash& id) const;
        Amg::Vector3D nodeform_localTubePos(const int multilayer, const int tubelayer, const int tube) const;
        // in the global reference system
        Amg::Vector3D tubePos(const Identifier& id) const;
        Amg::Vector3D tubePos(const IdentifierHash& id) const;
        Amg::Vector3D tubePos(const int multilayer, const int tubelayer, const int tube) const;
        Amg::Vector3D nodeform_tubePos(const Identifier& id) const;
        Amg::Vector3D nodeform_tubePos(const IdentifierHash& id) const;
        Amg::Vector3D nodeform_tubePos(const int multilayer, const int tubelayer, const int tube) const;
        // in the Amdb local (szt) reference system //////////////// check here ////////////
        Amg::Vector3D AmdbLRStubePos(const Identifier& id) const;
        Amg::Vector3D AmdbLRStubePos(const int multilayer, const int tubelayer, const int tube) const;

        // Readout / HV side
        double signedRODistanceFromTubeCentre(const Identifier& id) const;
        double signedRODistanceFromTubeCentre(const int ml, const int tl, const int tube) const;
        double RODistanceFromTubeCentre(const Identifier& id) const;
        double RODistanceFromTubeCentre(const int ml, const int tl, const int tube) const;
        double distanceFromRO(const Amg::Vector3D& GlobalHitPosition, const Identifier& id) const;
        double distanceFromRO(const Amg::Vector3D& GlobalHitPosition, const int multilayer, const int tubelayer, const int tube) const;
        int isAtReadoutSide(const Amg::Vector3D& GlobalHitPosition, const Identifier& id) const;
        int isAtReadoutSide(const Amg::Vector3D& GlobalHitPosition, const int multilayer, const int tubelayer, const int tube) const;
        Amg::Vector3D localROPos(const int multilayer, const int tubelayer, const int tube) const;
        Amg::Vector3D ROPos(const int multilayer, const int tubelayer, const int tube) const;
        Amg::Vector3D tubeFrame_localROPos(const int multilayer, const int tubelayer, const int tube) const;
        Amg::Vector3D localROPos(const Identifier& id) const;
        Amg::Vector3D ROPos(const Identifier& id) const;
        Amg::Vector3D tubeFrame_localROPos(const Identifier& id) const;

        // defining B-line parameters
        /*inline*/ void setBLinePar(const BLinePar* bLine);
        inline void clearBLinePar();
        inline const BLinePar* getBLinePar() const { return m_BLinePar; }

        ////////////////////////////////////////////////////////////
        //// Tracking interfaces
        ////////////////////////////////////////////////////////////

        virtual void clearCache() override;
        virtual void fillCache() override;
        virtual void refreshCache() override { clearCache(); }
        void clearBLineCache();
        void fillBLineCache();

        virtual const Trk::Surface& surface() const override final;
        virtual const Trk::SaggedLineSurface& surface(const Identifier& id) const override final;
        const Trk::SaggedLineSurface& surface(const int tubeLayer, const int tube) const;
        virtual const Trk::SurfaceBounds& bounds() const override final;
        virtual const Trk::CylinderBounds& bounds(const Identifier& id) const override final;
        const Trk::CylinderBounds& bounds(const int tubeLayer, const int tube) const;

        virtual const Amg::Transform3D& transform(const Identifier&) const override final;
        const Amg::Transform3D& transform(const int tubeLayer, const int tube) const;
        virtual const Amg::Transform3D& transform() const override final { return absTransform(); }

        virtual const Amg::Vector3D& center(const Identifier&) const override final;
        const Amg::Vector3D& center(const int tubeLayer, const int tube) const;
        virtual const Amg::Vector3D& center() const override final;

        virtual const Amg::Vector3D& normal(const Identifier&) const override final { return normal(); }
        Amg::Vector3D tubeNormal(const Identifier&) const;
        const Amg::Vector3D& normal(const int, const int) const { return normal(); }
        Amg::Vector3D tubeNormal(const int, const int) const;
        const Amg::Vector3D& normal() const override final;

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Mdt; }

        /** returns all the surfaces contained in this detector element */
        std::vector<const Trk::Surface*> surfaces() const;

        // methods handling deformations
        const Amg::Transform3D& fromIdealToDeformed(const Identifier&) const;
        const Amg::Transform3D& fromIdealToDeformed(const int multilayer, const int tubelayer, const int tube) const;

    private:
        // Called from MuonChamber
        void geoInitDone();

        double getTubeLengthForCaching(const int tubeLayer, const int tube) const;
        double getNominalTubeLengthWoCutouts(const int tubeLayer, const int tube) const;
        Amg::Vector3D localNominalTubePosWoCutouts(const int tubelayer, const int tube) const;

        Amg::Vector3D posOnDefChamWire(const Amg::Vector3D& locAMDBPos, double, double, double, double, double, double, double, double,
                                       double, double, double, double, double, double, double, const Amg::Vector3D& fixedPoint) const;
        Amg::Vector3D posOnDefChamWire(const Amg::Vector3D& locAMDBPos, const BLinePar* bLine, const Amg::Vector3D& fixedPoint) const;
        void wireEndpointsAsBuilt(Amg::Vector3D& locAMDBWireEndP, Amg::Vector3D& locAMDBWireEndN, const int multilayer, const int tubelayer,
                                  const int tube) const;

        // methods used only by friend class MdtAlignModule to shift chambers
        void shiftTube(const Identifier& id);
        void restoreTubes();

        int m_multilayer{0};
        int m_nlayers{-1};
        double m_tubepitch{-9999.};
        double m_tubelayerpitch{-9999.};
        int m_ntubesperlayer{-1};
        int m_nsteps{-1};
        int m_ntubesinastep{-1};
        double m_tubelenStepSize{-9999.};
        std::array<double, maxnsteps> m_tubelength{-9999.};
        double m_cutoutShift{-9999.};
        double m_endpluglength{-9999.};
        double m_deadlength{-9999.};
        bool m_inBarrel{false};
        std::array<double, maxnlayers> m_firstwire_x{-9999.};
        std::array<double, maxnlayers> m_firstwire_y{-9999.};
        double m_innerRadius{-9999.};
        double m_tubeWallThickness{-9999.};
        CxxUtils::CachedValue<int> m_zsignRO_tubeFrame;  // comes from AMDB CRO location in the station

        Amg::Transform3D globalTransform(const Amg::Vector3D& tubePos, const Amg::Transform3D& toDeform) const;
        Amg::Transform3D globalTransform(const Amg::Vector3D& tubePos) const;

        struct GeoInfo {
            GeoInfo(const Amg::Transform3D& transform) : m_transform(transform), m_center(transform.translation()) {}
            Amg::Transform3D m_transform;
            Amg::Vector3D m_center;
        };
        std::unique_ptr<GeoInfo> makeGeoInfo(const int tubelayer, const int tube) const;
        const GeoInfo& geoInfo(const int tubeLayer, const int tube) const;
        Amg::Transform3D deformedTransform(const int multilayer, const int tubelayer, const int tube) const;

        std::vector<CxxUtils::CachedUniquePtr<GeoInfo> > m_tubeGeo;                      // one per tube
        std::vector<CxxUtils::CachedUniquePtr<GeoInfo> > m_backupTubeGeo;                // one per tube
        std::vector<CxxUtils::CachedUniquePtr<Amg::Transform3D> > m_deformTransf;        // one per tube
        std::vector<CxxUtils::CachedUniquePtr<Amg::Transform3D> > m_backupDeformTransf;  // one per tube

        const BLinePar* m_BLinePar{nullptr};
        CxxUtils::CachedValue<Amg::Vector3D> m_elemNormal;                               // one
        std::vector<CxxUtils::CachedUniquePtr<Trk::SaggedLineSurface> > m_tubeSurfaces;  // one per tube
        std::vector<CxxUtils::CachedUniquePtr<Trk::CylinderBounds> > m_tubeBounds;       // one per step in tube-length

        /// Flag whether any elements have been inserted
        /// into the corresponding vectors.
        /// Used to speed up the clear-cache operations for the case where
        /// the vectors are empty.
        mutable std::atomic<bool> m_haveTubeSurfaces = false;
        mutable std::atomic<bool> m_haveTubeGeo = false;
        mutable std::atomic<bool> m_haveTubeBounds = false;
        mutable std::atomic<bool> m_haveDeformTransf = false;

        // the single surface information representing the DetElement
        CxxUtils::CachedUniquePtr<Trk::Surface> m_associatedSurface;
        CxxUtils::CachedUniquePtr<Trk::SurfaceBounds> m_associatedBounds;
    };

    int MdtReadoutElement::getMultilayer() const { return m_multilayer; }
    int MdtReadoutElement::getNLayers() const { return m_nlayers; }
    int MdtReadoutElement::getNtubesperlayer() const { return m_ntubesperlayer; }
    int MdtReadoutElement::getNtubesinastep() const { return m_ntubesinastep; }
    void MdtReadoutElement::setMultilayer(const int ml) { m_multilayer = ml; }
    void MdtReadoutElement::setNLayers(const int nl) { m_nlayers = nl; }
    bool MdtReadoutElement::isInBarrel() const { return m_inBarrel; }
    double MdtReadoutElement::tubePitch() const { return m_tubepitch; }
    void MdtReadoutElement::clearBLinePar() { m_BLinePar = 0; }
    double MdtReadoutElement::getWireLength(const int tubeLayer, const int tube) const { return getGasLength(tubeLayer, tube); }
    double MdtReadoutElement::getGasLength(const int tubeLayer, const int tube) const {
        return getTubeLength(tubeLayer, tube) - 2. * m_endpluglength;
    }
    double MdtReadoutElement::getTubeLength(const int tubeLayer, const int tube) const {
        return getActiveTubeLength(tubeLayer, tube) + 2. * m_deadlength;
    }
    double MdtReadoutElement::getActiveTubeLength(const int tubeLayer, const int tube) const {
        return 2. * (bounds(tubeLayer, tube).halflengthZ());
    }

    inline std::vector<const Trk::Surface*> MdtReadoutElement::surfaces() const {
        std::vector<const Trk::Surface*> elementSurfaces;
        elementSurfaces.reserve(m_tubeSurfaces.size() + 1);
        if (m_associatedSurface) { elementSurfaces.push_back(m_associatedSurface.get()); }
        for (const auto& s : m_tubeSurfaces) {
            if (s) elementSurfaces.push_back(s.get());
        }
        return elementSurfaces;
    }

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_MDTREADOUTELEMENT_H
