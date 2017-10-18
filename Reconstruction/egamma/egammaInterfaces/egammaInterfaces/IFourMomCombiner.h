/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IFOURMOMCOMBINER_H
#define EGAMMAINTERFACES_IFOURMOMCOMBINER_H

/** @class IFOURMOMCOMBINER
  Interface for the tool Reconstruction/egamma/egammaTools/FourMomCombiner
  @author David Di Valentino david.di.valentino@cern.ch

CREATED  : 25/04/2013
MODIFIED : 
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h" 

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

// Forward declarations.
class egamma;
//class EMClusterErrorsParametrizations;

namespace Trk {
  class MeasuredPerigee;
  class MeasuredNeutralPerigee;
}

namespace CombinationTypes {
  enum ComboTypes {COVMAT,KALMAN,LIKELIHOOD};
}

static const InterfaceID IID_IFourMomCombiner("IFourMomCombiner", 1, 0);

class IFourMomCombiner : virtual public IAlgTool
{

 public:

  /** @brief Virtual destructor*/
  virtual ~IFourMomCombiner() {};
	
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;

  /** @brief execute methods */
  virtual StatusCode TrackClusterExecute(xAOD::Egamma* eg, 
					 unsigned int, 
					 bool hasSi) = 0;

  virtual StatusCode ClusterExecute(xAOD::Egamma* eg, 
				    const Amg::Vector3D&) = 0;

  //Public return routines.
  virtual const Amg::VectorX &getClusterVector()  const = 0;
  virtual const Amg::VectorX &getCombinedVector() const = 0;
  virtual const Amg::MatrixX &getClusterMatrix()  const = 0;
  virtual const Amg::MatrixX &getCombinedMatrix() const = 0;

  virtual const AmgMatrix(4,4) get4x4ClusterErrorMatrix()  const = 0;
  virtual const AmgMatrix(4,4) get4x4CombinedErrorMatrix() const = 0;

};

inline const InterfaceID& IFourMomCombiner::interfaceID()
{
  return IID_IFourMomCombiner;
}
#endif

