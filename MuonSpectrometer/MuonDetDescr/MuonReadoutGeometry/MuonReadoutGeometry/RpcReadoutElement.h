/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Rpc Readout Element properties
 -----------------------------------------
***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_RPCREADOUTELEMENT_H
#define MUONREADOUTGEOMETRY_RPCREADOUTELEMENT_H

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStripDesign.h"

class GeoVFullPhysVol;

namespace Muon {
    class CombinedMuonAlignModule;
    class RpcAlignModule;
}  // namespace Muon

namespace MuonGM {
    //<<<<<< PUBLIC DEFINES >>>>>>
    constexpr int maxphipanels = 2;
    constexpr int maxetapanels = 2;
    /**
       An RpcReadoutElement corresponds to a single RPC module; therefore
       typicaly a barrel muon station contains:
       - 4 RpcReadoutElements in the middle layer, two adjacent to each
       other (in z) on the inner side (low-pt plane) and two on the
       outer side (pivot plane);
       - 2 RpcReadoutElements in the outer layer (inner or outer side
       depends on the Large or Small sector type).
       RpcReadoutElements are identified by StationName, StationEta, StationPhi,
       Technology=2, doubletR, doubletZ and doubletPhi (the latter being
       actually needed only in a few special cases). Therefore the granularity
       of the data-collections is less fine-grained than the granularity of the
       geometry description (1 collection -> 2 RpcReadoutElements, typically).

       Pointers to all RpcReadoutElements are created in the build() method of
       the MuonChamber class, and are held in arrays by the MuonDetectorManager,
       which is responsible for storing, deleting and providing access to these
       objects.

       An RpcReadoutElement holds properties related to its internal structure
       (i.e. number of strip panels) and general geometrical properties (size);
       it implements tracking interfaces and provide access to typical
       readout-geometry information: i.e. number of strips, strip positions, etc.

       The globalToLocalCoords and globalToLocalTransform methods (+ their
       opposite) define the link between the ATLAS global reference frame and
       the internal (geo-model defined) local reference frame of any gas gap
       volume (which is the frame where local coordinates of SimHits, in output
       from G4, are expressed).
    */

    class RpcReadoutSet;

    class RpcReadoutElement final : public MuonClusterReadoutElement {
        friend class Muon::RpcAlignModule;
        friend class Muon::CombinedMuonAlignModule;
        friend class MuonChamber;

    public:
        /** constructor */
        RpcReadoutElement(GeoVFullPhysVol* pv, const std::string& stName, int zi, int fi, bool is_mirrored, MuonDetectorManager* mgr);

        /** destructor */
        ~RpcReadoutElement();

        int getDoubletR() const;  //!< return DoubletR value for the given readout element
        int getDoubletZ() const;  //!< return DoubletZ value for the given readout element
        int getDoubletPhi()
            const;  //!< return DoubletPhi value for the given readout element, be aware that one RE can contain two DoubletPhis!!!!
        bool hasDEDontop() const;  //!< return whether the RPC is 'up-side-down'

        /** function to be used to check whether a given Identifier is contained in the readout element */
        virtual bool containsId(const Identifier& id) const override;

        /** returns whether the RE is in the ribs of the muon spectrometer */
        bool inTheRibs() const;

        int Nphigasgaps() const;                    //!< returns the number of phi gas gaps
        int NphiStripPanels() const;                //!< returns the number of phi strip panels (1 or 2)
        int NetaStripPanels() const;                //!< returns the number of eta strip panels (should always be 1)
        int NphiStrips() const;                     //!< returns the number of phi strips
        int NetaStrips() const;                     //!< returns the number of eta strips
        int Nstrips(int measphi) const;             //!< returns the number of strips for the phi or eta plane
        double StripWidth(int measphi) const;       //!< returns the strip width for the phi or eta plane
        double StripLength(int measphi) const;      //!< returns the strip length for the phi or eta plane
        double StripPitch(int measphi) const;       //!< returns the strip pitch for the phi or eta plane
        double StripPanelDead(int measphi) const;   //!< returns strip panel dead area for the phi or eta plane
        double stripPanelSsize(int measphi) const;  //!< returns strip panel S size for the phi or eta plane
        double stripPanelZsize(int measphi) const;  //!< returns strip panel Z size for the phi or eta plane
        double gasGapSsize() const;                 //!< returns the gas gap S size
        double gasGapZsize() const;                 //!< returns the gas gap Z size

        /** distance to readout.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual double distanceToReadout(const Amg::Vector2D& pos, const Identifier& id) const override final;

        /** strip number corresponding to local position.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual int stripNumber(const Amg::Vector2D& pos, const Identifier& id) const override final;

        /** strip position
            If the strip number is outside the range of valid strips, the function will return false */
        virtual bool stripPosition(const Identifier& id, Amg::Vector2D& pos) const override final;

