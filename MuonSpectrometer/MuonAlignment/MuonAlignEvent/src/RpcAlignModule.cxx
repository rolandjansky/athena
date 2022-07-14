/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignEvent/RpcAlignModule.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

namespace Muon {
    //________________________________________________________________________
    RpcAlignModule::RpcAlignModule(const AlgTool* algtool, const Amg::Transform3D& transform) :
        CombinedMuonAlignModule(algtool, transform), m_log(new MsgStream(algtool->msgSvc(), "RpcAlignModule")) {
        const AthAlgTool* athAlgTool = dynamic_cast<const AthAlgTool*>(algtool);
        if (athAlgTool) { m_log->setLevel(athAlgTool->msg().level()); }
    }

    //________________________________________________________________________
    RpcAlignModule::~RpcAlignModule() { delete m_log; }

    //________________________________________________________________________
    // void RpcAlignModule::shiftSurface(Trk::TrkDetElementBase* det, Identifier id) const {
    //  (dynamic_cast<MuonGM::RpcReadoutElement*>(det))->shiftSurface(id);
    //}

    //________________________________________________________________________
    // void RpcAlignModule::restoreSurfaces(Trk::TrkDetElementBase* det) const {
    //  (dynamic_cast<MuonGM::RpcReadoutElement*>(det))->restoreSurfaces();
    // }

}  // namespace Muon
