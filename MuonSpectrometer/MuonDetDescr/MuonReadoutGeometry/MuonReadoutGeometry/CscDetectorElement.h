/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Collect CSC readout elements - granularity is same as for EDM (hash ids)
 -------------------------------------------------------
 ***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_CSCDETECTORELEMENT_H
#define MUONREADOUTGEOMETRY_CSCDETECTORELEMENT_H

#include <vector>

#include "MuonReadoutGeometry/MuonDetectorElement.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

class GeoVFullPhysVol;

namespace MuonGM {

    class CscReadoutElement;
    class MuonDetectorManager;
    class MuonStation;

    class CscDetectorElement final : public MuonDetectorElement {
    public:
        CscDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idhash);

        virtual int getStationEta() const override { return 0; };  //!< returns stationEta
        virtual int getStationPhi() const override { return 0; };  //!< returns stationPhi

        unsigned int nMDTinStation() const override { return 0; }
        unsigned int nCSCinStation() const override { return 1; }
        unsigned int nTGCinStation() const override { return 0; }
        unsigned int nRPCinStation() const override { return 0; }

        // access to the readout-elements in this DetectorElement
        const CscReadoutElement* readoutElement() const { return m_cscre; }

        void setReadoutElement(const CscReadoutElement* re) {
            m_cscre = re;
            ++m_nREinDetectorElement;
        }
        // access to the MuonStation this DetectorElement belongs to
        MuonStation* parentMuonStation() const;

        unsigned int NreadoutElements() const { return 1; }

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

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Csc; }

    private:
        const CscReadoutElement* m_cscre{nullptr};
    };

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_CSCDETECTORELEMENT_H
