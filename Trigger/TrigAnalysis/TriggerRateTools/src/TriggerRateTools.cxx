/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateTools/TriggerRateTools.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

#include "TrigConfL1Data/TriggerItem.h"
#include "TriggerRateTools/Prescaler.h"
#include "TrigNtDecision/TD_Signature.h"
#include "TrigNtDecision/TD_CplxSignature.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <map>

#include "TROOT.h"
#include <TSystem.h>
#include "GaudiKernel/NTuple.h"


using namespace Trig;

TriggerRateTools::TriggerRateTools(const std::string &name, ISvcLocator *pSvcLocator)
        : Algorithm(name, pSvcLocator),
        m_log(0),
        m_storeGate("StoreGateSvc", name),
        m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
        m_vetoTools(),
        m_slicesInfoTool("SlicesInfoTool",this) {
    declareProperty("VetoTools", m_vetoTools);
    declareProperty("SlicesInfoTool", m_slicesInfoTool);
    declareProperty("TrigDecisionTool", m_trigDec, "The tool to access TrigDecision");
    declareProperty("xSection", m_xsec = 1);
    declareProperty("Luminosity", m_lumi = 1);
    declareProperty("collsPerEvent", m_NColls =1);
    declareProperty("PrescaleOverrideList", m_psOverride);
    declareProperty("IgnoreList", m_ignore);
    declareProperty("CplxOrList", m_cplx_or);
    declareProperty("CplxAndList", m_cplx_and);
    declareProperty("CumulateList", m_cum);
    declareProperty("StreamsList", m_streams);
    declareProperty("MenusList", m_menus);
    declareProperty("doTextOutput", m_dumpText = false);
    declareProperty("doVeto", m_doVeto = false);
    declareProperty("doRawTD", m_doRawTD = true);
    declareProperty("doSlicesInfo", m_doSlicesInfo = true);
    declareProperty("ignorePassThrough", m_ignorePassThrough = false);  
    declareProperty("doRetrieveItems", m_doRetrieveItems = false);
}

/**************/
/* Destructor */
/**************/

TriggerRateTools::~TriggerRateTools() {

}

/**************/
/* Initialize */
/**************/

StatusCode TriggerRateTools::initialize() {
    
    doOnce=true;

    //gSystem->Load("Signature_cxx.so");
    
    // get message service and print out properties
    m_log = new MsgStream(messageService(), name());

    // get StoreGate
    StatusCode sc = m_storeGate.retrieve();

    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
        return sc;
    }

    // get HistSvc
    sc = service("THistSvc", m_histSvc);

    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve THistSvc!" << endreq;
        return sc;
    }

    // get TrigDecisionTool 
    sc = m_trigDec.retrieve();

    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
        return sc;
    }
    //enable TrigDecisionTool methods to retrieve chain/item pointers
    m_trigDecXmethods = m_trigDec->ExperimentalAndExpertMethods();
    m_trigDecXmethods->enable();

    if (m_doVeto && !m_vetoTools.size()) {
        sc = m_vetoTools.retrieve();
        //sc = m_evtVetoTool.retrieve();

        if ( sc.isFailure() ) {
            (*m_log) << MSG::ERROR << "Could not retrieve EvtVetoTool!" << endreq;
            return sc;
        }
    }

    if (m_doSlicesInfo) {
        sc = m_slicesInfoTool.retrieve();

        if ( sc.isFailure() ) {
            (*m_log) << MSG::ERROR << "Could not retrieve SlicesInfoTool!" << endreq;
            return sc;
        }
    }

    lvl_pnum.push_back(0.0);
    lvl_pnum.push_back(0.0);
    lvl_pnum.push_back(0.0);

    num_events = 0;
    resetResults();


    return StatusCode::SUCCESS;
}

/************/
/* Finalize */
/************/

StatusCode TriggerRateTools::finalize() {

    (*m_log) << MSG::INFO << "In finalize()" << endreq;
    
    // FIX: This should NOT be the case, config should be retrieved regardless... somehow
    if (doOnce) {
        (*m_log) << MSG::FATAL << "Trigger was not satisfied once, job failed due to rates calculations..." << endreq;
        return StatusCode::FAILURE;
    } else {
        for (int i = 0; i < (int)s_sigs_L1.size(); i++) {
            s_sigs_L1.at(i)->setNumEvts(num_events);
            sigs_L1_rate_hist->Fill(2*i+1,s_sigs_L1.at(i)->getRate());
            for (int j = 0; j < (int)s_sigs_L1.size(); j++) {
                sigs_L1_overlap_hist->Fill(i+1,j+1,s_sigs_L1.at(i)->getOverlapRateAt(j));
            }
        }
        for (int i = 0; i < (int)s_sigs_L2.size(); i++) {
            s_sigs_L2.at(i)->setNumEvts(num_events);
            sigs_L2_rate_hist->Fill(2*i+1,s_sigs_L2.at(i)->getRate());
            for (int j = 0; j < (int)s_sigs_L2.size(); j++) {
                sigs_L2_overlap_hist->Fill(i+1,j+1,s_sigs_L2.at(i)->getOverlapRateAt(j));
            }
        }
        for (int i = 0; i < (int)s_sigs_EF.size(); i++) {
            s_sigs_EF.at(i)->setNumEvts(num_events);
            sigs_EF_rate_hist->Fill(2*i+1,s_sigs_EF.at(i)->getRate());
            for (int j = 0; j < (int)s_sigs_EF.size(); j++) {
                sigs_EF_overlap_hist->Fill(i+1,j+1,s_sigs_EF.at(i)->getOverlapRateAt(j));
            }
        }
        for (int i = 0; i < (int)c_sigs_L1.size(); i++) {
            c_sigs_L1.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)c_sigs_L2.size(); i++) {
            c_sigs_L2.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)c_sigs_EF.size(); i++) {
            c_sigs_EF.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)menu_sigs_L1.size(); i++) {
            menu_sigs_L1.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)menu_sigs_L2.size(); i++) {
            menu_sigs_L2.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)menu_sigs_EF.size(); i++) {
            menu_sigs_EF.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)stream_sigs_L1.size(); i++) {
            stream_sigs_L1.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)stream_sigs_L2.size(); i++) {
            stream_sigs_L2.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)stream_sigs_EF.size(); i++) {
            stream_sigs_EF.at(i)->setNumEvts(num_events);
        }
        for (int i = 0; i < (int)l_sigs.size(); i++) {
            l_sigs.at(i)->setNumEvts(num_events);
        }


        TD_Signature *sig;
        TD_CplxSignature *csig;
        (*m_log) << MSG::INFO << "========== Trigger Rates Summary ==========" << endreq;
        for (int i = 0; i < (int) s_sigs_L1.size(); i++) {
            sig = s_sigs_L1.at(i);
            (*m_log) << MSG::INFO << *(sig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << sig->getNumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << sig->getRate() << " (+/- " << sig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) s_sigs_L2.size(); i++) {
            sig = s_sigs_L2.at(i);
            (*m_log) << MSG::INFO << *(sig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << sig->getNumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << sig->getRate() << " (+/- " << sig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) s_sigs_EF.size(); i++) {
            sig = s_sigs_EF.at(i);
            (*m_log) << MSG::INFO << *(sig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << sig->getNumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << sig->getRate() << " (+/- " << sig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) c_sigs_L1.size(); i++) {
            csig = c_sigs_L1.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) c_sigs_L2.size(); i++) {
            csig = c_sigs_L2.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) c_sigs_EF.size(); i++) {
            csig = c_sigs_EF.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) menu_sigs_L1.size(); i++) {
            csig = menu_sigs_L1.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) menu_sigs_L2.size(); i++) {
            csig = menu_sigs_L2.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) menu_sigs_EF.size(); i++) {
            csig = menu_sigs_EF.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) stream_sigs_L1.size(); i++) {
            csig = stream_sigs_L1.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) stream_sigs_L2.size(); i++) {
            csig = stream_sigs_L2.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        for (int i = 0; i < (int) stream_sigs_EF.size(); i++) {
            csig = stream_sigs_EF.at(i);
            (*m_log) << MSG::INFO << *(csig->getName());
            (*m_log) << MSG::INFO << "\t# evts passed: " << csig->getPnumPassed();
            (*m_log) << MSG::INFO << "\tRate: " << csig->getRate() << " (+/- " << csig->getRateErr() << ")" << endreq;
        }
        (*m_log) << MSG::INFO << "===========================================" << endreq;
     
        if (m_dumpText) {
            std::ofstream ratesFile ("rates.out");
            if (ratesFile.is_open()) {
                ratesFile << "------------------------------ Estimated Rates ------------------------------\n";
                for (int i = 0; i < (int) s_sigs_L1.size(); i++) {
                    sig = s_sigs_L1.at(i);
                    ratesFile << "\t-----" << *(sig->getName()) << "-----\n";
                    ratesFile << "\t\tLevel: " << *(sig->getLevel()) << "\n";
                    ratesFile << "\t\tPrescale: " << sig->getPrescale() << "\n";
                    ratesFile << "\t\t# evts passed: " << sig->getNumPassed() << "\n";
                    ratesFile << "\t\tEfficiency: " << sig->getEff() << " +/- " << sig->getEffErr() <<"\n";
                    ratesFile << "\t\tRate: " << sig->getRate() << " +/- " << sig->getRateErr() <<"\n";
                    ratesFile << "\t\tIndep frac.: " << sig->getIndepFrac() <<"\n";
                    ratesFile << "\t\tUnique Rate: " << sig->getUniqueRate() << " +/- \n";
                    ratesFile << "\n";
                }
                for (int i = 0; i < (int) s_sigs_L2.size(); i++) {
                    sig = s_sigs_L2.at(i);
                    ratesFile << "\t-----" << *(sig->getName()) << "-----\n";
                    ratesFile << "\t\tLevel: " << *(sig->getLevel()) << "\n";
                    ratesFile << "\t\tPrescale: " << sig->getPrescale() << "\n";
                    ratesFile << "\t\t# evts passed: " << sig->getNumPassed() << "\n";
                    ratesFile << "\t\tEfficiency: " << sig->getEff() << " +/- " << sig->getEffErr() <<"\n";
                    ratesFile << "\t\tRate: " << sig->getRate() << " +/- " << sig->getRateErr() <<"\n";
                    ratesFile << "\t\tIndep frac.: " << sig->getIndepFrac() <<"\n";
                    ratesFile << "\t\tUnique Rate: " << sig->getUniqueRate() << " +/- \n";
                    ratesFile << "\n";
                }
                for (int i = 0; i < (int) s_sigs_EF.size(); i++) {
                    sig = s_sigs_EF.at(i);
                    ratesFile << "\t-----" << *(sig->getName()) << "-----\n";
                    ratesFile << "\t\tLevel: " << *(sig->getLevel()) << "\n";
                    ratesFile << "\t\tPrescale: " << sig->getPrescale() << "\n";
                    ratesFile << "\t\t# evts passed: " << sig->getNumPassed() << "\n";
                    ratesFile << "\t\tEfficiency: " << sig->getEff() << " +/- " << sig->getEffErr() <<"\n";
                    ratesFile << "\t\tRate: " << sig->getRate() << " +/- " << sig->getRateErr() <<"\n";
                    ratesFile << "\t\tIndep frac.: " << sig->getIndepFrac() <<"\n";
                    ratesFile << "\t\tUnique Rate: " << sig->getUniqueRate() << " +/- \n";
                    ratesFile << "\n";
                }
                ratesFile << "-----------------------------------------------------------------------------";

                ratesFile.close();
            }
        }

        s_sigs_tree->Fill();
        c_sigs_tree->Fill();
        menu_sigs_tree->Fill();
        stream_sigs_tree->Fill();
        l_sigs_tree->Fill();
        
        //m_ignore.clear();
        //m_cplx_or.clear();
        //m_cplx_and.clear();
        //m_sort.clear();
        //delete m_log; m_log = 0;
    }
    
    return StatusCode::SUCCESS;
}

