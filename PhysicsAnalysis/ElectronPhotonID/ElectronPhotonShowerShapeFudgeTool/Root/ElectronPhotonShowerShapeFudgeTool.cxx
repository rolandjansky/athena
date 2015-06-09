/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AsgFudgeMCTool
   @brief Tool to fudge photon MC shower shapes.

   @author Rob Roy Fletcher (University of Pennsylvania) rob.fletcher@cern.ch
   @date   August 2014
*/

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/FudgeMCTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/TElectronMCShifterTool.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include <string>


//Standard Constructor
ElectronPhotonShowerShapeFudgeTool::ElectronPhotonShowerShapeFudgeTool(std::string myname) :
  AsgTool(myname),
  m_ph_rootTool(0),
  m_el_rootTool(0)
{

  declareProperty("Preselection",m_preselection=-999);

  // Create an instance of the underlying ROOT tool
  m_ph_rootTool = new FudgeMCTool();
  m_el_rootTool = new TElectronMCShifterTool();
}

// Standard Destructor
ElectronPhotonShowerShapeFudgeTool::~ElectronPhotonShowerShapeFudgeTool()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in ElectronPhotonShowerShapeFudgeTool finalize()");
  }

  if ( m_ph_rootTool ) delete m_ph_rootTool;
  if ( m_el_rootTool ) delete m_el_rootTool;
}


StatusCode ElectronPhotonShowerShapeFudgeTool::initialize() 
{
  return StatusCode::SUCCESS;
}


StatusCode ElectronPhotonShowerShapeFudgeTool::finalize() 
{
  return StatusCode::SUCCESS;
}


const CP::CorrectionCode ElectronPhotonShowerShapeFudgeTool::applyCorrection( xAOD::Photon& ph) const {

  //Input vars to the underlying root tool.

  float Rhad1;
  float Rhad;
  float e277;
  float Reta;
  float Rphi;
  float weta2;
  float f1;
  float fracs1;
  float wtot;
  float w1;
  float DeltaE;
  float Eratio;

  // vars to hold the extra shower shape variable that are needed
  // to calculate the vars that are not already contained in the xAOD.

  // Simple check to make sure it can get all needed values from the xAOD.
  bool allFound = true;

  allFound = allFound && ph.showerShapeValue( e277, xAOD::EgammaParameters::e277);
  allFound = allFound && ph.showerShapeValue( Reta, xAOD::EgammaParameters::Reta);
  allFound = allFound && ph.showerShapeValue( Rphi, xAOD::EgammaParameters::Rphi);
  allFound = allFound && ph.showerShapeValue( weta2, xAOD::EgammaParameters::weta2);
  allFound = allFound && ph.showerShapeValue( f1, xAOD::EgammaParameters::f1);
  allFound = allFound && ph.showerShapeValue( wtot, xAOD::EgammaParameters::wtots1);
  allFound = allFound && ph.showerShapeValue( w1, xAOD::EgammaParameters::weta1);
  allFound = allFound && ph.showerShapeValue( Rhad1, xAOD::EgammaParameters::Rhad1);
  allFound = allFound && ph.showerShapeValue( Rhad, xAOD::EgammaParameters::Rhad);
  allFound = allFound && ph.showerShapeValue( fracs1, xAOD::EgammaParameters::fracs1);
  allFound = allFound && ph.showerShapeValue( DeltaE, xAOD::EgammaParameters::DeltaE);
  allFound = allFound && ph.showerShapeValue( Eratio, xAOD::EgammaParameters::Eratio);

  if (!allFound) {
    ATH_MSG_ERROR("Could not retrieve all shower shapes");
    return CP::CorrectionCode::Error;
  }

  // protection against bad clusters
  const xAOD::CaloCluster* cluster  = ph.caloCluster(); 
  if ( cluster == 0 ) {
    ATH_MSG_ERROR("cluster == " << cluster);
    return CP::CorrectionCode::Error;
  }

  // eta position in second sampling
  const float eta2   = fabsf(cluster->etaBE(2));
  // transverse energy in calorimeter (using eta position in second sampling)
  const double energy =  cluster->e();
  double et = 0.;
  if (eta2<999.) {
    const double cosheta = cosh(eta2);
    et = (cosheta != 0.) ? energy/cosheta : 0.;
  }

  //pass the root tool the variables to be fudged.
  //returns nothing, just modifies references.
  m_ph_rootTool->FudgeShowers( 
			      //ph.pt(),
			      //ph.eta(),
			      et,
			      eta2,
			      Rhad1,
			      Rhad,
			      e277,
			      Reta,
			      Rphi,
			      weta2,
			      f1,
			      fracs1,
			      wtot,
			      w1,
			      DeltaE,
			      Eratio,
			      xAOD::EgammaHelpers::isConvertedPhoton(&ph),
			      m_preselection);



  //Set Shower shapes in xAOD
  bool allSet = true;

  allSet = allSet && ph.setShowerShapeValue( Rhad1, xAOD::EgammaParameters::Rhad1);
  allSet = allSet && ph.setShowerShapeValue( Rhad, xAOD::EgammaParameters::Rhad);
  allSet = allSet && ph.setShowerShapeValue( Reta, xAOD::EgammaParameters::Reta);
  allSet = allSet && ph.setShowerShapeValue( Rphi, xAOD::EgammaParameters::Rphi);
  allSet = allSet && ph.setShowerShapeValue( f1, xAOD::EgammaParameters::f1);
  allSet = allSet && ph.setShowerShapeValue( e277, xAOD::EgammaParameters::e277);
  allSet = allSet && ph.setShowerShapeValue( weta2, xAOD::EgammaParameters::weta2);
  allSet = allSet && ph.setShowerShapeValue( wtot, xAOD::EgammaParameters::wtots1);
  allSet = allSet && ph.setShowerShapeValue( w1, xAOD::EgammaParameters::weta1);
  allSet = allSet && ph.setShowerShapeValue( fracs1, xAOD::EgammaParameters::fracs1);
  allSet = allSet && ph.setShowerShapeValue( DeltaE, xAOD::EgammaParameters::DeltaE);
  allSet = allSet && ph.setShowerShapeValue( Eratio, xAOD::EgammaParameters::Eratio);

  if (!allSet) {
    ATH_MSG_ERROR("Could not set all shower shapes");
    return CP::CorrectionCode::Error;
  }

  return CP::CorrectionCode::Ok;
}


