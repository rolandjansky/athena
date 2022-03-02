/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Csc Readout Element properties
 -----------------------------------------
***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_CSCREADOUTELEMENT_H
#define MUONREADOUTGEOMETRY_CSCREADOUTELEMENT_H

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

namespace Muon {
    class CscAlignModule;
    class CombinedMuonAlignModule;
}  // namespace Muon

class CscInternalAlignmentPar;
class GeoVFullPhysVol;

namespace MuonGM {

    //<<<<<< PUBLIC DEFINES >>>>>>
    constexpr int maxwlay = 4;

    /*
      A CscReadoutElement corresponds to a single CSC chamber; therefore typically
      (in the final MS layout, i.e. if no detector stagein is considered) a CSC
      station contains 2 CscReadoutElements.
      CscReadoutElements are identified by StationName, StationEta, StationPhi,
      Technology=1 and ChamberLayer. Therefore the granularity of the data
      collections is not equal to the granularity of the geometry description
      (1 collection -> 2 CscReadoutElement [for final layout]; ->1 CscReadoutElement
      in the initial layout).

      Pointers to all CscReadoutElements are created in the build() method of the
      MuonChamber class, and are held in arrays by the MuonDetectorManager, which
      is responsible for storing, deleting and provide access to these objects.

      A CscReadoutElement holds properties related to its internal structure
      (i.e. number of strip panels) and general geometrical properties (size);
      it implements tracking interfaces and provides access to typical
      readout-geometry information: i.e. number of strips, strip  positions, etc.

      The globalToLocalCoords and globalToLocalTransform methods (+ their opposite)
      define the link between the ATLAS global reference frame and the internal
      (geo-model defined) local reference frame of any gas gap volume (which is the
      frame where local coordinates of SimHits, in output from G4, are expressed).
    */

    class CscReadoutElement : public MuonClusterReadoutElement {
        friend class Muon::CscAlignModule;
        friend class Muon::CombinedMuonAlignModule;
        friend class MuonChamber;

    public:
        CscReadoutElement(GeoVFullPhysVol* pv, const std::string& stName, int zi, int fi, bool is_mirrored, MuonDetectorManager* mgr);

        virtual ~CscReadoutElement();

        /** distance to readout.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual double distanceToReadout(const Amg::Vector2D& pos, const Identifier& id) const override final;

        /** strip number corresponding to local position.
            If the local position is outside the active volume, the function first shift the position back into the active volume */
        virtual int stripNumber(const Amg::Vector2D& pos, const Identifier& id) const override final;

        /** strip position
            If the strip number is outside the range of valid strips, the function will return false */
        virtual bool stripPosition(const Identifier& id, Amg::Vector2D& pos) const override final;

        /** returns the hash function to be used to look up the center and the normal of the tracking surface for a given identifier */
        virtual int layerHash(const Identifier& id) const override final;

        /** returns the hash function to be used to look up the surface and surface transform for a given identifier */
        virtual int surfaceHash(const Identifier& id) const override final;

        /** returns the hash function to be used to look up the surface boundary for a given identifier */
        virtual int boundaryHash(const Identifier& id) const override final;

        /** returns whether the given identifier measures phi or not */
        virtual bool measuresPhi(const Identifier& id) const override final;

        /** number of layers in phi/eta projection */
        virtual int numberOfLayers(bool) const override final;

        /** number of strips per layer */
        virtual int numberOfStrips(const Identifier& layerId) const override final;
        virtual int numberOfStrips(int layer, bool) const override final;

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Csc; }

