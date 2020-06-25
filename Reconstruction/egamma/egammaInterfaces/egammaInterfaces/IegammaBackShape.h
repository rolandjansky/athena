/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAINTERFACES_IEGAMMABACKSHAPE_H
#define EGAMMAINTERFACES_IEGAMMABACKSHAPE_H

/*
 * @name   IegammaBackShape.h, (c) ATLAS Detector software 2009
 * @class IegammaBackShape
 * @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaBackShape
 *
 * @author Frederic Derue derue@lpnhe.in2p3.fr
 * @author Christos Anastopoulos
 */

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;
class CaloDetDescrManager;
static const InterfaceID IID_IegammaBackShape("IegammaBackShape", 1, 0);

class IegammaBackShape : virtual public IAlgTool {
 
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaBackShape(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  class Info {
    public:
        double f3= 0.;
        double f3core = 0.;
        double e333 = 0.;
        double e335 = 0.; 
        double e355 = 0.; 
        double e337 = 0.; 
        double e377 = 0.; 
  };
  /** @brief AlgTool method.*/
  virtual StatusCode execute(const xAOD::CaloCluster& cluster,
                             const CaloDetDescrManager& cmgr,
                             const CaloCellContainer& cell_container,
                             Info& info) const = 0;
 };

inline const InterfaceID& IegammaBackShape::interfaceID()
{
  return IID_IegammaBackShape;
}

#endif // EGAMMAINTERFACES_IEGAMMABACKSHAPE_H