/***********/
/* Execute */
/***********/

StatusCode TriggerRateTools::execute() {
    
    const DataHandle<EventInfo> evt;
    StatusCode sc = m_storeGate->retrieve(evt);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve evt from StoreGate!" << endreq;
        return sc;
    }

    evt_num = evt->event_ID()->event_number();
    
    if (doOnce) {

	//open a file to print out trigger items 
	std::ofstream itemsFile ("triggerItems.out");;	


        int count = 0;
        std::string sig_name;
        
        L1Items = m_trigDec->getChainGroup("L1_.*");
	std::vector<std::string> L1Trigs = L1Items->getListOfTriggers();
        (*m_log) << MSG::DEBUG << "# L1 Items : " << !L1Trigs.empty() << endreq; //FIX: return size

        if ( !L1Trigs.empty() ) {
	    std::vector<std::string>::const_iterator aItem;
	    for (aItem = L1Trigs.begin(); aItem != L1Trigs.end(); ++aItem) {
		(*m_log) << MSG::DEBUG << "string (*aItem).size(): " << (*aItem).size() << endreq; 
                if (!(*aItem).empty() && (*aItem).size()<200) { //check pointer not null
                    sig_name=(std::string)(*aItem);
		    if (!sig_name.empty()) {
			if (m_doRetrieveItems && itemsFile.is_open()) itemsFile << sig_name << "\n";
			(*m_log) << MSG::DEBUG << "- sig_name = " << sig_name << ", passedBits : " << m_trigDec->isPassedBits(sig_name) << endreq;

                        count += (int)(m_trigDec->isPassedBits(sig_name) & TrigDefs::L1_isPassedBeforePrescale);
		    } //endif sig_name empty
                } //endif 
            } //endfor
        } else {
            (*m_log) << MSG::WARNING << "List of L1 Items appears to be empty in event #" << evt_num << "..." << endreq;
        }

        // Work on L2 chains
        L2Chains = m_trigDec->getChainGroup("L2_.*");
	std::vector<std::string> L2Trigs = L2Chains->getListOfTriggers();
        (*m_log) << MSG::DEBUG << "# L2 Chains : " << !L2Trigs.empty() << endreq;  //fix return size

        if ( !L2Trigs.empty() ) {
	    std::vector<std::string>::const_iterator aChain;
            for (aChain = L2Trigs.begin(); aChain != L2Trigs.end(); ++aChain) {
                (*m_log) << MSG::DEBUG << "L2 (*aChain).size(): " << (*aChain).size() << endreq;
                if (!(*aChain).empty() && (*aChain).size()<200) {
                    sig_name=(std::string)(*aChain);
 		    if (!sig_name.empty()) {
			if (m_doRetrieveItems && itemsFile.is_open()) itemsFile << sig_name << "\n";
			(*m_log) << MSG::DEBUG << "- sig_name = " << sig_name << ", passedBits : " << m_trigDec->isPassedBits(sig_name) << endreq;
                        count += (int)( (m_trigDec->isPassedBits(sig_name) & TrigDefs::L2_passedRaw) || (!m_ignorePassThrough && m_trigDec->isPassed(sig_name, TrigDefs::passedThrough)) );
                    }
                } else {
                    (*m_log) << MSG::DEBUG << "Chain is null!" << endreq;
                    continue;
                }

            }
        } else {
            (*m_log) << MSG::WARNING << "Could not retrieve HLT Chains (L2) !!" << endreq;
        }

        // Work on EF chains
        EFChains = m_trigDec->getChainGroup("EF_.*");
	std::vector<std::string> EFTrigs = EFChains->getListOfTriggers();
        (*m_log) << MSG::DEBUG << "# EF Chains : " << !EFTrigs.empty() << endreq;  //fix return size

        if ( !EFTrigs.empty() ) {
	    std::vector<std::string>::const_iterator aChain;
            for (aChain = EFTrigs.begin(); aChain != EFTrigs.end(); ++aChain) {
		(*m_log) << MSG::DEBUG << "EF (*aChain).size(): " << (*aChain).size() << endreq;

                if (!(*aChain).empty() && (*aChain).size()<200) {
                    sig_name=(std::string)(*aChain);
		    if (!sig_name.empty()) {
			if (m_doRetrieveItems && itemsFile.is_open()) itemsFile << sig_name  << "\n";
			(*m_log) << MSG::DEBUG << "- sig_name = " << sig_name << ", passedBits : " << m_trigDec->isPassedBits(sig_name) << endreq;
                        count += (int)( (m_trigDec->isPassedBits(sig_name) & TrigDefs::EF_passedRaw) || (!m_ignorePassThrough && m_trigDec->isPassed(sig_name, TrigDefs::passedThrough)) );
                    }

                } else {
                    (*m_log) << MSG::DEBUG << "Chain is null!" << endreq;
                    continue;
                } 
            }
        } else {
            (*m_log) << MSG::WARNING << "Could not retrieve HLT Chains (EF) !!" << endreq;
        }

	if (itemsFile.is_open()) itemsFile.close();

        if (count > 0) {
            sc = prepare();
	    if ( sc.isFailure() ) {
        	(*m_log) << MSG::ERROR << "Could not prepare TriggerRateTools ..." << endreq;
        	return sc;
    	    }
            doOnce=false;
        } else {
            (*m_log) << MSG::WARNING << "Could not prepare TriggerRateTools..." << endreq;
        }
            
    }

    //if (!m_doVeto || m_evtVetoTool->isPassed()) {
        if (!doOnce) {
            if (m_doVeto) {
                bool veto_res = true;
                for (unsigned int i = 0; i < m_vetoTools.size() && veto_res; i++) { 
                    veto_res = (m_vetoTools[i])->isPassed();
                }
                applyVeto("L1", veto_res);
                applyVeto("L2", veto_res);
                applyVeto("EF", veto_res);
            }
            retrieveResults();
            if (fillSingleRates("L1") >= 1) {
                fillCplxRates("L1");
            }
            if (fillSingleRates("L2") >= 1) {
                fillCplxRates("L2");
            }
            if (fillSingleRates("EF") >= 1) {
                fillCplxRates("EF");
            }
        }
        
        sc = endEvent();
        if ( sc.isFailure() ) {                
		(*m_log) << MSG::ERROR << "in endEvent()" << endreq;
                return sc;            
	}

        
        num_events++;
        fprintf(stderr,"Processed event #%i : event number %i\n", num_events, evt_num);
    //}
    
    return StatusCode::SUCCESS;
}

