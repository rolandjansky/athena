/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaL2CaloRingerSelectorTool_H
#define TrigEgammaL2CaloRingerSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "TrigMultiVarHypo/preprocessor/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/discriminator/MultiLayerPerceptron.h"
#include "AsgTools/AsgTool.h"
#include <vector>

namespace Trig{
class TrigEgammaL2CaloRingerSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaL2CaloRingerSelectorTool,  Trig::ITrigEgammaSelectorBaseTool)

  public:
    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaL2CaloRingerSelectorTool(const std::string& myname);
    ~TrigEgammaL2CaloRingerSelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 
    bool emulation( const xAOD::TrigEMCluster*, bool &, const std::string&);

  private:
    //Is good emulator?
    bool is_correct_trigger(const std::string&);

    std::string m_pidname;
    std::string m_signature;
    std::string m_str_etthr;
    float       m_etCut;
    unsigned    m_nDiscr;
    unsigned    m_nPreproc;
    float       m_output;

    //Prepoc configuration
    std::vector<unsigned int>            m_nRings;
    std::vector<unsigned int>            m_normRings;
    std::vector<unsigned int>            m_sectionRings;
    //Discriminator configuration
    std::vector<unsigned int>            m_nodes;
    std::vector<std::vector<double>>     m_weights;
    std::vector<std::vector<double>>     m_bias;
    std::vector<double>                  m_threshold;
    std::vector<std::vector<double>>     m_etaBins;
    std::vector<std::vector<double>>     m_etBins;
    ///Discriminator holder
    std::vector<MultiLayerPerceptron*>   m_discriminators;
    ///Pre-processing holder
    std::vector<TrigRingerPreprocessor*> m_preproc; 
  };

}//namespace

#endif
