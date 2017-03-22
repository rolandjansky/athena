/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2CALORINGERHYPO_H
#define TRIGL2CALORINGERHYPO_H

///std include(s)
#include <string>
#include <vector>

#include "TrigMultiVarHypo/preprocessor/TrigRingerHelper.h"

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
    double    m_lumiCut;
    unsigned  m_nThresholds;

    ///feature keys
    std::string m_hlt_feature;
    std::string m_feature;
    std::string m_key;

    std::vector<std::vector<double>>                   m_thresholds;
    std::vector<std::vector<double>>                   m_etaBins;
    std::vector<std::vector<double>>                   m_etBins;

 
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
