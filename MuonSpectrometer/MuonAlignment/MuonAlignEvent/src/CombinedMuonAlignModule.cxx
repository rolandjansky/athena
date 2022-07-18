/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignEvent/CombinedMuonAlignModule.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

namespace Muon {

    //________________________________________________________________________
    CombinedMuonAlignModule::CombinedMuonAlignModule(const AlgTool* algtool, const Amg::Transform3D& transform) :
        AlignModule(algtool, transform) {
        const AthAlgTool* athAlgTool = dynamic_cast<const AthAlgTool*>(algtool);
        if (athAlgTool) setLevel(athAlgTool->msg().level());
    }

    //________________________________________________________________________
    CombinedMuonAlignModule::~CombinedMuonAlignModule() { }

    //________________________________________________________________________
    void CombinedMuonAlignModule::shiftSurface(Trk::TrkDetElementBase* det, Identifier id) const {
        MuonGM::MdtReadoutElement* mdt = dynamic_cast<MuonGM::MdtReadoutElement*>(det);
        if (mdt) {
            mdt->shiftTube(id);
            return;
        }

        MuonGM::TgcReadoutElement* tgc = dynamic_cast<MuonGM::TgcReadoutElement*>(det);
        if (tgc) {
            tgc->shiftSurface(id);
            return;
        }

        MuonGM::RpcReadoutElement* rpc = dynamic_cast<MuonGM::RpcReadoutElement*>(det);
        if (rpc) {
            rpc->shiftSurface(id);
            return;
        }

        MuonGM::CscReadoutElement* csc = dynamic_cast<MuonGM::CscReadoutElement*>(det);
        if (csc) {
            csc->shiftSurface(id);
            return;
        }

        return;
    }

    //________________________________________________________________________
    void CombinedMuonAlignModule::restoreSurfaces(Trk::TrkDetElementBase* det) const {
        MuonGM::MdtReadoutElement* mdt = dynamic_cast<MuonGM::MdtReadoutElement*>(det);
        if (mdt) {
            mdt->restoreTubes();
            return;
        }

        MuonGM::TgcReadoutElement* tgc = dynamic_cast<MuonGM::TgcReadoutElement*>(det);
        if (tgc) {
            tgc->restoreSurfaces();
            return;
        }

        MuonGM::RpcReadoutElement* rpc = dynamic_cast<MuonGM::RpcReadoutElement*>(det);
        if (rpc) {
            rpc->restoreSurfaces();
            return;
        }

        MuonGM::CscReadoutElement* csc = dynamic_cast<MuonGM::CscReadoutElement*>(det);
        if (csc) {
            csc->restoreSurfaces();
            return;
        }

        ATH_MSG_ERROR("det element not MDT, TGC, or RPC!");

        return;
    }

}  // namespace Muon
