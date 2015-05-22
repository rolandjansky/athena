/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaBackShape.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMABACKSHAPE_H
#define EGAMMAINTERFACES_IEGAMMABACKSHAPE_H

/// @class IegammaBackShape
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaBackrShape
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;

static const InterfaceID IID_IegammaBackShape("IegammaBackShape", 1, 0);

class IegammaBackShape : virtual public IAlgTool {

  
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaBackShape(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  
  // pure virtual
  /** @brief AlgTool method.*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, const CaloCellContainer *cell_container) = 0;
  
  /** @brief uncor. energy in 3x3 cells*/
  virtual double e333() const = 0;
  /** @brief uncor. energy in 3x5 cells*/
  virtual double e335() const = 0;
  /** @brief uncor. energy in 5x5 cells*/
  virtual double e355() const = 0;
  /** @brief uncor. energy in 3x7 cells */
  virtual double e337() const = 0;
  /** @brief uncor. energy in 7x7 cells*/
  virtual double e377() const = 0;
  /** @brief ratio of energy deposit in 3rd sampling over all energy*/
  virtual double f3()   const = 0; 
  /** @brief ratio of energy deposited in the third longitudinal compartment
      of the total energy in the cluster*/
  virtual double f3core()   const = 0; 

};

inline const InterfaceID& IegammaBackShape::interfaceID()
{
  return IID_IegammaBackShape;
}

#endif // EGAMMAINTERFACES_IEGAMMABACKSHAPE_H
