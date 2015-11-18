/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2CALORINGERHYPO_H
#define TRIGL2CALORINGERHYPO_H

///std include(s)
#include <string>
#include <vector>

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

    //Interface:
    bool      m_acceptAll;
    double    m_emEtCut;
    unsigned  m_nThresholds;

    ///feature keys
    std::string m_hlt_feature;
    std::string m_feature;
    std::string m_key;

    std::vector<double>                                m_thresholds;
    std::vector<std::vector<double>>                   m_etaBins;
    std::vector<std::vector<double>>                   m_etBins;

 
    /* Helper functions for feature extraction */
    const xAOD::TrigRNNOutput* get_rnnOutput(const HLT::TriggerElement* te);

    ///Helper class
    class CutDefsHelper{
      private:
        double m_etamin;
        double m_etamax;
        double m_etmin;
        double m_etmax;
        double m_threshold;

      public:
        CutDefsHelper(double th, double etamin, double etamax,
                      double etmin, double etmax):m_etamin(etamin),
                      m_etamax(etamax),m_etmin(etmin),m_etmax(etmax),
                      m_threshold(th)
        {;}

        ~CutDefsHelper()
        {;}
        double threshold(){return m_threshold;};
        double etamin(){return m_etamin;};
        double etamax(){return m_etamax;};
        double etmin(){return m_etmin;};
        double etmax(){return m_etmax;};

    };///end of configuration

    std::vector<TrigL2CaloRingerHypo::CutDefsHelper*>  m_cutDefs; 
};
//!===============================================================================================
/// get the cluster inside of container
const xAOD::TrigRNNOutput* TrigL2CaloRingerHypo::get_rnnOutput(const HLT::TriggerElement* te) {
    const xAOD::TrigRNNOutput *pattern = 0;
    HLT::ErrorCode status = getFeature(te, pattern, m_hlt_feature);
    return (status == HLT::OK) ? pattern : 0;
}
//!===============================================================================================
#endif /* TRIGL2CALORINGERHYPO_H */
