/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/NavRateValidation.h"
#include "TrigEgammaValidation/FEXValidationAlgo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"
#include "GeneratorObjects/McEventCollection.h"
//Trigger Element
#include "TrigNavigation/NavigationCore.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTSignature.h"

#include "TH1F.h"
#include "TMath.h"

using namespace HLT;
using namespace std;

NavRateValidation::NavRateValidation(const std::string & name, ISvcLocator * pSvcLocator) 
   : NavAuxValidation(name, pSvcLocator) , m_trigdec("TrigDec::TrigDecisionTool"),
    fHLTFrame()
{
	declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
	declareProperty("Chains", vec_chain);
	declareProperty("IsemMask", m_isemMask = 0xFFFFFFFF);
	declareProperty("CheckPhotons", m_checkPhotons = true);
	declareProperty("CheckElectrons", m_checkElectrons = true);
	error = 0.4;	
}

NavRateValidation::~NavRateValidation()
{
}

StatusCode NavRateValidation::childInitialize() {

   if ( m_trigdec.retrieve().isFailure() ) {
      (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
      return StatusCode::FAILURE;
   }

   for (unsigned int i = 0; i < vec_chain.size(); i++) {
      const string & monitoredChain = vec_chain[i];
      if( ( fHLTFrame->getHLTChainList().chain("HLT_"+monitoredChain) == 0 ) &&
          ( fHLTFrame->getHLTChainList().chain("L2_"+monitoredChain) == 0 ) &&
          ( fHLTFrame->getHLTChainList().chain("EF_"+monitoredChain) == 0 ) ) {
          
         (*m_log) << MSG::WARNING << "The chains \"L2_" << monitoredChain << "\" and \"EF_" << monitoredChain << "\" and \"HLT_" << monitoredChain << "\" do not exist. These chains will be skipped." << endreq;
         vec_chain.erase(vec_chain.begin() + i);
         i--;
         continue;
      }
   }


   for (int i = 0; i < (int) vec_chain.size(); i++) {
      //Rates
      addDirectory ("NavRateValidation/" + vec_chain[i] + "/PreRates");
      addHistogram(new TH1F("TE_L1_Et", "Level 1 Rate E_{T} ; E_{T} [GeV]" , 80, 0. , 80.));
      addHistogram(new TH1F("TE_L1_Eta", "Level 1 Rate \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L1_Phi", "Level 1 Rate \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step1_Et", "EF Step2 Rate E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step1_Eta", "EF Step2 Rate \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step1_Phi", "EF Step2 Rate \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step2_Et", "EF Step2 Rate E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step2_Eta", "EF Step2 Rate \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step2_Phi", "EF Step2 Rate \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step3_Et", "EF Step2 Rate E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step3_Eta", "EF Step2 Rate \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step3_Phi", "EF Step2 Rate \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step1_Et", "EF Step2 Rate E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step1_Phi", "EF Step2 Rate \\phi ; \\phi" , 128, -3.2 , 3.2));
      addHistogram(new TH1F("TE_EF_Step1_Eta", "EF Step2 Rate \\eta ; \\eta" , 128 , -3.2, 3.2));

      addHistogram(new TH1F("TE_EF_Step2_Et", "EF Step2 Rate E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step2_Eta", "EF Step2 Rate \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step2_Phi", "EF Step2 Rate \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step3_Et", "EF Step2 Rate E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step3_Eta", "EF Step2 Rate \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step3_Phi", "EF Step2 Rate \\phi ; \\phi" , 128, -3.2 , 3.2));

   }
   (*m_log) << MSG::DEBUG << "End of NavRateValidation::initialize()"  << endreq;
   return StatusCode::SUCCESS;
}

StatusCode NavRateValidation::childExecute()
{
	m_nav = m_trigdec->ExperimentalAndExpertMethods()->getNavigation();
        if (!m_nav)
	{ 
                (*m_log) << MSG::ERROR << "Navigation is pointer to NULL!" << endreq;;
		return StatusCode::SUCCESS;
	}

	if (vec_chain.size() == 0)
	{
		(*m_log) << MSG::ERROR << "Chain vector is empty, there is nothing to be done" << endreq;
		return StatusCode::FAILURE;
	}
	for (int i = 0; i < (int)vec_chain.size(); i++)
	{
		m_chain = vec_chain[i];
		StatusCode sc = chainExecute();
		if (sc.isFailure())
		{
			(*m_log) << MSG::ERROR << "An Error ocurred during the processing of chain: \"" << m_chain << "\"" << endreq;
			return StatusCode::FAILURE;
		}
	}
	
	(*m_log) << MSG::DEBUG << "End of NavRateValidation::Execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode NavRateValidation::chainExecute()
{
	vector <TriggerElement *> L2Step1, L2Step2;
	vector <vector <const TriggerElement *> > chainTEs;

	const TriggerElement *inode = m_nav->getInitialNode();
	const vector<TriggerElement *> suc_initial = m_nav->getDirectSuccessors(inode); // Initial Node's successor is the Initial RoI
	bool foundTE = false;
	bool goodToGo = false;
	float et, eta, phi;

	//Initializing variables:
	et = eta = phi = -3.5;

	for (int rois = 0; rois != (int) suc_initial.size(); rois++)
	{
		TriggerElement *roi = (suc_initial.at(rois)); // Initial Roi
		chainTEs.push_back(vector<const TriggerElement *> ());

		const vector<TriggerElement *> TEs_l1 = m_nav->getDirectSuccessors(roi);
		for (vector<TriggerElement *>::const_iterator l1_itr = TEs_l1.begin(); l1_itr != TEs_l1.end(); l1_itr++) 
		{
			const vector<TriggerElement *> temp_L2Step1 = m_nav->getDirectSuccessors(*l1_itr);

			for (vector<TriggerElement *>::const_iterator L2Step1_itr = temp_L2Step1.begin(); L2Step1_itr != temp_L2Step1.end(); L2Step1_itr++) 
			{
				string chain_name = "L2_" + m_chain + "cl"; 
				if (chain_name == getTELabel(*L2Step1_itr))
				{ 
					L2Step1.push_back(*L2Step1_itr);
					foundTE = true;

					const TrigEMCluster * trigem;
					string temp;
					if (m_chain == "e10Ringer")
						temp = "";
					else
						temp = "TrigT2CaloEgamma";
					
					const string label = temp;
					if (!const_cast<NavigationCore *>(m_nav)->getRecentFeature(const_cast <const TriggerElement *>(*L2Step1_itr), trigem, label))
					{
						(*m_log) << MSG::WARNING << "It was not possible to get the features from " << getTELabel(*L2Step1_itr) << endreq;
						continue;
					}
					if (!trigem)
					{
						(*m_log) << MSG::WARNING << "TrigEMCluster is NULL" << endreq;
						continue;
					}
					et = trigem->et();
					eta = trigem->eta();
					phi = trigem->phi();
				}
			}
		}
	} 

	if (foundTE)
	{ 
		cd("NavRateValidation/" + m_chain + "/PreRates"); 
		hist1("TE_L1_Et")->Fill(et/1.e3);
		hist1("TE_L1_Eta")->Fill(eta);
		hist1("TE_L1_Phi")->Fill(phi);

		for (vector<TriggerElement *>::const_iterator L2Step1_itr = L2Step1.begin(); L2Step1_itr != L2Step1.end(); L2Step1_itr++)
		{
			if ((*L2Step1_itr)->getActiveState())
			{
				cd("NavRateValidation/" + m_chain + "/PreRates");
				hist1("TE_L2_Step1_Et")->Fill(et/1.e3);
				hist1("TE_L2_Step1_Eta")->Fill(eta);
				hist1("TE_L2_Step1_Phi")->Fill(phi);
				goodToGo = true;

				const vector <TriggerElement *> L2Step2 = m_nav->getDirectSuccessors(*L2Step1_itr);
				if (L2Step2.size() == 0)
				{
					(*m_log) << MSG::DEBUG << "It was not possible to get " << getTELabel(*L2Step1_itr) << "'s direct successors" << endreq;
					continue;
				}
				for (vector <TriggerElement *>::const_iterator L2Step2_itr = L2Step2.begin(); L2Step2_itr != L2Step2.end(); L2Step2_itr++)
				{
					if ((*L2Step2_itr)->getActiveState())
					{
						cd("NavRateValidation/" + m_chain + "/PreRates"); 
						hist1("TE_L2_Step2_Et")->Fill(et/1.e3);
						hist1("TE_L2_Step2_Eta")->Fill(eta);
						hist1("TE_L2_Step2_Phi")->Fill(phi);

						const vector <TriggerElement *> L2Step3 = m_nav->getDirectSuccessors(*L2Step2_itr);
						if (L2Step3.size() == 0)
						{
							(*m_log) << MSG::DEBUG << "It was not possible to get " << getTELabel(*L2Step2_itr) << "'s direct successors" << endreq;
							continue;
						}
						for (vector <TriggerElement *>::const_iterator L2Step3_itr = L2Step3.begin(); L2Step3_itr != L2Step3.end(); L2Step3_itr++)
						{
							if ((*L2Step3_itr)->getActiveState())
							{
								cd("NavRateValidation/" + m_chain + "/PreRates"); 
								hist1("TE_L2_Step3_Et")->Fill(et/1.e3);
								hist1("TE_L2_Step3_Eta")->Fill(eta);
								hist1("TE_L2_Step3_Phi")->Fill(phi);

								const vector <TriggerElement *> tempEFStep1 = m_nav->getDirectSuccessors(*L2Step3_itr);
								if (tempEFStep1.size() == 0)
								{
									(*m_log) << MSG::DEBUG << "It was not possible to get " << getTELabel(*L2Step3_itr) << "'s direct successors" << endreq;
									continue;
								}
								vector <TriggerElement *> EFStep1;
								for (vector <TriggerElement *>::const_iterator EFStep1_itr = tempEFStep1.begin(); EFStep1_itr != tempEFStep1.end(); EFStep1_itr++)
								{
									string chain_name = "EF_" + m_chain + "calo";
									if (chain_name == getTELabel(*EFStep1_itr))
										EFStep1.push_back(*EFStep1_itr);
								}
								if (EFStep1.size() == 0)
									continue;

								for (vector <TriggerElement *>::const_iterator EFStep1_itr = EFStep1.begin(); EFStep1_itr != EFStep1.end(); EFStep1_itr++)
								{
									if ((*EFStep1_itr)->getActiveState())
									{
										cd("NavRateValidation/" + m_chain + "/PreRates"); 
										hist1("TE_EF_Step1_Et")->Fill(et/1.e3);
										hist1("TE_EF_Step1_Eta")->Fill(eta);
										hist1("TE_EF_Step1_Phi")->Fill(phi);

										const vector <TriggerElement *> EFStep2 = m_nav->getDirectSuccessors(*EFStep1_itr);
										if (EFStep2.size() == 0)
										{
											(*m_log) << MSG::DEBUG << "It was not possible to get " << getTELabel(*EFStep1_itr) << "'s direct successors" << endreq;
											continue;
										}
										for (vector <TriggerElement *>::const_iterator EFStep2_itr = EFStep2.begin(); EFStep2_itr != EFStep2.end(); EFStep2_itr++)
										{
											if ((*EFStep2_itr)->getActiveState())
											{

												cd("NavRateValidation/" + m_chain + "/PreRates"); 
												hist1("TE_EF_Step2_Et")->Fill(et/1.e3);
												hist1("TE_EF_Step2_Eta")->Fill(eta);
												hist1("TE_EF_Step2_Phi")->Fill(phi);

												const vector <TriggerElement *> EFStep3 = m_nav->getDirectSuccessors(*EFStep2_itr);
												if (EFStep3.size() == 0)
												{
													(*m_log) << MSG::DEBUG << "It was not possible to get " << getTELabel(*EFStep2_itr) << "'s direct successors" << endreq;
													continue;
												}
												for (vector <TriggerElement *>::const_iterator EFStep3_itr = EFStep3.begin(); EFStep3_itr != EFStep3.end(); EFStep3_itr ++)
												{
													if ((*EFStep3_itr)->getActiveState())
													{
														cd("NavRateValidation/" + m_chain + "/PreRates"); 
														hist1("TE_EF_Step3_Et")->Fill(et/1.e3);
														hist1("TE_EF_Step3_Eta")->Fill(eta);
														hist1("TE_EF_Step3_Phi")->Fill(phi);
													} // if EFStep3
												} //for EFStep3
											} //if EFStep2 is active
										} // for EFStep2
									} //if EFStep1 is active
								} //for EFStp1
							} // if L2Step3 is active
						} //for L2Step3
					} // if L2Step2 is active
				} // for L2Step2				
			} //if L2Step1 active
			if (goodToGo)
				break;
		} // for L2Step1
	} //if foundTE

	(*m_log) << MSG::DEBUG << "End of NavRateValidation::execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode NavRateValidation::childFinalize()
{

	for (int chain = 0; chain < (int) vec_chain.size(); chain++)
        {       
                addDirectory("NavRateValidation/" + vec_chain[chain] + "/Rates");
                std::vector<std::string> PreEffPattern;
                std::vector<std::string> PreEffNotPattern;
                PreEffPattern.push_back("NavRateValidation/" + vec_chain[chain] + "/PreRates");
                PreEffNotPattern.push_back("L1");
                hist1("TE_L1_Et", "NavRateValidation/" + vec_chain[chain] + "/PreRates")->Sumw2();
                hist1("TE_L1_Eta", "NavRateValidation/" + vec_chain[chain] + "/PreRates")->Sumw2();
                hist1("TE_L1_Phi", "NavRateValidation/" + vec_chain[chain] + "/PreRates")->Sumw2();
        
                std::map<std::string, TH1 *> PreHists;
                getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);
                for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++) 
                {
                        TH1F *newhist = new TH1F(*( (TH1F *) i->second));
                        addHistogram(newhist);
                        newhist->GetYaxis()->SetTitle("Efficiency");
                        newhist->Sumw2();
                        if (i->first.find_last_of("Et") == (i->first.size() - 1)) 
                                newhist->Divide(hist1("TE_L1_Et", "NavRateValidation/" + vec_chain[chain] + "/PreRates"));
                        else if (i->first.find_last_of("Eta") == (i->first.size() - 1))         
                                newhist->Divide(hist1("TE_L1_Eta", "NavRateValidation/" + vec_chain[chain] + "/PreRates"));
                        else if (i->first.find_last_of("Phi") == (i->first.size() - 1))         
                                newhist->Divide(hist1("TE_L1_Phi", "NavRateValidation/" + vec_chain[chain] + "/PreRates"));
                }
        }

	(*m_log) << MSG::DEBUG << "End of NavRateValidation::finalize()" << endreq;
  	return StatusCode::SUCCESS;
}

