/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Iegammaqweta2c.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAQWETA2C_H
#define EGAMMAINTERFACES_IEGAMMAQWETA2C_H

/// @class Iegammaqweta2c
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaqweta2c
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
///
/// $Revision: 741442 $
/// $Date: 2016-04-19 21:13:26 +0200 (Tue, 19 Apr 2016) $
///

// Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_Iegammaqweta2c("Iegammaqweta2c", 1, 0);
        
class Iegammaqweta2c : virtual public IAlgTool {

 public:
  /** @brief Virtual destructor */
  virtual ~Iegammaqweta2c(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID(); 
  /** @brief returns corrected width at eta. */
  virtual float  Correct(float eta, float etacell, float width) const = 0;
  /** @brief returns relative position within cell*/
  virtual double RelPosition(float eta, float etacell) const = 0;
  
};

inline const InterfaceID& Iegammaqweta2c::interfaceID()
{
  return IID_Iegammaqweta2c;
}

#endif // EGAMMAINTERFACES_IEGAMMAQWETA2C_H
