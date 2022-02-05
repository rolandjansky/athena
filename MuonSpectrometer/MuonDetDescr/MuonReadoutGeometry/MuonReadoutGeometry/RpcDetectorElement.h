/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Collect RPC readout elements belonging to the same DoubletR - granularity is same as for EDM (hash ids)
 -------------------------------------------------------
***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_RPCDETECTORELEMENT_H
#define MUONREADOUTGEOMETRY_RPCDETECTORELEMENT_H

#include "MuonReadoutGeometry/MuonDetectorElement.h"

namespace Trk {
    class Surface;
    class SurfaceBounds;
}  // namespace Trk

class GeoVFullPhysVol;
class RpcIdHelper;

namespace MuonGM {

    class MuonDetectorManager;
    class MuonStation;
    class RpcReadoutElement;

    typedef std::vector<const RpcReadoutElement*> REVector;
    typedef std::vector<const RpcReadoutElement*>::const_iterator REIterator;

    class RpcDetectorElement final : public MuonDetectorElement {
    public:
        RpcDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idHash);

        virtual int getStationEta() const override { return 0; };  //!< returns stationEta
        virtual int getStationPhi() const override { return 0; };  //!< returns stationPhi

        // access to the readout-elements in this DetectorElement
        const RpcReadoutElement* getRpcReadoutElement(Identifier id) const;
        // this is a channelId

        const RpcReadoutElement* getRpcReadoutElement(int dbz, int dbp) const;
        // access to the MuonStation this DetectorElement belongs to
        MuonStation* parentMuonStation() const;

        unsigned int NdoubletZ() const;
        unsigned int NsegmentedDoubletZ() const;
        unsigned int NPhimodules(int dbz) const;
        void addRpcReadoutElement(const RpcReadoutElement* rpc, int index);

        virtual unsigned int nMDTinStation() const override { return 0; }
        virtual unsigned int nCSCinStation() const override { return 0; }
        virtual unsigned int nTGCinStation() const override { return 0; }
        virtual unsigned int nRPCinStation() const override { return nReadoutElements(); }

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

        enum RpcGMRanges { NDoubletZ = 4 };
        // using some trick to save space: dbz=4 if rib's chambers and doubletphi=2;

        /** TrkDetElementInterface */
        virtual Trk::DetectorElemType detectorType() const override final { return Trk::DetectorElemType::Rpc; }

    private:
        const RpcIdHelper* m_helper;
        int m_ndbz;
        const RpcReadoutElement* m_rpcVector[NDoubletZ]{};
    };

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_RPCDETECTORELEMENT_H
