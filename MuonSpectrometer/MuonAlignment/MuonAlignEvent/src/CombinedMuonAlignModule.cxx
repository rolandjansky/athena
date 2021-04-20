/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignEvent/CombinedMuonAlignModule.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

namespace Muon {

    //________________________________________________________________________
    CombinedMuonAlignModule::CombinedMuonAlignModule(MsgStream* log, const Amg::Transform3D& transform) :
        AlignModule(log, transform), m_log(new MsgStream(*log)) {}

    //________________________________________________________________________
    CombinedMuonAlignModule::CombinedMuonAlignModule(const AlgTool* algtool, const Amg::Transform3D& transform) :
        AlignModule(algtool, transform), m_log(new MsgStream(algtool->msgSvc(), "CombinedMuonAlignModule")) {
        const AthAlgTool* athAlgTool = dynamic_cast<const AthAlgTool*>(algtool);
        if (athAlgTool) m_log->setLevel(athAlgTool->msg().level());
    }

    //________________________________________________________________________
    CombinedMuonAlignModule::~CombinedMuonAlignModule() { delete m_log; }

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

        *m_log << MSG::ERROR << "det element not MDT, TGC, or RPC!" << endmsg;

        return;
    }

}  // namespace Muon
