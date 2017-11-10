/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGL2CALORINGERHYPO_H
#define TRIGL2CALORINGERHYPO_H

///std include(s)
#include <string>
#include <vector>

#include "TrigMultiVarHypo/tools/TrigRingerHelper.h"
#include "TrigMultiVarHypo/tools/TrigL2CaloRingerReader.h"

///Base from trigger
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimer.h"

///xAOD include(s)
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigCalo/TrigEMCluster.h"

class TrigL2CaloRingerHypo: public HLT::HypoAlgo {
 
  public:

    TrigL2CaloRingerHypo(const std::string & name, ISvcLocator* pSvcLocator);
    virtual ~TrigL2CaloRingerHypo()
    {;}

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    HLT::ErrorCode hltFinalize();

  private:
    bool      m_acceptAll;
    bool      m_useNoActivationFunctionInTheLastLayer;
    bool      m_doPileupCorrection; 
    double    m_emEtCut;
    int       m_lumiCut;

    std::string m_calibPath;
    
    ///feature keys
    std::string m_hlt_feature;
    std::string m_feature;
    std::string m_key;


    TrigL2CaloRingerReader m_reader;
 
    /* Helper functions for feature extraction */
    const xAOD::TrigRNNOutput* get_rnnOutput(const HLT::TriggerElement* te);

    std::vector<TrigCaloRingsHelper::CutDefsHelper*>  m_cutDefs; 
};
//!===============================================================================================
/// get the cluster inside of container
const xAOD::TrigRNNOutput* TrigL2CaloRingerHypo::get_rnnOutput(const HLT::TriggerElement* te) {
    const xAOD::TrigRNNOutput *pattern = nullptr;
    HLT::ErrorCode status = getFeature(te, pattern, m_hlt_feature);
    return (status == HLT::OK) ? pattern : nullptr;
}
//!===============================================================================================
#endif /* TRIGL2CALORINGERHYPO_H */
