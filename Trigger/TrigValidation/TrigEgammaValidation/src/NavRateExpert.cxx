/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/NavRateExpert.h"
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
#include "GeneratorObjects/McEventCollection.h"
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

NavRateExpert::NavRateExpert(const std::string & name, ISvcLocator * pSvcLocator) 
   : NavAuxValidation(name, pSvcLocator) , m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
     fHLTFrame()
{
	declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
	declareProperty("Chains", vec_chain);
	declareProperty("IsemMask", m_isemMask = 0xFFFFFFFF);
	declareProperty("CheckPhotons", m_checkPhotons = true);
	declareProperty("CheckElectrons", m_checkElectrons = true);
}

NavRateExpert::~NavRateExpert()
{
}

StatusCode NavRateExpert::HLTRates(string lvl, int step, vector <TriggerElement *> &vec_TEs, vector < vector <ConfSeedTE> > &outputTEs)
{ 	
	int lvl_int;
	stringstream realStep;	
	vector <ConfSeedTE>::iterator itr;

	if (vec_TEs.size() == 0)
		throw "HLTRates: vector of TEs is empty";
	if (outputTEs.size() == 0)
		throw "HLTRates: vector of outputTEs is empty";

	if (lvl == "L2_")
		lvl_int = 0;
	else
		lvl_int = 1;

	realStep << step+1;

	for (vector <TriggerElement *>::const_iterator TE_itr = vec_TEs.begin(); TE_itr != vec_TEs.end(); TE_itr++)
	{
		for (itr = outputTEs.at(lvl_int*3 + step).begin(); itr != outputTEs.at(lvl_int*3 + step).end(); itr++)
		{
			if ((*itr).m_te->name() == getTELabel(*TE_itr))
			{
				if ((*itr).m_found == false)
				{
					(*itr).m_found = true;
					break;
				}
			}
		} // for outputTEs
	}//for vec_TEs

	bool stepPassed = true;
	for (itr = outputTEs.at(lvl_int*3 + step).begin(); itr != outputTEs.at(lvl_int*3 + step).end(); itr++)
	{
		if ((*itr).m_found == false)
			stepPassed = false;
	}
	for (vector <TriggerElement *>::const_iterator TEs_itr = vec_TEs.begin(); TEs_itr != vec_TEs.end(); TEs_itr++)
	{
		if ((*TEs_itr)->getActiveState() == false)
			stepPassed = false;
	}

	if (stepPassed)
	{
		ElectronFound[lvl_int*3 + step + 1] = true;
	
		if (step < 2)
		{
			vector <TriggerElement *> nextTEs;
			nextTEs.clear();

			for (vector <TriggerElement *>::const_iterator TEs_itr = vec_TEs.begin(); TEs_itr != vec_TEs.end(); TEs_itr++)
			{
				vector <TriggerElement *> temp = m_nav->getDirectSuccessors(*TEs_itr);
				for (vector <TriggerElement *>::const_iterator temp_itr = temp.begin(); temp_itr != temp.end(); temp_itr++)
					nextTEs.push_back(*temp_itr);
			}

			if (nextTEs.size() != 0)
			{
				try
				{
					StatusCode sc = HLTRates (lvl, step+1, nextTEs, outputTEs);
					if (sc.isFailure())
						return StatusCode::FAILURE;
				}
				catch (string message)
				{
					throw message;
				}
			}
		}
		else
		{
			vector <TriggerElement *> nextTEs;
			nextTEs.clear();
			
			for (vector <TriggerElement *>::const_iterator TEs_itr = vec_TEs.begin(); TEs_itr != vec_TEs.end(); TEs_itr++)
			{
				vector <TriggerElement *> temp = m_nav->getDirectSuccessors(*TEs_itr);
				for (vector <TriggerElement *>::const_iterator temp_itr = temp.begin(); temp_itr != temp.end(); temp_itr++)
					nextTEs.push_back(*temp_itr);
			}
			if ((lvl == "L2_") && (nextTEs.size() != 0))
			{
				try
				{
					StatusCode sc = HLTRates ("EF_", 0, nextTEs, outputTEs);
					if (sc.isFailure())
						return StatusCode::FAILURE;
				}
				catch (string message)
				{
					throw message;
				}
			}
		}
	} //if stepPassed
	return StatusCode::SUCCESS;
}

