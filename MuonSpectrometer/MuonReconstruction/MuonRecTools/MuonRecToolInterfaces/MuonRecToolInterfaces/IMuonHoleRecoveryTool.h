/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONHOLERECOVERYTOOL_H
#define MUON_IMUONHOLERECOVERYTOOL_H
 
#include "GaudiKernel/IAlgTool.h"

#include <vector>

static const InterfaceID IID_IMuonHoleRecoveryTool("Muon::IMuonHoleRecoveryTool",1,0);

namespace Trk {
  class Track;
}

namespace Muon {

  /** @brief The IMuonHoleRecoveryTool is a pure virtual interface for tools that recover hole on a track */
  class IMuonHoleRecoveryTool : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();


    /** @brief recover missing hits (holes) on tracks
	@return a pointer to a track, the ownership of the output track is passed to the client calling the tool.
    */
    virtual Trk::Track* recover( const Trk::Track& track ) const = 0;

    virtual void cleanUp() const {};

  };
  
  inline const InterfaceID& IMuonHoleRecoveryTool::interfaceID()
  {
    return IID_IMuonHoleRecoveryTool;
  }

} // end of name space

#endif 

