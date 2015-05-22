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
/// $Revision: 583267 $
/// $Date: 2014-02-14 13:39:31 +0100 (Fri, 14 Feb 2014) $
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
  /** @brief execute method*/
  virtual StatusCode execute() = 0;
  
  /** @brief returns corrected width at eta. */
  virtual float  Correct(float eta, float etacell, float width) = 0;
  /** @brief returns relative position within cell*/
  virtual double RelPosition(float eta, float etacell) = 0;
  
};

inline const InterfaceID& Iegammaqweta2c::interfaceID()
{
  return IID_Iegammaqweta2c;
}

#endif // EGAMMAINTERFACES_IEGAMMAQWETA2C_H
