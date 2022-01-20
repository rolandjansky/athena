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

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include <string>
#include "PathResolver/PathResolver.h"


//Standard Constructor
ElectronPhotonShowerShapeFudgeTool::ElectronPhotonShowerShapeFudgeTool(std::string myname) :
  AsgTool(myname),
  m_ph_rootTool(0),
  m_el_rootTool(0),
  m_configFile(""),
  m_ffFile("")
{

  declareProperty("Preselection",m_preselection=-999);
  declareProperty("FFCalibFile", m_ffFile="ElectronPhotonShowerShapeFudgeTool/v2/PhotonFudgeFactors.root", "Calib path file for Photon MC corrections");
  declareProperty("ConfigFile",m_configFile="","The config file to use for the Electron Shifter");

  // Create an instance of the underlying ROOT tool
#ifdef USE_NEW_TOOL  
  m_ph_rootTool = new TPhotonMCShifterTool();
#else
  m_ph_rootTool = new FudgeMCTool();
#endif
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
  if(m_configFile.empty()){
    ATH_MSG_INFO("No config file set! Using default shift values for the electron shifter.");
    m_configFile = "ElectronPhotonShowerShapeFudgeTool/DefaultShifts.conf";
  }

  if(m_ffFile.empty()){
    ATH_MSG_ERROR ( "Could NOT resolve file name " << m_ffFile);
    return StatusCode::FAILURE ;
  }
  std::string configFile = PathResolverFindCalibFile(m_configFile);
  TEnv env;
  env.ReadFile(configFile.c_str(), kEnvLocal);

  m_el_rootTool->Shifts[ElePIDNames::Var::DeltaPoverP] = GetFloatVector("shift_DeltaPoverP", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::TRTHighTOutliersRatio] = GetFloatVector("shift_TRTHighTOutliersRatio", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::d0significance] = GetFloatVector("shift_d0significance", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::deltaeta1] = GetFloatVector("shift_deltaeta1", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::deltaphi2] = GetFloatVector("shift_deltaphi2", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::deltaphiRescaled] = GetFloatVector("shift_deltaphiRescaled", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::eratio] = GetFloatVector("shift_eratio", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::f1] = GetFloatVector("shift_f1", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::f3] = GetFloatVector("shift_f3", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::fside] = GetFloatVector("shift_fside", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::reta] = GetFloatVector("shift_reta", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::rhad] = GetFloatVector("shift_rhad", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::rphi] = GetFloatVector("shift_rphi", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::trackd0pvunbiased] = GetFloatVector("shift_trackd0pvunbiased", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::weta2] = GetFloatVector("shift_weta2", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::ws3] = GetFloatVector("shift_ws3", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::wstot] = GetFloatVector("shift_wstot", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::e277] = GetFloatVector("shift_e277", env);
  m_el_rootTool->Shifts[ElePIDNames::Var::DeltaE] = GetFloatVector("shift_DeltaE", env);

  //Not all of these are width adjusted but Im putting them in for the future.
  m_el_rootTool->Widths[ElePIDNames::Var::DeltaPoverP] = GetFloatVector("width_DeltaPoverP", env);
  m_el_rootTool->Widths[ElePIDNames::Var::TRTHighTOutliersRatio] = GetFloatVector("width_TRTHighTOutliersRatio", env);
  m_el_rootTool->Widths[ElePIDNames::Var::d0significance] = GetFloatVector("width_d0significance", env);
  m_el_rootTool->Widths[ElePIDNames::Var::deltaeta1] = GetFloatVector("width_deltaeta1", env);
  m_el_rootTool->Widths[ElePIDNames::Var::deltaphi2] = GetFloatVector("width_deltaphi2", env);
  m_el_rootTool->Widths[ElePIDNames::Var::deltaphiRescaled] = GetFloatVector("width_deltaphiRescaled", env);
  m_el_rootTool->Widths[ElePIDNames::Var::eratio] = GetFloatVector("width_eratio", env);
  m_el_rootTool->Widths[ElePIDNames::Var::f1] = GetFloatVector("width_f1", env);
  m_el_rootTool->Widths[ElePIDNames::Var::f3] = GetFloatVector("width_f3", env);
  m_el_rootTool->Widths[ElePIDNames::Var::fside] = GetFloatVector("width_fside", env);
  m_el_rootTool->Widths[ElePIDNames::Var::reta] = GetFloatVector("width_reta", env);
  m_el_rootTool->Widths[ElePIDNames::Var::rhad] = GetFloatVector("width_rhad", env);
  m_el_rootTool->Widths[ElePIDNames::Var::rphi] = GetFloatVector("width_rphi", env);
  m_el_rootTool->Widths[ElePIDNames::Var::trackd0pvunbiased] = GetFloatVector("width_trackd0pvunbiased", env);
  m_el_rootTool->Widths[ElePIDNames::Var::weta2] = GetFloatVector("width_weta2", env);
  m_el_rootTool->Widths[ElePIDNames::Var::ws3] = GetFloatVector("width_ws3", env);
  m_el_rootTool->Widths[ElePIDNames::Var::wstot] = GetFloatVector("width_wstot", env);
  m_el_rootTool->Widths[ElePIDNames::Var::e277] = GetFloatVector("width_e277", env);
  m_el_rootTool->Widths[ElePIDNames::Var::DeltaE] = GetFloatVector("width_DeltaE", env);
  
#ifdef USE_NEW_TOOL  
  m_ph_rootTool->LoadFFs(m_preselection, m_ffFile);
#endif
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
  m_ph_rootTool->FudgeShowers( et,
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
                               m_preselection,
                               m_ffFile);



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
  float deltaEta;
  float deltaPhiRescaled2;
  float e277;
  float DeltaE;

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
  allFound = allFound && el.showerShapeValue( e277, xAOD::EgammaParameters::e277);
  allFound = allFound && el.showerShapeValue( DeltaE, xAOD::EgammaParameters::DeltaE);

  allFound = allFound && el.trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1);
  allFound = allFound && el.trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);

  if (!allFound) {
    ATH_MSG_ERROR("Could not retrieve all shower shapes");
    return CP::CorrectionCode::Error;
  }

  // protection against bad clusters
  const xAOD::CaloCluster* cluster  = el.caloCluster();
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

  m_el_rootTool->shiftAll( et     ,
                           eta2   ,
                           Rhad1  ,
                           Rhad   ,
                           Reta   ,
                           Rphi   ,
                           weta2  ,
                           f1     ,
                           f3     ,
                           fside  ,
                           ws3    ,
                           wtot   ,
                           Eratio ,
                           e277   ,
                           DeltaE ,
                           deltaEta,
                           deltaPhiRescaled2
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
  allSet = allSet && el.setShowerShapeValue( e277, xAOD::EgammaParameters::e277);
  allSet = allSet && el.setShowerShapeValue( DeltaE, xAOD::EgammaParameters::DeltaE);

  allSet = allSet && el.setTrackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1);
  allSet = allSet && el.setTrackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);

  if (!allSet) {
    ATH_MSG_ERROR("Could not set all shower shapes");
    return CP::CorrectionCode::Error;
  }
  return CP::CorrectionCode::Ok;
}

