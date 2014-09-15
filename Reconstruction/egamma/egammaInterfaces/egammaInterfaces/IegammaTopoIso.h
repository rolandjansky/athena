/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMATOPOISO_H
#define EGAMMAINTERFACES_IEGAMMATOPOISO_H

/// @class IegammaTopoIso.h 
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaTopoIso
///
/// @author T. Cuhadar Donszelmann
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///


// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <vector>

// Forward declarations
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h" 

static const InterfaceID IID_IegammaTopoIso("IegammaTopoIso", 1, 0);
        
class IegammaTopoIso : virtual public IAlgTool {

     
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaTopoIso(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief method: Et cone variables for topo cluster passing the parameters to calculate*/
  virtual StatusCode execute(const xAOD::Egamma*, 
			     const xAOD::CaloClusterContainer*, 
			     const std::vector<int>& parsToCalculate) = 0;

  /** @brief method: Et cone variables for topo cluster passing the radii*/
  virtual StatusCode execute(const xAOD::Egamma*, 
			     const xAOD::CaloClusterContainer*, 
			     const std::vector<float>& Rs) = 0;

  /** @brief method: Et cone variables for topo cluster passing the radii*/
  virtual StatusCode execute(const xAOD::Egamma*,
			     const std::vector<const xAOD::CaloCluster*>& clusts,
			     const std::vector<float>& Rs) = 0;


  /** @brief total et in hadron cal and EM; the argument is the vector element number if more than 1*/
  virtual double topoetcone(int i=0) const = 0; 

};

inline const InterfaceID& IegammaTopoIso::interfaceID()
{
  return IID_IegammaTopoIso;
}

#endif // EGAMMAINTERFACES_IEGAMMAISO_H