        /** space point position for a given pair of phi and eta identifiers
            The LocalPosition is expressed in the reference frame of the phi projection.
            If one of the identifiers is outside the valid range, the function will return false */
        virtual bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos) const override;

        /** Global space point position for a given pair of phi and eta identifiers
            If one of the identifiers is outside the valid range, the function will return false */
        virtual bool spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const override;

        /** space point position for a pair of phi and eta local positions and a layer identifier
            The LocalPosition is expressed in the reference frame of the phi projection.
        */
        void spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const;

        // Id set/get methods
        int ChamberLayer() const;
        void setChamberLayer(int cl);

        int Ngasgaps() const;
        int NphiStripLayers() const;
        int NetaStripLayers() const;
        int NphiStrips(int gasgaplayer) const;
        int NetaStrips(int gasgaplayer) const;
        // in the following methods the chlayer argument is irrelevant
        // the first 3 will disappear
        double StripWidth(int chlayer, int measphi) const;
        double StripPitch(int chlayer, int measphi) const;
        double cathodeReadoutPitch(int chLayer, int measuresPhi) const;
        double StripWidth(int measphi) const;
        double StripPitch(int measphi) const;
        double cathodeReadoutPitch(int measuresPhi) const;

        double shortWidth() const;
        double length() const;
        double longWidth() const;
        int maxNumberOfStrips(int measuresPhi) const;
        int maxNumberOfStrips(int measuresPhi, double width) const;
        double activeWidth(int measuresPhi) const;
        double z0() const;
        inline double roxacellWidth() const;
        inline double lengthUpToMaxWidth() const;
        inline double excent() const;

        Amg::Vector3D stripLayerPos(const Identifier& id) const;
        Amg::Vector3D stripLayerPos(const IdentifierHash& id) const;
        Amg::Vector3D stripLayerPos(int chamberLayer, int wireLayer, int measPhi, int channel) const;
        Amg::Vector3D localStripLayerPos(const Identifier& id) const;
        Amg::Vector3D localStripLayerPos(const IdentifierHash& id) const;
        Amg::Vector3D localStripLayerPos(int chamberLayer, int wireLayer, int measPhi, int channel) const;
        //
        Amg::Vector3D localWireLayerPos(const Identifier& id) const;
        Amg::Vector3D localWireLayerPos(int gg) const;
        Amg::Vector3D wireLayerPos(const Identifier& id) const;
        Amg::Vector3D wireLayerPos(int gg) const;

        //
        Amg::Vector3D nominalStripPos(const Identifier& id) const;
        /**< ignores internal alignment parameters, hence gives generally incorrect answer */
        Amg::Vector3D nominalStripPos(int eta, int chamberLayer, int wireLayer, int measPhi, int channel) const;
        /**< ignores internal alignment parameters, hence gives generally incorrect answer */
        Amg::Vector3D stripPos(const Identifier& id) const;
        /**< takes into account internal alignment parameters, hence gives accurate answer */
        Amg::Vector3D stripPos(int eta, int chamberLayer, int wireLayer, int measPhi, int channel) const;
        /**< takes into account internal alignment parameters, hence gives accurate answer */
        Amg::Vector3D nominalGlobalPos(const Amg::Vector3D& localP) const;
        /**<  ignores internal alignment parameters, hence gives generally incorrect answer */
        Amg::Vector3D globalPos(const Amg::Vector3D& localP) const;
        /**<  station-level method: does not depend on the strip view/layer, hence it cannot account
                                                   for internal alignment parameters */
        Amg::Vector3D localPos(const Amg::Vector3D& globalP) const;
        /**<  station-level method: does not depend on the strip view/layer, hence it cannot
                account for internal alignment parameters  */
        Amg::Vector3D nominalLocalStripPos(const Identifier& id) const;
        /**< ignores internal alignment parameters, hence gives generally incorrect answer (local here is the
                                     station frame, coherent with the gas gas frames) */

        Amg::Vector3D nominalLocalStripPos(int eta, int chamberLayer, int wireLayer, int measPhi, int channel) const;
        /**< ignores internal alignment parameters, hence gives generally incorrect answer (local here is the station frame,
                  coherent with the gas gas frames) */
        Amg::Vector3D nominalLocalClusterPos(int eta, int wireLayer, int measPhi, double x0) const;
        /**< ignores internal alignment parameters, hence gives generally incorrect answer (local here is the station frame,
                      coherent with the gas gas frames) */
        Amg::Vector3D localStripPos(const Identifier& id) const;
        /**< takes into account internal alignment parameters, hence gives generally accurate answer (local here
             is the station frame, coherent with the gas gas frames) - digitization should use this method to
             emulate internal alignment parameters */
        Amg::Vector3D localStripPos(int eta, int chamberLayer, int wireLayer, int measPhi, int channel) const;
        /**< takes into account internal alignment parameters, hence gives generally accurate answer (local here is the station
                  frame, coherent with the gas gas frames) - digitization should use this method to emulate internal alignment
                  parameters */
        Amg::Vector3D localClusterPos(int eta, int wireLayer, int measPhi, double x0) const;
        /**< takes into account internal alignment parameters, hence gives generally  answer (local here is the station frame,
                      coherent with the gas gas frames) */
        Amg::Vector3D stripPosOnTrackingSurface(const Identifier& id) const;
        /**< nominal strip pos in the tracking local frame of the measurement surface*/
        Amg::Vector3D stripPosOnTrackingSurface(int eta, int chamberLayer, int wireLayer, int measPhi, int channel) const;
        /**< nominal strip pos in the tracking local frame of the measurement surface*/
        Amg::Transform3D nominalTransform(const Identifier& id) const;
        /**< like tracking Transform but nominal - returns a transform not a reference to it */
        Amg::Transform3D nominalTransform(int gasGap, int measPhi) const;
        /**< like tracking Transform but nominal - returns a transform not a reference to it */
        Amg::Vector3D nominalCenter(int gasGap) const;
        /**< like tracking center but nominal - returns a Amg::Vector3D not a reference to it */
        Amg::Vector3D originForInternalALines(int gasGap) const;
        /**< like tracking center but nominal - returns a Amg::Vector3D not a reference to it */

        double stripLength(int chamberLayer, int measuresPhi, int stripNumber, double& epsilon) const;
        double stripLength(const Identifier& id) const;
        double lengthCorrection(int measuresPhi, double stripPos) const;

        double anodeCathodeDistance() const;

        /*!
          localToGlobalCoords and Transf connect the Gas Gap Frame (defined as a Sensitive Detector) to the Global Frame;
          Notice that internal CSC alignment affects the strip plane orientation and position in the CSC station frame, but
          does not impact on the gas gap geometry within the station; therefore the following methods do not depen on internal alignment
          parameters. Otherwise stated, the internal alignment parameters in MC can be emulated at the digitization stage.
          @param[out] localToGlobalCoords the coordinates in the Global Atlas Frame of point at x in the local frame
          @param[in]  id CSC gas gap identifier - view and strip field must be 0
          @param[in]  x  local coordinates of the point in the gas gap = sensitive volume
        */
        // local to global and viceversa
        Amg::Vector3D localToGlobalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
        Amg::Transform3D localToGlobalTransf(const Identifier& id) const;
        //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
        Amg::Transform3D localToGlobalTransf(int gasGap) const;
        //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */

        // global to local
        Amg::Vector3D globalToLocalCoords(const Amg::Vector3D& x, const Identifier& id) const;
        //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */
        Amg::Transform3D globalToLocalTransf(const Identifier& id) const;
        //**< localToGlobalCoords and Transf relates gas-gap frame (SensitiveDetectors) to the Global Frame  */

        // modifiers
        void setIdentifier(const Identifier& id);
        void set_ngasgaps(int);
        void set_nstriplayers(int);
        void set_nwirelayers(int);
        void set_nstripsperlayer(int);
        void set_nwiresperlayer(int);
        void set_stripwidth(double);
        void set_strippitch(double);
        void set_wirepitch(double);
        void set_first_strip_localcoo(double);
        void set_first_wire_localcoo(double);
        void setCscInternalAlignmentPar(const CscInternalAlignmentPar&);
        void setCscInternalAlignmentParams();
        //    // internal alignment

        const CscInternalAlignmentPar getCscInternalAlignmentPar(int gasGap) const;
        double getGasGapIntAlign_s(int gasGap) const;
        double getGasGapIntAlign_z(int gasGap) const;
        double getGasGapIntAlign_t(int gasGap) const;
        double getGasGapIntAlign_rots(int gasGap) const;
        double getGasGapIntAlign_rotz(int gasGap) const;
        double getGasGapIntAlign_rott(int gasGap) const;

        virtual bool containsId(const Identifier& id) const override;

        // compute sin(stereo angle) at a given position:
        double sinStereo(const Identifier& stripId) const;

        virtual void fillCache() override;
        virtual void refreshCache() override {
            clearCache();
            fillCache();
        }
        double xCoordinateInTrackingFrame(const Identifier& id) const;

    private:
        void doTests();

    private:
        double m_excent{-9999.};
        double m_roxacellwidth{-9999.};
        double m_RlengthUpToMaxWidth{-9999.};
        double m_anodecathode_distance{-9999.};
        int m_chamberlayer{-1};
        int m_ngasgaps{-1};
        int m_nstriplayers{-1};
        int m_nwirelayers{-1};
        int m_nPhistripsperlayer{-1};
        int m_nEtastripsperlayer{-1};
        int m_nwiresperlayer{-1};
        double m_Phistripwidth{-9999.};
        double m_Phistrippitch{-9999.};
        double m_Etastripwidth{-9999.};
        double m_Etastrippitch{-9999.};
        double m_wirepitch{-9999.};
        double m_first_strip_localcoo{-9999.};
        double m_first_wire_localcoo{-9999.};
        std::array<double, maxwlay> m_wireplanez{-9999.};

        // internal alignment
        double m_cscIntTransl[4][3]{};  // first index is wireLayer, second = 0,1,2 for    s,z,t
        double m_cscIntRot[4][3]{};     // first index is wireLayer, second = 0,1,2 for rots,z,t
    };

    inline int CscReadoutElement::ChamberLayer() const { return m_chamberlayer; }

    inline void CscReadoutElement::setChamberLayer(int cl) { m_chamberlayer = cl; }

    inline double CscReadoutElement::shortWidth() const { return getSsize(); }

    inline double CscReadoutElement::length() const { return getRsize(); }

    inline double CscReadoutElement::longWidth() const { return getLongSsize(); }

    inline double CscReadoutElement::roxacellWidth() const { return m_roxacellwidth; }

    inline double CscReadoutElement::lengthUpToMaxWidth() const { return m_RlengthUpToMaxWidth; }

    inline double CscReadoutElement::excent() const { return m_excent; }

    inline double CscReadoutElement::anodeCathodeDistance() const { return m_anodecathode_distance; }

    inline int CscReadoutElement::Ngasgaps() const { return m_ngasgaps; }

    inline int CscReadoutElement::NphiStripLayers() const { return m_ngasgaps; }

    inline int CscReadoutElement::NetaStripLayers() const { return m_ngasgaps; }

    inline int CscReadoutElement::NphiStrips(int) const { return m_nPhistripsperlayer; }

    inline int CscReadoutElement::NetaStrips(int) const { return m_nEtastripsperlayer; }

    inline int CscReadoutElement::layerHash(const Identifier& id) const { return manager()->cscIdHelper()->wireLayer(id) - 1; }

    inline int CscReadoutElement::surfaceHash(const Identifier& id) const {
        return 2 * (manager()->cscIdHelper()->wireLayer(id) - 1) + (manager()->cscIdHelper()->measuresPhi(id) ? 0 : 1);
    }

    inline int CscReadoutElement::boundaryHash(const Identifier& id) const { return (measuresPhi(id) ? 0 : 1); }

    inline bool CscReadoutElement::measuresPhi(const Identifier& id) const { return manager()->cscIdHelper()->measuresPhi(id); }

    inline int CscReadoutElement::numberOfLayers(bool) const { return Ngasgaps(); }
    inline int CscReadoutElement::numberOfStrips(const Identifier&) const { return NphiStrips(1); }
    inline int CscReadoutElement::numberOfStrips(int, bool) const { return NphiStrips(1); }

    inline bool CscReadoutElement::spacePointPosition(const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos) const {
        Amg::Vector2D lpos;
        spacePointPosition(phiId, etaId, lpos);
        surface(phiId).localToGlobal(lpos, pos, pos);
        return true;
    }

    inline void CscReadoutElement::spacePointPosition(const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos) const {
        pos[0] = phiPos.x();
        pos[1] = etaPos.x();
    }

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_CSCREADOUTELEMENT_H