/* Run only once at the beginning of job */
StatusCode TriggerRateTools::prepare() {
    
    (*m_log) << MSG::INFO << "Preparing TriggerRateTools..." << endreq;
    //m_file = new TFile("TriggerRateTools.root","RECREATE");
    and_or_change_index.push_back(-1);
    and_or_change_index.push_back(-1);
    and_or_change_index.push_back(-1);
    num_sigs = 0;
    std::string sig_name;
   
     /* Create and register the TTree(s) */
    raw_TD = new TTree("raw_TD", "Raw TrigDecision Results");
    StatusCode sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + raw_TD->GetName(), raw_TD);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register raw_TD Tree" << endreq;
        return sc;
    }
    raw_TD->Branch("EventNumber", &evt_num, "evt_num/I");
    raw_PS = new TTree("raw_PS", "Raw isPrescaled Results");
    
    sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + raw_PS->GetName(), raw_PS);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register raw_PS Tree" << endreq;
        return sc;
    }
    raw_PS->Branch("EventNumber", &evt_num, "evt_num/I");

    s_sigs_tree = new TTree("SingleSignatures", "Single Signatures");
    sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + s_sigs_tree->GetName(), s_sigs_tree);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register s_sigs_tree Tree" << endreq;
        return sc;
    }

    c_sigs_tree = new TTree("CplxSignatures", "Complex Signatures");
    sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + c_sigs_tree->GetName(), c_sigs_tree);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register c_sigs_tree Tree" << endreq;
        return sc;
    }

    menu_sigs_tree = new TTree("Menus", "Menus");
    sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + menu_sigs_tree->GetName(), menu_sigs_tree);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register menu_sigs_tree Tree" << endreq;
        return StatusCode::FAILURE;
    }

    stream_sigs_tree = new TTree("Streams", "Streams");
    sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + stream_sigs_tree->GetName(), stream_sigs_tree);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register stream_sigs_tree Tree" << endreq;
        return StatusCode::FAILURE;
    }

    l_sigs_tree = new TTree("Levels", "Level Signatures");
    sc = m_histSvc->regTree(std::string("/TriggerRateTools/") + l_sigs_tree->GetName(), l_sigs_tree);
    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not register l_sigs_tree Tree" << endreq;
        return StatusCode::FAILURE;
    }

    bool do_ignore = false;
    double tmp_prescale = 1.0;

    const ChainGroup* all_L1Items = m_trigDec->getChainGroup("L1_.*");
    std::vector<std::string> L1Trigs = all_L1Items->getListOfTriggers();
    (*m_log) << MSG::DEBUG << "retrieved configured L1 items" << endreq;
    if ( !L1Trigs.empty() ) {

	std::vector<std::string>::const_iterator aItem;
        for (aItem = L1Trigs.begin(); aItem != L1Trigs.end(); ++aItem) {

            if (!(*aItem).empty() && (*aItem).size()<200) {
                sig_name=(std::string)(*aItem);
		if (!sig_name.empty()) {
                    do_ignore = false;
                    for (int i = 0; i < (int)m_ignore.size(); i++) {
                        if (sig_name.compare(m_ignore.at(i)) == 0) {
                            do_ignore = true;
                        }
                    }
                    if (!do_ignore) {
                        (*m_log) << MSG::DEBUG << "Adding item " << sig_name << " to configuration" << endreq;
                        tmp_prescale = m_trigDec->getPrescale(sig_name);
                        if (tmp_prescale <= 0) {
     			    	(*m_log) << MSG::WARNING << "Trigger " << sig_name << " was reported to have a fullChainPrescale of" << tmp_prescale << ", setting it to 1 instead" << endreq;
        			tmp_prescale = 1;
    			}

			for (int j = 0; j < (int)m_psOverride.size(); j++) {
                            	if ((m_psOverride.at(j)).size() == 2) {
                                	if (TString(sig_name).CompareTo(TString( (m_psOverride.at(j)).at(0) )) == 0) {
                                    		tmp_prescale = (strtod(((m_psOverride.at(j)).at(1)).c_str(), NULL));
                                	    	(*m_log) << MSG::DEBUG << "Overriding " << sig_name << " prescale with new value of " << (m_psOverride.at(j)).at(1) << endreq; 
                                	}
                            	}
                        }
                       	 addSig(sig_name, "L1", tmp_prescale, 0.0, "n/a", "n/a");
			(*m_log) << MSG::DEBUG << "Item added successfully" << endreq;
			tmp_prescale=1.0;
                    } else {
                        (*m_log) << MSG::DEBUG << "Ignoring item " << sig_name << endreq;
                    }
                }
            } else {
                (*m_log) << MSG::DEBUG << "Item is null!" << endreq;
                continue;
            }

        }
    } else {
        (*m_log) << MSG::WARNING << "List of L1 Items appears to be empty..." << endreq;
    }

    const ChainGroup* L2EFChains = m_trigDec->getChainGroup("L2_.*, EF_.*");
    std::vector<std::string> L2EFTrigs = L2EFChains->getListOfTriggers();

    if ( !L2EFTrigs.empty() ) {
	std::vector<std::string>::const_iterator aChain;
        for (aChain = L2EFTrigs.begin(); aChain != L2EFTrigs.end(); ++aChain) {

            if (!(*aChain).empty() && (*aChain).size()<200) {
                sig_name=(std::string)(*aChain);
		if (!sig_name.empty()) {
                    do_ignore = false;
                    for (int i = 0; i < (int)m_ignore.size(); i++) {
                        if (sig_name.compare(m_ignore.at(i)) == 0) {
                            do_ignore = true;
                        }
                    }
                    if (!do_ignore) {
                        (*m_log) << MSG::DEBUG << "Adding chain " << sig_name << " to configuration" << endreq;
                        tmp_prescale = m_trigDec->getPrescale(sig_name);
                        if (tmp_prescale <= 0) {
                                (*m_log) << MSG::WARNING << "Trigger " << sig_name << " was reported to have a fullChainPrescale of" << tmp_prescale << ", setting it to 1 instead" << endreq;
                                tmp_prescale = 1;
                        }

			for (int j = 0; j < (int)m_psOverride.size(); j++) {
                            if ((m_psOverride.at(j)).size() == 2) {
                                if (TString(sig_name).CompareTo(TString( (m_psOverride.at(j)).at(0) )) == 0) {
                                    tmp_prescale = (strtod(((m_psOverride.at(j)).at(1)).c_str(), NULL));
                                    (*m_log) << MSG::DEBUG << "Overriding " << sig_name << " prescale with new value of " << (m_psOverride.at(j)).at(1) << endreq; 
                                }
                            }
                        }
			//get a pointer to the HLT::Chain objects with TDT expert methods 
			std::string parent_name = "";
			std::string Gparent_name = "";
			const TrigConf::HLTChain *cchain = m_trigDecXmethods->getChainConfigurationDetails(sig_name);  //new
			//get parent chain name
			if (cchain!=0) {
				parent_name = cchain->lower_chain_name();
				//get grand-parent chain name for EF chains
				if (!parent_name.empty() && cchain->level()=="EF") {
					const TrigConf::HLTChain *cchainParent = m_trigDecXmethods->getChainConfigurationDetails(parent_name);  //new
					if (cchainParent!=0) Gparent_name = cchainParent->lower_chain_name();
					else (*m_log) << MSG::DEBUG << "Trigger chain " << parent_name << " parent not retrieved " << endreq;
				}
			} 
			else (*m_log) << MSG::DEBUG << "Trigger chain " << sig_name << " parent not retrieved " << endreq;
			
			addSig(sig_name, cchain->level(), tmp_prescale, (m_ignorePassThrough)?(0.0):((double)cchain->pass_through()), parent_name, Gparent_name );
			(*m_log) << MSG::DEBUG << "Chain added successfully" << endreq;
			tmp_prescale=1.0;

                    } else {
                        (*m_log) << MSG::DEBUG << "Ignoring chain " << sig_name << endreq;
                    }
                }
            } else {
                (*m_log) << MSG::DEBUG << "Item is null!" << endreq;
                continue;
            }

        }
    } else {
        (*m_log) << MSG::WARNING << "List of HLT Chains appears to be empty..." << endreq;
    }

    sigs_L1_rate_hist = new TH1F("L1_rates","L1 individual trigger rates",2*s_sigs_L1.size(),0.,2*((float)s_sigs_L1.size())+2);
    m_histSvc->regHist("/TriggerRateTools/L1_rates",sigs_L1_rate_hist).ignore();
    sigs_L2_rate_hist = new TH1F("L2_rates","L2 individual trigger rates",2*s_sigs_L2.size(),0.,2*((float)s_sigs_L2.size())+2);
    m_histSvc->regHist("/TriggerRateTools/L2_rates",sigs_L2_rate_hist).ignore();
    sigs_EF_rate_hist = new TH1F("EF_rates","EF individual trigger rates",2*s_sigs_EF.size(),0.,2*((float)s_sigs_EF.size())+2);
    m_histSvc->regHist("/TriggerRateTools/EF_rates",sigs_EF_rate_hist).ignore();

    sigs_L1_overlap_hist = new TH2F("L1_overlaps","L1 individual trigger overlap rates",s_sigs_L1.size(),0.,((float)s_sigs_L1.size()),s_sigs_L1.size()+2,0.,((float)s_sigs_L1.size()+2));
    m_histSvc->regHist("/TriggerRateTools/L1_overlaps",sigs_L1_overlap_hist).ignore();                                                                                                  
    sigs_L2_overlap_hist = new TH2F("L2_overlaps","L2 individual trigger overlap rates",s_sigs_L2.size(),0.,((float)s_sigs_L2.size()),s_sigs_L2.size()+2,0.,((float)s_sigs_L2.size()+2));
    m_histSvc->regHist("/TriggerRateTools/L2_overlaps",sigs_L2_overlap_hist).ignore();                                                                                                  
    sigs_EF_overlap_hist = new TH2F("EF_overlaps","EF individual trigger overlap rates",s_sigs_EF.size(),0.,((float)s_sigs_EF.size()),s_sigs_EF.size()+2,0.,((float)s_sigs_EF.size()+2));
    m_histSvc->regHist("/TriggerRateTools/EF_overlaps",sigs_EF_overlap_hist).ignore();

    initializeSigFeatures("L1");
    initializeSigFeatures("L2");
    initializeSigFeatures("EF");
    initializeNtuple("L1");
    initializeNtuple("L2");
    initializeNtuple("EF");
    initializeHistos("L1");
    initializeHistos("L2");
    initializeHistos("EF");
    
    if (m_doSlicesInfo) {
        sc = m_slicesInfoTool->bookSlices();
	if ( sc.isFailure() ) {
	        (*m_log) << MSG::ERROR << "Could not book Slices in TriggerRateTools ..." << endreq;
        	return sc;
    	}
    }

    
    return StatusCode::SUCCESS;
}

/* Store TrigDecision results in vectors for further manipulation */
void TriggerRateTools::retrieveResults() {
    (*m_log) << MSG::INFO << "Retrieving Trigger results" << endreq;
    //(*m_log) << MSG::INFO << "Pass state = " << m_trigDec->isPassed() << endreq;
    //(*m_log) << MSG::INFO << "Pass state L1 = " << m_trigDec->isPassed(L1) << endreq;
    //(*m_log) << MSG::INFO << "Pass state L2 = " << m_trigDec->isPassed(L2) << endreq;
    //(*m_log) << MSG::INFO << "Pass state EF = " << m_trigDec->isPassed(EF) << endreq;

    int temp_index = 0;

    // Work on L1 items

    num_sigs = 0;
    std::string sig_name;
    bool do_ignore = false;
   
    L1Items = m_trigDec->getChainGroup("L1_.*");
    std::vector<std::string> L1Trigs = L1Items->getListOfTriggers(); 
    (*m_log) << MSG::DEBUG << "# L1 Items : " << !L1Trigs.empty() << endreq;
    if ( !L1Trigs.empty() ) {

	std::vector<std::string>::const_iterator aItem;
        for (aItem = L1Trigs.begin(); aItem != L1Trigs.end(); ++aItem) {

            if (!(*aItem).empty()) {  //fix
                sig_name=(std::string)(*aItem);
		(*m_log) << MSG::DEBUG << "Item " << sig_name << " : " << *aItem << endreq;//(m_trigDec->isPassedBits(sig_name)&TrigDefs::L1_isPassedBeforePrescale) << endreq;
                if (!sig_name.empty()) {
                    do_ignore = false;
                    for (int i = 0; i < (int)m_ignore.size(); i++) {
                        if (sig_name.compare(m_ignore.at(i)) == 0) {
                            do_ignore = true;
                        }
                    }
                    if (!do_ignore) {
                        (*m_log) << MSG::DEBUG << "Item " << sig_name << " : " << (( m_trigDec->isPassedBits(sig_name) & TrigDefs::L1_isPassedBeforePrescale )>0) << endreq;
                        temp_index = findSigIndex(sig_name,"L1");
                        sigs_L1_res[temp_index-1] = (int)((m_trigDec->isPassedBits(sig_name) & TrigDefs::L1_isPassedBeforePrescale)>0);
                        sigs_L1_ps[temp_index-1] = (int)(m_trigDec->getPrescale(sig_name)==1);
                        num_sigs++;
                    }
                }
            } else {
                (*m_log) << MSG::DEBUG << "Item is null!" << endreq;
                continue;
            }

        }
    } else {
        (*m_log) << MSG::WARNING << "List of L1 Items appears to be empty..." << endreq;
    }

    
    // Work on L2 chains
    L2Chains = m_trigDec->getChainGroup("L2_.*");
    std::vector<std::string> L2Trigs = L2Chains->getListOfTriggers();
    (*m_log) << MSG::DEBUG << "# L2 Chains : " << !L2Trigs.empty() << endreq;
    if ( !L2Trigs.empty() ) {

	std::vector<std::string>::const_iterator aChain;
        for (aChain = L2Trigs.begin(); aChain != L2Trigs.end(); ++aChain) {

            if (!(*aChain).empty()) { //fix
                sig_name=(std::string)(*aChain);
		if (!sig_name.empty()) {
                    do_ignore = false;
                    for (int i = 0; i < (int)m_ignore.size(); i++) {
                        if (sig_name.compare(m_ignore.at(i)) == 0) {
                            do_ignore = true;
                        }
                    }
                    if (!do_ignore) {
                        temp_index = findSigIndex(sig_name,"L2");
                        sigs_L2_res[temp_index-1] = (int)( (m_trigDec->isPassedBits(sig_name) & TrigDefs::L2_passedRaw)>0 || (!m_ignorePassThrough && m_trigDec->isPassed(sig_name, TrigDefs::passedThrough)) );
                        sigs_L2_ps[temp_index-1] = (int)(m_trigDec->getPrescale(sig_name)==1);
                        (*m_log) << MSG::DEBUG << sig_name << " : " << (int)( (m_trigDec->isPassedBits(sig_name) & TrigDefs::L2_passedRaw)>0 || (!m_ignorePassThrough && m_trigDec->isPassed(sig_name, TrigDefs::passedThrough)) )<< endreq;
                        num_sigs++;
                    }
                }

            } else {
                (*m_log) << MSG::DEBUG << "Chain is null!" << endreq;
                continue;
            }

        }
    } else {
        (*m_log) << MSG::WARNING << "Could not retrieve HLT Chains (L2) !!" << endreq;
    }

    // Work on EF chains
    EFChains = m_trigDec->getChainGroup("EF_.*");
    std::vector<std::string> EFTrigs = EFChains->getListOfTriggers();
    (*m_log) << MSG::DEBUG << "# EF Chains : " << !EFTrigs.empty() << endreq;
    if ( !EFTrigs.empty() ) {

	std::vector<std::string>::const_iterator aChain;
        for (aChain = EFTrigs.begin(); aChain != EFTrigs.end(); ++aChain) {

            if (!(*aChain).empty()) { //fix
                sig_name=(std::string)(*aChain);
		if (!sig_name.empty()) {
                    do_ignore = false;
                    for (int i = 0; i < (int)m_ignore.size(); i++) {
                        if (sig_name.compare(m_ignore.at(i)) == 0) {
                            do_ignore = true;
                        }
                    }
                    if (!do_ignore) {
                        temp_index = findSigIndex(sig_name,"EF");
                        sigs_EF_res[temp_index-1] = (int)( (m_trigDec->isPassedBits(sig_name) & TrigDefs::EF_passedRaw)>0 || (!m_ignorePassThrough && m_trigDec->isPassed(sig_name, TrigDefs::passedThrough)) );
                        sigs_EF_ps[temp_index-1] = (int)(m_trigDec->getPrescale(sig_name)==1);
                        (*m_log) << MSG::DEBUG << sig_name << " : " << (int)( (m_trigDec->isPassedBits(sig_name) & TrigDefs::EF_passedRaw)>0 || (!m_ignorePassThrough && m_trigDec->isPassed(sig_name, TrigDefs::passedThrough)) )<< endreq;
                        num_sigs++;
                    }
                }

            } else {
                (*m_log) << MSG::DEBUG << "Chain is null!" << endreq;
                continue;
            } 
        }
    } else {
        (*m_log) << MSG::WARNING << "Could not retrieve HLT Chains (EF) !!" << endreq;
    }

}

