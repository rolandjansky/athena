//  @file HLTTauMonTool_L1TopoLeptons.cxx
//  created by Eleni-Myrto Asimakopoulou <eleni.myrto.asimakopoulou@cern.ch>
#include "TProfile.h"
#include "HLTTauMonTool.h"
#include "AthenaKernel/Units.h"
#include "xAODEventInfo/EventInfo.h"
using namespace std;
using Athena::Units::GeV;
StatusCode HLTTauMonTool::L1TopoLeptons(const std::string & trigItem, const std::string & typeOfChain){
   
    ATH_MSG_DEBUG ("HLTTauMonTool::L1TopoLeptons");
   
	const xAOD::EventInfo* evtInfo = 0;	
	if( !evtStore()->retrieve(evtInfo, "EventInfo" ).isSuccess() )
	{
		ATH_MSG_DEBUG("Failed to retrieve EventInfo container.");
		//ATH_MSG_WARNING("Failed to retrieve EventInfo container.");
	}

    std::string chain = "HLT_"+trigItem;

	std::string chain_ditau_nonL1Topo = "HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM";
	std::string chain_eltau_nonL1Topo = "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo";
	std::string chain_mutau_nonL1Topo = "HLT_mu14_ivarloose_tau25_medium1_tracktwo";//_L1MU10_TAU12I-J25";

	int counter_ditau;
	int counter_mutau;
	int counter_eltau;

	if (typeOfChain == "ditau")
	{
	    setCurrentMonGroup("HLT/TauMon/Expert/TopoDiTau/"+trigItem);
	    if(getTDT()->isPassed(chain))
		{
			// creating the feature container
            Trig::FeatureContainer f = ( getTDT()->features(chain,m_HLTTriggerCondition) );
            Trig::FeatureContainer::combination_const_iterator comb(f.getCombinations().begin()), combEnd(f.getCombinations().end());
			//ATH_MSG_WARNING("Trigger chain " << chain << " contains " << comb->size() << " di-tau combinations.");
            if(comb->size()!=2){
                    ATH_MSG_DEBUG("Number of combinations for chain " << chain << " is "<< comb->size());		                    
            }
            std::vector<float> v_eta, v_phi, v_pt;
			for(;comb!=combEnd;++comb)
			{
				const std::vector< Trig::Feature<xAOD::TauJetContainer> > vec_HLTtau = comb->get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
				std::vector<Trig::Feature<xAOD::TauJetContainer> >::const_iterator topoTau = vec_HLTtau.begin(), topoTau_e = vec_HLTtau.end();
				if(topoTau==topoTau_e) ATH_MSG_DEBUG("TrigTauMerged TauJet container EMPTY in " << chain);
				ATH_MSG_DEBUG("Item "<< chain << ": " << vec_HLTtau.size() << " " << topoTau->label() << " containers");
				//ATH_MSG_WARNING("comb# "<< (comb-f.getCombinations().begin())+1 << " out of: " << comb->size() << ", has: " << vec_HLTtau.size() << " " << topoTau->label() << " containers");

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
				counter_ditau++;
			} // end of possible combinations loop
			if(v_eta.size()!=2)
			{
				ATH_MSG_DEBUG("Number of taus for chain " << chain << " is "<< v_eta.size());
			}
			float min_dR(999.);
			float min_eta1, min_eta2, min_phi1, min_phi2;
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
				//ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << min_dR << ". v_eta_tau1=" << min_eta1 << ", v_phi_tau1=" << min_phi1 << ". v_eta_tau2=" << min_eta2 << ", v_phi_tau2=" << min_phi2);
			}

            hist("hHLTdR")->Fill(min_dR);
			//ATH_MSG_WARNING("Final taus: v_eta_tau1=" << min_eta1 << ", v_phi_tau1=" << min_phi1 << ", v_eta_tau2=" << min_eta2 << ", v_phi_tau2=" << min_phi2 << " and dR=" << min_dR);

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
			//ATH_MSG_WARNING("Trigger contains " << mutauCombinations.size() << " mu-tau combinations.");

            float v_eta_muon, v_phi_muon;
            float v_eta_tau, v_phi_tau;

			// loop over all possible combinations
			std::vector<Trig::Combination>::const_iterator cIt;
			for ( cIt = mutauCombinations.begin(); cIt != mutauCombinations.end(); ++cIt ) 
			{
				const Trig::Combination& comb = *cIt;
				std::vector< Trig::Feature<xAOD::TauJetContainer> > tauCont = comb.get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
				std::vector< Trig::Feature<xAOD::MuonContainer> > muonCont;

				muonCont = comb.get<xAOD::MuonContainer>("",m_HLTTriggerCondition);
				ATH_MSG_DEBUG("Combination has " << tauCont.size() << " TauJetContainer and " << muonCont.size() << " MuonContainer.");
				//ATH_MSG_WARNING("Comb#: " << (cIt-mutauCombinations.begin())+1  << " out of: " << mutauCombinations.size() << ", has " << tauCont.size() << " TauJetContainer and " << muonCont.size() << " MuonContainer.");

				if(tauCont.size()>0 || muonCont.size()>0) 
				{
					//ATH_MSG_WARNING("Looking at TauJet combination with " << taus->size() << " taus and " << muon->size() << " muons," << " this one was " << (comb.active()?"":"not ") << "active.");

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

/*
					// Match highest-pt tau to an offline tau
					const xAOD::TauJetContainer * tau_cont = 0; // offline tau container
					if(evtStore()->retrieve(tau_cont, "Taus").isFailure()) 
					{
						ATH_MSG_WARNING("Failed to retrieve Taus container. Exiting.");
						return StatusCode::FAILURE;
					}
					else 
					{
						xAOD::TauJetContainer::const_iterator tauItr, tau_cont_end = tau_cont->end();
						for(tauItr=tau_cont->begin(); tauItr!=tau_cont_end; ++tauItr)
						{
							TLorentzVector offineTauTLV = (*tauItr)->p4();
							float dR = offineTauTLV.DeltaR(tauTLV_maxPt);
							if (dR < tmpR)
							{
								tmpR = dR;
								matchedOfflineTau = offineTauTLV;
							}
						}
						ATH_MSG_WARNING("Found matched Tau to Offline container! tmpR = " << tmpR << " and TLV.Mag(): " << matchedOfflineTau.Mag() << ".");
					}
*/
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

/*
					// Match highest-pt muon to an offline muon
					const xAOD::MuonContainer * muon_cont = 0; // offline muon container
					if(evtStore()->retrieve(muon_cont, "Muons").isFailure()) 
					{
						ATH_MSG_WARNING("Failed to retrieve Muons container. Exiting.");
						return StatusCode::FAILURE;
					}
					else 
					{
						xAOD::MuonContainer::const_iterator muonItr, muon_cont_end = muon_cont->end();
						for(muonItr=muon_cont->begin(); muonItr!=muon_cont_end; ++muonItr)
						{
							TLorentzVector offineMuonTLV = (*muonItr)->p4();
							float dR = offineMuonTLV.DeltaR(muonTLV_maxPt);
							if (dR < tmpR)
							{
								tmpR = dR;
								matchedOfflineMuon = offineMuonTLV;
							}
						}
						ATH_MSG_WARNING("Found matched Muon to Offline container! tmpR = " << tmpR << " and TLV.Mag(): " << matchedOfflineMuon.Mag() << ".");
					}
*/
					// Calculate the dR between the (highest-pT tau) and (highest-pT muon)
					float dR(999.);
					//dR = matchedOfflineTau.DeltaR(matchedOfflineMuon);
					//dR = tauTLV_maxPt.DeltaR(muonTLV_maxPt);
					dR = deltaR(v_eta_tau,v_eta_muon,v_phi_tau,v_phi_muon);

					// some monitoring messages:
					if ((dR==-999.) || (dR==999.))
					{
						//profile("TProfRecoL1_dR999_pt")->Fill(tauTLV_maxPt,muonTLV_maxPt);
						profile("TProfRecoL1_dR999_eta")->Fill(v_eta_tau,v_eta_muon);
						profile("TProfRecoL1_dR999_phi")->Fill(v_phi_tau,v_phi_muon);
						//ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". muonTLV_maxPt: (" << muonTLV_maxPt(0) << ", " << muonTLV_maxPt(1) << ", " << muonTLV_maxPt(2) << ", " << muonTLV_maxPt(3) << "), v_eta_muon=" << v_eta_muon << ", v_phi_muon=" << v_phi_muon << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}
					if (dR>3.)
					{
						//ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". muonTLV_maxPt: (" << muonTLV_maxPt(0) << ", " << muonTLV_maxPt(1) << ", " << muonTLV_maxPt(2) << ", " << muonTLV_maxPt(3) << "), v_eta_muon=" << v_eta_muon << ", v_phi_muon=" << v_phi_muon << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}

					if (dR!=0.)
					{
						hist("hHLTdR")->Fill(dR);
					}
					else
					{
						counterOfdR0_Topomutau++;
					}
					//ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". muonTLV_maxPt: (" << muonTLV_maxPt(0) << ", " << muonTLV_maxPt(1) << ", " << muonTLV_maxPt(2) << ", " << muonTLV_maxPt(3) << "), v_eta_muon=" << v_eta_muon << ", v_phi_muon=" << v_phi_muon << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);

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
				counter_mutau++;
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
			//ATH_MSG_WARNING("Trigger contains " << eltauCombinations.size() << " el-tau combinations.");

            float v_eta_electron, v_phi_electron;
            float v_eta_tau, v_phi_tau;

			// loop over all possible combinations
			std::vector<Trig::Combination>::const_iterator cIt;
			for ( cIt = eltauCombinations.begin(); cIt != eltauCombinations.end(); ++cIt ) 
			{
				const Trig::Combination& comb = *cIt;
				std::vector< Trig::Feature<xAOD::TauJetContainer> > tauCont = comb.get<xAOD::TauJetContainer>("TrigTauRecMerged",m_HLTTriggerCondition);
				std::vector< Trig::Feature<xAOD::ElectronContainer> > electronCont;
				electronCont = comb.get<xAOD::ElectronContainer>("",m_HLTTriggerCondition);
				//std::vector< Trig::Feature<JetCollection> >   jetC = comb.get<xAOD::JetCollection>();
				//ATH_MSG_WARNING("Comb#: " << (cIt-eltauCombinations.begin())+1  << " out of: " << eltauCombinations.size() << ", has " << tauCont.size() << " TauJetContainer and " << electronCont.size() << " ElectronContainer.");

				if(tauCont.size()>0 || electronCont.size()>0) 
				{
					//ATH_MSG_WARNING("Looking at TauJet combination with " << taus->size() << " taus and " << muon->size() << " muons," << " this one was " << (comb.active()?"":"not ") << "active.");

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

/*
					// Match highest-pt tau to an offline tau
					const xAOD::TauJetContainer * tau_cont = 0; // offline tau container
					if(evtStore()->retrieve(tau_cont, "Taus").isFailure()) 
					{
						ATH_MSG_WARNING("Failed to retrieve Taus container. Exiting.");
						return StatusCode::FAILURE;
					}
					else 
					{
						xAOD::TauJetContainer::const_iterator tauItr, tau_cont_end = tau_cont->end();
						for(tauItr=tau_cont->begin(); tauItr!=tau_cont_end; ++tauItr)
						{
							TLorentzVector offineTauTLV = (*tauItr)->p4();
							float dR = offineTauTLV.DeltaR(tauTLV_maxPt);
							if (dR < tmpR)
							{
								tmpR = dR;
								matchedOfflineTau = offineTauTLV;
							}
						}
						ATH_MSG_WARNING("Found matched Tau to Offline container! tmpR = " << tmpR << " and TLV.Mag(): " << matchedOfflineTau.Mag() << ".");
					}
*/

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

/*
					// Match highest-pt electron to an offline electron
					const xAOD::ElectronContainer * electron_cont = 0; // offline electron container
					if(evtStore()->retrieve(electron_cont, "Electrons").isFailure()) 
					{
						ATH_MSG_WARNING("Failed to retrieve Electrons container. Exiting.");
						return StatusCode::FAILURE;
					}
					else 
					{
						xAOD::ElectronContainer::const_iterator electronItr, electron_cont_end = electron_cont->end();
						for(electronItr=electron_cont->begin(); electronItr!=electron_cont_end; ++electronItr)
						{
							TLorentzVector offineElectronTLV = (*electronItr)->p4();
							float dR = offineElectronTLV.DeltaR(electronTLV_maxPt);
							if (dR < tmpR)
							{
								tmpR = dR;
								matchedOfflineElectron = offineElectronTLV;
							}
						}
						ATH_MSG_WARNING("Found matched Electron to Offline container! tmpR = " << tmpR << " and TLV.Mag(): " << matchedOfflineElectron.Mag() << ".");
					}
*/
					// Calculate the dR between the (highest-pT offline tau) and (highest-pT offline electron)
				    float dR(999.);
					//dR = matchedOfflineTau.DeltaR(matchedOfflineElectron);
					//dR = tauTLV_maxPt.DeltaR(electronTLV_maxPt);
					dR = deltaR(v_eta_tau,v_eta_electron,v_phi_tau,v_phi_electron);

					// some monitoring messages:
					if ((dR==-999.) || (dR==999.))
					{
						//profile("TProfRecoL1_dR999_pt")->Fill(tauTLV_maxPt,electronTLV_maxPt);
						profile("TProfRecoL1_dR999_eta")->Fill(v_eta_tau,v_eta_electron);
						profile("TProfRecoL1_dR999_phi")->Fill(v_phi_tau,v_phi_electron);
						//ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". electronTLV_maxPt: (" << electronTLV_maxPt(0) << ", " << electronTLV_maxPt(1) << ", " << electronTLV_maxPt(2) << ", " << electronTLV_maxPt(3) << "), v_eta_electron=" << v_eta_electron << ", v_phi_electron=" << v_phi_electron << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}
					if (dR>3.)
					{
						//ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". electronTLV_maxPt: (" << electronTLV_maxPt(0) << ", " << electronTLV_maxPt(1) << ", " << electronTLV_maxPt(2) << ", " << electronTLV_maxPt(3) << "), v_eta_electron=" << v_eta_electron << ", v_phi_electron=" << v_phi_electron << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);
					}

					if (dR!=0.)
					{
						hist("hHLTdR")->Fill(dR);
					}
					else
					{
						counterOfdR0_Topoeltau++;
					}
					//ATH_MSG_WARNING("Processing run: " << evtInfo->runNumber() << ", event: " << evtInfo->eventNumber() << ". Chain: "<< chain << ". Kinematics: dR=" << dR << ". electronTLV_maxPt: (" << electronTLV_maxPt(0) << ", " << electronTLV_maxPt(1) << ", " << electronTLV_maxPt(2) << ", " << electronTLV_maxPt(3) << "), v_eta_electron=" << v_eta_electron << ", v_phi_electron=" << v_phi_electron << ". tauTLV_maxPt: (" << tauTLV_maxPt(0) << ", " << tauTLV_maxPt(1) << ", " << tauTLV_maxPt(2) << ", " << tauTLV_maxPt(3) << "), v_eta_tau=" << v_eta_tau << ", v_phi_tau=" << v_phi_tau);

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
				counter_eltau++;
			} // end of possible combinations loop
		} // end of: "if(getTDT()->isPassed(chain))"
	} // end of: "if (typeOfChain == "eltau")"

	return StatusCode::SUCCESS;
} // end of "HLTTauMonTool::L1TopoLeptons"
