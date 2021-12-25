/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignEvent/CscAlignModule.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

namespace Muon {

    //________________________________________________________________________
    CscAlignModule::CscAlignModule(MsgStream* log, const Amg::Transform3D& transform) :
        CombinedMuonAlignModule(log, transform), m_rootfile(nullptr), m_tubePositionTree(nullptr), m_log(new MsgStream(*log)) {}

    //________________________________________________________________________
    CscAlignModule::CscAlignModule(const AlgTool* algtool, const Amg::Transform3D& transform) :
        CombinedMuonAlignModule(algtool, transform),
        m_rootfile(nullptr),
        m_tubePositionTree(nullptr),
        m_log(new MsgStream(algtool->msgSvc(), "CscAlignModule")) {
        const AthAlgTool* athAlgTool = dynamic_cast<const AthAlgTool*>(algtool);
        if (athAlgTool) m_log->setLevel(athAlgTool->msg().level());
    }

    //________________________________________________________________________
    CscAlignModule::~CscAlignModule() { delete m_log; }

    //________________________________________________________________________
    // void CscAlignModule::shiftSurface(Trk::TrkDetElementBase* det, Identifier tubeId) const {
    //(dynamic_cast<MuonGM::MdtReadoutElement*>(det))->shiftTube(tubeId);
    //}

    //________________________________________________________________________
    // void CscAlignModule::restoreSurfaces(Trk::TrkDetElementBase* det) const {
    //(dynamic_cast<MuonGM::MdtReadoutElement*>(det))->restoreTubes();
    //}

}  // namespace Muon