const CP::CorrectionCode ElectronPhotonShowerShapeFudgeTool::correctedCopy( const xAOD::Photon& ph, xAOD::Photon*& output ) const {

  output = new xAOD::Photon(ph);
  return applyCorrection(*output);
}

const CP::CorrectionCode ElectronPhotonShowerShapeFudgeTool::correctedCopy( const xAOD::Electron& el, xAOD::Electron*& output ) const{

  output = new xAOD::Electron(el);
  return applyCorrection(*output);
}


std::vector<float> ElectronPhotonShowerShapeFudgeTool::GetFloatVector(const std::string& input,  TEnv& env){
	  std::vector<float> CutVector;
	  std::string env_input(env.GetValue(input.c_str(), ""));
	  if (env_input.size() > 0) {
	    std::string::size_type end;
	    do {
	      end = env_input.find(";");
	      float myValue(0);
	      if(ElectronPhotonShowerShapeFudgeTool::strtof(env_input.substr(0,end),myValue)){
	        CutVector.push_back(myValue);
	      }
	      if (end != std::string::npos)     env_input= env_input.substr(end+1);
	    } while (end != std::string::npos);
	  }
	  return CutVector;
}

bool ElectronPhotonShowerShapeFudgeTool::strtof(const std::string& input, float& f){

  int diff = 0 ;
  std::string tmp = input;
  std::string::size_type first(0);
  std::string::size_type last(0);

  first = ( input.find("#") ) ;
  if (first == std::string::npos) {
    f = ( atof (input.c_str() ) ) ;
    return true;
  }
  else {
    last = (input.find("#",first+1) );
    if (last == std::string::npos) {
      static asg::AsgMessaging msg("Egamma::ElectronPhotonShowerShapeFudgeTool");
      msg.msg(MSG::WARNING)<<" Improper comment format , inline comment should be enclosed between two #  "<<endmsg;
      return false;
    }
    diff = last - first ;
    tmp= tmp.erase(first,diff+1);
    std::istringstream buffer (tmp);
    buffer>>f;
    return true;
  }
}
