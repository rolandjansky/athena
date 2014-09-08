/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AsgFudgeMCTool
   @brief Tool to fudge photon MC shower shapes.

   @author Rob Roy Fletcher (University of Pennsylvania) rob.fletcher@cern.ch
   @date   August 2014


*/
#include "ElectronPhotonSelectorTools/AsgFudgeMCTool.h"

#include <string>


//Standard Constructor
AsgFudgeMCTool::AsgFudgeMCTool(std::string myname) :
  AsgTool(myname),
  m_rootTool(0)
{

  // Create an instance of the underlying ROOT tool
  m_rootTool = new FudgeMCTool();
} 

// Standard Destructor
AsgFudgeMCTool::~AsgFudgeMCTool() 
{
  finalize().ignore(); // since no finalize in current version
  if ( m_rootTool ) delete m_rootTool;
}


StatusCode AsgFudgeMCTool::initialize() 
{
  return StatusCode::SUCCESS;
}


StatusCode AsgFudgeMCTool::finalize() 
{
  return StatusCode::SUCCESS;
}


const CP::CorrectionCode AsgFudgeMCTool::applyCorrection( xAOD::Photon* gamma, int isConv, int preselection=-999) const 
{

  //Input vars to the underlying root tool.

  float rhad1; // Not in xAOD
  float rhad;  // Not in xAOD
  float e277;
  float reta;  // Not in xAOD
  float rphi;  // Not in xAOD
  float weta2;
  float f1;
  float fracs1;
  float wtot;
  float w1;
  float deltae; //Not in xAOD
  float eratio; //Not in xAOD

  // vars to hold the extra shower shape variable that are needed
  // to calculate the vars that are not already contained in the xAOD.
  float emaxs1;
  float e237;
  float e233;
  float ethad1;
  float ethad;
  float e2tsts1;
  float emins1;

  // Simple check to make sure it can get all needed values from the xAOD.
  bool allFound = true;

  allFound = allFound && gamma->showerShapeValue( e277, xAOD::EgammaParameters::e277);	
  allFound = allFound && gamma->showerShapeValue( e237, xAOD::EgammaParameters::e237);	
  allFound = allFound && gamma->showerShapeValue( e233, xAOD::EgammaParameters::e233);	
  allFound = allFound && gamma->showerShapeValue( weta2, xAOD::EgammaParameters::weta2);	
  allFound = allFound && gamma->showerShapeValue( f1, xAOD::EgammaParameters::f1);	
  allFound = allFound && gamma->showerShapeValue( wtot, xAOD::EgammaParameters::wtots1);	
  allFound = allFound && gamma->showerShapeValue( w1, xAOD::EgammaParameters::widths1);	
  allFound = allFound && gamma->showerShapeValue( emaxs1, xAOD::EgammaParameters::emaxs1);	
  allFound = allFound && gamma->showerShapeValue( ethad1, xAOD::EgammaParameters::ethad1);	
  allFound = allFound && gamma->showerShapeValue( ethad, xAOD::EgammaParameters::ethad);	
  allFound = allFound && gamma->showerShapeValue( fracs1, xAOD::EgammaParameters::fracs1);
  allFound = allFound && gamma->showerShapeValue( e2tsts1, xAOD::EgammaParameters::e2tsts1);	
  allFound = allFound && gamma->showerShapeValue( emins1, xAOD::EgammaParameters::emins1);	

  if (!allFound) { 
    ATH_MSG_ERROR("Could not retrieve all shower shapes");
    return CP::CorrectionCode::Error;
  }

  //calculate variables that need to be calculated.
  rhad1 = ethad1/(gamma->pt());
  rhad  = ethad/(gamma->pt());
  reta  = e237/e277;
  rphi  = e233/e237;
  deltae= emaxs1 - emins1;
  eratio= (emaxs1 - e2tsts1)/(emaxs1 + e2tsts1); 

  //pass the root tool the variables to be fudged.
  //returns nothing, just modifies references.
  m_rootTool->FudgeShowers( gamma->pt(),
		                        gamma->eta(),
                            rhad1,
                            rhad,
                            e277,
                            reta,
                            rphi,
                            weta2,
                            f1,
                            fracs1,
                            wtot,
                            w1,
                            deltae,
                            eratio,
                            isConv,
                            preselection);


  //Decorate the xAOD with the fudged variables.
  gamma->auxdata<float>("fudged_rhad1") = rhad1;
  gamma->auxdata<float>("fudged_rhad") = rhad;
  gamma->auxdata<float>("fudged_e277") = e277;
  gamma->auxdata<float>("fudged_reta") = reta;
  gamma->auxdata<float>("fudged_rphi") = rphi;
  gamma->auxdata<float>("fudged_weta2") = weta2;
  gamma->auxdata<float>("fudged_f1") = f1;
  gamma->auxdata<float>("fudged_fracs1") = fracs1;
  gamma->auxdata<float>("fudged_wtot") = wtot;
  gamma->auxdata<float>("fudged_w1") = w1;
  gamma->auxdata<float>("fudged_deltae") = deltae;
  gamma->auxdata<float>("fudged_eratio") = eratio;

  return CP::CorrectionCode::Ok;
}




