/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CLHEP/Units/SystemOfUnits.h"
#include "SCT_ElectricFieldTool.h"

#include "TF1.h"



SCT_ElectricFieldTool::SCT_ElectricFieldTool(const std::string& t, const std::string& n, const IInterface*  p) : 
        AthAlgTool(t,n,p),
        m_model{nullptr}{
  declareInterface<ISCT_ElectricFieldTool> ( this );
  declareProperty("EFieldModel",m_eFieldModel=0);           //!< uniform E solution as default
}

SCT_ElectricFieldTool::~SCT_ElectricFieldTool(){
}

StatusCode 
SCT_ElectricFieldTool::initialize(){
/**
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Unable to initialize the service!" <<  endmsg;
    return sc;
  }**/
  return StatusCode::SUCCESS;
}



StatusCode SCT_ElectricFieldTool::finalize(){
  return StatusCode::SUCCESS;
}

double SCT_ElectricFieldTool::getElectricField(double positionZ,
                                          double /**fluence**/,
                                          double depletionVoltage,
                                          double sensorThickness,
                                          double biasVoltage){
  //(void) fluence;
  if (depletionVoltage<=0){
    ATH_MSG_FATAL( "Depletion voltage is zero or negative: " << depletionVoltage );
    return 0;
  }
  enum FieldModel{UNIFORM_FIELD, FLAT_DIODE};
  if (m_eFieldModel == UNIFORM_FIELD || m_eFieldModel == FLAT_DIODE){
    //--------------------------------------------------------------
    //   Electric Field Ez
    //--------------------------------------------------------------
    // y is depth and should be give in mm
    // the electric field is returned in V/mm
    //
    double y = positionZ*0.1;    // from mm to cm
    double bulkDepth=sensorThickness*0.1;   // from mm to cm; 
    double Ey(0.);
    y = bulkDepth - y;
    
    //------------ find depletion depth for model=0 and 1 -------------
    // ATH_MSG_INFO ("\tmodel= "<< m_eFieldModel<<" VB= "<< biasVoltage);
    double depletionDepth{bulkDepth};
    if (biasVoltage < depletionVoltage) depletionDepth = sqrt(biasVoltage/depletionVoltage) * bulkDepth;
    if (y<=depletionDepth){
      //---------- case for uniform electric field ------------------------
      if( m_eFieldModel ==UNIFORM_FIELD ) {
        Ey = biasVoltage / depletionDepth ;   
        Ey = Ey*0.1;
        return Ey / CLHEP::volt;
      }
      
      //---------- case for flat diode model ------------------------------
      if(m_eFieldModel==FLAT_DIODE) {       
        if(biasVoltage > depletionVoltage) {
          Ey = (biasVoltage+depletionVoltage)/depletionDepth - 2.*depletionVoltage*(bulkDepth-y)/(bulkDepth*bulkDepth);
        }else {
          double Emax = 2.* depletionDepth * depletionVoltage / (bulkDepth*bulkDepth);
          Ey = Emax*(1-(bulkDepth-y)/depletionDepth);
        }
        Ey = Ey*0.1;
        return Ey / CLHEP::volt;
      }
    } else{
      return 0;
    }
  }else{
    
    //simple toy model --> to be replaced with complete parameterization later
    ATH_MSG_INFO( "Using for E-field simple toy model corresponding to 10^15 fluence, full depletion end for holes." );
    
    if (!m_model) {
      m_model = std::make_unique<TF1>("pol6","pol6",0,0.03);
      m_model->SetParameters(1.04176,
             2095.14,
             -606483.,
             8.60976e+07,
             -7.20639e+09,
             3.59418e+11,
             -9.43812e+12,
             9.78747e+13);
      }
      double E_D = m_model->Eval(positionZ*0.1)*0.1; //the model is in cm
      return E_D;

  }
  return -99;
}//end method

  
      
  
    