/* Runs the end of each event */
StatusCode TriggerRateTools::endEvent() {
    (*m_log) << MSG::DEBUG << "in endEvent()" << endreq;
    
    if (m_doRawTD) {
        if (raw_TD) { raw_TD->Fill(); }
        if (raw_PS) { raw_PS->Fill(); }
    }

    if (m_doSlicesInfo) {
        if (m_slicesInfoTool) {
		StatusCode sc = m_slicesInfoTool->fillSlices();
		if ( sc.isFailure() ) {
        		(*m_log) << MSG::ERROR << "Unable to fill slices info. Please disable slicesInfoTool in JobOptions!" << endreq;
        		return sc;
    		}

	}
    }

    resetResults();

    return StatusCode::SUCCESS;
}

/* Prepare temp result containers for the next event processing */
void TriggerRateTools::resetResults() {
    (*m_log) << MSG::DEBUG << "in resetResults()" << endreq;

    num_sigs = 0;
    (*m_log) << MSG::DEBUG << "Resetting results containers" << endreq;
    for(int i = 0; i < (int) sigs_L1_res.size(); i++) {
        sigs_L1_res[i] = 0;
        sigs_L1_ps[i] = 0;
    }
    for(int i = 0; i < (int) sigs_L2_res.size(); i++) {
        sigs_L2_res[i] = 0;
        sigs_L2_ps[i] = 0;
    }
    for(int i = 0; i < (int) sigs_EF_res.size(); i++) {
        sigs_EF_res[i] = 0;
        sigs_EF_ps[i] = 0;
    }
    
    lvl_pnum[0] = 0.0;
    lvl_pnum[1] = 0.0;
    lvl_pnum[2] = 0.0;
    
}

/* Retrieves the location in the array that corresponds to the item/chain *
 * If not present, then a new entry is created                            */
int TriggerRateTools::findSigIndex(std::string name, std::string lvl) {
    
    (*m_log) << MSG::DEBUG << "Seeking " << name << " in array" << endreq;
    
    std::vector<std::string> *sigs_name;
    
    if (lvl.compare("L1") == 0) {
        
        sigs_name = &sigs_L1_name;

    } else if (lvl.compare("L2") == 0) {
        
        sigs_name = &sigs_L2_name;

    } else if (lvl.compare("EF") == 0) {
        
        sigs_name = &sigs_EF_name;

    } else { return 0; }

    
    // Look for array index for item/chain
    if (name.at(0) == '!') {
        for (int i = 0; i < (int)sigs_name->size(); i++) {
            if ((sigs_name->at(i)).compare(name.substr(1)) == 0) {
                (*m_log) << MSG::DEBUG << name << " found at index " << -i << endreq;
                return -i-1;
            }
        }
    } else {
        for (int i = 0; i < (int)sigs_name->size(); i++) {
            if ((sigs_name->at(i)).compare(name) == 0) {
                (*m_log) << MSG::DEBUG << name << " found at index " << i << endreq;
                return i+1;
            }
        }
    }

    return 0;
}

/* Add Signature to the configuration */
void TriggerRateTools::addSig(std::string name, std::string lvl, double prescale, double passthrough, std::string L1parentName, std::string L2parentName) {

    (*m_log) << MSG::DEBUG << "in addSig() :" << endreq;

    int lvl_index = 0;
    std::vector<TD_Signature*> *s_sigs = NULL;
    std::vector<Int_t> *sigs_res = NULL;
    std::vector<Int_t> *sigs_ps = NULL;
    std::vector<double> *sigs_p = NULL;
    std::vector<std::string> *sigs_name = NULL;
 
    if (lvl.compare("L1") == 0) {
        
        s_sigs = &s_sigs_L1;
        sigs_res = &sigs_L1_res;
        sigs_ps = &sigs_L1_ps;
        sigs_p = &sigs_L1_p;
        sigs_name = &sigs_L1_name;
        lvl_index = 0;

    } else if (lvl.compare("L2") == 0) {
        
        s_sigs = &s_sigs_L2;
        sigs_res = &sigs_L2_res;
        sigs_ps = &sigs_L2_ps;
        sigs_p = &sigs_L2_p;
        sigs_name = &sigs_L2_name;
        lvl_index = 1;

    } else if (lvl.compare("EF") == 0) {
        
        s_sigs = &s_sigs_EF;
        sigs_res = &sigs_EF_res;
        sigs_ps = &sigs_EF_ps;
        sigs_p = &sigs_EF_p;
        sigs_name = &sigs_EF_name;
        lvl_index = 2;

    }

   (*m_log) << MSG::DEBUG << "Chain parents: " << L1parentName << "," << L2parentName << endreq;
    
    sigs_name->push_back(name);
    sigs_res->push_back(0);
    sigs_ps->push_back(0);
    sigs_p->push_back(prescale);

    if (prescale <= 0) {
         (*m_log) << MSG::WARNING << "Trigger " << name << " was reported to have a fullChainPrescale of" << prescale << ", setting it to 1 instead" << endreq;
        prescale = 1;
    }

    s_sigs->push_back(new TD_Signature(name, lvl, prescale, passthrough));
    s_sigs->at(s_sigs->size()-1)->setXsec(m_xsec);
    s_sigs->at(s_sigs->size()-1)->setLumi(m_lumi);
    s_sigs->at(s_sigs->size()-1)->setNColls(m_NColls);
    s_sigs->at(s_sigs->size()-1)->setL1Parent(L1parentName);
    s_sigs->at(s_sigs->size()-1)->setL2Parent(L2parentName);
}    

