// this file is -*- C++ -*-
/*
  Copyright (C) 2019-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackCaloClusterTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_ITRACKCALOCLUSTTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_ITRACKCALOCLUSTTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include "xAODPFlow/FlowElementContainer.h"

static const InterfaceID IID_ITrackCaloClusterTool("ITrackCaloClusterTool", 1, 0);


struct TrackCaloClusterInfo;

/** @brief Interface for tools creating particles using TCC methods and filling a FlowElementContainer */
class ITrackCaloClusterTool : virtual public IAlgTool {
public:
    
    
  
  virtual StatusCode fillTCC(xAOD::FlowElementContainer* container, const TrackCaloClusterInfo & wmap ) const = 0;
  
  static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& ITrackCaloClusterTool::interfaceID() {
  return IID_ITrackCaloClusterTool; 
}

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_IPARTICLEEXTRAPOLATIONTOOL_H