        /** number of layers in phi/eta projection, same for eta/phi planes */
        virtual int numberOfLayers(bool measPhi = true) const override final;
        void setNumberOfLayers(const int = 2);

        /** number of strips per layer */
        virtual int numberOfStrips(const Identifier& layerId) const override final;
        virtual int numberOfStrips(int, bool measuresPhi) const override final;

        /** space point position for a given pair of phi and eta identifiers
            The LocalPosition is expressed in the reference frame of the phi surface.
            If one of the identifiers is outside the valid range, the function will return false */
        virtual bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos) const override final;

        /** Global space point position for a given pair of phi and eta identifiers
            If one of the identifiers is outside the valid range, the function will
           return false */
        virtual bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const override final;

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Rpc; }

        /** space point position for a pair of phi and eta local positions and a layer identifier
            The LocalPosition is expressed in the reference frame of the phi projection.
        */
        void spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const;

        /** @brief center of an RPC RE is not trivially the center of the first surface, overloading MuonClusterReadoutElement */
        const Amg::Vector3D REcenter() const;

        /** @brief function to fill tracking cache */
        virtual void fillCache() override final;
        virtual void refreshCache() override final {
            clearCache();
            fillCache();
        }

        /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
        virtual int surfaceHash(const Identifier& id) const override final;

        /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
        int surfaceHash(int dbPhi, int gasGap, int measPhi) const;

        /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
        virtual int layerHash(const Identifier& id) const override final;
        /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
        int layerHash(int dbPhi, int gasGap) const;

        /** returns the hash function to be used to look up the surface boundary for a given identifier */
        virtual int boundaryHash(const Identifier& id) const override final;

        /** @brief returns whether the current identifier corresponds to a phi measurement */
        virtual bool measuresPhi(const Identifier& id) const override final;

        /** @brief initialize the design classes for this readout element */
        void initDesign();

        /** set methods only to be used by MuonGeoModel */
        void setIdentifier(const Identifier& id);
        void setDoubletR(int);
        void setDoubletZ(int);
        void setDoubletPhi(int);
        void set_nphigasgaps(int);
        void set_nphistrippanels(int);
        void set_netastrippanels(int);
        void set_nphistripsperpanel(int);
        void set_netastripsperpanel(int);
        void set_phistripwidth(double);
        void set_etastripwidth(double);
        void set_phistriplength(double);
        void set_etastriplength(double);
        void set_first_phistrip_s(double*);
        void set_first_etastrip_z(double*);

        /** local MuonGeoModel to global transforms and positions, only to be used by digitization */
        Amg::Vector3D stripPanelPos(const Identifier& id) const;
        Amg::Vector3D stripPanelPos(const IdentifierHash& id) const;
        Amg::Vector3D stripPanelPos(int doubletR, int doubletZ, int DoubletPhi, int gasGap, int measPhi) const;
        Amg::Vector3D localStripPanelPos(const Identifier& id) const;
        Amg::Vector3D localStripPanelPos(const IdentifierHash& id) const;
        Amg::Vector3D localStripPanelPos(int doubletR, int doubletZ, int DoubletPhi, int gasGap, int measPhi) const;
        // local to global
        Amg::Vector3D SDtoModuleCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Vector3D localToGlobalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D localToGlobalTransf(const Identifier& id) const;
        Amg::Transform3D localToGlobalTransf(int dbZ, int dbPhi, int gasGap) const;
        // global to local
        Amg::Vector3D globalToLocalCoords(const Amg::Vector3D& x, Identifier id) const;
        Amg::Transform3D globalToLocalTransf(const Identifier& id) const;

        Amg::Vector3D stripPos(const Identifier& id) const;
        Amg::Vector3D stripPos(const IdentifierHash& id) const;
        Amg::Vector3D stripPos(int doubletR, int doubletZ, int DoubletPhi, int gasGap, int measPhi, int strip) const;
        Amg::Vector3D localStripPos(const Identifier& id) const;
        Amg::Vector3D localStripPos(const IdentifierHash& id) const;
        Amg::Vector3D localStripPos(int doubletR, int doubletZ, int DoubletPhi, int gasGap, int measPhi, int strip) const;
        double localStripSCoord(int doubletZ, int doubletPhi, int measphi, int strip) const;
        double localStripZCoord(int doubletZ, int doubletPhi, int measphi, int strip) const;
        double localGasGapDepth(int gasGap) const;
        Amg::Vector3D localGasGapPos(const Identifier& id) const;
        Amg::Vector3D localGasGapPos(int dbZ, int dbP, int gg) const;
        Amg::Vector3D gasGapPosconst(Identifier& id) const;
        Amg::Vector3D gasGapPos(int dbZ, int dbP, int gg) const;
        Amg::Vector3D gasGapPos(const Identifier& id) const;

        bool rotatedRpcModule() const;
        bool localTopGasGap(const Identifier& id) const;
        bool localTopGasGap(int gasGap) const;
        bool rotatedGasGap(const Identifier& id) const;
        bool rotatedGasGap(int gasGap) const;

        // Readout side infos
        // P is a point in the global reference frame
        // we want to have the distance from the side of the phi readout (length travelled along a phi strip) from a signal produced at P)
        double distanceToPhiReadout(const Amg::Vector3D& P) const;
        // P is a point in the global reference frame
        // we want to have the distance from the side of the eta readout (length travelled along a eta strip) from a signal produced at P)
        double distanceToEtaReadout(const Amg::Vector3D& P) const;
        // for backward compatibility of the interfaces keep the following 2 methods
        double distanceToPhiReadout(const Amg::Vector3D& x, const Identifier& id) const;
        double distanceToEtaReadout(const Amg::Vector3D& x, const Identifier& id) const;

        void setYTranslation(const float y);
        void setZTranslation(const float z);

    private:
        /** returns the MuonStripDesign class for the given identifier */
        const MuonStripDesign* getDesign(const Identifier& id) const;

        int m_dbR{0};
        int m_dbZ{0};
        int m_dbPhi{0};
        bool m_hasDEDontop{false};
        int m_nlayers{2};  // default=2, all BI RPCs always have 3 gas gaps, need this flag since amdb only supports a maximum of 2 gas
                           // gaps, so this is steering the hardcoded third gas gap for Run3/4 layouts based on amdb primary numbers

        int m_nphigasgaps{-1};
        int m_netagasgaps{-1};
        double m_gasgapssize{-9999.};
        double m_gasgapzsize{-9999.};

        int m_nphistrippanels{-1};
        int m_netastrippanels{-1};
        int m_nphistripsperpanel{-1};
        int m_netastripsperpanel{-1};
        double m_phistripwidth{-9999.};
        double m_etastripwidth{-9999.};
        double m_phistrippitch{-9999.};
        double m_etastrippitch{-9999.};
        double m_phistriplength{-9999.};
        double m_etastriplength{-9999.};
        double m_phipaneldead{-9999.};
        double m_etapaneldead{-9999.};
        double m_exthonthick{-9999.};
        std::array<double, maxphipanels> m_first_phistrip_s{-9999.};
        std::array<double, maxphipanels> m_first_etastrip_z{-9999.};
        std::array<double, maxphipanels> m_etastrip_s{-9999.};
        std::array<double, maxetapanels> m_phistrip_z{-9999.};

        Amg::Transform3D m_Xlg[3][2];

        Amg::Transform3D localToGlobalStripPanelTransf(int dbZ, int dbPhi, int gasGap) const;
        Amg::Vector3D localStripPanelPos(int dbZ, int dbP, int gg) const;

        std::vector<MuonStripDesign> m_phiDesigns;
        std::vector<MuonStripDesign> m_etaDesigns;
        std::unique_ptr<RpcReadoutSet> m_set;

        float m_y_translation{0.f};
        float m_z_translation{0.f};
    };

    inline int RpcReadoutElement::getDoubletR() const { return m_dbR; }
    inline int RpcReadoutElement::getDoubletZ() const { return m_dbZ; }
    inline int RpcReadoutElement::getDoubletPhi() const { return m_dbPhi; }
    inline void RpcReadoutElement::setDoubletR(int dbr) { m_dbR = dbr; }
    inline void RpcReadoutElement::setDoubletZ(int dbz) { m_dbZ = dbz; }
    inline void RpcReadoutElement::setDoubletPhi(int dbp) { m_dbPhi = dbp; }
    inline bool RpcReadoutElement::hasDEDontop() const { return m_hasDEDontop; }

    inline int RpcReadoutElement::Nphigasgaps() const { return m_nphigasgaps; }
    inline int RpcReadoutElement::NphiStripPanels() const { return m_nphistrippanels; }
    inline int RpcReadoutElement::NetaStripPanels() const { return m_netastrippanels; }
    inline int RpcReadoutElement::NphiStrips() const { return m_nphistripsperpanel; }
    inline int RpcReadoutElement::NetaStrips() const { return m_netastripsperpanel; }

    inline int RpcReadoutElement::Nstrips(int measphi) const { return measphi == 0 ? m_netastripsperpanel : m_nphistripsperpanel; }
    inline double RpcReadoutElement::StripWidth(int measphi) const { return measphi == 1 ? m_phistripwidth : m_etastripwidth; }
    inline double RpcReadoutElement::StripLength(int measphi) const { return measphi == 1 ? m_phistriplength : m_etastriplength; }
    inline double RpcReadoutElement::StripPitch(int measphi) const { return measphi == 1 ? m_phistrippitch : m_etastrippitch; }
    inline double RpcReadoutElement::StripPanelDead(int measphi) const { return measphi == 1 ? m_phipaneldead : m_etapaneldead; }
    inline double RpcReadoutElement::stripPanelSsize(int measphi) const {
        if (measphi == 1)
            return Nstrips(measphi) * StripPitch(measphi) - (StripPitch(measphi) - StripWidth(measphi));
        else
            return StripLength(measphi);
    }

    inline double RpcReadoutElement::stripPanelZsize(int measphi) const {
        if (measphi == 0)
            return Nstrips(measphi) * StripPitch(measphi) - (StripPitch(measphi) - StripWidth(measphi));
        else
            return StripLength(measphi);
    }

    inline double RpcReadoutElement::gasGapSsize() const { return m_gasgapssize; }

    inline double RpcReadoutElement::gasGapZsize() const { return m_gasgapzsize; }

    inline int RpcReadoutElement::surfaceHash(const Identifier& id) const {
        return surfaceHash(manager()->rpcIdHelper()->doubletPhi(id), manager()->rpcIdHelper()->gasGap(id),
                           manager()->rpcIdHelper()->measuresPhi(id));
    }

    inline const Amg::Vector3D RpcReadoutElement::REcenter() const {
        if (NphiStripPanels() == 1) return MuonClusterReadoutElement::center(0);
        return 0.5 * (MuonClusterReadoutElement::center(0) + MuonClusterReadoutElement::center(2));
    }

    inline int RpcReadoutElement::layerHash(const Identifier& id) const {
        return layerHash(manager()->rpcIdHelper()->doubletPhi(id), manager()->rpcIdHelper()->gasGap(id));
    }

    inline int RpcReadoutElement::boundaryHash(const Identifier& id) const { return (measuresPhi(id) ? 0 : 1); }

    inline bool RpcReadoutElement::measuresPhi(const Identifier& id) const { return manager()->rpcIdHelper()->measuresPhi(id); }

    inline double RpcReadoutElement::distanceToReadout(const Amg::Vector2D& pos, const Identifier& id) const {
        const MuonStripDesign* design = getDesign(id);
        if (!design) return 0;
        return design->distanceToReadout(pos);
    }

    inline int RpcReadoutElement::stripNumber(const Amg::Vector2D& pos, const Identifier& id) const {
        const MuonStripDesign* design = getDesign(id);
        if (!design) return 1;
        return design->stripNumber(pos);
    }

    inline bool RpcReadoutElement::stripPosition(const Identifier& id, Amg::Vector2D& pos) const {
        const MuonStripDesign* design = getDesign(id);
        if (!design) return 0;
        return design->stripPosition(manager()->rpcIdHelper()->strip(id), pos);
    }

    inline int RpcReadoutElement::numberOfLayers(bool) const { return m_nlayers; }
    inline void RpcReadoutElement::setNumberOfLayers(const int nlay) { m_nlayers = nlay; }

    inline int RpcReadoutElement::numberOfStrips(const Identifier& layerId) const {
        return numberOfStrips(1, manager()->rpcIdHelper()->measuresPhi(layerId));
    }
    inline int RpcReadoutElement::numberOfStrips(int, bool measuresPhi) const { return Nstrips(measuresPhi); }

    inline bool RpcReadoutElement::spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos) const {
        Amg::Vector2D phiPos;
        Amg::Vector2D etaPos;
        if (!stripPosition(phiId, phiPos) || !stripPosition(etaId, etaPos)) return false;
        spacePointPosition(phiPos, etaPos, pos);
        return true;
    }

    inline bool RpcReadoutElement::spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const {
        Amg::Vector2D lpos;
        spacePointPosition(phiId, etaId, lpos);
        surface(phiId).localToGlobal(lpos, pos, pos);
        return true;
    }

    inline void RpcReadoutElement::spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const {
        pos[0] = phiPos.x();
        pos[1] = etaPos.x();
    }
    inline void RpcReadoutElement::setYTranslation(const float y) { m_y_translation = y; }
    inline void RpcReadoutElement::setZTranslation(const float z) { m_z_translation = z; }

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_RPCREADOUTELEMENT_H