/* Create complex signature objects */
void TriggerRateTools::initializeSigFeatures(std::string lvl) {

    (*m_log) << MSG::DEBUG << "in initializeSigFeatures(): " << lvl << endreq;    

    int lvl_index = 0;
    std::vector<TD_Signature*> *s_sigs = NULL;
    std::vector<Int_t> *sigs_res = NULL;
    std::vector<std::string> *sigs_name = NULL;
    std::vector<TD_CplxSignature*> *c_sigs = NULL;
    std::vector<TD_CplxSignature*> *menu_sigs = NULL;
    std::vector<TD_CplxSignature*> *stream_sigs = NULL;
    
    if (lvl.compare("L1") == 0) {
        
        s_sigs = &s_sigs_L1;
        sigs_res = &sigs_L1_res;
        sigs_name = &sigs_L1_name;
        c_sigs = &c_sigs_L1;
        menu_sigs = &menu_sigs_L1;
        stream_sigs = &stream_sigs_L1;
        lvl_index = 0;

    } else if (lvl.compare("L2") == 0) {
        
        s_sigs = &s_sigs_L2;
        sigs_res = &sigs_L2_res;
        sigs_name = &sigs_L2_name;
        c_sigs = &c_sigs_L2;
        menu_sigs = &menu_sigs_L2;
        stream_sigs = &stream_sigs_L2;
        lvl_index = 1;

    } else if (lvl.compare("EF") == 0) {
        
        s_sigs = &s_sigs_EF;
        sigs_res = &sigs_EF_res;
        sigs_name = &sigs_EF_name;
        c_sigs = &c_sigs_EF;
        menu_sigs = &menu_sigs_EF;
        stream_sigs = &stream_sigs_EF;
        lvl_index = 2;

    }
 
    //Add overlaps to single sigs
    
    for (int i = 0; i < (int)s_sigs->size(); i++) {
        for (int j = 0; j < (int)s_sigs->size(); j++) {
            s_sigs->at(i)->addOverlap(*(s_sigs->at(j)->getName()));
        }
        for (int j = 0; j < (int)m_psOverride.size(); j++) {
            if ((m_psOverride.at(j)).size() == 2) {
                if ((s_sigs->at(i)->getName())->CompareTo(TString( (m_psOverride.at(j)).at(0) )) == 0) {
                    s_sigs->at(i)->setPrescale(strtod(((m_psOverride.at(j)).at(1)).c_str(), NULL));
                    (*m_log) << MSG::DEBUG << "Overriding " << *(s_sigs->at(i)->getName()) << " prescale with new value of " << (m_psOverride.at(j)).at(1) << endreq; 
                }
            }
        }
    }
    
    //Create 'AND' cplx sigs
    
    std::string name;
    int unnamed = 0;
    int sig_index = -1;
    //bool isWindow = false;
    for (int i = 0; i < (int)m_cplx_and.size(); i++) {
        if ((int)(m_cplx_and.at(i)).size() > 3) {
            if (((m_cplx_and.at(i)).at(1)).compare(lvl) == 0) {
                name = (m_cplx_and.at(i)).at(0);
                if (name.empty()) {
                    name = std::string("cplx_and_"+unnamed);
                    unnamed++;
                }

                c_sigs->push_back(new TD_CplxSignature(name, "And", lvl));
                c_sigs->at(c_sigs->size()-1)->setXsec(m_xsec);
                c_sigs->at(c_sigs->size()-1)->setLumi(m_lumi);
                c_sigs->at(c_sigs->size()-1)->setNColls(m_NColls);
                
                for (int j = 2; j < (int)(m_cplx_and.at(i)).size(); j++) {
                    sig_index = findSigIndex((m_cplx_and.at(i)).at(j),lvl);
                    if (sig_index < 0) {
                        c_sigs->at(c_sigs->size()-1)->setType("Window");
                        (*m_log) << MSG::DEBUG << "Type of CplxSignature changed to Window due to " << (m_cplx_and.at(i)).at(j) << endreq;
                        c_sigs->at(c_sigs->size()-1)->addSig((m_cplx_and.at(i)).at(j),sig_index, 1.0);
                    }
                    if (sig_index > 0) {
                        c_sigs->at(c_sigs->size()-1)->addSig((m_cplx_and.at(i)).at(j),sig_index-1, 1.0);
                    }
                }
                
                if (c_sigs->at(c_sigs->size()-1)->getSigIndicesSize() < 1) {
                    c_sigs->pop_back();
                } else {
                    ((c_sigs->at(c_sigs->size()-1))->getSigsBitSet())->Compact();
                }
            }
        }
    }

    and_or_change_index[lvl_index] = c_sigs->size();
    
    //Create 'OR' cplx sigs

    // Prescaled and Unprescaled
    
    c_sigs->push_back(new TD_CplxSignature("Unprescaled_"+lvl, "Or", lvl));
    c_sigs->push_back(new TD_CplxSignature("Prescaled_"+lvl, "Or", lvl));
    c_sigs->at(c_sigs->size()-1)->setXsec(m_xsec);
    c_sigs->at(c_sigs->size()-2)->setXsec(m_xsec);
    c_sigs->at(c_sigs->size()-1)->setNColls(m_NColls);
    c_sigs->at(c_sigs->size()-2)->setNColls(m_NColls);
    c_sigs->at(c_sigs->size()-1)->setLumi(m_lumi);
    c_sigs->at(c_sigs->size()-2)->setLumi(m_lumi);
    c_sigs->at(c_sigs->size()-1)->setDoUnique(true);
    c_sigs->at(c_sigs->size()-2)->setDoUnique(true);
    for (int i = 0; i < (int)s_sigs->size(); i++) {
        if (s_sigs->at(i)->getPrescale() == 1) {
            c_sigs->at(c_sigs->size()-2)->addSig(std::string((s_sigs->at(i)->getName())->Data()),i, 1.0);
        } else {
            c_sigs->at(c_sigs->size()-1)->addSig(std::string((s_sigs->at(i)->getName())->Data()),i, 1.0);
        }
    }

    ((c_sigs->at(c_sigs->size()-1))->getSigsBitSet())->Compact();
    ((c_sigs->at(c_sigs->size()-2))->getSigsBitSet())->Compact();

    //List of items and groups and streams for the particular level 
    const ChainGroup* allItems;
    const ChainGroup* oneItem;
    std::string item_lvl = lvl+std::string("_.*");
    allItems = m_trigDec->getChainGroup(item_lvl);
    std::vector<std::string> allTrigs = allItems->getListOfTriggers();
    std::vector<std::string> groups_all = allItems->getListOfGroups();
    std::vector<std::string> streams_all = allItems->getListOfStreams();
 
    //Groups
    std::vector<std::string> groups_item;
    std::vector<std::string>::iterator group;
    std::vector<std::string>::iterator group_second;
    std::string tmp_group_name;
    std::string tmp_group_second_name;
    
    for (group = groups_all.begin(); group != groups_all.end(); ++group) {
        name = (std::string)(*group)+std::string("_")+lvl;
        (*m_log) << MSG::DEBUG << "Adding configuration for group " << name << " :" << endreq;
        c_sigs->push_back(new TD_CplxSignature(name, "Group", lvl));
        c_sigs->at(c_sigs->size()-1)->setXsec(m_xsec);
        c_sigs->at(c_sigs->size()-1)->setNColls(m_NColls);
        c_sigs->at(c_sigs->size()-1)->setLumi(m_lumi);
        c_sigs->at(c_sigs->size()-1)->setDoUnique(true);
	
	//inefficient way, due to changes in TDT
	//check if each item is part of this group 
        std::vector<std::string>::const_iterator aItem;
	for (aItem = allTrigs.begin(); aItem != allTrigs.end(); ++aItem) {
		oneItem = m_trigDec->getChainGroup(*aItem);
		groups_item = oneItem->getListOfGroups();
		for (group_second = groups_item.begin(); group_second != groups_item.end(); ++group_second) {
			tmp_group_name = (std::string)(*group);
			tmp_group_second_name = (std::string)(*group_second); 
			if ( tmp_group_name == tmp_group_second_name ) {
            			sig_index = findSigIndex( *aItem,lvl);
         		   	if (sig_index > 0) {
        				c_sigs->at(c_sigs->size()-1)->addSig( *aItem,sig_index -1, 1.0);
       	    			} //endif
			} //endif
		} //endfor	
	} //endfor
		
        if (c_sigs->at(c_sigs->size()-1)->getSigIndicesSize() < 1) {
            c_sigs->pop_back();
	    (*m_log) << MSG::DEBUG << name << ": not saved!" << endreq;
        } else {
            ((c_sigs->at(c_sigs->size()-1))->getSigsBitSet())->Compact();
        }
     	
    } //endfor groups
    
    //Streams 
    std::vector<std::string> streams_item;
    std::vector<std::string>::iterator stream;
    std::vector<std::string>::iterator stream_second;
    std::string tmp_stream_name;
    std::string tmp_stream_second_name;

    for (stream = streams_all.begin(); stream != streams_all.end(); ++stream) {
        name = (std::string)(*stream)+std::string("_")+lvl;
        (*m_log) << MSG::DEBUG << "Adding configuration for stream " << name << " :" << endreq;
        c_sigs->push_back(new TD_CplxSignature(name, "StreamGroup", lvl));
        c_sigs->at(c_sigs->size()-1)->setXsec(m_xsec);
        c_sigs->at(c_sigs->size()-1)->setNColls(m_NColls);
        c_sigs->at(c_sigs->size()-1)->setLumi(m_lumi);
        c_sigs->at(c_sigs->size()-1)->setDoUnique(true);
       
	//inefficient way, due to changes in TDT
        //check if each item is part of this group 
        std::vector<std::string>::const_iterator aItem;
        for (aItem = allTrigs.begin(); aItem != allTrigs.end(); ++aItem) {
                oneItem = m_trigDec->getChainGroup(*aItem);
                streams_item = oneItem->getListOfStreams();
                for (stream_second = streams_item.begin(); stream_second != streams_item.end(); ++stream_second) {
                        tmp_stream_name = (std::string)(*stream);
                        tmp_stream_second_name = (std::string)(*stream_second);
                        if ( tmp_stream_name == tmp_stream_second_name ) {
                                sig_index = findSigIndex( *aItem,lvl);
                                if (sig_index > 0) {
                                        c_sigs->at(c_sigs->size()-1)->addSig( *aItem,sig_index -1, 1.0);
                                } //endif
                        } //endif
                } //endfor      
        } //endfor

        if (c_sigs->at(c_sigs->size()-1)->getSigIndicesSize() < 1) {
            c_sigs->pop_back();
	    (*m_log) << MSG::DEBUG << name << ": not saved!" << endreq;
        } else {
            ((c_sigs->at(c_sigs->size()-1))->getSigsBitSet())->Compact();
        }
     
    }

    unnamed = 0;
    std::ostringstream stm;
    int temp_j;
    for (int i = 0; i < (int)m_cum.size(); i++) {
        if ((int)(m_cum.at(i)).size() > 2) {
            
            if (((m_cum.at(i)).at(1)).compare(lvl) == 0) {
                stm.str(""); //empty stream
		if ((m_cum.at(i)).at(0).empty()) {
                    stm << "cum_rate_" << unnamed;
                    unnamed++;
                } else {
                    stm << (m_cum.at(i)).at(0);
                }

                name = stm.str();
		temp_j=0;
                for (int j = 2; j < (int)(m_cum.at(i)).size(); j++) {
        	        sig_index = findSigIndex((m_cum.at(i)).at(j),lvl);
	                //do only if sig really exists
			if (sig_index > 0) {     
			    stm.str("");
        	            stm << name;
                	    stm << "_" << temp_j;
	                    c_sigs->push_back(new TD_CplxSignature(stm.str(), "Cumulative", lvl));
        	            c_sigs->at(c_sigs->size()-1)->setXsec(m_xsec);
        	            c_sigs->at(c_sigs->size()-1)->setNColls(m_NColls);
                	    c_sigs->at(c_sigs->size()-1)->setLumi(m_lumi);
                
	                    //loop for all sigs in the group, UP TO 'j'
			    for (int k = 2; k <= j; k++) {
        	                sig_index = findSigIndex((m_cum.at(i)).at(k),lvl);
                	        if (sig_index > 0) {
           	                 c_sigs->at(c_sigs->size()-1)->addSig((m_cum.at(i)).at(k),sig_index -1, 1.0);
               	         	}
               	     	    }
                    
	                    if (c_sigs->at(c_sigs->size()-1)->getSigIndicesSize() < 1) {
        	                c_sigs->pop_back();
                	    } else {
                        	((c_sigs->at(c_sigs->size()-1))->getSigsBitSet())->Compact();
			    	temp_j++; //increase cumul sig counter only if it got added to c_sigs 
                    	    }

                    	    stm.str("");
		     	} //endif findSigIndex 1
                }
		temp_j--; //decrease the cumul sig counter by one to compensate for the last increment
		//temp_j should count down now...
                for (int j = (int)(m_cum.at(i)).size()-1; j >= 2; j--) {
        	        sig_index = findSigIndex((m_cum.at(i)).at(j),lvl);
	                //do only if sig really exists
			if (sig_index > 0) {    
			    stm.str("");
        	            stm << name;
                	    stm << "_rev_" << temp_j;
	                    c_sigs->push_back(new TD_CplxSignature(stm.str(), "CumulativeReverse", lvl));
        	            c_sigs->at(c_sigs->size()-1)->setXsec(m_xsec);
        	            c_sigs->at(c_sigs->size()-1)->setNColls(m_NColls);
                	    c_sigs->at(c_sigs->size()-1)->setLumi(m_lumi);
                    
			    //loop for all sigs in the group, UP TO 'j'
	                    for (int k = (int)(m_cum.at(i)).size()-1; k >= j; k--) {
        	                sig_index = findSigIndex((m_cum.at(i)).at(k),lvl);
                	        if (sig_index > 0) {
	                            c_sigs->at(c_sigs->size()-1)->addSig((m_cum.at(i)).at(k),sig_index -1, 1.0);
        	                }
                	    }
                    
	                    if (c_sigs->at(c_sigs->size()-1)->getSigIndicesSize() < 1) {
        	                c_sigs->pop_back();
                	    } else {
	                        ((c_sigs->at(c_sigs->size()-1))->getSigsBitSet())->Compact();
			    	temp_j--; //decrease cumul sig counter only if it got added to c_sigs
        	            }

                	    stm.str("");
                	} //endif findSigIndex 1
		}

            }
        }
    }

    unnamed = 0;
    for (int i = 0; i < (int)m_cplx_or.size(); i++) {
        if ((int)(m_cplx_or.at(i)).size() > 3) {
            if (((m_cplx_or.at(i)).at(1)).compare(lvl) == 0) {
                name = (m_cplx_or.at(i)).at(0);
                if (name.empty()) {
                    stm.str("");
                    stm << "cplx_or_" << unnamed;
                    name = stm.str();
                    stm.str("");
                    unnamed++;
                }

                c_sigs->push_back(new TD_CplxSignature(name, "Or", lvl));
                c_sigs->at(c_sigs->size()-1)->setXsec(m_xsec);
                c_sigs->at(c_sigs->size()-1)->setNColls(m_NColls);
                c_sigs->at(c_sigs->size()-1)->setLumi(m_lumi);
                
                for (int j = 2; j < (int)(m_cplx_or.at(i)).size(); j++) {
                    sig_index = findSigIndex((m_cplx_or.at(i)).at(j),lvl);
                    if (sig_index > 0) {
                        c_sigs->at(c_sigs->size()-1)->addSig((m_cplx_or.at(i)).at(j),sig_index -1, 1.0);
                    }
                }
                
                if (c_sigs->at(c_sigs->size()-1)->getSigIndicesSize() < 1) {
                    c_sigs->pop_back();
                } else {
                    ((c_sigs->at(c_sigs->size()-1))->getSigsBitSet())->Compact();
                }
            }
        }
    }

    if ((int) c_sigs->size() == 0) {
        and_or_change_index[lvl_index] = -1;
    }

    for (int i = 0; i < (int)c_sigs->size(); i++) {
        for (int j = 0; j < (int)c_sigs->size(); j++) {
            c_sigs->at(i)->addOverlap(*(c_sigs->at(j)->getName()));
        }
        for (int j = 0; j < (int)s_sigs->size(); j++) {
            c_sigs->at(i)->addOverlap(*(s_sigs->at(j)->getName()));
        }
    }

    //Create Menus cplx sigs
    
    std::string tmp_str1 = "";
    std::string tmp_str2 = "";
    unnamed = 0;
    for (int i = 0; i < (int)m_menus.size(); i++) {
        (*m_log) << MSG::DEBUG << "CHECK: m_menus.size() =  " << m_menus.size() << endreq;
        if ((int)(m_menus.at(i)).size() > 3) {
            (*m_log) << MSG::DEBUG << "CHECK: m_menus.at(" << i << ").size() = " << (int)(m_menus.at(i)).size() << endreq;
            if (((m_menus.at(i)).at(1)).compare(lvl) == 0) {
                name = (m_menus.at(i)).at(0);
                if (name.empty()) {
                    name = std::string("menu_"+unnamed);
                    unnamed++;
                }

                (*m_log) << MSG::DEBUG << "Creating menu sig " << name << endreq;

                menu_sigs->push_back(new TD_CplxSignature(name, "Menu", lvl));
                menu_sigs->at(menu_sigs->size()-1)->setXsec(m_xsec);
                menu_sigs->at(menu_sigs->size()-1)->setNColls(m_NColls);
                menu_sigs->at(menu_sigs->size()-1)->setLumi(m_lumi);
                
                for (int j = 2; j < (int)(m_menus.at(i)).size(); j++) {
                    std::string::size_type loc = ((m_menus.at(i)).at(j)).find( ":", 0 );
                    if( loc != std::string::npos ) {
                        tmp_str1=((m_menus.at(i)).at(j)).substr(0,loc);
                        tmp_str2=((m_menus.at(i)).at(j)).substr(loc+1);
                    } else {
                        tmp_str1=(m_menus.at(i)).at(j);
                        tmp_str2="1.0";
                    }
                    (*m_log) << MSG::DEBUG << "Creating menu sig " << name << " with component " << tmp_str1 << " : " << tmp_str2 << endreq;
                    sig_index = findSigIndex(tmp_str1,lvl);
                    if (sig_index > 0) {
                        menu_sigs->at(menu_sigs->size()-1)->addSig(tmp_str1,sig_index -1, strtod(tmp_str2.c_str(), NULL));
                    }
                }
                
                if (menu_sigs->at(menu_sigs->size()-1)->getSigIndicesSize() < 1) {
                    menu_sigs->pop_back();
                } else {
                    ((menu_sigs->at(menu_sigs->size()-1))->getSigsBitSet())->Compact();
                }

             
            }
        }
    }

    for (int i = 0; i < (int)menu_sigs->size(); i++) {
        for (int j = 0; j < (int)menu_sigs->size(); j++) {
            menu_sigs->at(i)->addOverlap(*(menu_sigs->at(j)->getName()));
        }
    }

    //Create Streams cplx sigs
    
    unnamed = 0;
    for (int i = 0; i < (int)m_streams.size(); i++) {
        if ((int)(m_streams.at(i)).size() > 3) {
            if (((m_streams.at(i)).at(1)).compare(lvl) == 0) {
                name = (m_streams.at(i)).at(0);
                if (name.empty()) {
                    name = std::string("stream_"+unnamed);
                    unnamed++;
                }

                stream_sigs->push_back(new TD_CplxSignature(name, "Stream", lvl));
                stream_sigs->at(stream_sigs->size()-1)->setXsec(m_xsec);
                stream_sigs->at(stream_sigs->size()-1)->setNColls(m_NColls);
                stream_sigs->at(stream_sigs->size()-1)->setLumi(m_lumi);
                
                for (int j = 2; j < (int)(m_streams.at(i)).size(); j++) {
                    std::string::size_type loc = ((m_streams.at(i)).at(j)).find( ":", 0 );
                    if( loc != std::string::npos ) {
                        tmp_str1=((m_streams.at(i)).at(j)).substr(0,loc);
                        tmp_str2=((m_streams.at(i)).at(j)).substr(loc+1);
                    } else {
                        tmp_str1=(m_streams.at(i)).at(j);
                        tmp_str2="1.0";
                    }
                    sig_index = findSigIndex(tmp_str1,lvl);
                    if (sig_index > 0) {
                        stream_sigs->at(stream_sigs->size()-1)->addSig(tmp_str1,sig_index -1, strtod(tmp_str2.c_str(), NULL));
                    }
                }
                
                if (stream_sigs->at(stream_sigs->size()-1)->getSigIndicesSize() < 1) {
                    stream_sigs->pop_back();
                } else {
                    ((stream_sigs->at(stream_sigs->size()-1))->getSigsBitSet())->Compact();
                }

             
            }
        }
    }

    for (int i = 0; i < (int)stream_sigs->size(); i++) {
        for (int j = 0; j < (int)stream_sigs->size(); j++) {
            stream_sigs->at(i)->addOverlap(*(stream_sigs->at(j)->getName()));
        }
    }

    //Create level cplx sigs

    l_sigs.push_back(new TD_CplxSignature(std::string(lvl+"_overall").c_str(),"Overall", lvl));

    TD_CplxSignature* tmp_sig = l_sigs.at(lvl_index);
    
    tmp_sig->setLevel(lvl);
    tmp_sig->setXsec(m_xsec);
    tmp_sig->setNColls(m_NColls);
    tmp_sig->setLumi(m_lumi);

    for (int j = 0; j < (int)s_sigs->size(); j++) {
        tmp_sig->addOverlap(*(s_sigs->at(j)->getName()));
        tmp_sig->addSig(*(s_sigs->at(j)->getName()), j, 1.0);
    }
    for (int j = 0; j < (int)c_sigs->size(); j++) {
        tmp_sig->addOverlap(*(c_sigs->at(j)->getName()));
    }
    for (int j = 0; j < (int)menu_sigs->size(); j++) {
        tmp_sig->addOverlap(*(menu_sigs->at(j)->getName()));
    }
    for (int j = 0; j < (int)stream_sigs->size(); j++) {
        tmp_sig->addOverlap(*(stream_sigs->at(j)->getName()));
    }
    (tmp_sig->getSigsBitSet())->Compact();


}

