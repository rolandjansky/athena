/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaEnergyPositionAllSamples.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAENERGYPOSITIONALLSAMPLES_H
#define EGAMMAINTERFACES_IEGAMMAENERGYPOSITIONALLSAMPLES_H

/// @class IegammaMiddleShape
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaMiddleShape
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
/// @author Anthony Morley
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

static const InterfaceID IID_IegammaEnergyPositionAllSamples("IegammaEnergyPositionAllSamples", 1, 0);

class IegammaEnergyPositionAllSamples : virtual public IAlgTool {

 public:
  /** @brief Virtual destructor */
  virtual ~IegammaEnergyPositionAllSamples(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  // pure virtual
  /** @brief AlgTool method.*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster) = 0;
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, int is) = 0;
  
  /** @brief uncorrected sum of energy in all samplings*/
  virtual double e() const = 0;
  /** @brief return the uncorrected cluster energy in pre-sampler*/
  virtual double e0() const = 0;
  /** @brief return the uncorrected cluster energy in 1st sampling*/
  virtual double e1() const = 0;
  /** @brief return the uncorrected cluster energy in 2nd sampling*/
  virtual double e2() const = 0;
  /** @brief return the uncorrected cluster energy in 3rd sampling*/
  virtual double e3() const = 0;
  /** @brief boolean to know if we are in barrel or end-cap*/
  virtual bool inBarrel() const = 0;
};

inline const InterfaceID& IegammaEnergyPositionAllSamples::interfaceID()
{
  return IID_IegammaEnergyPositionAllSamples;
}

#endif // EGAMMAINTERFACES_IEGAMMAENERGYPOSITIONALLSAMPLES_H
