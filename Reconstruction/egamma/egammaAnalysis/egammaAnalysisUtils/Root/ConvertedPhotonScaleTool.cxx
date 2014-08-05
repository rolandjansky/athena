/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaAnalysisUtils/ConvertedPhotonScaleTool.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <TDirectory.h>
#include <TObject.h>
#include <TAxis.h>
#include <TFile.h>
#include <TF1.h>

//ClassImp(ConvertedPhotonScaleTool)

// this flag is used in case you're using the calibration framework
#undef CALIBRATION_FRAMEWORK

#ifdef CALIBRATION_FRAMEWORK
#include <cassert>
#include "../naming.h"
#include "../utils/debug_macro.h"
#include "../utils/calibgeometry.h"
const DetectorBinning binning = LARGE2;  // <-- remember here
#endif

#ifndef ROOTCORE
// using athena
#include "PathResolver/PathResolver.h"
#endif


ConvertedPhotonScaleTool::ConvertedPhotonScaleTool(const std::string & filename)
  : eta_max(0)
{
    TFile* old_file = gDirectory->GetFile();
#ifdef ROOTCORE
    const std::string filename_tool = filename;
#else
    const std::string filename_tool = PathResolver::find_file(filename, "DATAPATH");
#endif

    TFile file(filename_tool.c_str());
    if (file.IsZombie()) 
	std::cerr << "FATAL <ConvertedPhotonScaleTool>: can't find file "
		  << filename_tool << std::endl;

    axis_eta = dynamic_cast<TAxis*>(GetFromFileAndCheck("eta_binning", file));
    if (!axis_eta) {
	std::cerr << "FATAL <ConvertedPhotonScaleTool>: can't convert objects"
		  << std::endl;
        file.Close();
        if (old_file) old_file->cd();
        return;
    }

    eta_max = axis_eta->GetXmax();
#ifdef CALIBRATION_FRAMEWORK
    const unsigned int n_eta_bins = N_ACCORDION_BINS(binning);
    assert(n_eta_bins == (unsigned)axis_eta->GetNbins());  // checked
#else
    const unsigned int n_eta_bins = axis_eta->GetNbins();
#endif

    for (unsigned int etabin=0; etabin < n_eta_bins; ++etabin)
    {
#ifndef CALIBRATION_FRAMEWORK
	std::ostringstream ss;
	ss << "_bin" << std::setfill('0') << std::setw(2) << etabin;
	const std::string f_par0_name = "f_par0" + ss.str();
	const std::string f_par1_name = "f_par1" + ss.str();
	const std::string f_par2_name = "f_par2" + ss.str();
#else
	const std::string f_par0_name = Naming("f_par0").SetBin(etabin).GetName();
	const std::string f_par1_name = Naming("f_par1").SetBin(etabin).GetName();
	const std::string f_par2_name = Naming("f_par2").SetBin(etabin).GetName();
#endif
	// use RTTI to check type
	TF1* f_par0 = dynamic_cast<TF1*>(GetFromFileAndCheck(f_par0_name, file));
	TF1* f_par1 = dynamic_cast<TF1*>(GetFromFileAndCheck(f_par1_name, file));
	TF1* f_par2 = dynamic_cast<TF1*>(GetFromFileAndCheck(f_par2_name, file));

	if (!(f_par0 and f_par1 and f_par2))
	    std::cerr << "FATAL <ConvertedPhotonScaleTool>: can't convert objects" << std::endl;
    
	functions_par0.push_back(f_par0);
	functions_par1.push_back(f_par1);
	functions_par2.push_back(f_par2);
    }

    file.Close();
    if (old_file) old_file->cd();
}

TObject* 
ConvertedPhotonScaleTool::GetFromFileAndCheck(const std::string & object_name,
					      TFile & file) const
{
    TObject* obj = file.Get(object_name.c_str());
    if (!obj) std::cerr << "FATAL <ConvertedPhotonScaleTool>: can't find "
			<< object_name << std::endl;
    return obj;
}


double ConvertedPhotonScaleTool::Scale(double eta, double energy, double radius) const
{
    if (radius <= 0 or radius > 800) return 1.; // do not correct for photon with r > 800
    if (energy < 19000) return 1.;             // do not correct for cluster energy < 22 GeV
    const double aeta = fabs(eta);             
    if (aeta >= eta_max) return 1.;            // do not correct for photon with eta > 2.5
    if (aeta > 1.425 and aeta < 1.550) return 1.;  // do not correct for crack
#ifdef CALIBRATION_FRAMEWORK
    const int etabin = eta2bin(eta, binning);
    const int etabin2 = axis_eta->FindFixBin(aeta) - 1;
    if (etabin != etabin2)                     // definition are equal: checked
    {
	std::cerr << "eta: " << eta << std::endl;
	std::cerr << "NOT EQUAL, etabin: " << etabin << " etabin2: " << etabin2 << std::endl;
	assert(false);
    }
#else
    const int etabin = axis_eta->FindFixBin(aeta) - 1; // count from 0
#endif

    const TF1 *f_par0 = functions_par0[etabin];
    const TF1 *f_par1 = functions_par1[etabin];
    const TF1 *f_par2 = functions_par2[etabin];

    double pp0 = f_par0->Eval(energy);
    double pp1 = f_par1->Eval(energy);
    double pp2 = f_par2->Eval(energy);

    double correction = pp0 + pp1 * radius + pp2 * sqrt(radius);
#ifdef CALIBRATION_FRAMEWORK
    DEBUG(std::cout << "correction: " << 1./correction << std::endl);
#endif
    return 1. / correction;    
}

