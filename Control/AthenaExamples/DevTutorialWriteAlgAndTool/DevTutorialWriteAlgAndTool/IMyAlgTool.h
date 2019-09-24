/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MYNEWPACKAGE_IMYALGTOOL_H
#define MYNEWPACKAGE_IMYALGTOOL_H
#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackParticle.h"

static const InterfaceID IID_IMyAlgTool("IMyAlgTool", 1, 0);

/** @class IMyAlgTool
    @brief Interface to demonstrate how to make an interface which actually does something useful.
*/

class IMyAlgTool : virtual public IAlgTool {
  public:
  static const InterfaceID& interfaceID( );
  virtual bool selectTrackParticle( const xAOD::TrackParticle& particle) const = 0 ;
};

inline const InterfaceID& IMyAlgTool::interfaceID()
{ 
	return IID_IMyAlgTool; 
}

#endif 

