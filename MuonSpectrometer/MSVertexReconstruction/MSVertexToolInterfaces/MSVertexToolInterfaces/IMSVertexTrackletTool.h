/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXTOOLINTERFACES_MUON_IMSVERTEXTRACKLETTOOL_H
#define MSVERTEXTOOLINTERFACES_MUON_IMSVERTEXTRACKLETTOOL_H
 
#include "GaudiKernel/IAlgTool.h"
#include "MSVertexUtils/Tracklet.h"
#include "GaudiKernel/EventContext.h"
#include <vector>
//                           
static const InterfaceID IID_IMSVertexTrackletTool("Muon::IMSVertexTrackletTool",1,0);

namespace Muon {

  /** @brief The IMSVertexTrackletTool is a pure virtual interface */  
  class IMSVertexTrackletTool : virtual public IAlgTool 
  {      
    public:

    /** access to tool interface */
    static const InterfaceID& interfaceID();
    
    virtual StatusCode findTracklets(std::vector<Tracklet>& traklets, const EventContext &ctx) const = 0;

  };
  
  inline const InterfaceID& IMSVertexTrackletTool::interfaceID()
  {
    return IID_IMSVertexTrackletTool;
  }

} // end of name space

#endif 

