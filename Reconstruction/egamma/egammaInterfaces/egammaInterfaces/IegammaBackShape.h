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
            const CaloCellContainer& cell_container, Info& info) const = 0;
 };

inline const InterfaceID& IegammaBackShape::interfaceID()
{
  return IID_IegammaBackShape;
}

#endif // EGAMMAINTERFACES_IEGAMMABACKSHAPE_H
