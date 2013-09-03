/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateTools_H
#define TriggerRateTools_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TriggerRateTools/Prescaler.h"
#include "TriggerRateTools/EvtVetoTool.h"
#include "TriggerRateTools/SlicesInfoTool.h"
#include "TrigNtDecision/TD_Signature.h"
#include "TrigNtDecision/TD_CplxSignature.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


#include <vector>
#include <string> 
#include <map>

#include "TBranch.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TBits.h"

namespace HLT {
	class Chain;
	class TriggerElement;
	class HLTResult;
	class ILvl1ResultAccessTool;
	class Signature;
	class Sequence;
	//class Navigation;
}

namespace LVL1CTP {
	class Lvl1Item;
	class Lvl1Result;
}

namespace TrigConf {
	class ITrigConfigSvc;
	class TriggerItem;
	class HLTChain;
	class HLTTriggerElement;
}

class MsgStream;

using namespace Trig;

namespace Trig {
    class ChainGroup;
    class TriggerRateTools : public Algorithm {

    public:

        TriggerRateTools(const std::string &name, ISvcLocator *pSvcLocator); 
        ~TriggerRateTools();

        StatusCode initialize();
        StatusCode execute(); 
        StatusCode finalize();

    private:
        
        StatusCode prepare();
        void retrieveResults();
        StatusCode endEvent();
        void resetResults();
        int findSigIndex(std::string name, std::string lvl);
        void addSig(std::string name, std::string lvl, double prescale, double passthrough, std::string, std::string);
        void initializeSigFeatures(std::string lvl);
        void initializeNtuple(std::string lvl);
        void initializeHistos(std::string lvl);
        int fillSingleRates(std::string lvl);
        void fillCplxRates(std::string lvl);
        double getPnum(std::vector<double> *list);
        void applyVeto(std::string lvl, bool veto);

    private:

        MsgStream* m_log;

        ITHistSvc* m_histSvc;
        ServiceHandle<StoreGateSvc> m_storeGate;
        double m_xsec;
        double m_lumi;
	double m_NColls;
        std::vector<std::string> m_ignore;
        std::vector< std::vector<std::string> > m_cplx_or;
        std::vector< std::vector<std::string> > m_cum;
        std::vector< std::vector<std::string> > m_cplx_and;
        std::vector< std::vector<std::string> > m_menus;
        std::vector< std::vector<std::string> > m_streams;
        std::vector< std::vector<std::string> > m_psOverride;
        bool m_dumpText;
        bool m_doVeto;
        bool m_doRawTD;
        bool m_doSlicesInfo;
	bool m_ignorePassThrough;
	bool m_doRetrieveItems;

	Trig::ExpertMethods *m_trigDecXmethods; 
        ToolHandle<Trig::TrigDecisionTool> m_trigDec;
        //ToolHandle<IEvtVetoTool> m_evtVetoTool;
        ToolHandleArray<IEvtVetoTool> m_vetoTools;
        ToolHandle<SlicesInfoTool> m_slicesInfoTool;
        
	const ChainGroup* L1Items;
	const ChainGroup* L2Chains;
	const ChainGroup* EFChains;
	std::vector<std::string> ChainNames;

        TFile *m_file;
        TTree *raw_TD;
        TTree *raw_PS;
        TTree *s_sigs_tree;
        TTree *c_sigs_tree;
        TTree *menu_sigs_tree;
        TTree *stream_sigs_tree;
        TTree *l_sigs_tree;
        int num_events;
        int evt_num;
        int num_sigs; // Number of L1 items + HLT chains found in the events
        int num_sigs_persist; // Maximum number of L1 items + HLT chains found in all events

        TH1F* sigs_L1_rate_hist;
        TH1F* sigs_L2_rate_hist;
        TH1F* sigs_EF_rate_hist;
        TH2F* sigs_L1_overlap_hist;
        TH2F* sigs_L2_overlap_hist;
        TH2F* sigs_EF_overlap_hist;

        std::vector<Int_t> sigs_L1_res;
        std::vector<Int_t> sigs_L2_res;
        std::vector<Int_t> sigs_EF_res;
        std::vector<Int_t> sigs_L1_ps;
        std::vector<Int_t> sigs_L2_ps;
        std::vector<Int_t> sigs_EF_ps;
        std::vector<double> sigs_L1_p;
        std::vector<double> sigs_L2_p;
        std::vector<double> sigs_EF_p;
        std::vector<std::string> sigs_L1_name;
        std::vector<std::string> sigs_L2_name;
        std::vector<std::string> sigs_EF_name;
        std::vector<double> lvl_pnum;

        std::vector<TD_Signature*> s_sigs_L1;
        std::vector<TD_Signature*> s_sigs_L2;
        std::vector<TD_Signature*> s_sigs_EF;
        std::vector<TD_CplxSignature*> c_sigs_L1;
        std::vector<TD_CplxSignature*> c_sigs_L2;
        std::vector<TD_CplxSignature*> c_sigs_EF;
        std::vector<TD_CplxSignature*> menu_sigs_L1;
        std::vector<TD_CplxSignature*> menu_sigs_L2;
        std::vector<TD_CplxSignature*> menu_sigs_EF;
        std::vector<TD_CplxSignature*> stream_sigs_L1;
        std::vector<TD_CplxSignature*> stream_sigs_L2;
        std::vector<TD_CplxSignature*> stream_sigs_EF;
        std::vector<int> and_or_change_index;
        std::vector<TD_CplxSignature*> l_sigs;
        
        bool doOnce;

    };	
}
#endif
