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


Root::TPhotonEfficiencyCorrectionTool::TPhotonEfficiencyCorrectionTool(const char* name):
    Root::TElectronEfficiencyCorrectionTool(name){
    }

Root::TPhotonEfficiencyCorrectionTool::~TPhotonEfficiencyCorrectionTool(){
}

int Root::TPhotonEfficiencyCorrectionTool::initialize(){
    //Apparently the TResult needs a "specific convention" for the 1st  2
   return Root::TElectronEfficiencyCorrectionTool::initialize();
}

typedef Root::TPhotonEfficiencyCorrectionTool::Result Result;
const Result Root::TPhotonEfficiencyCorrectionTool::calculate( const PATCore::ParticleDataType::DataType dataType,
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

    Result output;
    output.scaleFactor= result[static_cast<size_t>(Root::TElectronEfficiencyCorrectionTool::Position::SF)];
    output.totalUncertainty=result[static_cast<size_t>(Root::TElectronEfficiencyCorrectionTool::Position::Total)];
    return output;
}
