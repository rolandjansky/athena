/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    m_result.setResult(0, -999.0);
    m_result.setResult(1, 1.0);
    return Root::TElectronEfficiencyCorrectionTool::initialize();
}

int Root::TPhotonEfficiencyCorrectionTool::finalize(){
    return 1;
}

const Root::TResult& Root::TPhotonEfficiencyCorrectionTool::calculate( const PATCore::ParticleDataType::DataType dataType,
                                  const unsigned int runnumber,
                                  const double cluster_eta,
                                  const double et /* in MeV */
                                  ){

    size_t CorrIndex{0},MCToysIndex{0} ;// The Photons for now do not break down those
    std::vector<double> result;
    Root::TElectronEfficiencyCorrectionTool::calculate(dataType,
            runnumber,
            cluster_eta,
            et, /* in MeV */
            result,
            CorrIndex,
            MCToysIndex
            );

    // Write the retrieved values into the return object
    /*
     * From the TResult comments
     * Get the zeroth entry, by convention, this is the efficiency or scale factor or MVA response or..
     * Get the first entry, by convention, this is the total uncertainty
     *  The underlying tool always fills these 2. If invalid return this will be SF=-999, Total 1
     */
    m_result.setResult(0, result[static_cast<size_t>(Root::TElectronEfficiencyCorrectionTool::Position::SF)]);
    // Assume symmetric uncertainties and return up variation
    m_result.setResult(1, result[static_cast<size_t>(Root::TElectronEfficiencyCorrectionTool::Position::TotalUp)]);
    return m_result;
}
