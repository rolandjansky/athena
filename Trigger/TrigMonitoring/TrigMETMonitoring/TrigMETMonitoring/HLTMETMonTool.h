/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTMETMONTOOL_H
#define HLTMETMONTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
//#include "TrigDecision/TrigDecisionTool.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
//#include "TrigCaloEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "MissingETEvent/MissingET.h"

#include <string>
#include <vector>


class HLTMETMonTool: public IHLTMonTool {
  public:

    HLTMETMonTool(const std::string & type, const std::string & name, const IInterface* parent);
    ~HLTMETMonTool();

    StatusCode init();

    StatusCode book();
    StatusCode fill();
    StatusCode proc();


  private:

    /** methods called by book() */
    void bookHistograms_allStats();
    void bookL1Histograms();
    void bookL2Histograms();
    void bookEFHistograms();
    void bookEfficHistograms();

    /** methods to make booking easier */
    void addBasicL1Histograms();
    void addBasicL2Histograms();
    void addMoreL2Histograms();
    void addL2FEBHistograms();
    void addL2StatusHistogram();
    void addBasicEFHistograms();
    void addMoreEFHistograms();
    void addEFStatusHistogram();
    void addBasicRecHistograms();
    void addL1vsL2Histograms();
    void addL1vsEFHistograms();
    void addL2vsEFHistograms();
    void addL1vsRecHistograms();
    void addL2vsRecHistograms();
    void addEFvsRecHistograms();
    void addDQFlagHistograms();

    void bookXSCalibHistograms(int level);
    void trigger_decision();

    StatusCode fillMETHistos();

    StatusCode fillL1ShiftHistos(const LVL1_ROI::energysums_type & L1METROI);
    StatusCode fillL2ShiftHistos(const LVL1_ROI::energysums_type & L1METROI,
        const TrigMissingET *& missETL2);
    
    StatusCode fillL2FEBShiftHistos(const TrigMissingET *& missL2FEB);

    StatusCode fillEFShiftHistos(const LVL1_ROI::energysums_type & L1METROI,
        const TrigMissingET *& missETL2,
        const TrigMissingET *& missETEF);
    StatusCode fillRecShiftHistos(const LVL1_ROI::energysums_type & L1METROI,
        const TrigMissingET *& missETL2,
        const TrigMissingET *& missETEF,
        const MissingET *& missETRec);

    StatusCode fillExpertHistos(const LVL1_ROI::energysums_type & L1METROI,
        const TrigMissingET *& missETL2,
        const TrigMissingET *& missETEF,
        const MissingET *& missETRec);

    void getAllMETTriggers();
    void checkTriggers(std::vector<std::string>& m_triggers,
        bool isInputMETTriggers);

    void printMETTriggerStats();

    // calculate phi1-phi2 keeping value between -pi and pi
    double signed_delta_phi(double ph11, double phi2);

  private:

    double low[4];
    double high[4];
    int nbins[4];

    bool m_debuglevel; //!< private member to control debug messages

    std::map<std::string,int> m_all_l1_met_triggers;
    std::map<std::string,int> m_all_l2_met_triggers;
    std::map<std::string,int> m_all_ef_met_triggers;

    std::map<std::string,int> m_l1_met_signatures;
    std::map<std::string,int> m_l2_met_signatures;
    std::map<std::string,int> m_ef_met_signatures;   

    std::map<std::string, int> *m_l1_met_signatures_tolook;
    std::map<std::string, int> *m_l2_met_signatures_tolook;
    std::map<std::string, int> *m_ef_met_signatures_tolook;

    std::map<std::string, int> m_sample_selection_signatures;


    bool m_is_print_met_trig_stats; 
    bool m_is_make_histos_for_all_met_trig;
    bool m_is_make_expert_histos;
    bool m_doRecMET;
    std::string m_mon_path;
    bool m_is_do_trigger_effic;

    std::vector<std::string> m_met_triggers;

    std::vector<std::string> m_sample_selection_triggers;

    std::string m_lvl1_roi_key;
    std::string m_l2met_key, m_l2feb_key;
    std::string m_efmet_key;
    std::string m_recmet_key;
    std::string m_recmetcalo_key;

    const LVL1_ROI * m_lvl1_roi; 

  private:

    std::vector<std::string> m_compNamesL2,  m_compNamesEF, m_bitNamesL2, m_bitNamesEF;
};

#endif // HLTMETMONTOOL_H

