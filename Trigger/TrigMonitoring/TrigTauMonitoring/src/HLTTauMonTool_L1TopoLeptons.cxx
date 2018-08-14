//  @file HLTTauMonTool_L1TopoLeptons.cxx
//  created by Eleni-Myrto Asimakopoulou <eleni.myrto.asimakopoulou@cern.ch>
#include "TProfile.h"
#include "HLTTauMonTool.h"
#include "AthenaKernel/Units.h"
#include "xAODEventInfo/EventInfo.h"
//#include "GaudiKernel/Property.h"

using namespace std;
using Athena::Units::GeV;
StatusCode HLTTauMonTool::L1TopoLeptons(const std::string & trigItem, const std::string & typeOfChain){
   
    ATH_MSG_DEBUG ("HLTTauMonTool::L1TopoLeptons");
   
	const xAOD::EventInfo* evtInfo = 0;	
	ATH_CHECK ( evtStore()->retrieve(evtInfo) );

    std::string chain = "HLT_"+trigItem;

	std::string chain_ditau_nonL1Topo = "HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM";
	std::string chain_eltau_nonL1Topo = "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo";
	std::string chain_mutau_nonL1Topo = "HLT_mu14_ivarloose_tau25_medium1_tracktwo";//_L1MU10_TAU12I-J25";

	if (typeOfChain == "ditau")
	{
	    setCurrentMonGroup("HLT/TauMon/Expert/TopoDiTau/"+trigItem);
	    if(getTDT()->isPassed(chain))
		{
			// creating the feature container
            Trig::FeatureContainer f = ( getTDT()->features(chain,m_HLTTriggerCondition) );
            Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
			if(comb->size()!=2){
                    ATH_MSG_DEBUG("Number of di-tau combinations for chain " << chain << " is "<< comb->size());
					if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("Number of di-tau combinations for chain " << chain << " is "<< comb->size()); 		                    
            }
            std::vector<float> v_eta, v_phi, v_pt;
			for(;comb!=combEnd;++comb)
			{
				const std::vector< Trig::Feature<xAOD::TauJetContainer> > vec_HLTtau = comb->get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
				std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator topoTau = vec_HLTtau.begin(), topoTau_e = vec_HLTtau.end();
				if(topoTau==topoTau_e) ATH_MSG_DEBUG("TrigTauMerged TauJet container EMPTY in " << chain);
				ATH_MSG_DEBUG("Item "<< chain << ": " << vec_HLTtau.size() << " " << topoTau->label() << " containers");
				if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("comb# "<< (comb-f.getCombinations().begin())+1 << " out of: " << comb->size() << ", has: " << vec_HLTtau.size() << " " << topoTau->label() << " containers");

				for(; topoTau != topoTau_e; ++topoTau)
				{
					if(topoTau->cptr())
					{
						if(topoTau->cptr()->size()==0) ATH_MSG_DEBUG("item "<< chain << ": TauJetContainer with " << topoTau->cptr()->size() << " TauJets");
						ATH_MSG_DEBUG("item "<< chain << ": TauJetContainer with " << topoTau->cptr()->size() << " TauJets");
						xAOD::TauJetContainer::const_iterator tauItr = topoTau->cptr()->begin();
						xAOD::TauJetContainer::const_iterator tauEnd = topoTau->cptr()->end();
						for(; tauItr != tauEnd; ++tauItr)
						{
							v_eta.push_back((*tauItr)->eta()); v_phi.push_back((*tauItr)->phi()); v_pt.push_back((*tauItr)->pt());
						}
					}
				}
			} // end of possible combinations loop
			if(v_eta.size()!=2)
			{
				ATH_MSG_DEBUG("Number of taus for chain " << chain << " is "<< v_eta.size());
			}
			float min_dR(999.);
			float min_eta1(999.), min_eta2(999.), min_phi1(999.), min_phi2(999.);
			for(unsigned int t1=0;t1<v_eta.size();t1++)
			{
				for(unsigned int t2=t1+1;t2<v_eta.size();t2++)
				{
					float dR = deltaR(v_eta.at(t1),v_eta.at(t2),v_phi.at(t1),v_phi.at(t2));
					if(dR<min_dR && dR!=0.)
					{
						min_dR=dR;
						min_eta1=v_eta.at(t1);
						min_eta2=v_eta.at(t2);
						min_phi1=v_phi.at(t1);
						min_phi1=v_phi.at(t2);
					}
	            }
			}
			if(min_dR<0.1)
			{
				ATH_MSG_DEBUG(" tau pair with dR="<<min_dR);
				for(unsigned int t1=0;t1<v_eta.size();t1++) ATH_MSG_DEBUG(" tau "<<t1<<": eta "<<v_eta.at(t1)<<", phi "<<v_phi.at(t1)<<", pt "<<v_pt.at(t1));
			}
			if (min_dR>3.0)
			{
				if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << min_dR << ". v_eta_tau1=" << min_eta1 << ", v_phi_tau1=" << min_phi1 << ". v_eta_tau2=" << min_eta2 << ", v_phi_tau2=" << min_phi2);
			}

            hist("hHLTdR")->Fill(min_dR);

			// check if chain is "HLT_mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25", i.e. reference nonL1Topo to calculate the L1DR efficiency
			if (chain == chain_ditau_nonL1Topo)
			{
				for(unsigned int i=0; i<m_topo_chains_ditau.size(); ++i)
				{
					setCurrentMonGroup("HLT/TauMon/Expert/TopoDiTau/"+m_topo_chains_ditau.at(i));
					int pass(0);
					std::string ditauChain = "HLT_"+m_topo_chains_ditau.at(i);
					if(getTDT()->isPassed(ditauChain)) pass = 1;
					profile("TProfRecoL1_dREfficiency")->Fill(min_dR,pass); // this dR value corresponds to the dR of the reference nonL1Topo chain
				}
			}
		} // end of: "if(getTDT()->isPassed(chain))"
	} // end of: "if (typeOfChain == "ditau")"

	if (typeOfChain == "mutau")
	{
		TLorentzVector tauTLV_maxPt, muonTLV_maxPt;//(0., 0., 0., 0.);
		TLorentzVector matchedOfflineTau, matchedOfflineMuon;

	    setCurrentMonGroup("HLT/TauMon/Expert/TopoMuTau/"+trigItem);
	    if(getTDT()->isPassed(chain))
		{
			// creating the feature container
			Trig::FeatureContainer f = ( getTDT()->features(chain,m_HLTTriggerCondition) );
	  		const std::vector<Trig::Combination>& mutauCombinations = f.getCombinations();
			
            float v_eta_muon(999.), v_phi_muon(999.);
            float v_eta_tau(999.), v_phi_tau(999.);

			// loop over all possible combinations
			std::vector<Trig::Combination>::const_iterator cIt;
			for ( cIt = mutauCombinations.begin(); cIt != mutauCombinations.end(); ++cIt ) 
			{
				const Trig::Combination& comb = *cIt;
				std::vector< Trig::Feature<xAOD::TauJetContainer> > tauCont = comb.get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
				std::vector< Trig::Feature<xAOD::MuonContainer> > muonCont;

				muonCont = comb.get<xAOD::MuonContainer>("",m_HLTTriggerCondition);
				ATH_MSG_DEBUG("Combination has " << tauCont.size() << " TauJetContainer and " << muonCont.size() << " MuonContainer.");
				if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("Comb#: " << (cIt-mutauCombinations.begin())+1  << " out of: " << mutauCombinations.size() << ", has " << tauCont.size() << " TauJetContainer and " << muonCont.size() << " MuonContainer.");

				if(tauCont.size()>0 || muonCont.size()>0) 
				{
					// Tau
					std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator topoTau = tauCont.begin(), topoTau_e = tauCont.end();
					if(topoTau==topoTau_e) ATH_MSG_DEBUG("TrigTauMerged TauJet container EMPTY in " << chain);
					// find the tau with the highest pT (in case there are multiple taus in the container)
					for(; topoTau != topoTau_e; ++topoTau)
					{
						if(topoTau->cptr())
						{
							if(topoTau->cptr()->size()==0) ATH_MSG_DEBUG("item "<< chain << ": TauJetContainer with " << topoTau->cptr()->size() << " TauJets");
							ATH_MSG_DEBUG("item "<< chain << ": TauJetContainer with " << topoTau->cptr()->size() << " TauJets");
							xAOD::TauJetContainer::const_iterator tauItr = topoTau->cptr()->begin();
							xAOD::TauJetContainer::const_iterator tauEnd = topoTau->cptr()->end();
							xAOD::TauJetContainer::const_iterator tauItr_maxPt;
							for(; tauItr != tauEnd; ++tauItr)
							{
								if ((*tauItr)->p4().Pt() > tauTLV_maxPt.Pt())
								{
									tauItr_maxPt = tauItr;
									tauTLV_maxPt = (*tauItr)->p4();
									v_eta_tau = (*tauItr)->eta();
									v_phi_tau = (*tauItr)->phi();
								}
							}
						}
					} // end loop over topoTau

					// Muon
					std::vector<Trig::Feature<xAOD::MuonContainer> >::const_iterator topoMuon = muonCont.begin(), topoMuon_e = muonCont.end();
					if(topoMuon==topoMuon_e) ATH_MSG_DEBUG("Muon container EMPTY in " << chain);
					// find the muon with the highest pT (in case there are multiple muons in the container)
					for(; topoMuon != topoMuon_e; ++topoMuon)
					{
						if(topoMuon->cptr())
						{
							if(topoMuon->cptr()->size()==0) ATH_MSG_DEBUG("item "<< chain << ": MuonContainer with " << topoMuon->cptr()->size() << " Muons");
							ATH_MSG_DEBUG("item "<< chain << ": MuonContainer with " << topoMuon->cptr()->size() << " Muons");
							xAOD::MuonContainer::const_iterator muonItr = topoMuon->cptr()->begin();
							xAOD::MuonContainer::const_iterator muonEnd = topoMuon->cptr()->end();
							xAOD::MuonContainer::const_iterator muonItr_maxPt;
							for(; muonItr != muonEnd; ++muonItr)
							{
								if ((*muonItr)->p4().Pt() > muonTLV_maxPt.Pt())
								{
									muonItr_maxPt = muonItr;
									muonTLV_maxPt = (*muonItr)->p4();
									v_eta_muon = (*muonItr)->eta();
									v_phi_muon = (*muonItr)->phi();
								}
							}
						}
					} // end loop over topoMuon

					// Calculate the dR between the (highest-pT tau) and (highest-pT muon)
					float dR(999.);
					dR = deltaR(v_eta_tau,v_eta_muon,v_phi_tau,v_phi_muon);

					if (dR!=0.)
					{
						hist("hHLTdR")->Fill(dR);
					}
					else
					{
						if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("mutau dR=0 ! Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". muonTLV_maxPt: (" << muonTLV_maxPt(0) << ", " << muonTLV_maxPt(1) << ", " << muonTLV_maxPt(2) << ", " << muonTLV_maxPt(3) << "), v_eta_muon=" << v_eta_muon << ", v_phi_muon=" << v_phi_muon << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}
					// some monitoring messages:
					if ( (dR>3.) || (dR<.1) )
					{
						if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". muonTLV_maxPt: (" << muonTLV_maxPt(0) << ", " << muonTLV_maxPt(1) << ", " << muonTLV_maxPt(2) << ", " << muonTLV_maxPt(3) << "), v_eta_muon=" << v_eta_muon << ", v_phi_muon=" << v_phi_muon << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}

					// check if chain is "HLT_mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25", i.e. reference nonL1Topo to calculate the L1DR efficiency
					if (chain == chain_mutau_nonL1Topo)
					{
						for(unsigned int i=0; i<m_topo_chains_mutau.size(); ++i){
							setCurrentMonGroup("HLT/TauMon/Expert/TopoMuTau/"+m_topo_chains_mutau.at(i));
							int pass(0);
							std::string mutauChain = "HLT_"+m_topo_chains_mutau.at(i);
							if(getTDT()->isPassed(mutauChain)) pass = 1;
							profile("TProfRecoL1_dREfficiency")->Fill(dR,pass); // this dR value corresponds to the dR of the reference nonL1Topo chain
						}
					}
				}
				else 
				{
					ATH_MSG_WARNING("MuonContainer or MuonContainer missing.");
				}
			} // end of possible combinations loop
		} // end of: "if(getTDT()->isPassed(chain))"
	} // end of: "if (typeOfChain == "mutau")"

	if (typeOfChain == "eltau")
	{
		TLorentzVector tauTLV_maxPt, electronTLV_maxPt;//(0., 0., 0., 0.);
		TLorentzVector matchedOfflineTau, matchedOfflineElectron;

	    setCurrentMonGroup("HLT/TauMon/Expert/TopoElTau/"+trigItem);
		if(getTDT()->isPassed(chain))
		{
			// creating the feature container
			Trig::FeatureContainer f = ( getTDT()->features(chain,m_HLTTriggerCondition) );
	  		const std::vector<Trig::Combination>& eltauCombinations = f.getCombinations();
			if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("Trigger contains " << eltauCombinations.size() << " el-tau combinations.");

            float v_eta_electron(999.), v_phi_electron(999.);
            float v_eta_tau(999.), v_phi_tau(999.);

			// loop over all possible combinations
			std::vector<Trig::Combination>::const_iterator cIt;
			for ( cIt = eltauCombinations.begin(); cIt != eltauCombinations.end(); ++cIt ) 
			{
				const Trig::Combination& comb = *cIt;
				std::vector< Trig::Feature<xAOD::TauJetContainer> > tauCont = comb.get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
				std::vector< Trig::Feature<xAOD::ElectronContainer> > electronCont;
				electronCont = comb.get<xAOD::ElectronContainer>("",m_HLTTriggerCondition);
				if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("Comb#: " << (cIt-eltauCombinations.begin())+1  << " out of: " << eltauCombinations.size() << ", has " << tauCont.size() << " TauJetContainer and " << electronCont.size() << " ElectronContainer.");

				if(tauCont.size()>0 || electronCont.size()>0) 
				{
					// Tau
					std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator topoTau = tauCont.begin(), topoTau_e = tauCont.end();
					if(topoTau==topoTau_e) ATH_MSG_DEBUG("TrigTauMerged TauJet container EMPTY in " << chain);

					// find the tau with the highest pT (in case there are multiple taus in the container)
					for(; topoTau != topoTau_e; ++topoTau)
					{
						if(topoTau->cptr())
						{
							if(topoTau->cptr()->size()==0) ATH_MSG_DEBUG("item "<< chain << ": TauJetContainer with " << topoTau->cptr()->size() << " TauJets");
							ATH_MSG_DEBUG("item "<< chain << ": TauJetContainer with " << topoTau->cptr()->size() << " TauJets");
							xAOD::TauJetContainer::const_iterator tauItr = topoTau->cptr()->begin();
							xAOD::TauJetContainer::const_iterator tauEnd = topoTau->cptr()->end();
							xAOD::TauJetContainer::const_iterator tauItr_maxPt;
							for(; tauItr != tauEnd; ++tauItr)
							{
								if ((*tauItr)->p4().Pt() > tauTLV_maxPt.Pt())
								{
									tauItr_maxPt = tauItr;
									tauTLV_maxPt = (*tauItr)->p4();
									v_eta_tau = (*tauItr)->eta();
									v_phi_tau = (*tauItr)->phi();
								}
							}
						}
					} // end loop over topoTau

					// Electron
					std::vector<Trig::Feature<xAOD::ElectronContainer> >::const_iterator topoElectron = electronCont.begin(), topoElectron_e = electronCont.end();
					if(topoElectron==topoElectron_e) ATH_MSG_DEBUG("Electron container EMPTY in " << chain);

					// find the electron with the highest pT (in case there are multiple electrons in the container)
					for(; topoElectron != topoElectron_e; ++topoElectron)
					{
						if(topoElectron->cptr())
						{
							if(topoElectron->cptr()->size()==0) ATH_MSG_DEBUG("item "<< chain << ": ElectronContainer with " << topoElectron->cptr()->size() << " Electrons");
							ATH_MSG_DEBUG("item "<< chain << ": ElectronContainer with " << topoElectron->cptr()->size() << " Electrons");
							xAOD::ElectronContainer::const_iterator electronItr = topoElectron->cptr()->begin();
							xAOD::ElectronContainer::const_iterator electronEnd = topoElectron->cptr()->end();
							xAOD::ElectronContainer::const_iterator electronItr_maxPt;
							for(; electronItr != electronEnd; ++electronItr)
							{
								if ((*electronItr)->p4().Pt() > electronTLV_maxPt.Pt())
								{
									electronItr_maxPt = electronItr;
									electronTLV_maxPt = (*electronItr)->p4();
									v_eta_electron = (*electronItr)->eta();
									v_phi_electron = (*electronItr)->phi();
								}
							}
						}
					} // end loop over topoElectron

					// Calculate the dR between the (highest-pT offline tau) and (highest-pT offline electron)
				    float dR(999.);
					dR = deltaR(v_eta_tau,v_eta_electron,v_phi_tau,v_phi_electron);

					if (dR!=0.)
					{
						hist("hHLTdR")->Fill(dR);
					}
					else
					{
						if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("mutau dR=0 ! Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". electronTLV_maxPt: (" << electronTLV_maxPt(0) << ", " << electronTLV_maxPt(1) << ", " << electronTLV_maxPt(2) << ", " << electronTLV_maxPt(3) << "), v_eta_electron=" << v_eta_electron << ", v_phi_electron=" << v_phi_electron << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}
					// some monitoring messages:
					if ( (dR>3.) || (dR<.1) )
					{
						if (m_doL1TopoLeptonsMonitoringWarnings) ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". electronTLV_maxPt: (" << electronTLV_maxPt(0) << ", " << electronTLV_maxPt(1) << ", " << electronTLV_maxPt(2) << ", " << electronTLV_maxPt(3) << "), v_eta_electron=" << v_eta_electron << ", v_phi_electron=" << v_phi_electron << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}

					// check if chain is "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo", i.e. reference nonL1Topo to calculate the L1DR efficiency
					if (chain == chain_eltau_nonL1Topo)
					{
						for(unsigned int i=0; i<m_topo_chains_eltau.size(); ++i)
						{
							setCurrentMonGroup("HLT/TauMon/Expert/TopoElTau/"+m_topo_chains_eltau.at(i));
							int pass(0);
							std::string eltauChain = "HLT_"+m_topo_chains_eltau.at(i);
							if(getTDT()->isPassed(eltauChain)) pass = 1;
							profile("TProfRecoL1_dREfficiency")->Fill(dR,pass); // this dR value corresponds to the dR of the reference nonL1Topo chain
						}
					}
				} 
				else 
				{
					ATH_MSG_WARNING("TauJetContainer or ElectronContainer missing.");
				}
			} // end of possible combinations loop
		} // end of: "if(getTDT()->isPassed(chain))"
	} // end of: "if (typeOfChain == "eltau")"

	return StatusCode::SUCCESS;
} // end of "HLTTauMonTool::L1TopoLeptons"
