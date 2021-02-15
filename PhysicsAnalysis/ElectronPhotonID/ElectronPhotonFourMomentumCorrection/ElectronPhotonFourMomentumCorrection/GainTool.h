/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINTOOL_H
#define ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINTOOL_H

#include <string>
#include <memory>
#include <PATCore/PATCoreEnums.h>

class TFile;
class TF1;

namespace egGain {

  class GainTool {

  public:
    GainTool(const std::string& filenameTO, const std::string& filenameVar);
    void Init(const std::string& filenameTO, const std::string& filenameVar);
    virtual ~GainTool();

    double CorrectionGainTool(double eta_input, double energy_input, double energy_layer2_input,
			      PATCore::ParticleType::Type ptype=PATCore::ParticleType::Electron);

  private:

    static const int m_NUM_ETA_BINS=28;
    static const int m_NUM_ENERGY_BINS=3;
    static const int m_NUM_UNCONV_ENERGY_BINS=4;

    std::unique_ptr <TFile> m_TOFile;
    std::unique_ptr <TFile> m_varFile;

    TF1* m_funcTO[m_NUM_ETA_BINS];
    TF1* m_funcG[m_NUM_ENERGY_BINS][m_NUM_ETA_BINS];
    TF1* m_conv_funcG[m_NUM_ENERGY_BINS][m_NUM_ETA_BINS];
    TF1* m_unconv_funcG[m_NUM_UNCONV_ENERGY_BINS][m_NUM_ETA_BINS];

  };


}
#endif
