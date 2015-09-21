/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINTOOL_H
#define ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINTOOL_H

#include <string>

#include <PATCore/PATCoreEnums.h>

class TFile;
class TF1;

namespace egGain {

  class GainTool {
    
  public:
    GainTool();
    GainTool(std::string filenameTO, std::string filenameVar);
    void Init(std::string filenameTO, std::string filenameVar);
    virtual ~GainTool();

    double CorrectionGainTool(double eta_input, double energy_input, double energy_layer2_input,
			      PATCore::ParticleType::Type ptype=PATCore::ParticleType::Electron);

  private:
    
    TFile* m_TOFile;
    TFile* m_varFile; 
    TF1* m_funcTO[28]; 
    TF1* m_funcG[3][28];
    TF1* m_conv_funcG[3][28]; 
    TF1* m_unconv_funcG[4][28]; 
    TF1* m_funcG_com[3][28]; 
    
  };
  
  
} 
#endif
