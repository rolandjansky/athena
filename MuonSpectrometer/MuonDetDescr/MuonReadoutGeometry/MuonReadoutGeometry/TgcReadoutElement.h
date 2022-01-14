/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREADOUTGEOMETRY_TGCREADOUTELEMENT_H
#define MUONREADOUTGEOMETRY_TGCREADOUTELEMENT_H

#include <string>

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

class GeoVFullPhysVol;

namespace Muon {
    class TgcAlignModule;
    class CombinedMuonAlignModule;
}  // namespace Muon

namespace MuonGM {
    //<<<<<< PUBLIC DEFINES >>>>>>

    constexpr int maxwpl = 3;
    constexpr int maxstrp = 3;

    /**
       A TgcReadoutElement corresponds to a single TGC chamber; therefore typically
       a TGC station contains several TgcReadoutElements.  TgcReadoutElements are
       identified by StationName, StationEta, StationPhi, Technology=3. Therefore
       the granularity of the data-collections is equal to the granularity of the
       geometry description (1 collection -> 1 TgcReadoutElement).

       Pointers to all TgcReadoutElements are created in the build() method of the
       MuonChamber class, and are held in arrays by the MuonDetectorManager, which
       is responsible for storing, deleting and provide access to these objects.

       A TgcReadoutElement holds properties related to its internal structure
       (i.e. number of strip panels) and general geometrical properties (size);
       it implements tracking interfaces and provide access to typical
       readout-geometry information: i.e. number of strips, strip and wire-gang
       positions, etc.

       The globalToLocalCoords and globalToLocalTransform methods (+ their
       opposite) define the link between the ATLAS global reference frame and the
       internal (geo-model defined) local reference frame of any gas gap volume
       (which is the frame where local coordinates of SimHits, in output from G4,
       are expressed).
    */

    class TgcReadoutElement final : public MuonClusterReadoutElement {
        friend class Muon::TgcAlignModule;
        friend class Muon::CombinedMuonAlignModule;
        friend class MuonChamber;

    public:
        TgcReadoutElement(GeoVFullPhysVol* pv, const std::string& stName, int zi, int fi, bool is_mirrored, MuonDetectorManager* mgr);

        virtual ~TgcReadoutElement();

        /** distance to readout.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual inline double distanceToReadout(const Amg::Vector2D& pos, const Identifier& id) const override;

        /** strip number corresponding to local position.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual int stripNumber(const Amg::Vector2D& pos, const Identifier& id) const override final;

        /** strip position
            If the strip number is outside the range of valid strips, the function will return false */
        virtual bool stripPosition(const Identifier& id, Amg::Vector2D& pos) const override;

        /** returns the hash function to be used to look up the center and the normal of the tracking surface for a given identifier */
        virtual inline int layerHash(const Identifier& id) const override;

        /** returns the hash function to be used to look up the surface and surface transform for a given identifier */
        virtual inline int surfaceHash(const Identifier& id) const override;

        /** returns the hash function to be used to look up the surface boundary for a given identifier */
        virtual inline int boundaryHash(const Identifier& id) const override;

        /** returns whether the given identifier measures phi or not */
        virtual inline bool measuresPhi(const Identifier& id) const override;

        /** number of layers in phi/eta projection */
        virtual inline int numberOfLayers(bool) const override;

        /** number of strips per layer */
        virtual inline int numberOfStrips(const Identifier& layerId) const override;
        virtual inline int numberOfStrips(int layer, bool) const override;

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Tgc; }

