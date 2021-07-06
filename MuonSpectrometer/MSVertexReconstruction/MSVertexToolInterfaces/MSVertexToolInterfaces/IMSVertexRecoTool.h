/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXTOOLINTERFACES_MUON_IMSVERTEXRECOTOOL_H
#define MSVERTEXTOOLINTERFACES_MUON_IMSVERTEXRECOTOOL_H

#include <vector>

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "MSVertexUtils/MSVertex.h"
#include "MSVertexUtils/Tracklet.h"
//
static const InterfaceID IID_IMSVertexRecoTool("Muon::IMSVertexRecoTool", 1, 0);

namespace Muon {

    /** @brief The IMSVertexRecoTool is a pure virtual interface */
    class IMSVertexRecoTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID();

        virtual StatusCode findMSvertices(std::vector<Tracklet>& tracklets, std::vector<MSVertex*>& vertices,
                                          const EventContext& ctx) const = 0;
    };

    inline const InterfaceID& IMSVertexRecoTool::interfaceID() { return IID_IMSVertexRecoTool; }

}  // namespace Muon

#endif