StatusCode NavRateExpert::NavigationRates()
{ 
	bool foundTE;

	vector <SeedTE> chainTEs;
	vector <SeedTE>::iterator TEs_itr;
	vector <vector <ConfSeedTE> > outputTEs;
	vector <ConfSeedTE>::iterator itr;

	const TriggerElement *inode = m_nav->getInitialNode();
	const vector<TriggerElement *> suc_initial = m_nav->getDirectSuccessors(inode); // Initial Node's successor is the Initial RoI
	
	foundTE = false;
	chainTEs.clear();

	//Finding configured TEs for each chain
	const TrigConf::HLTChainList& confChains = fHLTFrame->getHLTChainList();
	if (confChains.size() == 0)
		throw "NavigationRates: vector of chains is empty";

    for(unsigned int i_lvl = 0; i_lvl < 3; i_lvl++) {

       string s_lvl = i_lvl==0?"HLT_":(i_lvl==1?"L2_":"EF_");

       int lvl = i_lvl==0?0:i_lvl-1;

       TrigConf::HLTChain * chain = confChains.chain(s_lvl + m_chain);
       
       if(chain==0) continue; // no chain at this level

       vector <TrigConf::HLTSignature*> vec_sigs = chain->signatureList();

       if (vec_sigs.size() == 0)
          throw "NavigationRates: Vector of signatures is empty";

       for (unsigned step = 0; step < 3; step++) { // why only 3 steps? seems dangerous to hardcode this here
          
          for (unsigned sigs = 0; sigs < vec_sigs.size(); sigs++) {

             outputTEs.push_back(vector<ConfSeedTE> ());

             if( vec_sigs.at(sigs)->signature_counter() == (step + 1)) {  // that is very strange code, there is no guaranty that the step+1 is the counter

                vector <TrigConf::HLTTriggerElement *> sig_TEs = vec_sigs.at(sigs)->outputTEs();
                if (sig_TEs.size() == 0)
                   throw ("NavigationRates: vector of outputTEs from " + chain->name() + " is empty");
                
                for (unsigned TEs = 0; TEs < sig_TEs.size(); TEs++)
                   outputTEs.at(lvl*3 + step).push_back(ConfSeedTE (sig_TEs.at(TEs), false));
             } //if found signature

          } // for vec_sigs
          
       } //for step
    } //for lvl

	for (int rois = 0; rois != (int) suc_initial.size(); rois++)
	{
		TriggerElement *roi = (suc_initial.at(rois)); // Initial Roi

		const vector<TriggerElement *> TEs_l1 = m_nav->getDirectSuccessors(roi);
		for (vector<TriggerElement *>::const_iterator l1_itr = TEs_l1.begin(); l1_itr != TEs_l1.end(); l1_itr++) 
		{
			const vector<TriggerElement *> L2Step1 = m_nav->getDirectSuccessors(*l1_itr);

			for (vector<TriggerElement *>::const_iterator L2Step1_itr = L2Step1.begin(); L2Step1_itr != L2Step1.end(); L2Step1_itr++) 
			{
				for (itr = outputTEs.at(0).begin(); itr != outputTEs.at(0).end(); itr++)
				{
					if ((*itr).m_te->name() == getTELabel(*L2Step1_itr))
					{
						if ((*itr).m_found == false)
						{
							chainTEs.push_back(SeedTE (*l1_itr, *L2Step1_itr, rois));
							(*itr).m_found = true;
							foundTE = true;
							break;
						}
					}
				} //for outputTEs
			}
		}
	}

	if (foundTE)
	{
		ElectronFound[0] = true;

		bool stepPassed = true;
		for (itr = outputTEs.at(0).begin(); itr != outputTEs.at(0).end(); itr++)
		{
			if ((*itr).m_found == false)
				stepPassed = false;
		}
		for (TEs_itr = chainTEs.begin(); TEs_itr != chainTEs.end(); TEs_itr++)
		{
			if (!((*TEs_itr).m_teL2->getActiveState()))
				stepPassed = false;
		}
		if (stepPassed)
		{
			ElectronFound[1] = true;

			vector <TriggerElement *> nextTEs;
			nextTEs.clear();	
			for (TEs_itr = chainTEs.begin(); TEs_itr != chainTEs.end(); TEs_itr++)
			{
				vector <TriggerElement *> temp = m_nav->getDirectSuccessors((*TEs_itr).m_teL2);
				for (vector<TriggerElement *>::const_iterator temp_itr = temp.begin(); temp_itr != temp.end(); temp_itr++)
					nextTEs.push_back(*temp_itr);
			}
			try 
			{
				StatusCode sc = HLTRates("L2_", 1, nextTEs, outputTEs);
				if (sc.isFailure())
					return StatusCode::FAILURE;
			}
			catch (string message)
			{
				throw message;
			}
		} //if stepPassed
	}

	return StatusCode::SUCCESS;
}

