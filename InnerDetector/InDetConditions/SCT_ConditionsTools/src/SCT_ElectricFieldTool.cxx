/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ElectricFieldTool.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

SCT_ElectricFieldTool::SCT_ElectricFieldTool(const std::string& t, const std::string& n, const IInterface* p) : 
  base_class(t, n, p) {
}

StatusCode 
SCT_ElectricFieldTool::initialize() {
  m_model.SetParameters(1.04176,
                        2095.14,
                        -606483.,
                        8.60976e+07,
                        -7.20639e+09,
                        3.59418e+11,
                        -9.43812e+12,
                        9.78747e+13);

  return StatusCode::SUCCESS;
}



StatusCode SCT_ElectricFieldTool::finalize() {
  return StatusCode::SUCCESS;
}

double SCT_ElectricFieldTool::getElectricField(double positionZ,
                                               double /**fluence**/,
                                               double depletionVoltage,
                                               double sensorThickness,
                                               double biasVoltage) const {
  if ((m_eFieldModel == UNIFORM_FIELD) or (m_eFieldModel == FLAT_DIODE)) {
    //--------------------------------------------------------------
    //   Electric Field Ez
    //--------------------------------------------------------------
    // y is depth and should be give in mm
    // the electric field is returned in V/mm
    //
    double y{positionZ*0.1};    // from mm to cm
    double bulkDepth{sensorThickness*0.1};   // from mm to cm; 
    double Ey{0.};
    y = bulkDepth - y;
    
    //------------ find depletion depth for model=0 and 1 -------------
    double depletionDepth{bulkDepth};
    if (biasVoltage < std::abs(depletionVoltage)) depletionDepth = sqrt(biasVoltage/std::abs(depletionVoltage)) * bulkDepth;
    if (y<=depletionDepth){
      //---------- case for uniform electric field ------------------------
      if( m_eFieldModel ==UNIFORM_FIELD ) {
        Ey = biasVoltage / depletionDepth ;   
        Ey = Ey*0.1;
        return Ey / CLHEP::volt;
      }
      
      //---------- case for flat diode model ------------------------------
      if (m_eFieldModel==FLAT_DIODE) {       
        if (biasVoltage > std::abs(depletionVoltage)) {
          Ey = (biasVoltage+depletionVoltage)/depletionDepth - 2.*depletionVoltage*(bulkDepth-y)/(bulkDepth*bulkDepth);
        } else {
          double Emax{2.* depletionDepth * depletionVoltage / (bulkDepth*bulkDepth)};
          Ey = Emax*(1-(bulkDepth-y)/depletionDepth);
        }
        Ey = Ey*0.1;
        return Ey / CLHEP::volt;
      }
    } else {
      return 0.;
    }
  } else {
    //simple toy model --> to be replaced with complete parameterization later
    ATH_MSG_INFO("Using for E-field simple toy model corresponding to 10^15 fluence, full depletion end for holes.");
    
    double E_D{m_model.Eval(positionZ*0.1)*0.1}; //the model is in cm
    return E_D;
  }

  return -99.;
}//end method
