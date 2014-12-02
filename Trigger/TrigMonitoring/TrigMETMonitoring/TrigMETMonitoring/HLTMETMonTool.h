/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTMETMONTOOL_H
#define HLTMETMONTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "MissingETEvent/MissingET.h"

#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

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
    void bookHLTHistograms();
    void bookEfficHistograms();

    /** methods to make booking easier */
    void addBasicL1Histograms();
    void addBasicHLTHistograms();
    void addMoreHLTHistograms();
    void addHLTStatusHistogram();
    void addBasicOffHistograms();
    void addL1vsHLTHistograms();
    void addL1vsOffHistograms();
    void addHLTvsOffHistograms();
    void addDQFlagHistograms();

    void bookXSCalibHistograms(int level);
    void trigger_decision();

    StatusCode fillMETHistos();
    StatusCode fillL1ShiftHistos(); 
    StatusCode fillHLTShiftHistos();
    StatusCode fillOffShiftHistos();
    StatusCode fillExpertHistos();

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
    std::map<std::string,int> m_all_hlt_met_triggers;

    std::map<std::string,int> m_l1_met_signatures;
    std::map<std::string,int> m_hlt_met_signatures;   

    std::map<std::string, int> *m_l1_met_signatures_tolook;
    std::map<std::string, int> *m_hlt_met_signatures_tolook;

    std::map<std::string, int> m_sample_selection_signatures;

    bool m_is_print_met_trig_stats; 
    bool m_is_make_histos_for_all_met_trig;
    bool m_is_make_expert_histos;
    bool m_doRecMET;
    std::string m_mon_path;
    bool m_is_do_trigger_effic;

    std::vector<std::string> m_met_triggers;
    std::vector<std::string> m_sample_selection_triggers;

    std::string m_lvl1_roi_key, m_hlt_met_key, m_hlt_met_feb_key, m_hlt_met_topocl_key;
    std::string m_recmet_key, m_recmetcalo_key;

    const xAOD::EnergySumRoI *m_l1_roi_cont;
    const xAOD::TrigMissingET           *m_hlt_met;
    const xAOD::TrigMissingETContainer  *m_hlt_met_cont;
    const xAOD::TrigMissingETContainer  *m_hlt_met_feb_cont;
    const xAOD::TrigMissingETContainer  *m_hlt_met_topocl_cont;

  private:
    std::vector<std::string> m_compNames, m_bitNames;
};

#endif // HLTMETMONTOOL_H

