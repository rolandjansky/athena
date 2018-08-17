/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ElectricFieldTool.h
 * @note primary author for Pixels Carolina Deluca (carolina.deluca.silberberg@cern.ch)
 * @author for SCT Peter Vankov (peter.vankov@cern.ch)
 **/

#ifndef ISCT_ElectricFieldTool_h
#define ISCT_ElectricFieldTool_h

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"

/**
 * @class ISCT_ElectricFieldTool
 * Interface class for tool providing Electric Field
 **/
class ISCT_ElectricFieldTool: virtual public IAlgTool
{
 public:
  virtual ~ISCT_ElectricFieldTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ElectricFieldTool, 1, 0);

  /// Get the Ramo potential for a given detector element (x,y,z)-dependent
  /// Assumes the center of the detector
  virtual double getElectricField(double positionZ,
                                  double fluence,
                                  double depletionVoltage,
                                  double sensorThickness,
                                  double biasVoltage) const =0;
  
};

#endif // ISCT_ElectricFieldTool_h