        /** space point position for a given pair of phi and eta identifiers
            The LocalPosition is expressed in the reference frame of the phi projection.
            If one of the identifiers is outside the valid range, the function will return false */
        virtual inline bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos) const override;

        /** Global space point position for a given pair of phi and eta identifiers
            If one of the identifiers is outside the valid range, the function will return false */
        virtual inline bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const override;

        void setIdentifier(const Identifier& id);
        virtual bool containsId(const Identifier& id) const override;
        inline int Ngasgaps() const;
        inline int NstripPlanes() const;
        inline int NwirePlanes() const;
        inline int Nstrips(int plane) const;
        inline int Nwires(int plane) const;
        inline int NwireGangs(int plane) const;
        double StripWidth(int plane) const;
        double stripWidth(int plane, int strip) const;
        inline double StripLength(int plane) const;
        double StripPitch(int plane) const;
        double stripPitch(int plane, int strip) const;
        inline double StripOffset(int plane) const;
        inline double WireLength(int plane, int wire) const;
        inline double WirePitch(int plane) const;
        inline double WireOffset(int plane) const;

        // strip pitch
        double StripPitch(int plane, int strip, float zlocal) const;

        // local to global
        Amg::Vector3D localToGlobalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D localToGlobalTransf(const Identifier& id) const;
        Amg::Transform3D localToGlobalTransf(int gasGap) const;
        // global to local
        Amg::Vector3D globalToLocalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        Amg::Transform3D globalToLocalTransf(const Identifier& id) const;
        //
        Amg::Vector3D channelPos(int gasGap, int isStrip, int channel) const;
        Amg::Vector3D channelPos(const Identifier& id) const;
        Amg::Vector3D localChannelPos(int gasGap, int isStrip, int channel) const;
        Amg::Vector3D localChannelPos(const Identifier& id) const;

        Amg::Vector3D gangPos(int gasGap, int gang) const;
        Amg::Vector3D gangPos(const Identifier& id) const;
        Amg::Vector3D gangPos(const IdentifierHash& id) const;
        Amg::Vector3D localGangPos(int gasGap, int gang) const;
        Amg::Vector3D localGangPos(const Identifier& id) const;
        Amg::Vector3D localGangPos(const IdentifierHash& id) const;

        Amg::Vector3D stripPos(int gasGap, int strip) const;
        Amg::Vector3D stripPos(const Identifier& id) const;
        Amg::Vector3D stripPos(const IdentifierHash& id) const;
        Amg::Vector3D localStripPos(int gasGap, int strip) const;
        Amg::Vector3D localStripPos(const Identifier& id) const;
        Amg::Vector3D localStripPos(const IdentifierHash& id) const;

        double localStripSCoord(int gasgap, int strip) const;
        double localWireRCoord(int gasgap, int wire) const;
        double localWireGangRCoord(int gasgap, int wiregang) const;

        Amg::Vector3D localGasGapPos(const Identifier& id) const;
        Amg::Vector3D localGasGapPos(int gg) const;
        Amg::Vector3D gasGapPos(const Identifier& id) const;
        Amg::Vector3D gasGapPos(int gg) const;

        std::string stationType() const;
        int chamberType() const;
        bool endcap() const;
        bool forward() const;
        bool doublet() const;
        bool triplet() const;
        float shortWidth() const;
        float longWidth() const;
        float length() const;
        float thickness() const;
        float frameZwidth() const;
        float frameXwidth() const;
        float chamberWidth(float z) const;
        float chamberMinPhi() const;
        float chamberMaxPhi() const;
        float wirePitch() const;
        int nPhiChambers() const;
        int nPhiSectors() const;
        int nGaps() const;
        int nVolumes() const;
        //     std::string volumeType(int) const;
        //     float volumeYpos(int) const;
        //     float volumeThickness(int) const;
        int nGangs(int) const;
        int nWires(int, int) const;
        int nWiresTotal(int) const;
        int gangOffset(int) const;
        float wireCoverage(int) const;
        float gangCtrZ(int, int) const;
        float gangMinZ(int, int) const;
        float gangMaxZ(int, int) const;
        float gangLength(int, int) const;
        float gangShortWidth(int, int) const;
        float gangLongWidth(int, int) const;
        float gangThickness(int, int) const;
        int nStrips(int) const;
        float stripDeltaPhi(int) const;
        float stripDeltaPhi(int, int) const;
        float stripStaggerPhi(int) const;
        float stripMinPhi(int, int) const;
        float stripMaxPhi(int, int) const;
        float stripCtrPhi(int, int) const;
        float stripCtrX(int, int, float) const;
        float stripMinX(int, int, float) const;
        float stripMaxX(int, int, float) const;
        float stripLength(int, int) const;
        float stripShortWidth(int, int) const;
        float stripLongWidth(int, int) const;
        float stripSkew(int, int) const;
        int findChannel(int, int, const Amg::Vector3D&) const;
        int findGang(int, const Amg::Vector3D&) const;
        int findStrip(int, const Amg::Vector3D&, const Amg::Vector3D&) const;
        int gapVolume(int) const;
        bool isAgap(const std::string&) const;
        bool validGap(int) const;
        bool validGang(int, int) const;
        bool validStrip(int, int) const;
        void print() const;

        inline void set_ngasgaps(int);
        inline void set_nstripplanes(int);
        inline void set_nwireplanes(int);
        inline void set_nstripsperpanel(int, int);
        inline void set_nwiresperpanel(int, int);
        inline void set_nwiregangsperpanel(int, int);
        inline void set_stripwidth(double, int);
        inline void set_strippitch(double, int);
        inline void set_wirepitch(double, int);
        inline void set_stripoffset(double x, int ip);
        inline void set_wireoffset(double x, int ip);
        inline void set_stripplanez(double x, int ip);
        inline void set_wireplanez(double x, int ip);

        double sinStereo(const Identifier& id) const;

        // Access to readout parameters
        inline int getReadoutType() const;
        inline const std::string getReadoutName() const;
        inline int getReadoutVersion() const;
        inline int getNPhiChambers() const;
        inline int getNGaps() const;
        inline float getWirePitch() const;
        inline float getGangThickness() const;
        inline int getNGangs(int gasGap) const;
        inline int getTotalWires(int gasGap) const;
        inline int getNWires(int gasGap, int gang) const;
        inline int getGangOffset(int gasGap) const;
        inline float getStripThickness() const;
        inline int getNStrips(int gasGap) const;
        inline float getStripOffset(int gasGap) const;

        inline const std::string getGeometryVersion() const;
        inline float getPhysicalDistanceFromBase() const;
        inline float getStripPositionOnLargeBase(int strip) const;
        inline float getStripPositionOnShortBase(int strip) const;

        inline const TgcReadoutParams* getReadoutParams() const;

        virtual void fillCache() override;
        virtual void refreshCache() override {
            clearCache();
            fillCache();
        }

    private:
        int m_ngasgaps{0};
        int m_nstripplanes{0};
        int m_nwireplanes{0};
        std::array<int, maxstrp> m_nstrips_per_plane{0};
        std::array<double, maxstrp> m_strippitch{-9999.};
        std::array<double, maxstrp> m_stripwidth{-9999.};
        std::array<double, maxstrp> m_wirepitch{-9999.};

        std::array<double, maxstrp> m_stripoffset{-9999.};
        std::array<double, maxstrp> m_stripplanez{-9999.};

        std::array<int, maxwpl> m_nwires_per_plane{0};
        std::array<int, maxwpl> m_nwiregangs_per_plane{0};

        std::array<double, maxwpl> m_wireoffset{};
        std::array<double, maxwpl> m_wireplanez{};

        int m_readout_type{-1};
        std::string m_readout_name;
        const TgcReadoutParams* m_readoutParams{nullptr};
    };

    int TgcReadoutElement::Ngasgaps() const { return m_ngasgaps; }
    int TgcReadoutElement::NstripPlanes() const { return m_nstripplanes; }
    int TgcReadoutElement::NwirePlanes() const { return m_nwireplanes; }
    int TgcReadoutElement::Nstrips(int plane) const { return m_nstrips_per_plane[plane - 1]; }
    int TgcReadoutElement::Nwires(int plane) const { return m_nwires_per_plane[plane - 1]; }
    int TgcReadoutElement::NwireGangs(int plane) const { return m_nwiregangs_per_plane[plane - 1]; }
    double TgcReadoutElement::StripLength(int /*plane*/) const { return m_Rsize; }
    double TgcReadoutElement::StripOffset(int plane) const { return m_stripoffset[plane - 1]; }

    double TgcReadoutElement::WireLength(int plane, int wire) const {
        double pitch = m_wirepitch[plane - 1];
        return m_Ssize + (wire - 1) * pitch * (m_LongSsize - m_Ssize) / m_Rsize;
    }
    double TgcReadoutElement::WirePitch(int plane) const { return m_wirepitch[plane - 1]; }
    double TgcReadoutElement::WireOffset(int plane) const { return m_wireoffset[plane - 1]; }
    void TgcReadoutElement::set_ngasgaps(int ix) { m_ngasgaps = ix; }
    void TgcReadoutElement::set_nstripplanes(int ix) { m_nstripplanes = ix; }
    void TgcReadoutElement::set_nwireplanes(int ix) { m_nwireplanes = ix; }
    void TgcReadoutElement::set_nstripsperpanel(int ix, int ip) { m_nstrips_per_plane[ip - 1] = ix; }
    void TgcReadoutElement::set_nwiresperpanel(int ix, int ip) { m_nwires_per_plane[ip - 1] = ix; }
    void TgcReadoutElement::set_nwiregangsperpanel(int ix, int ip) { m_nwiregangs_per_plane[ip - 1] = ix; }
    void TgcReadoutElement::set_stripwidth(double x, int ip) { m_stripwidth[ip - 1] = x; }
    void TgcReadoutElement::set_strippitch(double x, int ip) { m_strippitch[ip - 1] = x; }
    void TgcReadoutElement::set_wirepitch(double x, int ip) { m_wirepitch[ip - 1] = x; }
    void TgcReadoutElement::set_stripoffset(double x, int ip) { m_stripoffset[ip - 1] = x; }
    void TgcReadoutElement::set_wireoffset(double x, int ip) { m_wireoffset[ip - 1] = x; }
    void TgcReadoutElement::set_stripplanez(double x, int ip) { m_stripplanez[ip - 1] = x; }
    void TgcReadoutElement::set_wireplanez(double x, int ip) { m_wireplanez[ip - 1] = x; }

    // readout Parameters go though TgcReadoutParams
    int TgcReadoutElement::getReadoutType() const { return m_readout_type; }
    const std::string TgcReadoutElement::getReadoutName() const { return m_readoutParams->GetName(); }
    int TgcReadoutElement::getReadoutVersion() const { return m_readoutParams->readoutVersion(); }
    int TgcReadoutElement::getNPhiChambers() const { return m_readoutParams->nPhiChambers(); }
    int TgcReadoutElement::getNGaps() const { return m_readoutParams->nGaps(); }
    float TgcReadoutElement::getWirePitch() const { return m_readoutParams->wirePitch(); }
    float TgcReadoutElement::getGangThickness() const { return m_readoutParams->gangThickness(); }
    int TgcReadoutElement::getNGangs(int gasGap) const { return m_readoutParams->nGangs(gasGap); }
    int TgcReadoutElement::getTotalWires(int gasGap) const { return m_readoutParams->totalWires(gasGap); }
    int TgcReadoutElement::getNWires(int gasGap, int gang) const { return m_readoutParams->nWires(gasGap, gang); }
    int TgcReadoutElement::getGangOffset(int gasGap) const { return m_readoutParams->gangOffset(gasGap); }
    float TgcReadoutElement::getStripThickness() const { return m_readoutParams->stripThickness(); }
    int TgcReadoutElement::getNStrips(int gasGap) const { return m_readoutParams->nStrips(gasGap); }
    float TgcReadoutElement::getStripOffset(int gasGap) const { return m_readoutParams->stripOffset(gasGap); }

    const std::string TgcReadoutElement::getGeometryVersion() const { return manager()->geometryVersion(); }

    float TgcReadoutElement::getPhysicalDistanceFromBase() const { return m_readoutParams->physicalDistanceFromBase(); }
    float TgcReadoutElement::getStripPositionOnLargeBase(int strip) const { return m_readoutParams->stripPositionOnLargeBase(strip); }
    float TgcReadoutElement::getStripPositionOnShortBase(int strip) const { return m_readoutParams->stripPositionOnShortBase(strip); }

    const TgcReadoutParams* TgcReadoutElement::getReadoutParams() const { return m_readoutParams; }

    int TgcReadoutElement::layerHash(const Identifier& id) const { return manager()->tgcIdHelper()->gasGap(id) - 1; }

    int TgcReadoutElement::surfaceHash(const Identifier& id) const {
        return 2 * (manager()->tgcIdHelper()->gasGap(id) - 1) + (manager()->tgcIdHelper()->isStrip(id) ? 0 : 1);
    }

    int TgcReadoutElement::boundaryHash(const Identifier& id) const { return (measuresPhi(id) ? 0 : 1); }

    bool TgcReadoutElement::measuresPhi(const Identifier& id) const { return manager()->tgcIdHelper()->isStrip(id); }

    inline int TgcReadoutElement::numberOfLayers(bool measuresPhi) const { return measuresPhi ? NstripPlanes() : NwirePlanes(); }

    inline int TgcReadoutElement::numberOfStrips(const Identifier& id) const {
        return numberOfStrips(manager()->tgcIdHelper()->gasGap(id), manager()->tgcIdHelper()->isStrip(id));
    }
    inline int TgcReadoutElement::numberOfStrips(int layer, bool measuresPhi) const {
        return measuresPhi ? Nstrips(layer) : NwireGangs(layer);
    }

    inline bool TgcReadoutElement::spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const {
        // get orientation angle of strip to rotate back from local frame to strip
        int stripNo = manager()->tgcIdHelper()->channel(phiId);
        int gasGap = manager()->tgcIdHelper()->gasGap(phiId);

        // calculate local position of endpoint of strip
        Amg::Vector3D lEtapos = localChannelPos(etaId);
        double localEtaY = stripCtrX(gasGap, stripNo, lEtapos.z());
        if (0 < getStationEta()) { localEtaY *= -1.; }
        lEtapos[1] = localEtaY;

        // transform to global
        pos = absTransform() * lEtapos;

        return true;
    }

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_TGCREADOUTELEMENT_H
