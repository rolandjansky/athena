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

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

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
    void bookExpertL1Histograms();
    void bookExpertHLTHistograms();
    void bookExpertEfficHistograms();

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
    StatusCode fillL1ShifterHistos(); 
    StatusCode fillHLTShifterHistos();
    StatusCode fillOffShifterHistos();
    StatusCode fillExpertHistos();

    void getAllMETTriggers();
    void checkTriggers(std::vector<std::string> &m_triggers,
                       std::map<std::string,int> &m_signatures);

    std::string get_trigger_level(std::string item);

    void printMETTriggerStats();

    // calculate phi1-phi2 keeping value between -pi and pi
    double signed_delta_phi(double ph11, double phi2);

  private:

    double m_et_min, m_sumet_min, m_phi_min, m_det_min;
    double m_et_max, m_sumet_max, m_phi_max, m_det_max;
    int m_et_bins, m_sumet_bins, m_phi_bins, m_det_bins;

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
    std::string m_shifter_path;
    std::string m_expert_path;

    bool m_is_do_trigger_effic;

    //std::vector<std::string> m_met_triggers;
    std::vector<std::string> m_met_triggers_l1, m_met_triggers_hlt;
    std::vector<std::string> m_sample_selection_triggers;

    std::string m_lvl1_roi_key;
    std::string m_hlt_met_key, m_hlt_met_feb_key, m_hlt_met_topocl_key;
    std::string m_off_met_key;
    // std::string m_recmet_key, m_recmetcalo_key;

    const xAOD::EnergySumRoI            *m_l1_roi_cont;
    const xAOD::TrigMissingET           *m_hlt_met;
    const xAOD::TrigMissingETContainer  *m_hlt_met_cont;
    const xAOD::TrigMissingETContainer  *m_hlt_met_feb_cont;
    const xAOD::TrigMissingETContainer  *m_hlt_met_topocl_cont;

    const xAOD::MissingET           *m_off_met;
    const xAOD::MissingETContainer  *m_off_met_cont;

  private:
    std::vector<std::string> m_compNames, m_bitNames;

};

#endif // HLTMETMONTOOL_H

