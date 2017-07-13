/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaL2CaloRingerSelectorTool_H
#define TrigEgammaL2CaloRingerSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "TrigMultiVarHypo/preproc/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/tools/TrigRingerHelper.h"
#include "TrigMultiVarHypo/tools/TrigL2CaloRingerReader.h"
#include "TrigMultiVarHypo/tools/MultiLayerPerceptron.h"
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
    bool emulation( const xAOD::TrigEMCluster*, bool &, const Trig::Info &);

  private:
    ///Thresholds Holder
    std::vector<TrigCaloRingsHelper::CutDefsHelper*>  m_cutDefs; 
    ///Discriminator holder
    std::vector<MultiLayerPerceptron*>   m_discriminators;
    ///Pre-processing holder
    std::vector<TrigRingerPreprocessor*> m_preproc; 

    TrigL2CaloRingerReader m_reader;
    
    /* Helper method to retrieve the bins from an index */
    //void index_to_et_eta_bins(unsigned, unsigned &, unsigned &);
    void setEtThr( float et ){m_etCut=et;};
    
    float       m_etCut;
    float       m_output;
    float       m_lumiCut;
    bool        m_useEtaVar;
    bool        m_useLumiVar;
    bool        m_doPileupCorrection;
    bool        m_useNoActivationFunctionInTheLastLayer;
    bool        m_useLumiTool;

    //Prepoc configuration
    std::vector<unsigned int>            m_nRings;
    std::vector<unsigned int>            m_normRings;
    std::vector<unsigned int>            m_sectionRings;

    //Discriminator configuration
    std::string m_calibPath_constants, m_calibPath_thresholds;

  };

}//namespace

#endif
