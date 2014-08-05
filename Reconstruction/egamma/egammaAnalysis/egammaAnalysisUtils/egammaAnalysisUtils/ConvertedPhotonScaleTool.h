/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONVERTEDPHOTONSCALETOOL_H
#define CONVERTEDPHOTONSCALETOOL_H

/*******************************************************************************
 * File:
 * Created:
 * Purpose:
 *******************************************************************************
 *
 * usage example
 * -------------
 *
 *   #include "ConvertedPhotonScaleTool.h"
 *   ConvertedPhotonScaleTool converted_tool("mypath_to_conversion.root relative from the executable");
 *   const double converted_scale = converted_tool.Scale(cluster_eta,
 *                                                       cluster_energy,
 *                                                       conversion_radius);
 *   const double cluster_energy_corrected = cluster_energy * converted_scale;
 *
 * if you don't give any parameter to the constructor it assume that the path is "./conversion.root"
 *
 * protections
 * ----------
 * The tool checks for:
 *  - R in [0, 800]
 *  - energy > 19 GeV
 *  - |eta| < 2.5
 *  - not 1.425 < |eta| < 1.550 (crack)
 * if one of the previous fails it returns 1.
 *
 * how to compile
 * --------------
 * you don't have to compile it, it is header-only, just include it.
 *
 ******************************************************************************/

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

// this flag is used in case you're using the calibration framework
#undef CALIBRATION_FRAMEWORK

#ifdef CALIBRATION_FRAMEWORK
#include <cassert>
#include "../naming.h"
#include "../utils/debug_macro.h"
#include "../utils/calibgeometry.h"
const DetectorBinning binning = LARGE2;  // <-- remember here
#endif


class ConvertedPhotonScaleTool
{
public:
    ConvertedPhotonScaleTool(const std::string & filename = "conversion.root");
    virtual ~ConvertedPhotonScaleTool() { };
    double Scale(double eta, double energy, double radius) const;
private:
    TObject* GetFromFileAndCheck(const std::string & object_name, TFile & file) const;
    std::vector<TF1*> functions_par0;
    std::vector<TF1*> functions_par1;
    std::vector<TF1*> functions_par2;
    TAxis* axis_eta;
    double eta_max;

  //ClassDef(ConvertedPhotonScaleTool, 1);
};


#endif
