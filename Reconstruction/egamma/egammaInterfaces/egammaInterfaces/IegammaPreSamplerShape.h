/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMAPRESAMPLERSHAPE_H
#define EGAMMAINTERFACES_IEGAMMAPRESAMPLERSHAPE_H


/// @class IegammaPreSamplerShape
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaPreSamplerShape
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

static const InterfaceID IID_IegammaPreSamplerShape("IegammaPreSamplerShape", 1, 0);

class IegammaPreSamplerShape : virtual public IAlgTool {

  
 public:

  /// @brief Virtual destructor 
  virtual ~IegammaPreSamplerShape(){};
  
  /// @brief AlgTool interface methods 
  static const InterfaceID& interfaceID();

  class Info{
    public :
     double e011=0;
     double e033=0;
  };

  /// @name IegammaPreSamplerShape virtual functions
  /// @{

  /// @brief AlgTool method.
  virtual StatusCode execute(const xAOD::CaloCluster& cluster, const CaloCellContainer& cell_container, Info& info) const = 0;
  
 /// @}

};

inline const InterfaceID& IegammaPreSamplerShape::interfaceID()
{
  return IID_IegammaPreSamplerShape;
}

#endif // EGAMMAINTERFACES_IEGAMMAPRESAMPLERSHAPE_H
