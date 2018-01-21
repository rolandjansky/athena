/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class TPhotonEfficiencyCorrectionTool
   @brief Calculate the photon scale factors and assosiated errors in pure ROOT

   @author Michael Pitt
   @date   April 2014
*/


// This class header
#include "PhotonEfficiencyCorrection/TPhotonEfficiencyCorrectionTool.h"

// STL includes
#include <iostream>
#include <math.h>
// ROOT includes
#include "TString.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TFile.h"
#include "TClass.h"
#include "TMD5.h"

Root::TPhotonEfficiencyCorrectionTool::TPhotonEfficiencyCorrectionTool(const char* name):
    Root::TCalculatorToolBase(name),
    Root::TElectronEfficiencyCorrectionTool(name){
    }

Root::TPhotonEfficiencyCorrectionTool::~TPhotonEfficiencyCorrectionTool(){
}

int Root::TPhotonEfficiencyCorrectionTool::initialize(){
    //Apparently the TResult needs a "specific convention" for the 1st  2
    m_result.addResult("efficiency_SF", "efficiency scale factor");
    m_result.addResult("efficiency_SF_err", "efficiency scale factor uncertainty");
    return Root::TElectronEfficiencyCorrectionTool::initialize();
}

int Root::TPhotonEfficiencyCorrectionTool::finalize(){
    return Root::TElectronEfficiencyCorrectionTool::finalize();
}

const Root::TResult& Root::TPhotonEfficiencyCorrectionTool::calculate( const PATCore::ParticleDataType::DataType dataType,
                                  const unsigned int runnumber,
                                  const double cluster_eta,
                                  const double et /* in MeV */
                                  ){

    size_t CorrIndex{0},MCToysIndex{0} ;// The Photons for now do not break down those
    const std::vector<double> result=Root::TElectronEfficiencyCorrectionTool::calculate(dataType,
            runnumber,
            cluster_eta,
            et, /* in MeV */
            CorrIndex,
            MCToysIndex
            );

    // Write the retrieved values into the return object
    /*
     * From the TResult comments
     * Get the zeroth entry, by convention, this is the efficiency or scale factor or MVA response or..
     * Get the first entry, by convention, this is the total uncertainty
     */
     m_result.setResult(0, result[static_cast<size_t>(Root::TElectronEfficiencyCorrectionTool::Position::SF)]);
     m_result.setResult(1, result[static_cast<size_t>(Root::TElectronEfficiencyCorrectionTool::Position::Total)]);
     return m_result;
}
    



//===================================================================================
// Calculate the assosiated uncertainties with various photon isolation prescriptions
//===================================================================================
double Root::TPhotonEfficiencyCorrectionTool::GetIsoSyst(bool isConv,double eta, int isolationcut)
{
switch (isolationcut){
  case 3:
	if(isConv){
		if(fabs(eta)<0.6) return 0.0038;
		else if(fabs(eta)<1.37) return 0.0045;
		else if(fabs(eta)<1.52) return 0;
		else if(fabs(eta)<1.81) return 2.7e-05;
		else if(fabs(eta)<2.37) return 0.0046;
	}
	else{
	  if(fabs(eta)<0.6) return 0.00043;
	  else if(fabs(eta)<1.37) return 0.00069;
	  else if(fabs(eta)<1.52) return 0;
	  else if(fabs(eta)<1.81) return 0.00095;
	  else if(fabs(eta)<2.37) return 0.0023;
	}
        return 0;
  case 4: return 0;	
  case 5:
	if(isConv){
		if(fabs(eta)<0.6) return 0.0036;
		else if(fabs(eta)<1.37) return 0.0007;
		else if(fabs(eta)<1.52) return 0;
		else if(fabs(eta)<1.81) return 0.0009;
		else if(fabs(eta)<2.37) return 0.0016;
	}
	else{
	  if(fabs(eta)<0.6) return 0.001;
	  else if(fabs(eta)<1.37) return 0.0019;
	  else if(fabs(eta)<1.52) return 0;
	  else if(fabs(eta)<1.81) return 0.002;
	  else if(fabs(eta)<2.37) return 0.0023;
	}
        return 0;
  case 6:
	if(isConv){
		if(fabs(eta)<0.6) return 0.0045;
		else if(fabs(eta)<1.37) return 0.0034;
		else if(fabs(eta)<1.52) return 0;
		else if(fabs(eta)<1.81) return 0.0057;
		else if(fabs(eta)<2.37) return 0.0016;
	}
	else{
	  if(fabs(eta)<0.6) return 0.0028;
	  else if(fabs(eta)<1.37) return 0.0035;
	  else if(fabs(eta)<1.52) return 0;
	  else if(fabs(eta)<1.81) return 0.0012;
	  else if(fabs(eta)<2.37) return 0.0048;
	}
        return 0;
  case 7:
	if(isConv){
		if(fabs(eta)<0.6) return 0.011;
		else if(fabs(eta)<1.37) return 0.0074;
		else if(fabs(eta)<1.52) return 0;
		else if(fabs(eta)<1.81) return 0.0089;
		else if(fabs(eta)<2.37) return 0.0071;
	}
	else{
	  if(fabs(eta)<0.6) return 0.0033;
	  else if(fabs(eta)<1.37) return 0.0042;
	  else if(fabs(eta)<1.52) return 0;
	  else if(fabs(eta)<1.81) return 0.00041;
	  else if(fabs(eta)<2.37) return 0.0037;
	}
        return 0;
  case 8:
	if(isConv){
		if(fabs(eta)<0.6) return 0.0033;
		else if(fabs(eta)<1.37) return 0.00051;
		else if(fabs(eta)<1.52) return 0;
		else if(fabs(eta)<1.81) return 0.0012;
		else if(fabs(eta)<2.37) return 0.00057;
	}
	else{
	  if(fabs(eta)<0.6) return 0.0044;
	  else if(fabs(eta)<1.37) return 0.0037;
	  else if(fabs(eta)<1.52) return 0;
	  else if(fabs(eta)<1.81) return 0.0012;
	  else if(fabs(eta)<2.37) return 0.0032;
	}
        return 0;
	default: std::cout<<"Wrong isolation input!!!!!"<<std::endl; return 0.;
}	
return 0.;

}
