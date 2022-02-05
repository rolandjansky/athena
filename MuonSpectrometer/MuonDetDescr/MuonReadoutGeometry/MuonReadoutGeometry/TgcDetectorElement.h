/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Collect TGC readout elements - granularity is same as for EDM (hash ids)
 -------------------------------------------------------
 ***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_TGCDETECTORELEMENT_H
#define MUONREADOUTGEOMETRY_TGCDETECTORELEMENT_H

#include "MuonReadoutGeometry/MuonDetectorElement.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

class GeoVFullPhysVol;

namespace MuonGM {

    class MuonDetectorManager;
    class TgcReadoutElement;

    class TgcDetectorElement final : public MuonDetectorElement {
    public:
        TgcDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idhash);

        virtual int getStationEta() const override { return 0; };  //!< returns stationEta
        virtual int getStationPhi() const override { return 0; };  //!< returns stationPhi

        virtual unsigned int nMDTinStation() const override { return 0; }
        virtual unsigned int nCSCinStation() const override { return 0; }
        virtual unsigned int nTGCinStation() const override { return 1; }
        virtual unsigned int nRPCinStation() const override { return 0; }

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

        // access to the readout-elements in this DetectorElement
        const TgcReadoutElement* readoutElement() const { return m_tgcre; }

        void setReadoutElement(const TgcReadoutElement* re) { m_tgcre = re; }

        unsigned int NreadoutElements() const { return 1; }
        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Tgc; }

    private:
        const TgcReadoutElement* m_tgcre{nullptr};
    };

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_TGCDETECTORELEMENT_H
