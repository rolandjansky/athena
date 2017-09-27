/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Iegammaqweta1c.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAQWETA1C_H
#define EGAMMAINTERFACES_IEGAMMAQWETA1C_H

/// @class Iegammaqweta1c
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaqweta1c
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
///
/// $Revision: 583267 $
/// $Date: 2014-02-14 13:39:31 +0100 (Fri, 14 Feb 2014) $
///


// Gaudi
#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_Iegammaqweta1c("Iegammaqweta1c", 1, 0);
        
class Iegammaqweta1c : virtual public IAlgTool {

     
 public:
  /** @brief Virtual destructor */
  virtual ~Iegammaqweta1c(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  /** @brief returns corrected width at eta*/
  virtual float      Correct(float eta, float etacell, float width) const = 0;
  /** @brief returns relative position within cell*/
  virtual double     RelPosition(float eta, float etacell) const = 0;
};

inline const InterfaceID& Iegammaqweta1c::interfaceID()
{
  return IID_Iegammaqweta1c;
}

#endif // EGAMMAINTERFACES_IEGAMMAQWETA1C_H
