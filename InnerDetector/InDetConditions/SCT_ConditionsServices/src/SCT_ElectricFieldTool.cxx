/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CLHEP/Units/SystemOfUnits.h"
#include "SCT_ElectricFieldTool.h"

#include "TF1.h"

//#include <algorithm>
//#include "TMath.h"

SCT_ElectricFieldTool::SCT_ElectricFieldTool(const std::string& t, const std::string& n, const IInterface*  p) : 
        AthAlgTool(t,n,p),
        m_model(0)
{

  declareInterface<ISCT_ElectricFieldTool> ( this );

  declareProperty("EFieldModel",m_eFieldModel=0);           //!< uniform E solution as default
}

SCT_ElectricFieldTool::~SCT_ElectricFieldTool()
{
  delete m_model;
}

StatusCode 
SCT_ElectricFieldTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Unable to initialize the service!" <<  endreq;
    return sc;
  }
  
  return sc;
}



StatusCode SCT_ElectricFieldTool::finalize()
{
  return StatusCode::SUCCESS;
}

double SCT_ElectricFieldTool::getElectricField(double positionZ,
                                          double fluence,
                                          double depletionVoltage,
                                          double sensorThickness,
                                          double biasVoltage)
{

  (void) fluence;
  
  if (depletionVoltage<=0)
  {
    msg(MSG::FATAL) << "Depletion voltage is zero or negattive negative: " << depletionVoltage <<endreq;
    return 0;
  }
  

  if (m_eFieldModel == 0 || m_eFieldModel == 1){
    //--------------------------------------------------------------
    //   Electric Field Ez
    //--------------------------------------------------------------
    // m_eFieldModel == 0; uniform E-field model
    // m_eFieldModel == 1; flat diode model
    
    // y is depth and should be give in mm
    // the electric field is returned in V/mm
    
    double y = positionZ/10.;    // from mm to cm
    double m_bulk_depth; 
    double Ey;
    m_bulk_depth = sensorThickness/10.;   // from mm to cm
    y = m_bulk_depth - y;
    
    //------------ find delepletion deph for model=0 and 1 -------------
    // ATH_MSG_INFO ("\tmodel= "<< m_eFieldModel<<" VB= "<< biasVoltage);
    double m_depletion_depth;
    m_depletion_depth = m_bulk_depth;
    if (biasVoltage < depletionVoltage) m_depletion_depth = sqrt(biasVoltage/depletionVoltage) * m_bulk_depth;
    if (y<=m_depletion_depth){
      //---------- case for uniform electric field ------------------------
      if( m_eFieldModel ==0 ) {
	Ey = biasVoltage / m_depletion_depth ;   
	Ey = Ey/10.;
	return Ey / CLHEP::volt;
      }
      
      //---------- case for flat diode model ------------------------------
      if(m_eFieldModel==1) {       
	if(biasVoltage > depletionVoltage) 
	  Ey = (biasVoltage+depletionVoltage)/m_depletion_depth - 2.*depletionVoltage*(m_bulk_depth-y)/(m_bulk_depth*m_bulk_depth);
	else {
	  double Emax = 2.* m_depletion_depth * depletionVoltage / (m_bulk_depth*m_bulk_depth);
	  Ey = Emax*(1-(m_bulk_depth-y)/m_depletion_depth);
	}
	Ey = Ey/10.;
	return Ey / CLHEP::volt;
      }
    }
    else{
      return 0;
    }
  }
  else{
    
    //simple toy model --> to be replaced with complete parameterization later
    msg(MSG::INFO) << "Using for E-field simple toy model corresponding to 10^15 fluence, full depletion end for holes." << endreq;
    
    if (!m_model) 
      {
	m_model = new TF1("pol6","pol6",0,0.03);
	m_model->SetParameters(1.04176,
			       2095.14,
			       -606483.,
			       8.60976e+07,
			       -7.20639e+09,
			       3.59418e+11,
			       -9.43812e+12,
			       9.78747e+13);
      }
    
    
    double E_D = m_model->Eval(positionZ/10.)/10.; //the model is in cm
    return E_D;

  }
  return -99;
}//end method

  
      
  
    