const CP::CorrectionCode ElectronPhotonShowerShapeFudgeTool::applyCorrection( xAOD::Electron& el) const {

  //Input vars to the underlying root tool.

  float Rhad1;
  float Rhad;
  float Reta;
  float Rphi;
  float weta2;
  float f1;
  float f3;
  float wtot;
  float ws3;
  float fside;
  float Eratio;

  bool allFound = true;

  allFound = allFound && el.showerShapeValue( Reta, xAOD::EgammaParameters::Reta);
  allFound = allFound && el.showerShapeValue( Rphi, xAOD::EgammaParameters::Rphi);
  allFound = allFound && el.showerShapeValue( weta2, xAOD::EgammaParameters::weta2);
  allFound = allFound && el.showerShapeValue( f1, xAOD::EgammaParameters::f1);
  allFound = allFound && el.showerShapeValue( wtot, xAOD::EgammaParameters::wtots1);
  allFound = allFound && el.showerShapeValue( ws3, xAOD::EgammaParameters::weta1);
  allFound = allFound && el.showerShapeValue( Rhad1, xAOD::EgammaParameters::Rhad1);
  allFound = allFound && el.showerShapeValue( Rhad, xAOD::EgammaParameters::Rhad);
  allFound = allFound && el.showerShapeValue( f3, xAOD::EgammaParameters::f3);
  allFound = allFound && el.showerShapeValue( fside, xAOD::EgammaParameters::fracs1);
  allFound = allFound && el.showerShapeValue( Eratio, xAOD::EgammaParameters::Eratio);

  if (!allFound) {
    ATH_MSG_ERROR("Could not retrieve all shower shapes");
    return CP::CorrectionCode::Error;
  }


  m_el_rootTool->shiftAll( el.pt(),
			   el.eta(),
			   Rhad1  ,
			   Rhad   ,
			   Reta   ,
			   Rphi   ,
			   weta2  ,
			   f1     ,
			   f3     ,
			   fside  ,
			   ws3     ,
			   Eratio 
			   );

  bool allSet = true;
  allSet = allSet && el.setShowerShapeValue( Rhad1, xAOD::EgammaParameters::Rhad1);
  allSet = allSet && el.setShowerShapeValue( Rhad, xAOD::EgammaParameters::Rhad);
  allSet = allSet && el.setShowerShapeValue( Reta, xAOD::EgammaParameters::Reta);
  allSet = allSet && el.setShowerShapeValue( Rphi, xAOD::EgammaParameters::Rphi);
  allSet = allSet && el.setShowerShapeValue( f1, xAOD::EgammaParameters::f1);
  allSet = allSet && el.setShowerShapeValue( f3, xAOD::EgammaParameters::f3);
  allSet = allSet && el.setShowerShapeValue( weta2, xAOD::EgammaParameters::weta2);
  allSet = allSet && el.setShowerShapeValue( wtot, xAOD::EgammaParameters::wtots1);
  allSet = allSet && el.setShowerShapeValue( ws3, xAOD::EgammaParameters::weta1);
  allSet = allSet && el.setShowerShapeValue( fside, xAOD::EgammaParameters::fracs1);
  allSet = allSet && el.setShowerShapeValue( Eratio, xAOD::EgammaParameters::Eratio);

  if (!allSet) {
    ATH_MSG_ERROR("Could not set all shower shapes");
    return CP::CorrectionCode::Error;
  }
  return CP::CorrectionCode::Ok;
}

const CP::CorrectionCode ElectronPhotonShowerShapeFudgeTool::correctedCopy( const xAOD::Photon& ph, xAOD::Photon*& output ) const {

  output = new xAOD::Photon(ph);
  applyCorrection(*output);
  return CP::CorrectionCode::Ok;
}

const CP::CorrectionCode ElectronPhotonShowerShapeFudgeTool::correctedCopy( const xAOD::Electron& el, xAOD::Electron*& output ) const{

  output = new xAOD::Electron(el);
  applyCorrection(*output);
  return CP::CorrectionCode::Ok;
}
