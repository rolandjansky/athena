/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVSOFFLINEMISSINGET_TOOL_H
#define TRIGVSOFFLINEMISSINGET_TOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <string>
#include <vector>

class ITHistSvc;
class MsgStream;
class StoreGateSvc;

class MissingETData;

class TH1;
class TH2;

static const InterfaceID IID_TrigVsOfflineMissingETTool("TrigVsOfflineMissingETTool", 1, 0);

class TrigVsOfflineMissingETTool: public AthAlgTool {

  public:

    TrigVsOfflineMissingETTool(const std::string & type, 
        const std::string & name, const IInterface* parent);

    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID( ) { return IID_TrigVsOfflineMissingETTool; };

    StatusCode CBNT_initialize();

    /** Overriding initialize and finalize */
    virtual StatusCode initialize();

    /** finalization */
    virtual StatusCode finalize();
    virtual StatusCode execute(MissingETData *);

   inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

  protected:

    /** Standard destructor */
    virtual ~TrigVsOfflineMissingETTool( );

  private:
    ITHistSvc * m_thistSvc;

    std::string m_folderName;

    // StatusCode book(); 
    StatusCode bookHistograms();

    // StatusCode fill(); // execute
    // StatusCode proc(); // finalize


    StatusCode trigVsOfflineMissingETPlots(MissingETData *);

    void setDir(const std::string&);
    std::string& getDir();
    TH1* hist(const std::string&);
    TH2* hist2(const std::string&);
    StatusCode addHistogram(TH1 *);
    StatusCode addHistogram(TH2 *);

    /** methods to make booking easier */
    void addBasicL1Histograms();
    void addBasicL2Histograms();
    void addBasicEFHistograms();
    void addBasicRecHistograms();

    /** methods called by bookHistograms() */
    void bookHistograms_allStats();
    void bookL1Histograms();
    void bookL2Histograms();
    void bookEFHistograms();
    void bookEffiHistograms();

    /** methods called by fillHistograms() */

    /** monitoring trigger decision result */
    void trigger_decision();

    /** monitoring trigger missingET */
    //StatusCode met();

    /** methods to get/check correct trigger configurations */
    void getAllMetTriggers();
    /** check if the triggers specified by user are defined in
      trigger menu, and put the correct ones in a map to be used
      (maps in case of met triggers) */
    void checkTriggers(std::vector<std::string>& m_triggers,
        bool isInputMetTriggers);

    void printMetTriggerStats();

    /** histogram ranges and bins */
    double low[4];
    double high[4];
    int nbins[4];

    bool m_debuglevel; //!< private member to control debug messages

    /** handle for TrigDecisionTool */
    // to be deleted, since now availabe in ManagedMonitorToolBase
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;

    /** maps to store trigger items */

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

    /** properties */

    /** trigger aware monitoring */

    bool m_printTrigStats; 
    bool m_useAllTriggers;
    bool m_doTrigEff;

    /** list of met triggers we want to make histos for */ 
    std::vector<std::string> m_met_triggers;

    /** list of sample selection triggers */
    std::vector<std::string> m_selectTriggers;

    /** reference met container for efficiency studies **/
    std::string m_met_ref;

    /** name of the met containers -- not used*/
    std::string m_lvl1_roi_key;
    std::string m_l2metKey;
    std::string m_efmetKey;
    std::string m_recmetKey;
    std::string m_recmetcaloKey;

    /** met cuts */
    // not used yet
    double m_deltaRCut;
    std::string m_curdir, m_name;
};

#endif // TRIGVSOFFLINEMISSINGET_TOOL_H

