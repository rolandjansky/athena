/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREADOUTGEOMETRY_MMREADOUTELEMENT_H
#define MUONREADOUTGEOMETRY_MMREADOUTELEMENT_H

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

class BLinePar;
class GeoVFullPhysVol;

namespace MuonGM {
    /**
       An MMReadoutElement corresponds to a single STGC module; therefore
       typicaly a barrel muon station contains:
    */

    class MMReadoutElement final : public MuonClusterReadoutElement {
    public:
        /** constructor */
        MMReadoutElement(GeoVFullPhysVol* pv, const std::string& stName, int zi, int fi, int mL, bool is_mirrored, MuonDetectorManager* mgr);

        /** destructor */
        ~MMReadoutElement();

        /** function to be used to check whether a given Identifier is contained in the readout element */
        virtual bool containsId(const Identifier& id) const override final;

        /** distance to readout.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual double distanceToReadout(const Amg::Vector2D& pos, const Identifier& id) const override final;

        /** strip number corresponding to local position.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual int stripNumber(const Amg::Vector2D& pos, const Identifier& id) const override final;

        /** strip position -- local or global
            If the strip number is outside the range of valid strips, the function will return false */
        virtual bool stripPosition(const Identifier& id, Amg::Vector2D& pos) const override final;
        bool stripGlobalPosition(const Identifier& id, Amg::Vector3D& gpos) const;

        /** strip length
        Wrappers to MuonChannelDesign::channelLength() taking into account the passivated width */
        double stripLength(const Identifier& id) const;
        double stripActiveLength(const Identifier& id) const;
        double stripActiveLengthLeft(const Identifier& id) const;
        double stripActiveLengthRight(const Identifier& id) const;

        /** boundary check
        Wrapper Trk::PlaneSurface::insideBounds() taking into account the passivated width */
        bool insideActiveBounds(const Identifier& id, const Amg::Vector2D& locpos, double tol1 = 0., double tol2 = 0.) const;

        /** number of layers in phi/eta projection */
        virtual int numberOfLayers(bool) const override;

        /** number of strips per layer */
        virtual int numberOfStrips(const Identifier& layerId) const override final;
        virtual int numberOfStrips(int, bool measuresPhi) const override final;

        /** Number of missing bottom and top strips (not read out) */
        int numberOfMissingTopStrips(const Identifier& layerId) const;
        int numberOfMissingBottomStrips(const Identifier& layerId) const;

        /** space point position for a given pair of phi and eta identifiers
            The LocalPosition is expressed in the reference frame of the phi surface.
            If one of the identifiers is outside the valid range, the function will return false */
        virtual bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos) const override final;

        /** Global space point position for a given pair of phi and eta identifiers
            If one of the identifiers is outside the valid range, the function will return false */
        virtual bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const override final;

        /** space point position for a pair of phi and eta local positions and a layer identifier
            The LocalPosition is expressed in the reference frame of the phi projection.
        */
        void spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const;

