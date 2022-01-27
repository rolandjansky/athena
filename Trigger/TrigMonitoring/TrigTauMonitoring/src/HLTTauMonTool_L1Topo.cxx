/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TProfile.h"
#include "HLTTauMonTool.h"
#include "AthenaKernel/Units.h"

using namespace std;
using Athena::Units::GeV;

StatusCode HLTTauMonTool::fillTopoValidation(const std::string & trigItem_topo, const std::string & trigItem_support){

//	if(!getTDT()->isPassed(trigItem_topo,m_L1TriggerCondition)) return StatusCode::SUCCESS;

	const xAOD::JetRoIContainer *l1jets = nullptr;
        const xAOD::EmTauRoIContainer *l1taus = nullptr;

	if ( evtStore()->retrieve( l1jets, "LVL1JetRoIs").isFailure() ){
		ATH_MSG_WARNING("Failed to retrieve LVL1JetRoIs container. Exiting.");
		return StatusCode::SUCCESS;
	}
	if ( evtStore()->retrieve( l1taus, "LVL1EmTauRoIs").isFailure() ){
		ATH_MSG_WARNING("Failed to retrieve LVL1EmTauRoIs container. Exiting.");
		return StatusCode::SUCCESS;
	}
   
	xAOD::JetRoIContainer::const_iterator itJetRoI;
	xAOD::JetRoIContainer::const_iterator itJetRoI_e = l1jets->end();
	xAOD::EmTauRoIContainer::const_iterator itEMTau = l1taus->begin();
	xAOD::EmTauRoIContainer::const_iterator itEMTau_e = l1taus->end();

	if(getTDT()->isPassed(trigItem_topo,m_L1TriggerCondition) && !getTDT()->isPassed(trigItem_support,m_L1TriggerCondition)){
		setCurrentMonGroup("HLT/TauMon/Expert/TopoValidation");
		hist("hSupportMismatch")->Fill(trigItem_topo.c_str(),1.);
		return StatusCode::SUCCESS;
	}
	if(!getTDT()->isPassed(trigItem_support,m_L1TriggerCondition)) return StatusCode::SUCCESS;

	setCurrentMonGroup("HLT/TauMon/Expert/TopoValidation/"+trigItem_topo);
	Trig::FeatureContainer f = ( getTDT()->features(trigItem_support,m_L1TriggerCondition) );
	Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());

	float min_dR(100.);
	//        float min_dR_nojets(100.);
	//        float min_dR_jets(100.);
	//        float min_dR_Bothjets(100.);
	//        float min_dR_Mixjets(100.);
	//        float min_dR_notaus(100.);
	//        float min_dR_taus(100.);
       	float min_dR_Bothtaus(100.);
	float min_dR_BothtausJet(100.);
	float min_dR_BothtausJetN(100.);
	//        float min_dR_Mixtaus(100.);
	//        float min_dR_tauJets(100.);
	//        float min_dR_NotauJets(100.);
	//        float min_dR_tauNoJets(100.);
	//        float min_dR_NotauNoJets(100.);
        bool disambiguation = false;
        int MultiJets = 0;
        int MultiTaus =0;
        std::vector<float> tau_val_roi;
        std::vector<float> pt_jet_roi;
        std::vector<float> eta_roi;
        std::vector<float> phi_roi;

        std::vector<bool> jet_roi;

        int index_tau12 = -100, index_tau20 = -100;
        int indexJet_tau12= -100, indexJet_tau20 = -100; 

        double roi_pt_max_jet = -1000.;
        double roi_pt_max = -1000.;
	for(;comb!=combEnd;++comb){
		const std::vector< Trig::Feature<TrigRoiDescriptor> > vec_roi = comb->get<TrigRoiDescriptor>("initialRoI",m_L1TriggerCondition);
		std::vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator roi = vec_roi.begin()/*, roi1 = vec_roi.begin()*/, roi_e = vec_roi.end();

                for(roi = vec_roi.begin(); roi != roi_e; ++roi) if(roi->cptr()){
                        jet_roi.push_back(false);
                        tau_val_roi.push_back(0.0);
			phi_roi.push_back(roi->cptr()->phi());
			eta_roi.push_back(roi->cptr()->eta());
			pt_jet_roi.push_back(-1000.);
                       
                        roi_pt_max = -1000.;
                        roi_pt_max_jet = -1000.;
                        bool isJet = false;
                        for(itJetRoI = l1jets->begin(); itJetRoI!=itJetRoI_e; ++itJetRoI){
                          double roi_pt = (*itJetRoI)->etLarge();
                                if(roi->cptr()->roiWord()==(*itJetRoI)->roiWord() && roi_pt > 25000.) {isJet=true;jet_roi.back()=true; if (roi_pt>roi_pt_max_jet) roi_pt_max_jet=roi_pt;pt_jet_roi.back()=roi_pt_max_jet;}
                        }
                        if (isJet) MultiJets++;
                        bool isTau = false;
                        for(itEMTau = l1taus->begin(); itEMTau!=itEMTau_e; ++itEMTau){
                          double roi_pt = (*itEMTau)->tauClus();
                          float iso_offset = 2000.;
                          float iso_slope = 0.1;
                          float isolation_cut = iso_offset + iso_slope * roi_pt;
                          double roi_isol = (*itEMTau)->emIsol();
                                if(roi->cptr()->roiWord()==(*itEMTau)->roiWord() &&
                                    roi_pt > 12000. &&
                                    roi_isol <= isolation_cut) {
                                        isTau = true;
                                        if (roi_pt>roi_pt_max) roi_pt_max=roi_pt;
                                        tau_val_roi.back()=roi_pt_max;
                                    }
                          }
                          if (isTau) MultiTaus++;
                }

          }

        for (unsigned int i = 0; i < phi_roi.size(); ++i) {
            for (unsigned int j = i + 1; j < phi_roi.size(); ++j) {
                if ((tau_val_roi.at(i) > 12000.0 && tau_val_roi.at(j) > 20000.0) ||
                    (tau_val_roi.at(i) > 20000.0 && tau_val_roi.at(j) > 12000.0)) {
                    float dR = deltaR(eta_roi.at(i),eta_roi.at(j),phi_roi.at(i),phi_roi.at(j));
                    if (dR >= 0.3) {
			if (dR < min_dR) min_dR = dR;
                        if (dR < min_dR_Bothtaus) {
                            min_dR_Bothtaus = dR;
                            if (tau_val_roi.at(i) > 12000.0 && tau_val_roi.at(j) > 20000.0) {
                              index_tau12 = i;
                              index_tau20 = j;
                            } else {
                              index_tau12 = j;
                              index_tau20 = i;
                            }
                          }
                        
                        bool triggerJet = false;
                        for (unsigned int k = 0; k < phi_roi.size(); ++k) {
                          if (jet_roi.at(k)) {
                            float dR1 = deltaR(eta_roi.at(i),eta_roi.at(k),phi_roi.at(i),phi_roi.at(k));
                            float dR2 = deltaR(eta_roi.at(j),eta_roi.at(k),phi_roi.at(j),phi_roi.at(k));
                            if (dR > 0.1 && dR1 > 0.1 && dR2 > 0.1 ) disambiguation = true;
                            triggerJet = true;
                           }
                         } 

                        if (triggerJet) {
                          if (dR < min_dR_BothtausJet) {
                            min_dR_BothtausJet = dR;
                            if (tau_val_roi.at(i) > 12000.0 && tau_val_roi.at(j) > 20000.0) {
                              indexJet_tau12 = i;
                              indexJet_tau20 = j;
                            } else {
                              indexJet_tau12 = j;
                              indexJet_tau20 = i;
                            }
                          }
                        } else {
                          if (dR < min_dR_BothtausJetN) {
                            min_dR_BothtausJetN = dR;
                           }
                        }
                     } 
                 }
             }
        }
        hist("hDisambiguation")->Fill(1.);
	if(getTDT()->isPassed(trigItem_topo,m_L1TriggerCondition)){
        	hist("hDRBothtaus")->Fill(min_dR_Bothtaus);
        	hist("hDRBothtausJet")->Fill(min_dR_BothtausJet);
        	hist("hDRBothtausJetN")->Fill(min_dR_BothtausJetN);
 
                if (!phi_roi.empty() &&min_dR_Bothtaus>2.8 && min_dR_Bothtaus < 50.) {
        	  if (index_tau20 > 0) hist("hPTLeading")->Fill(tau_val_roi.at(index_tau20)/GeV);
        	  if (index_tau12 > 0) hist("hPTSubLeading")->Fill(tau_val_roi.at(index_tau12)/GeV);
        	  if (index_tau20 > 0) hist("hEtaLeading")->Fill(eta_roi.at(index_tau20));
        	  if (index_tau12 > 0) hist("hEtaSubLeading")->Fill(eta_roi.at(index_tau12));
        	  if (index_tau20 > 0) hist("hPhiLeading")->Fill(phi_roi.at(index_tau20));
        	  if (index_tau12 > 0) hist("hPhiSubLeading")->Fill(phi_roi.at(index_tau12));
        	  hist("hMultiTaus")->Fill(MultiTaus);
        	  hist("hMultiJets")->Fill(MultiJets);
                }
                if (!phi_roi.empty() &&min_dR_BothtausJet>2.8 && min_dR_BothtausJet < 50.) {
        	  if (indexJet_tau20 > 0) hist("hPTLeadingJet")->Fill(tau_val_roi.at(indexJet_tau20)/GeV);
        	  if (indexJet_tau12 > 0) hist("hPTSubLeadingJet")->Fill(tau_val_roi.at(indexJet_tau12)/GeV);
        	  if (indexJet_tau20 > 0) hist("hEtaLeadingJet")->Fill(eta_roi.at(indexJet_tau20));
        	  if (indexJet_tau12 > 0) hist("hEtaSubLeadingJet")->Fill(eta_roi.at(indexJet_tau12));
        	  if (indexJet_tau20 > 0) hist("hPhiLeadingJet")->Fill(phi_roi.at(indexJet_tau20));
        	  if (indexJet_tau12 > 0) hist("hPhiSubLeadingJet")->Fill(phi_roi.at(indexJet_tau12));
        	  hist("hMultiTausJet")->Fill(MultiTaus);
        	  hist("hMultiJetsJet")->Fill(MultiJets);
                }
 
        	if (disambiguation) hist("hDisambiguation")->Fill(2.);
        	if (!disambiguation) hist("hDisambiguation")->Fill(3.);
        } else {
        	hist("hDRBothtaus_noTopo")->Fill(min_dR_Bothtaus);
        	hist("hDRBothtausJet_noTopo")->Fill(min_dR_BothtausJet);
        	hist("hDRBothtausJetN_noTopo")->Fill(min_dR_BothtausJetN);
        	if (disambiguation) hist("hDisambiguation")->Fill(4.);
        	if (!disambiguation) hist("hDisambiguation")->Fill(5.);
        }
	return StatusCode::SUCCESS;

}

