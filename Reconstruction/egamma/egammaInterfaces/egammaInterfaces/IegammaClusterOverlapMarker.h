/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMACLUSTEROVERLAPMARKER_H
#define EGAMMAINTERFACES_IEGAMMACLUSTEROVERLAPMARKER_H

/** @class IEGAMMASUPERCLUSTERTRACKMATCH
  Interface for the tool Reconstruction/egamma/egammaTools/egammaSuperClusterTrackMatch

  @author David Di Valentino david.di.valentino@cern.ch

CREATED  : 21/12/2013
MODIFIED : 
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IAlgTool.h"
#include <vector>

//EGAMMA INCLUDES
#include "xAODCaloEvent/CaloClusterFwd.h"

static const InterfaceID IID_IegammaClusterOverlapMarker("IegammaClusterOverlapMarker", 1, 0);

class IegammaClusterOverlapMarker : virtual public IAlgTool
{

 public:

  /** @brief Virtual destructor*/
  virtual ~IegammaClusterOverlapMarker() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize()   = 0;
  //virtual StatusCode execute()    = 0;
  virtual StatusCode execute(const xAOD::CaloCluster*) = 0;

};

inline const InterfaceID& IegammaClusterOverlapMarker::interfaceID()
{
  return IID_IegammaClusterOverlapMarker;
}

#endif