/* Create all branches in Ntuple once vectors are fixed */
void TriggerRateTools::initializeNtuple(std::string lvl) {

    
    std::string name = "";
    
    if (lvl.compare("L1") == 0) {
        
        for (int i = 0; i < (int)sigs_L1_name.size(); i++) {
            name = sigs_L1_name.at(i);
            raw_TD->Branch(name.c_str(), &sigs_L1_res[i], std::string(name+"/I").c_str());
            raw_PS->Branch(name.c_str(), &sigs_L1_ps[i], std::string(name+"/I").c_str());
            (*m_log) << MSG::DEBUG << "About to create branch " << name << " in " << s_sigs_tree << endreq; 
            TBranch *br = s_sigs_tree->Branch(name.c_str(), "TD_Signature", &s_sigs_L1[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)c_sigs_L1.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(c_sigs_L1.at(i)->getName()) << " in " << c_sigs_tree << endreq; 
            TBranch *br = c_sigs_tree->Branch(*(c_sigs_L1.at(i)->getName()), "TD_CplxSignature", &c_sigs_L1[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)menu_sigs_L1.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(menu_sigs_L1.at(i)->getName()) << " in " << menu_sigs_tree << endreq; 
            TBranch *br = menu_sigs_tree->Branch(*(menu_sigs_L1.at(i)->getName()), "TD_CplxSignature", &menu_sigs_L1[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)stream_sigs_L1.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(stream_sigs_L1.at(i)->getName()) << " in " << stream_sigs_tree << endreq; 
            TBranch *br = stream_sigs_tree->Branch(*(stream_sigs_L1.at(i)->getName()), "TD_CplxSignature", &stream_sigs_L1[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        l_sigs_tree->Branch(*(l_sigs.at(0)->getName()), "TD_CplxSignature", &l_sigs[0], 128000, 0);

    } else if (lvl.compare("L2") == 0) {
        
        for (int i = 0; i < (int)sigs_L2_name.size(); i++) {
            name = sigs_L2_name.at(i);
            raw_TD->Branch(name.c_str(), &sigs_L2_res[i], std::string(name+"/I").c_str());
            raw_PS->Branch(name.c_str(), &sigs_L2_ps[i], std::string(name+"/I").c_str());
            (*m_log) << MSG::DEBUG << "About to create branch " << name << " in " << s_sigs_tree << endreq; 
            TBranch *br = s_sigs_tree->Branch(name.c_str(), "TD_Signature", &s_sigs_L2[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)c_sigs_L2.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(c_sigs_L2.at(i)->getName()) << " in " << c_sigs_tree << endreq; 
            TBranch *br = c_sigs_tree->Branch(*(c_sigs_L2.at(i)->getName()), "TD_CplxSignature", &c_sigs_L2[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)menu_sigs_L2.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(menu_sigs_L2.at(i)->getName()) << " in " << menu_sigs_tree << endreq; 
            TBranch *br = menu_sigs_tree->Branch(*(menu_sigs_L2.at(i)->getName()), "TD_CplxSignature", &menu_sigs_L2[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)stream_sigs_L2.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(stream_sigs_L2.at(i)->getName()) << " in " << stream_sigs_tree << endreq; 
            TBranch *br = stream_sigs_tree->Branch(*(stream_sigs_L2.at(i)->getName()), "TD_CplxSignature", &stream_sigs_L2[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        l_sigs_tree->Branch(*(l_sigs.at(1)->getName()), "TD_CplxSignature", &l_sigs[1], 128000, 0);

    } else if (lvl.compare("EF") == 0) {
        
        for (int i = 0; i < (int)sigs_EF_name.size(); i++) {
            name = sigs_EF_name.at(i);
            raw_TD->Branch(name.c_str(), &sigs_EF_res[i], std::string(name+"/I").c_str());
            raw_PS->Branch(name.c_str(), &sigs_EF_ps[i], std::string(name+"/I").c_str());
            (*m_log) << MSG::DEBUG << "About to create branch " << name << " in " << s_sigs_tree << endreq; 
            TBranch *br = s_sigs_tree->Branch(name.c_str(), "TD_Signature", &s_sigs_EF[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)c_sigs_EF.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(c_sigs_EF.at(i)->getName()) << " in " << c_sigs_tree << endreq; 
            TBranch *br = c_sigs_tree->Branch(*(c_sigs_EF.at(i)->getName()), "TD_CplxSignature", &c_sigs_EF[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)menu_sigs_EF.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(menu_sigs_EF.at(i)->getName()) << " in " << menu_sigs_tree << endreq; 
            TBranch *br = menu_sigs_tree->Branch(*(menu_sigs_EF.at(i)->getName()), "TD_CplxSignature", &menu_sigs_EF[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        for (int i = 0; i < (int)stream_sigs_EF.size(); i++) {
            (*m_log) << MSG::DEBUG << "About to create branch " << *(stream_sigs_EF.at(i)->getName()) << " in " << stream_sigs_tree << endreq; 
            TBranch *br = stream_sigs_tree->Branch(*(stream_sigs_EF.at(i)->getName()), "TD_CplxSignature", &stream_sigs_EF[i], 128000, 0);
            (*m_log) << MSG::DEBUG << br << endreq;
        }
        l_sigs_tree->Branch(*(l_sigs.at(2)->getName()), "TD_CplxSignature", &l_sigs[2], 128000, 0);

    }

}

/* Create histograms */
void TriggerRateTools::initializeHistos(std::string lvl) {

    int lvl_index = 0;
    std::vector<TD_Signature*> *s_sigs = NULL;
    std::vector<Int_t> *sigs_res = NULL;
    std::vector<std::string> *sigs_name = NULL;
    std::vector<TD_CplxSignature*> *c_sigs = NULL;
    TH1F *sigs_rate_hist = NULL;
    TH2F *sigs_overlap_hist = NULL;
    
    if (lvl.compare("L1") == 0) {
        
        s_sigs = &s_sigs_L1;
        sigs_res = &sigs_L1_res;
        sigs_name = &sigs_L1_name;
        c_sigs = &c_sigs_L1;
        sigs_rate_hist = sigs_L1_rate_hist;
        sigs_overlap_hist = sigs_L1_overlap_hist;
        lvl_index = 0;

    } else if (lvl.compare("L2") == 0) {
        
        s_sigs = &s_sigs_L2;
        sigs_res = &sigs_L2_res;
        sigs_name = &sigs_L2_name;
        c_sigs = &c_sigs_L2;
        sigs_rate_hist = sigs_L2_rate_hist;
        sigs_overlap_hist = sigs_L2_overlap_hist;
        lvl_index = 1;

    } else if (lvl.compare("EF") == 0) {
        
        s_sigs = &s_sigs_EF;
        sigs_res = &sigs_EF_res;
        sigs_name = &sigs_EF_name;
        c_sigs = &c_sigs_EF;
        sigs_rate_hist = sigs_EF_rate_hist;
        sigs_overlap_hist = sigs_EF_overlap_hist;
        lvl_index = 2;

    }
 
    int num_sigs = (int)s_sigs->size();
    
    for (int i = 0; i < num_sigs; i++) {
        sigs_rate_hist->GetXaxis()->SetBinLabel(2*i+1,*(s_sigs->at(i)->getName()));
        sigs_overlap_hist->GetXaxis()->SetBinLabel(i+1,*(s_sigs->at(i)->getName()));
        sigs_overlap_hist->GetYaxis()->SetBinLabel(i+1,*(s_sigs->at(i)->getName()));
    }

    sigs_rate_hist->SetFillColor(2);
    sigs_rate_hist->GetYaxis()->SetTitle("Rate (Hz)");

    sigs_overlap_hist->GetZaxis()->SetTitle("Overlap Rate (Hz)");
}


/* Compute all single signature rates */
int TriggerRateTools::fillSingleRates(std::string lvl) {

    (*m_log) << MSG::DEBUG << "Computing rates for " << lvl << endreq;

    int lvl_index = 0;
    std::vector<TD_Signature*> *s_sigs = NULL;
    std::vector<Int_t> *sigs_res = NULL;
    std::vector<double> *sigs_p = NULL;
    std::vector<std::string> *sigs_name = NULL;
    
    if (lvl.compare("L1") == 0) {
        
        s_sigs = &s_sigs_L1;
        sigs_res = &sigs_L1_res;
        sigs_p = &sigs_L1_p;
        sigs_name = &sigs_L1_name;
        lvl_index = 0;

    } else if (lvl.compare("L2") == 0) {
        
        s_sigs = &s_sigs_L2;
        sigs_res = &sigs_L2_res;
        sigs_p = &sigs_L2_p;
        sigs_name = &sigs_L2_name;
        lvl_index = 1;

    } else if (lvl.compare("EF") == 0) {
        
        s_sigs = &s_sigs_EF;
        sigs_res = &sigs_EF_res;
        sigs_p = &sigs_EF_p;
        sigs_name = &sigs_EF_name;
        lvl_index = 2;

    }

        
    int num_passed = 0;

    std::vector<double> res;
    std::vector<double> tmp;
        
    for (int i = 0; i < (int)sigs_res->size(); i++) {
        if (sigs_res->at(i) > 0) {
	    s_sigs->at(i)->incNumPassed(1);
	    res.push_back((double)sigs_res->at(i)/(sigs_p->at(i)));
            tmp.push_back(0.0);
        }
    }

    num_passed = (int)res.size();

    if (num_passed >= 1) {
        TD_CplxSignature* tmp_sig = l_sigs.at(lvl_index);
        //Prescaler* ps = new Prescaler(&res, &tmp, 1.0, 0, num_passed,0.0);

        //double pnum = ps->getPnum();
        double pnum = getPnum(&res);

        lvl_pnum[lvl_index] = pnum;

        res.clear();
        tmp.clear();

        (*m_log) << MSG::DEBUG << lvl << " pnum: " << pnum << endreq;

        tmp_sig->incNumPassed(1);
        tmp_sig->incPnumPassed(pnum);
        

        double tmp_res = 0.0;
        std::vector<double> unique_res;
        std::vector<double> unique_tmp;
        
        for (int i = 0; i < (int)s_sigs->size(); i++) {
                
            s_sigs->at(i)->incIndep(1.0/((double)num_passed));

            tmp_res = sigs_res->at(i)/((double)sigs_p->at(i));
            if (tmp_res > 0 && tmp_res < 1) {
                s_sigs->at(i)->incPnumUnique(tmp_res*((1.0-pnum)/(1.0-tmp_res)));
            } else if (tmp_res > 0) {
                for (int j = 0; j < (int)s_sigs->size(); j++) {
                    if (i != j){
                        unique_res.push_back(sigs_res->at(j)/((double)sigs_p->at(j)));
                        unique_tmp.push_back(0.0);
                    }
                }
                //Prescaler* p_unique = new Prescaler(&unique_res, &unique_tmp, 1.0, 0, (int)unique_res.size(),0.0);
                //s_sigs->at(i)->incPnumUnique(pnum-p_unique->getPnum());
                s_sigs->at(i)->incPnumUnique(pnum-getPnum(&unique_res));

                unique_res.clear();
                unique_tmp.clear();
            }
            
            int k = 0;
            for (int j = 0; j < (int)s_sigs->size(); j++) {
                if (i != j) {
                    s_sigs->at(i)->incOverlapNum(k,(sigs_res->at(j)/((double)sigs_p->at(j)))*tmp_res);
                } else {
                    s_sigs->at(i)->incOverlapNum(k,tmp_res);
                }
                k++;
            }
            
        }

        for (int j = 0; j < (int)s_sigs->size(); j++) {
            tmp_sig->incOverlapNum(j, (sigs_res->at(j)/((double)sigs_p->at(j)))*pnum);
        }
    }

    return num_passed;
    
}

/* Compute all cplx signature rates */
void TriggerRateTools::fillCplxRates(std::string lvl) {
    
    (*m_log) << MSG::DEBUG << "Computing complex rates for " << lvl << endreq;
    
    int lvl_index = 0;
    std::vector<TD_Signature*> *s_sigs = NULL;
    std::vector<Int_t> *sigs_res = NULL;
    std::vector<double> *sigs_p = NULL;
    std::vector<std::string> *sigs_name = NULL;
    std::vector<TD_CplxSignature*> *c_sigs = NULL;
    std::vector<TD_CplxSignature*> *menu_sigs = NULL;
    std::vector<TD_CplxSignature*> *stream_sigs = NULL;
 
    if (lvl.compare("L1") == 0) {
        
        s_sigs = &s_sigs_L1;
        sigs_res = &sigs_L1_res;
        sigs_p = &sigs_L1_p;
        sigs_name = &sigs_L1_name;
        c_sigs = &c_sigs_L1;
        menu_sigs = &menu_sigs_L1;
        stream_sigs = &stream_sigs_L1;
        lvl_index = 0;

    } else if (lvl.compare("L2") == 0) {
        
        s_sigs = &s_sigs_L2;
        sigs_res = &sigs_L2_res;
        sigs_p = &sigs_L2_p;
        sigs_name = &sigs_L2_name;
        c_sigs = &c_sigs_L2;
        menu_sigs = &menu_sigs_L2;
        stream_sigs = &stream_sigs_L2;
        lvl_index = 1;

    } else if (lvl.compare("EF") == 0) {
        
        s_sigs = &s_sigs_EF;
        sigs_res = &sigs_EF_res;
        sigs_p = &sigs_EF_p;
        sigs_name = &sigs_EF_name;
        c_sigs = &c_sigs_EF;
        menu_sigs = &menu_sigs_EF;
        stream_sigs = &stream_sigs_EF;
        lvl_index = 2;

    }

    //Computing rates for "AND" triggers

    (*m_log) << MSG::DEBUG << "\t Working on 'AND' triggers" << endreq;
    
    int c_sigs_size = (int)c_sigs->size();
    int menu_sigs_size = (int)menu_sigs->size();
    int stream_sigs_size = (int)stream_sigs->size();
    int s_sigs_size = (int)s_sigs->size();
    TD_CplxSignature* tmp_sig;
    long double tmp_res = 1.0;
    std::vector<double> c_sigs_res;
    std::vector<double> menu_sigs_res;
    std::vector<double> stream_sigs_res;
    int tmp_index = 0;
    TBits sig_bs;    
    TBits tmp_bs;    
    
    for (int i = 0; i < and_or_change_index.at(lvl_index); i++) {
        tmp_res = 1.0;
        tmp_sig = c_sigs->at(i);

        for (int j = 0; j < tmp_sig->getSigIndicesSize(); j++) {
            tmp_index = tmp_sig->getSigIndexAt(j);
            if (tmp_index < 0) {
                tmp_res = tmp_res*((1-(long double)sigs_res->at(-tmp_index-1))/((long double)sigs_p->at(-tmp_index-1)));
            } else {
                tmp_res = tmp_res*((long double)sigs_res->at(tmp_index)/((long double)sigs_p->at(tmp_index)));
            }
        }

        c_sigs_res.push_back((double)tmp_res);

        if (tmp_res > 0) {
            tmp_sig->incNumPassed(1);
            tmp_sig->incPnumPassed(tmp_res);
        }

        //Unique rates not defined for cplx triggers...

    }

    //Computing rates for "OR" triggers

    (*m_log) << MSG::DEBUG << "\t Working on 'OR' triggers " << and_or_change_index.at(lvl_index) << endreq;
    
    std::vector<double> res;
    std::vector<double> tmp;
            
    for (int i = and_or_change_index.at(lvl_index); i < c_sigs_size && i >= 0; i++) {
        (*m_log) << MSG::DEBUG << "\t Working on 'OR' trigger #" << i << endreq;
        tmp_sig = c_sigs->at(i);

        sig_bs = *(tmp_sig->getSigsBitSet());
        tmp_bs.Clear();
        for (int j = 0; j < (int)s_sigs->size(); j++) {
            tmp_bs.SetBitNumber(j);
            if ( (tmp_bs & sig_bs).CountBits() == 1 && sigs_res->at(j) > 0 ){
                res.push_back(sigs_res->at(j)/((double)sigs_p->at(j)));
            }
            tmp_bs.Clear();
        }
        
        // OLD WAY PRE-BITSET
        //for (int j = 0; j < tmp_sig->getSigIndicesSize(); j++) {
        //    tmp_index = tmp_sig->getSigIndexAt(j);
        //    tmp_res = (double)sigs_res->at(tmp_index);
        //    if (tmp_res > 0) {
        //        res.push_back(tmp_res/((double)sigs_p->at(tmp_index)));
        //        tmp.push_back(0.0);
        //    }
        //}

        if ((int)res.size() > 0) {
            tmp_sig->incNumPassed(1);
            c_sigs_res.push_back(getPnum(&res));
            tmp_sig->incPnumPassed(c_sigs_res.at((int)c_sigs_res.size()-1));

            //              \/ Unique Rates \/
            //if (tmp_sig->getDoUnique()) { //Uncomment to filter CplxSignatures with do_unique
                tmp_res = c_sigs_res.at((int)c_sigs_res.size() -1);
                if (tmp_res < 1) {
                    tmp_sig->incPnumUnique(tmp_res*((1.0-lvl_pnum.at(lvl_index))/(1.0-tmp_res)));
                } else {
                    res.clear();

                    for (int j = 0; j < (int)s_sigs->size(); j++) {
                        tmp_bs.SetBitNumber(j);
                        if ( (tmp_bs & sig_bs).CountBits() == 0 && sigs_res->at(j) > 0 ){
                            res.push_back(sigs_res->at(j)/((double)sigs_p->at(j)));
                        }
                        tmp_bs.Clear();
                    }
                    tmp_sig->incPnumUnique(lvl_pnum.at(lvl_index)-getPnum(&res));
                }
            //}  //Uncomment to filter CplxSignatures with do_unique 

        } else {
            c_sigs_res.push_back(0.0);
        }

        res.clear();
        tmp.clear();

        //Unique rates not defined for cplx triggers...

    }

    //Cplx sigs overlaps filled
    
    (*m_log) << MSG::DEBUG << "Filling cplx sigs overlaps " << c_sigs_size << endreq;
    
    if (c_sigs_size > 0) {
        (*m_log) << MSG::DEBUG << "Filling cplx sigs overlaps " << c_sigs_size << " : " << c_sigs->at(0)->getOverlapNumSize() << endreq;
    }
    for (int i = 0; i < c_sigs_size; i++) {
        tmp_index = 0;
        tmp_res = c_sigs_res.at(i);
        for (int j = 0; j < c_sigs_size; j++) {
            (*m_log) << MSG::DEBUG << "Filling cplx sigs overlaps working on " << j << endreq;
            if (i != j) {
                c_sigs->at(i)->incOverlapNum(tmp_index,(double)(tmp_res*c_sigs_res.at(j)));
            } else {
                c_sigs->at(i)->incOverlapNum(tmp_index,tmp_res);
            }
            tmp_index++;
        }
        for (int j = 0; j < (int)s_sigs_size; j++) {
            c_sigs->at(i)->incOverlapNum(tmp_index,(double)(tmp_res*(sigs_res->at(j)/sigs_p->at(j))));
            tmp_index++;
        }
    }

    (*m_log) << MSG::DEBUG << "Filled cplx sigs overlaps " << endreq;
    

    //Computing rates for Menu sigs

    (*m_log) << MSG::DEBUG << "\t Working on menus" << endreq;
    
    res.clear();
    tmp.clear();
            
    for (int i = 0; i < menu_sigs_size; i++) {
        tmp_sig = menu_sigs->at(i);

        for (int j = 0; j < tmp_sig->getSigIndicesSize(); j++) {
            tmp_index = tmp_sig->getSigIndexAt(j);
            tmp_res = (double)sigs_res->at(tmp_index);
            if (tmp_res > 0) {
                res.push_back(tmp_res/(((double)sigs_p->at(tmp_index))*(tmp_sig->getSigPrescaleAt(j))));
                tmp.push_back(0.0);
            }
        }

        if ((int)res.size() > 0) {
            //Prescaler* ps = new Prescaler(&res, &tmp, 1.0, 0, (int)res.size(),0.0);
            tmp_sig->incNumPassed(1);
            //menu_sigs_res.push_back(ps->getPnum());
            menu_sigs_res.push_back(getPnum(&res));
            tmp_sig->incPnumPassed(menu_sigs_res.at((int)menu_sigs_res.size()-1));
        } else {
            menu_sigs_res.push_back(0.0);
        }

        res.clear();
        tmp.clear();

        //Unique rates not defined for cplx triggers...

    }

    //Menu sigs overlaps filled
    
    (*m_log) << MSG::DEBUG << "Filling menu sigs overlaps " << menu_sigs_size << endreq;
    
    for (int i = 0; i < menu_sigs_size; i++) {
        tmp_res = menu_sigs_res.at(i);
        for (int j = 0; j < menu_sigs_size; j++) {
            if (i != j) {
                menu_sigs->at(i)->incOverlapNum(j,(double)(tmp_res*menu_sigs_res.at(j)));
            } else {
                menu_sigs->at(i)->incOverlapNum(j,tmp_res);
            }
        }
    }

    (*m_log) << MSG::DEBUG << "Filled menu sigs overlaps " << endreq;


    
    //Computing rates for Stream sigs

    (*m_log) << MSG::DEBUG << "\t Working on streams" << endreq;
    
    res.clear();
    tmp.clear();
            
    for (int i = 0; i < stream_sigs_size; i++) {
        tmp_sig = stream_sigs->at(i);

        for (int j = 0; j < tmp_sig->getSigIndicesSize(); j++) {
            tmp_index = tmp_sig->getSigIndexAt(j);
            tmp_res = (double)sigs_res->at(tmp_index);
            if (tmp_res > 0) {
                res.push_back(tmp_res/(((double)sigs_p->at(tmp_index))*(tmp_sig->getSigPrescaleAt(j))));
                tmp.push_back(0.0);
            }
        }

        if ((int)res.size() > 0) {
            //Prescaler* ps = new Prescaler(&res, &tmp, 1.0, 0, (int)res.size(),0.0);
            tmp_sig->incNumPassed(1);
            stream_sigs_res.push_back(getPnum(&res));
            tmp_sig->incPnumPassed(stream_sigs_res.at((int)stream_sigs_res.size()-1));
        } else {
            stream_sigs_res.push_back(0.0);
        }

        res.clear();
        tmp.clear();

        //Unique rates not defined for cplx triggers...

    }

    //Stream sigs overlaps fill
    
    (*m_log) << MSG::DEBUG << "Filling stream sigs overlaps " << stream_sigs_size << endreq;
    
    for (int i = 0; i < stream_sigs_size; i++) {
        tmp_res = stream_sigs_res.at(i);
        for (int j = 0; j < stream_sigs_size; j++) {
            if (i != j) {
                stream_sigs->at(i)->incOverlapNum(j,(double)(tmp_res*stream_sigs_res.at(j)));
            } else {
                stream_sigs->at(i)->incOverlapNum(j,tmp_res);
            }
        }
    }

    (*m_log) << MSG::DEBUG << "Filled stream sigs overlaps " << endreq;
 

    //Level sigs overlap fill
    
    tmp_sig = l_sigs.at(lvl_index);
    
    for (int j = s_sigs_size; j < (s_sigs_size+c_sigs_size); j++) {
        tmp_sig->incOverlapNum(j,(double)(lvl_pnum.at(lvl_index)*c_sigs_res.at( j - s_sigs_size )));
    }
    for (int j = (s_sigs_size+c_sigs_size); j < (s_sigs_size+c_sigs_size+menu_sigs_size); j++) {
        tmp_sig->incOverlapNum(j,(double)(lvl_pnum.at(lvl_index)*menu_sigs_res.at( j - (s_sigs_size+c_sigs_size) )));
    }
    for (int j = (s_sigs_size+c_sigs_size+menu_sigs_size); j < (s_sigs_size+c_sigs_size+menu_sigs_size+stream_sigs_size); j++) {
        tmp_sig->incOverlapNum(j,(double)(lvl_pnum.at(lvl_index)*stream_sigs_res.at( j - (s_sigs_size+c_sigs_size+menu_sigs_size) )));
    }

}

/* Get Veto Decision and Apply it */
void TriggerRateTools::applyVeto(std::string lvl, bool veto) {
    
    (*m_log) << MSG::DEBUG << "Applying Veto" << endreq;
    
    int lvl_index = 0;
    std::vector<TD_Signature*> *s_sigs = NULL;
    std::vector<TD_CplxSignature*> *c_sigs = NULL;
    std::vector<TD_CplxSignature*> *menu_sigs = NULL;
    std::vector<TD_CplxSignature*> *stream_sigs = NULL;
 
    if (lvl.compare("L1") == 0) {
        
        s_sigs = &s_sigs_L1;
        c_sigs = &c_sigs_L1;
        menu_sigs = &menu_sigs_L1;
        stream_sigs = &stream_sigs_L1;
        lvl_index = 0;

    } else if (lvl.compare("L2") == 0) {
        
        s_sigs = &s_sigs_L2;
        c_sigs = &c_sigs_L2;
        menu_sigs = &menu_sigs_L2;
        stream_sigs = &stream_sigs_L2;
        lvl_index = 1;

    } else if (lvl.compare("EF") == 0) {
        
        s_sigs = &s_sigs_EF;
        c_sigs = &c_sigs_EF;
        menu_sigs = &menu_sigs_EF;
        stream_sigs = &stream_sigs_EF;
        lvl_index = 2;

    }


    int c_sigs_size = (int)c_sigs->size();
    int menu_sigs_size = (int)menu_sigs->size();
    int stream_sigs_size = (int)stream_sigs->size();
    int s_sigs_size = (int)s_sigs->size();
    
    for (int i = 0; i < s_sigs_size; i++) {
        s_sigs->at(i)->setVetoIsPassed(veto);
    }
    
    for (int i = 0; i < c_sigs_size; i++) {
        c_sigs->at(i)->setVetoIsPassed(veto);
    }

    for (int i = 0; i < menu_sigs_size; i++) {
        menu_sigs->at(i)->setVetoIsPassed(veto);
    }

    for (int i = 0; i < stream_sigs_size; i++) {
        stream_sigs->at(i)->setVetoIsPassed(veto);
    }

    l_sigs.at(lvl_index)->setVetoIsPassed(veto);
}


/* Compute prescaled results */
double TriggerRateTools::getPnum(std::vector<double> *list) {

    double pnum = 0.0;
    int tmp_size = (int)list->size();

    for (int i=0; i < tmp_size; i++) {
	(*m_log) << MSG::DEBUG << "i: " << i << ", list->at(i): " << list->at(i) << endreq; 
        pnum = pnum + (1-pnum)*list->at(i);
     } 

    return pnum;
}

