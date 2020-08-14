/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONERROROPTIMISATIONTOOL_H
#define MUON_IMUONERROROPTIMISATIONTOOL_H
 
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonErrorOptimisationTool("Muon::IMuonErrorOptimisationTool",1,0);

namespace Trk {
  class Track;
}

namespace Muon {

  /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */  
  class IMuonErrorOptimisationTool : virtual public IAlgTool 
  {      
  public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    /** optimise errors on a track to maximize the momentum resolution  */
    virtual std::unique_ptr<Trk::Track> optimiseErrors(Trk::Track* track ) const = 0;

  };
  
  inline const InterfaceID& IMuonErrorOptimisationTool::interfaceID()
  {
    return IID_IMuonErrorOptimisationTool;
  }

} // end of name space

#endif 

