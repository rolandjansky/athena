/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXTOOLINTERFACES_MUON_IMSVERTEXRECOTOOL_H
#define MSVERTEXTOOLINTERFACES_MUON_IMSVERTEXRECOTOOL_H
 
#include "GaudiKernel/IAlgTool.h"
#include "MSVertexUtils/Tracklet.h"
#include "MSVertexUtils/MSVertex.h"
#include "GaudiKernel/EventContext.h"
#include <vector>
//                           
static const InterfaceID IID_IMSVertexRecoTool("Muon::IMSVertexRecoTool",1,0);

namespace Muon {

  /** @brief The IMSVertexRecoTool is a pure virtual interface */  
  class IMSVertexRecoTool : virtual public IAlgTool 
  {      
    public:

    /** access to tool interface */
    static const InterfaceID& interfaceID();

    virtual StatusCode findMSvertices(std::vector<Tracklet>& tracklets, std::vector<MSVertex*>& vertices, const EventContext &ctx) const = 0;

  };
  
  inline const InterfaceID& IMSVertexRecoTool::interfaceID()
  {
    return IID_IMSVertexRecoTool;
  }

} // end of name space

#endif 

