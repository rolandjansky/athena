// this file is -*- C++ -*-
/*
  Copyright (C) 2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackCaloClusterTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_ITRACKCALOCLUSTTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_ITRACKCALOCLUSTTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"

static const InterfaceID IID_ITrackCaloClusterTool("ITrackCaloClusterTool", 1, 0);


class TrackCaloClusterInfo;

/** @brief Interface for extending Particles with calo intersections */
class ITrackCaloClusterTool : virtual public IAlgTool {
public:
    
  /** Method to dress a IParticle with the calo layers crossed by its track
   *  Tracks are decorated using the calo layer index and the intersection
   *  @param IParticle     reference to the particle
   *  @param extension     reference to a pointer to a CaloExtesion, will be updated if call is successfull
   *                       NEVER delete the pointer, you will cause a crash! 
   *  @return true if the call was successful
   * 
   */
    
  
  virtual StatusCode fillTCC(xAOD::TrackCaloClusterContainer* container, const TrackCaloClusterInfo & wmap ) const = 0;
  
  static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& ITrackCaloClusterTool::interfaceID() {
  return IID_ITrackCaloClusterTool; 
}

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_IPARTICLEEXTRAPOLATIONTOOL_H
