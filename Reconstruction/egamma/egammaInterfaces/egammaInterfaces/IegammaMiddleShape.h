/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaMiddleShape.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAMIDDLESHAPE_H
#define EGAMMAINTERFACES_IEGAMMAMIDDLESHAPE_H

/// @class IegammaMiddleShape
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaMiddleShape
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

static const InterfaceID IID_IegammaMiddleShape("IegammaMiddleShape", 1, 0);

class IegammaMiddleShape : virtual public IAlgTool {

  
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaMiddleShape(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
    
  // pure virtual
  /** @brief AlgTool method.*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, const CaloCellContainer *cell_container) = 0;
  
  /** @brief energy in a  3x3 window in cells in eta X phi*/
  virtual double e233()   const = 0; 
  /** @brief energy in a  3x5 window in cells in eta X phi*/
  virtual double e235()   const = 0; 
  /** @brief energy in a  3x7 window in cells in eta X phi*/
  virtual double e237()   const = 0; 
  /** @brief energy in a  5x5 window in cells in eta X phi*/
  virtual double e255()   const = 0; 
  /** @brief energy in a  7x7 window in cells in eta X phi*/
  virtual double e277()   const = 0; 
  /** @brief weighted eta in a 3X5 window in eta X phi */
  virtual double etaw()   const = 0; 
  /** @brief weighted phi in a 3X5 window in eta X phi*/
  virtual double phiw()   const = 0; 
  /** @brief uncorrected width in a 3X5 window in eta X phi*/
  virtual double width()  const = 0; 
  /** @brief relative position in eta within cell*/
  virtual double poscs2() const = 0; 

  double m_neta;
  double m_nphi;
};

inline const InterfaceID& IegammaMiddleShape::interfaceID()
{
  return IID_IegammaMiddleShape;
}

#endif // EGAMMAINTERFACES_IEGAMMAMIDDLESHAPE_H