StatusCode NavRateExpert::childInitialize()
{
   StatusCode sc = m_trigdec.retrieve();
   if ( sc.isFailure() ) {
      (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
      return StatusCode::FAILURE;
   }
   
   for (int i = 0; i < (int) vec_chain.size(); i++) {

      const string & monitoredChain = vec_chain[i];

      if( ( fHLTFrame->getHLTChainList().chain("HLT_"+monitoredChain) == 0 ) &&
          ( fHLTFrame->getHLTChainList().chain("L2_"+monitoredChain) == 0 ) &&
          ( fHLTFrame->getHLTChainList().chain("EF_"+monitoredChain) == 0 ) ) {
          
         (*m_log) << MSG::WARNING << "The chains \"L2_" << monitoredChain << "\" and \"EF_" << monitoredChain << "\" and \"HLT_" << monitoredChain << "\" do not exist. These chains will be skipped." << endreq;
         vec_chain.erase(vec_chain.begin() + i);
         i--;
         continue;
      }
   } //for all the input chains

   addDirectory ("NavRateExpert/PreRates");
   addHistogram(new TH1F("ExpertL1", "Level 1" , vec_chain.size() , 1. , (float)(vec_chain.size() + 1)));
   for (unsigned i = 1; i < vec_chain.size() +1; i++)
      hist1("ExpertL1")->GetXaxis()->SetBinLabel(i, vec_chain.at(i - 1).c_str());

   addHistogram(new TH1F("ExpertL2Step1", "Level 2 Step1" , vec_chain.size(), 1. , (float) (vec_chain.size() + 1)));
   for (unsigned i = 1; i < vec_chain.size() + 1; i++)
      hist1("ExpertL2Step1")->GetXaxis()->SetBinLabel(i, vec_chain.at(i - 1).c_str());

   addHistogram(new TH1F("ExpertL2Step2", "Level 2 Step2" , vec_chain.size(), 1. , (float) (vec_chain.size() + 1)));
   for (unsigned i = 1; i < vec_chain.size() + 1; i++)
      hist1("ExpertL2Step2")->GetXaxis()->SetBinLabel(i, vec_chain.at(i - 1).c_str());

   addHistogram(new TH1F("ExpertL2Step3", "Level 2 Step3" , vec_chain.size(), 1. , (float) (vec_chain.size() + 1)));
   for (unsigned i = 1; i < vec_chain.size() + 1; i++)
      hist1("ExpertL2Step3")->GetXaxis()->SetBinLabel(i, vec_chain.at(i - 1).c_str());

   addHistogram(new TH1F("ExpertEFStep1", "EF Step1" , vec_chain.size(), 1. , (float) (vec_chain.size() + 1)));
   for (unsigned i = 1; i < vec_chain.size() + 1; i++)
      hist1("ExpertEFStep1")->GetXaxis()->SetBinLabel(i, vec_chain.at(i -1).c_str());

   addHistogram(new TH1F("ExpertEFStep2", "EF Step2" , vec_chain.size(), 1. , (float) (vec_chain.size() + 1)));
   for (unsigned i = 1; i < vec_chain.size() + 1; i++)
      hist1("ExpertEFStep2")->GetXaxis()->SetBinLabel(i, vec_chain.at(i - 1).c_str());

   addHistogram(new TH1F("ExpertEFStep3", "EF Step3",  vec_chain.size(), 1. , (float) (vec_chain.size() + 1)));
   for (unsigned i = 1; i < vec_chain.size() + 1; i++)
      hist1("ExpertEFStep3")->GetXaxis()->SetBinLabel(i, vec_chain.at(i - 1).c_str());

   (*m_log) << MSG::DEBUG << "End of NavRateExpert::initialize()"  << endreq;
   return StatusCode::SUCCESS;
}

StatusCode NavRateExpert::childExecute()
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
		StatusCode sc;

		for (int j = 0; j < 6; j++)
			ElectronFound[j] = false;

		try 
		{
			sc = NavigationRates();
			if (sc.isFailure())
				return StatusCode::FAILURE;
		}
		catch (std::string message)
		{
			(*m_log) << MSG::WARNING << "Exception thrown: " << message << endreq;
		}	
		catch (...)
		{
			(*m_log) << MSG::WARNING << "Unknown Exception Thrown" << endreq;
		}

		cd ("NavRateExpert/PreRates");
		if (ElectronFound[0])
			hist1("ExpertL1")->Fill(i+1);

		if (ElectronFound[1])
			hist1("ExpertL2Step1")->Fill(i+1);

		if (ElectronFound[2])
			hist1("ExpertL2Step2")->Fill(i+1);

		if (ElectronFound[3])
			hist1("ExpertL2Step3")->Fill(i+1);

		if (ElectronFound[4])
			hist1("ExpertEFStep1")->Fill(i+1);

		if (ElectronFound[5])
			hist1("ExpertEFStep2")->Fill(i+1);

		if (ElectronFound[6])
			hist1("ExpertEFStep3")->Fill(i+1);
	} //for vec_chain
	
	(*m_log) << MSG::DEBUG << "End of NavRateExpert::Execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode NavRateExpert::childFinalize()
{
	addDirectory("NavRateExpert/Rates");
	std::vector<std::string> PreEffPattern;
	std::vector<std::string> PreEffNotPattern;
	PreEffPattern.push_back("NavRateExpert/PreRates");
	PreEffNotPattern.push_back("L1");
	hist1("ExpertL1", "NavRateExpert/PreRates")->Sumw2();

	std::map<std::string, TH1 *> PreHists;
	getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);
	for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++)
	{
		TH1F *newhist = new TH1F(*( (TH1F *) i->second));
		addHistogram(newhist);
		newhist->GetYaxis()->SetTitle("Efficiency");
		newhist->Sumw2();
		newhist->Divide(hist1("ExpertL1", "NavRateExpert/PreRates"));
	}

	(*m_log) << MSG::DEBUG << "End of NavRateExpert::finalize()" << endreq;
  	return StatusCode::SUCCESS;
}

