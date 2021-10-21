/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Common Mdt Detector Element properties
 -----------------------------------------
 ***************************************************************************/

//!  A MdtDetectorElement Class
/*!
  MdtDetectorElements are as granular as offline mdt data collections
  (prepRawData); hence they can be retrieved using the collection hash
  identifier.  MdtDetectorElement will hold an array of MdtReadoutElements
  which will actually implement the real functionality; MdtDetectorElement
  methods will delegate the job to the appropriate MdtReadoutElement.
*/

#ifndef MUONREADOUTGEOMETRY_MDTDETECTORELEMENT_H
#define MUONREADOUTGEOMETRY_MDTDETECTORELEMENT_H

#include "MuonReadoutGeometry/MuonDetectorElement.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

class GeoVFullPhysVol;

namespace MuonGM {

    constexpr int maxMdtREinDE = 2;

    class MuonDetectorManager;
    class MdtReadoutElement;

    /**
       Base class for the XxxDetectorelement, with Xxx = Mdt, Rpc, Tgc, Csc.
       It's a Trk::TrkDetElementBase, therefore it must implement the generic
       tracking interfaces requested to the geometry: center, normal,
       surfaceBound, transform.
       No link to raw geometry.
    */

    class MdtDetectorElement final : public MuonDetectorElement {
    public:
        MdtDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idHash);
        virtual ~MdtDetectorElement(){};

        virtual int getStationEta() const override { return 0; };  //!< returns stationEta
        virtual int getStationPhi() const override { return 0; };  //!< returns stationPhi

        //  DetectorElement content
        double getRsize() const;      //<! size of the DetectorElement (collection of readout elements)
        double getZsize() const;      //<! size of the DetectorElement (collection of readout elements)
        double getLongSsize() const;  //<! size of the DetectorElement (collection of readout elements)
        double getLongRsize() const;  //<! size of the DetectorElement (collection of readout elements)
        double getLongZsize() const;  //<! size of the DetectorElement (collection of readout elements)

        // Common tracking generic interfaces
        virtual const Amg::Transform3D& transform() const override final;

        virtual const Trk::Surface& surface() const override final;

        virtual const Trk::SurfaceBounds& bounds() const override final;

        virtual const Amg::Vector3D& center() const override final;

        virtual const Amg::Vector3D& normal() const override final;

        virtual const Amg::Vector3D& normal(const Identifier& id) const override final;

        virtual const Trk::Surface& surface(const Identifier& id) const override final;

        virtual const Trk::SurfaceBounds& bounds(const Identifier& id) const override final;

        virtual const Amg::Transform3D& transform(const Identifier& id) const override final;

        virtual const Amg::Vector3D& center(const Identifier& id) const override final;

        std::vector<const Trk::Surface*> surfaces() const;

        virtual unsigned int nMDTinStation() const override { return nReadoutElements(); }
        virtual unsigned int nCSCinStation() const override { return 0; }
        virtual unsigned int nTGCinStation() const override { return 0; }
        virtual unsigned int nRPCinStation() const override { return 0; }

        // Add a XxxReadoutElement to the Collection
        void addMdtReadoutElement(const MdtReadoutElement* x, Identifier id);
        //!< store the MdtReadoutElement using as "key" the identifier

        void addMdtReadoutElement(const MdtReadoutElement* x, int multilayer);
        //!< store the MdtReadoutElement using as "key" the multilayer

        // access to Readout Elements
        const MdtReadoutElement* getMdtReadoutElement(Identifier) const;
        //!< access via extended identifier (requires unpacking)

        const MdtReadoutElement* getMdtReadoutElement(int multilayer) const;
        //!< access via multilayer index

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Mdt; }

    private:
        std::array<const MdtReadoutElement*, maxMdtREinDE> m_mdtRE{nullptr};
        unsigned int m_nRE{0};
    };

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_MDTDETECTORELEMENT_H