        /** simHit local (SD) To Global position - to be used by MuonGeoAdaprors only
         */
        Amg::Vector3D localToGlobalCoords(const Amg::Vector3D& locPos, const Identifier& id) const;

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::MM; }

        /** @brief function to fill tracking cache */
        virtual void fillCache() override final;
        virtual void refreshCache() override final {
            clearCache();
            fillCache();
        }

        /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
        virtual int surfaceHash(const Identifier& id) const override final;

        /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
        int surfaceHash(int gasGap, int measPhi) const;

        /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
        virtual int layerHash(const Identifier& id) const override final;
        /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
        int layerHash(int gasGap) const;

        /** returns the hash function to be used to look up the surface boundary for a given identifier */
        virtual int boundaryHash(const Identifier& id) const override final;

        /** @brief returns whether the current identifier corresponds to a phi measurement */
        virtual bool measuresPhi(const Identifier& id) const override final;

        /** @brief initialize the design classes for this readout element */
        void initDesign(double largeY, double smallY, double lengthX, double pitch, double thickness);

        /** returns the MuonChannelDesign class for the given identifier */
        const MuonChannelDesign* getDesign(const Identifier& id) const;

        /** set methods only to be used by MuonGeoModel */
        void setIdentifier(const Identifier& id);

        /** set methods only to be used by MuonGeoModel */
        void setChamberLayer(int ml) { m_ml = ml; }

        inline double getALine_rots() const;
        inline double getALine_rotz() const;
        inline double getALine_rott() const;
        inline bool has_ALines() const;
        inline bool has_BLines() const;
        void setDelta(double, double, double, double, double, double);  // input: translations, rotations
        void setDelta(MuonDetectorManager* mgr);
        void setBLinePar(BLinePar* bLine);
        inline void clearBLinePar();
        inline const BLinePar* getBLinePar() const { return m_BLinePar; }

    private:
        // MuonChannelDesign m_phiDesign;
        std::vector<MuonChannelDesign> m_etaDesign;

        std::vector<int> m_nStrips;  // #of active strips
        int m_nlayers{0};            // #of gas gaps

        int m_ml{0};  // multilayer (values: 1,2)
        Identifier m_parentId;

        // surface dimensions
        double m_halfX{100.};      // 0.5*radial_size
        double m_minHalfY{1900.};  // 0.5*bottom length (active area)
        double m_maxHalfY{2000.};  // 0.5*top length (active area)
        double m_offset{0.};

        double m_rots{0.};
        double m_rotz{0.};
        double m_rott{0.};

        bool m_hasALines{false};
        bool m_hasBLines{false};

        Amg::Transform3D m_delta{Amg::Transform3D::Identity()};

        BLinePar* m_BLinePar{nullptr};

        // transforms (RE->layer)
        std::array<Amg::Transform3D, 4> m_Xlg{Amg::Transform3D::Identity()};
    };

    void MMReadoutElement::clearBLinePar() { m_BLinePar = 0; }

    double MMReadoutElement::getALine_rots() const { return m_rots; }

    double MMReadoutElement::getALine_rotz() const { return m_rotz; }

    double MMReadoutElement::getALine_rott() const { return m_rott; }

    bool MMReadoutElement::has_ALines() const { return m_hasALines; }

    bool MMReadoutElement::has_BLines() const { return m_hasBLines; }

    inline int MMReadoutElement::surfaceHash(const Identifier& id) const { return surfaceHash(manager()->mmIdHelper()->gasGap(id), 0); }

    inline int MMReadoutElement::surfaceHash(int gasGap, int /*measPhi*/) const {
        return gasGap - 1;  // measPhi not used
    }

    inline int MMReadoutElement::layerHash(const Identifier& id) const { return layerHash(manager()->mmIdHelper()->gasGap(id)); }

    inline int MMReadoutElement::layerHash(int gasGap) const { return gasGap - 1; }

    inline int MMReadoutElement::boundaryHash(const Identifier& /*id*/) const { return 0; }

    inline bool MMReadoutElement::measuresPhi(const Identifier& /*id*/) const { return false; }

    inline const MuonChannelDesign* MMReadoutElement::getDesign(const Identifier& id) const {
        // return measuresPhi(id) ? &m_phiDesign : &m_etaDesign;
        return &(m_etaDesign[layerHash(id)]);
    }
    inline double MMReadoutElement::distanceToReadout(const Amg::Vector2D& pos, const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1.;
        return design->distanceToReadout(pos);
    }

    inline int MMReadoutElement::stripNumber(const Amg::Vector2D& pos, const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;
        return design->channelNumber(pos);
    }

    inline bool MMReadoutElement::stripPosition(const Identifier& id, Amg::Vector2D& pos) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return false;
        return design->channelPosition(manager()->mmIdHelper()->channel(id), pos);
    }

    inline double MMReadoutElement::stripLength(const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;
        return design->channelLength(manager()->mmIdHelper()->channel(id));
    }

    inline double MMReadoutElement::stripActiveLength(const Identifier& id) const {
        double l = stripLength(id);
        if (l < 0) return -1;
        return std::max(0., l - manager()->getMMPassivationCorrection());  // temporary way to pass MM correction for passivation
    }

    inline double MMReadoutElement::stripActiveLengthLeft(const Identifier& id) const {
        double l = stripLength(id);
        if (l < 0) return -1;
        return 0.5 * std::max(0., l - manager()->getMMPassivationCorrection());  // temporary way to pass MM correction for passivation
    }

    inline double MMReadoutElement::stripActiveLengthRight(const Identifier& id) const {
        double l = stripLength(id);
        if (l < 0) return -1;
        return 0.5 * std::max(0., l - manager()->getMMPassivationCorrection());  // temporary way to pass MM correction for passivation
    }

    inline bool MMReadoutElement::insideActiveBounds(const Identifier& id, const Amg::Vector2D& locpos, double tol1, double tol2) const {
        // TODO: when the full passivation info is available (left/right), we should check here
        // on which side locpos is (sign of locpos.y()) to get the correct passivated width.
        int stripNo = stripNumber(locpos, id);
        if (stripNo < 0) return false;
        double passiveW = manager()->getMMPassivationCorrection();
        return bounds(id).inside(locpos, tol1, tol2 - passiveW);
    }

    inline bool MMReadoutElement::stripGlobalPosition(const Identifier& id, Amg::Vector3D& gpos) const {
        Amg::Vector2D lpos(0., 0.);
        if (!stripPosition(id, lpos)) return false;
        surface(id).localToGlobal(lpos, Amg::Vector3D(0., 0., 0.), gpos);
        return true;
    }

    inline int MMReadoutElement::numberOfLayers(bool) const { return m_nlayers; }

    inline int MMReadoutElement::numberOfStrips(const Identifier& layerId) const { return m_nStrips[layerHash(layerId)]; }

    inline int MMReadoutElement::numberOfStrips(int lay, bool /*measPhi*/) const {
        if (lay > -1 && lay < (int)m_nStrips.size())
            return m_nStrips[lay];
        else
            return -1;
    }

    inline int MMReadoutElement::numberOfMissingTopStrips(const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;
        int nStrips = design->sAngle == 0 ? design->nMissedTopEta : design->nMissedTopStereo;
        return nStrips;
    }

    inline int MMReadoutElement::numberOfMissingBottomStrips(const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;
        int nStrips = design->sAngle == 0 ? design->nMissedBottomEta : design->nMissedBottomStereo;
        return nStrips;
    }

    inline bool MMReadoutElement::spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos) const {
        Amg::Vector2D phiPos;
        Amg::Vector2D etaPos;
        if (!stripPosition(phiId, phiPos) || !stripPosition(etaId, etaPos)) return false;
        spacePointPosition(phiPos, etaPos, pos);
        return true;
    }

    inline bool MMReadoutElement::spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const {
        Amg::Vector2D lpos;
        spacePointPosition(phiId, etaId, lpos);
        surface(phiId).localToGlobal(lpos, pos, pos);
        return true;
    }

    inline void MMReadoutElement::spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const {
        pos[0] = phiPos.x();
        pos[1] = etaPos.x();
    }

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_MMREADOUTELEMENT_H
