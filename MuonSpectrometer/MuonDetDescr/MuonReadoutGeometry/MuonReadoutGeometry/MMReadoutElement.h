/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREADOUTGEOMETRY_MMREADOUTELEMENT_H
#define MUONREADOUTGEOMETRY_MMREADOUTELEMENT_H

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/ArrayHelper.h"
#include "MuonReadoutGeometry/NswPassivationDbData.h"

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
        MMReadoutElement(GeoVFullPhysVol* pv, const std::string& stName, int zi, int fi, int mL, bool is_mirrored, MuonDetectorManager* mgr, const NswPassivationDbData*);

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

        /** Wrapper to MuonChannelDesign::stereoAngle() */
        double stripAngle(const Identifier& id) const;

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

	/** space point position (global) for a given pair of stereo MM identifier */
	virtual bool spacePointPosition(const Identifier& id1, const Identifier& id2, Amg::Vector3D gpos_id1, Amg::Vector3D gpos_id2, Amg::Vector3D& gpos) const;

        /** space point position for a pair of phi and eta local positions and a layer identifier
            The LocalPosition is expressed in the reference frame of the phi projection.
        */
        void spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const;

        /** space point position to correct the position of 1D clusters.
            Accepts a precision (x) coordinate and a y-seed, in the local layer frame, and returns a 3D position, 
            in the same frame so that MMReadoutElement::transform() can be directly cast on it. Accounts for:
            a) the stereo angle: x += locYseed*tan(sAngle)
            b) PCB deformations (as-built), if as-built conditions are enabled
            c) Chamber deformations (b-lines), if b-lines are enabled
        */
        void spacePointPosition(const Identifier& layerId, double locXpos, double locYseed, Amg::Vector3D& pos) const;

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

        /** read A-line parameters and include the chamber rotation/translation 
            in the local-to-global (ATLAS) reference frame transformaton */
        void setDelta(const ALinePar& aline);
        void setDelta(MuonDetectorManager* mgr);

        /** read B-line (chamber-deformation) parameters */
        void setBLinePar(const BLinePar& bLine);
        void setBLinePar(MuonDetectorManager* mgr);

        /** transform a position (in chamber-frame coordinates) to the deformed-chamber geometry */
        void posOnDefChamber(Amg::Vector3D& locPosML) const;

        bool  has_ALines() const { return (m_ALinePar != nullptr); }
        bool  has_BLines() const { return (m_BLinePar != nullptr); }
        const ALinePar* getALinePar() const { return m_ALinePar; }
        const BLinePar* getBLinePar() const { return m_BLinePar; }
        void  clearALinePar();
        void  clearBLinePar() { m_BLinePar = nullptr; }

    private:
        using PCBPassivation = NswPassivationDbData::PCBPassivation;
        static constexpr PCBPassivation s_dummy_passiv{};
        // MuonChannelDesign m_phiDesign;
        std::vector<MuonChannelDesign> m_etaDesign;

        std::vector<int> m_nStrips;  // #of active strips
        int m_nlayers{0};            // #of gas gaps

        const NswPassivationDbData* m_passivData{nullptr};

        int m_ml{0};  // multilayer (values: 1,2)
        Identifier m_parentId;

        // surface dimensions
        double m_halfX{100.};       // 0.5*radial_size (active area)
        double m_minHalfY{1900.};   // 0.5*bottom length (active area)
        double m_maxHalfY{2000.};   // 0.5*top length (active area)
        double m_offset{0.};        // radial dist. of active area center w.r.t. chamber center

        double m_sWidthChamber{0.}; // bottom base length (full chamber)
        double m_lWidthChamber{0.}; // top base length (full chamber)
        double m_lengthChamber{0.}; // radial size (full chamber)
        double m_tckChamber{0.};    // thickness (full chamber)
        Amg::Transform3D m_delta{Amg::Transform3D::Identity()};
        const ALinePar*  m_ALinePar{nullptr};
        const BLinePar*  m_BLinePar{nullptr};

        // transforms (RE->layer)
        std::array<Amg::Transform3D, 4> m_Xlg{make_array<Amg::Transform3D,4>(Amg::Transform3D::Identity())};
    };

    inline void MMReadoutElement::clearALinePar() {
        if (has_ALines()) {
            m_ALinePar = nullptr; 
            m_delta = Amg::Transform3D::Identity(); 
            refreshCache();
        }
    }

    inline int MMReadoutElement::surfaceHash(const Identifier& id) const { return surfaceHash(manager()->mmIdHelper()->gasGap(id), 0); }

    inline int MMReadoutElement::surfaceHash(int gasGap, int /*measPhi*/) const { return gasGap - 1; } // measPhi not used

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
        // returns the position of the strip at pos, assuming the nominal geometry (no as-built conditions)
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;
        return design->channelNumber(pos);
    }

    inline bool MMReadoutElement::stripPosition(const Identifier& id, Amg::Vector2D& pos) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return false;
        return design->channelPosition(manager()->mmIdHelper()->channel(id), pos);
    }

    inline double MMReadoutElement::stripAngle(const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return 0;
        return design->sAngle;
    }

    inline double MMReadoutElement::stripLength(const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;
        return design->channelLength(manager()->mmIdHelper()->channel(id));
    }

    inline double MMReadoutElement::stripActiveLength(const Identifier& id) const {
        return stripActiveLengthLeft(id) + stripActiveLengthRight(id); 
    }

    inline double MMReadoutElement::stripActiveLengthLeft(const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;

        const PCBPassivation& passiv = m_passivData ? m_passivData->getPassivation(id) : s_dummy_passiv;
        // Let's keep it for the moment as we have to think about proper treatmeant of the non-passivated stuff
        // if (m_passivData && !passiv.valid) return -1;

        double l = design->channelHalfLength(manager()->mmIdHelper()->channel(id), true);
        if (l < 0) return -1;
        return std::max(0., l - passiv.left/std::cos(design->sideAngle - design->sAngle));
    }
        
    inline double MMReadoutElement::stripActiveLengthRight(const Identifier& id) const {
        const MuonChannelDesign* design = getDesign(id);
        if (!design) return -1;

        const PCBPassivation& passiv = m_passivData ? m_passivData->getPassivation(id) : s_dummy_passiv;
        // Let's keep it for the moment as we have to think about proper treatmeant of the non-passivated stuff
        // if (m_passivData && !passiv.valid) return -1;

        double l = design->channelHalfLength(manager()->mmIdHelper()->channel(id), false);
        if (l < 0) return -1;
        return std::max(0., l - passiv.right/std::cos(design->sideAngle + design->sAngle));
    }

    inline bool MMReadoutElement::insideActiveBounds(const Identifier& id, const Amg::Vector2D& locpos, double tol1, double tol2) const {
        const MuonChannelDesign* design = getDesign(id);
        if(!design) return false;
        // Get the nearest strip number; not the time yet to check boundaries (in case of tolerance) 
        int stripNo = stripNumber(locpos, id);
        if (stripNo < 0) stripNo = (locpos.x()<0) ? 1 : design->totalStrips;
        Identifier channelId = manager()->mmIdHelper()->channelID(id, m_ml, manager()->mmIdHelper()->gasGap(id), stripNo);

        // ** Horizontal passivation: mask entire strips
        //==============================================
        int pcb      = (stripNo-1)/1024 + 1; // starts from 1
        int pcbStrip = stripNo - 1024*(pcb - 1);
        PCBPassivation pcbPassiv = m_passivData ? m_passivData->getPassivation(channelId) : s_dummy_passiv;
        // if(m_passivData && !pcbPassiv.valid) return false;
        // the passivated width is constant along the PCB edge (not along y for stereo strips)
        if(pcb != 1) pcbPassiv.bottom /= std::cos(design->sAngle);
        bool topPcb{pcb == 5 || (std::abs(getStationEta()) == 2 && pcb == 3)};
        if(!topPcb) pcbPassiv.top     /= std::cos(design->sAngle);
        int pcbStripMin =    1 + (int)std::floor((pcbPassiv.bottom + 0.5*design->inputPitch - tol1)/design->inputPitch); // first pcb strip surviving passivation
        int pcbStripMax = 1024 - (int)std::floor((pcbPassiv.top    + 0.5*design->inputPitch - tol1)/design->inputPitch); //  last pcb strip surviving passivation
        if(pcbStrip < pcbStripMin || pcbStrip > pcbStripMax) return false;

        // ** Vertical passivation: cut strips from left and right
        //=======================================
        const double& passivW = (locpos[1]<0) ? pcbPassiv.left : pcbPassiv.right;
        return bounds(id).inside(locpos, tol1, tol2 - passivW/std::cos(design->sideAngle));
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

    inline bool MMReadoutElement::spacePointPosition( const Identifier& id1, const Identifier& id2, Amg::Vector3D gpos_id1, Amg::Vector3D gpos_id2, Amg::Vector3D& gpos ) const {
      
      int multilay1 = manager()->mmIdHelper()->multilayer(id1);
      int multilay2 = manager()->mmIdHelper()->multilayer(id2);
      int lay1 = manager()->mmIdHelper()->gasGap(id1);
      int lay2 = manager()->mmIdHelper()->gasGap(id2);

      //check to use only layer on the same multiplet, not the same layer and at least one stereo
      if ( (multilay1 != multilay2 || lay1 == lay2) || !( manager()->mmIdHelper()->isStereo(id1) || manager()->mmIdHelper()->isStereo(id2) ) ) return false;

      double angles = std::abs(getDesign(id1)->sAngle); //stereo angle
      double tan_angles = std::tan(angles);
      double phi = gpos_id1.phi(); //phi of the MM sector

      //parameters used to calculate the phi
      double A = ( gpos_id1.x() - gpos_id2.x() ) / ( gpos_id1.x() + gpos_id2.x() );
      double B = ( gpos_id1.z() - gpos_id2.z() ) / ( gpos_id1.z() + gpos_id2.z() );
      //tan(phi/2)
      double tan_phi2 = ( tan_angles - std::sqrt(tan_angles*tan_angles - B*B + A*A) ) / (A+B);

      //ad hoc change for the sector at PI/2
      if ( std::abs( std::abs(phi) - M_PI_2 ) < 0.01) {
	A = ( gpos_id1.y() - gpos_id2.y() ) / ( gpos_id1.y() + gpos_id2.y() );
	tan_phi2 = ( A - std::sqrt(A*A - B*B + tan_angles*tan_angles ) ) / (B-tan_angles);
      }

      double dphi = 0; //dphi = angle wrt the center of the sector 
      if (manager()->mmIdHelper()->stationEta(id1) > 0) dphi = -2*std::atan(tan_phi2);
      else dphi = 2*std::atan(tan_phi2);

      //ad hoc change for the sector at PI/2
      if ( std::abs( std::abs(phi) - M_PI_2 ) < 0.01) {
	if (dphi*phi < 0) dphi += phi;
	else dphi -= phi;
      }

      const double tan_phi_plus_dphi = std::tan(phi+dphi);
      const double tan_phi_minus_angles = std::tan(phi-angles);

      //global positions
      gpos[0] = (gpos_id1.y() + gpos_id1.x()/tan_phi_minus_angles)/(tan_phi_plus_dphi + 1./tan_phi_minus_angles);
      gpos[1] = gpos[0]*tan_phi_plus_dphi;
      gpos[2] = gpos_id1.z();

      //check if the stereo-superpoint is on the surface
      Amg::Vector2D lpos;
      const Trk::PlaneSurface& surf1 = surface(id1);
      surf1.globalToLocal(gpos,gpos,lpos);
      if( !surf1.insideBounds(lpos) ) return false;

      return true;
    }

    inline void MMReadoutElement::spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const {
        pos[0] = phiPos.x();
        pos[1] = etaPos.x();
    }
}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_MMREADOUTELEMENT_H
