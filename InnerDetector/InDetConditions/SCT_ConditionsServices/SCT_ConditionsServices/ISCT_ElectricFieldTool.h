/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ElectricFieldTool.h
 * @note primary author for Pixels Carolina Deluca (carolina.deluca.silberberg@cern.ch)
 * @author for SCT Peter Vankov (peter.vankov@cern.ch)
 **/

#ifndef ISCT_ElectricFieldTool_h
#define ISCT_ElectricFieldTool_h

//Gaudi Includes
#include "GaudiKernel/AlgTool.h"

/**
 * @class ISCT_ElectricFieldTool
 * Interface class for tool providing Electric Field
 **/
class ISCT_ElectricFieldTool: virtual public IAlgTool
{
 public:
  virtual ~ISCT_ElectricFieldTool() {}
  
  static const InterfaceID& interfaceID(); 

  /// Get the Ramo potential for a given detector element (x,y,z)-dependent
  /// Assumes the center of the detector
  virtual double getElectricField(double positionZ,
                                  double fluence,
                                  double depletionVoltage,
                                  double sensorThickness,
                                  double biasVoltage)=0;
  
};

inline const InterfaceID& ISCT_ElectricFieldTool::interfaceID() {
  static const InterfaceID IID_ISCT_ElectricFieldTool{"ISCT_ElectricFieldTool", 1, 0};
  return IID_ISCT_ElectricFieldTool; 

}

#endif // ISCT_ElectricFieldTool_h
