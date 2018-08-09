/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_PAITOOL_H
#define ITRT_PAITOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

/** @class ITRT_PAITool ITRT_PAITool.h
 *  Give and AlgTool interface to the PAI model
 *
 *  @author Davide Costanzo
 */
class ITRT_PAITool : virtual public IAlgTool {
  public:

  // Declaration of the interface ID (interface id, major version, minor version)
  DeclareInterfaceID(ITRT_PAITool, 1 , 0);
  /// GetMeanFreePath
  virtual double GetMeanFreePath(double scaledKineticEnergy,
                                 double squaredCharge) const =0;

   /// GetEnergyTransfer
   virtual double GetEnergyTransfer(double scaledKineticEnergy) const =0;

 };

#endif // ITRT_PAITOOL_H
