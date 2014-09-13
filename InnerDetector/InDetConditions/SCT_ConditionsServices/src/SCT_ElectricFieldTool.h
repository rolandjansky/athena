/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ElectricFieldTool.h
 * @primary author for Pixels giacinto.piacquadio@cern.ch
 * @author for SCT peter.vankov@cern.ch
**/


#ifndef SCT_ElectricFieldTool_h
#define SCT_ElectricFieldTool_h

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsServices/ISCT_ElectricFieldTool.h"
//#include "SiPropertiesSvc/SiliconProperties.h"
//#include "CLHEP/Geometry/Vector3D.h"
//#include <string>
//#include <vector>

//forward declarations
//class IdentifierHash;
//class ISiliconConditionsSvc;
//class IGeoModelSvc;
//class StoreGateSvc;
//class ElectricFieldSvc;

//namespace InDetDD {
//  class SiDetectorManager;
//}


class TF1;


class SCT_ElectricFieldTool:  public AthAlgTool , virtual public ISCT_ElectricFieldTool
{
public:
  SCT_ElectricFieldTool(const std::string& t, const std::string& n, const IInterface*  p);

  virtual ~SCT_ElectricFieldTool();
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
//  static const InterfaceID & interfaceID();
 
  virtual double getElectricField(double positionZ,
                                  double fluence,
                                  double depletionVoltage,
                                  double sensorThickness,
                                  double biasVoltage);
  int m_eFieldModel;      //!< 0 uniform E-field model, 1 flat diode model

private:
  
  bool m_useLinearModel;
  TF1* m_model;

};

//
//inline const InterfaceID & ElectricFieldTool::interfaceID(){
//  static const InterfaceID IID_ElectricFieldTool("ElectricFieldTool", 1, 0);
//  return IID_ElectricFieldTool;
//}

#endif
