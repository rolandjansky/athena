/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMTRACKISOLATIONTOOL_H
#define EGAMMAINTERFACES_IEMTRACKISOLATIONTOOL_H


/** @class IEMTrackIsolationTool
    Interface for the Reconstruction/egamma/egammaTools/EMTrackIsolationTool

    @author Jochen Hartert
    @author Christos Anastopoulos
*/

#include "GaudiKernel/IAlgTool.h"

// forward declarations
#include "xAODEgamma/EgammaFwd.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

static const InterfaceID IID_IEMTrackIsolationTool("IEMTrackIsolationTool", 1, 0);

class IEMTrackIsolationTool : virtual public IAlgTool
{

 public:

  /** @brief Destructor */
  virtual ~IEMTrackIsolationTool() {};

  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method */
  virtual StatusCode initialize() = 0;
  
  /** @brief execute method */
  virtual StatusCode execute(xAOD::Egamma* eg,const xAOD::TrackParticleContainer* aTrackParticleContainer = 0) = 0;
  
  /** @brief pt from tracker in a cone 0.2*/
  virtual double ptcone20()  const = 0; 
  /** @brief pt from tracker in a cone 0.3*/
  virtual double ptcone30()  const = 0; 
  /** @brief pt from tracker in a cone 0.4*/
  virtual double ptcone40()  const = 0; 
  /** @brief number of tracks in a cone 0.2*/
  virtual double nucone20()  const = 0; 
  /** @brief number of tracks in a cone 0.3*/
  virtual double nucone30()  const = 0; 
  /** @brief number of tracks in a cone 0.4*/
  virtual double nucone40()  const = 0; 

};


inline const InterfaceID& IEMTrackIsolationTool::interfaceID()
{
  return IID_IEMTrackIsolationTool;
}

#endif //EGAMMAINTERFACES_IEMTRACKISOLATIONTOOL_H





