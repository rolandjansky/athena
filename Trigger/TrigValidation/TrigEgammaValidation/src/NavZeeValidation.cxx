/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/NavZeeValidation.h"
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

#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TH1F.h"
#include "TMath.h"

using namespace HLT;
using namespace std;

//Temporary!!
const double m_lowEtCut = 3;
const double m_maxEtaCut = 2.5;

NavZeeValidation::NavZeeValidation(const std::string & name, ISvcLocator * pSvcLocator) 
   : NavAuxValidation(name, pSvcLocator) , m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
     fHLTFrame(0)
{
	declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
	declareProperty("Chains", vec_chain);
	declareProperty("IsemMask", m_isemMask = 0xFFFFFFFF);
	declareProperty("CheckPhotons", m_checkPhotons = true);
	declareProperty("CheckElectrons", m_checkElectrons = true);
	error = 0.2;	
}

NavZeeValidation::~NavZeeValidation()
{
}

StatusCode NavZeeValidation::HLTValidation(float eta, float phi, float et, string lvl, int step, vector <TriggerElement *> &vec_TEs, \
	vector < map <TrigConf::HLTTriggerElement *, bool> > &outputTEs)
{
	int lvl_int;
	stringstream realStep;
	TriggerElement *matchedTE = 0;	

	if (vec_TEs.size() == 0)
		throw "HLTValidation: vector of TEs is empty";
	if (outputTEs.size() == 0)
		throw "HLTValidation: vector of outputTEs is empty";

	if (lvl == "L2_")
		lvl_int = 0;
	else
		lvl_int = 1;

	realStep << step+1;

	for (vector <TriggerElement *>::const_iterator TE_itr = vec_TEs.begin(); TE_itr != vec_TEs.end(); TE_itr++)
	{
		for (map <TrigConf::HLTTriggerElement *, bool>::iterator map_itr = outputTEs.at(lvl_int*3 + step).begin(); map_itr != outputTEs.at(lvl_int*3 + step).end(); map_itr++)
		{
			if ((*map_itr).first->name() == getTELabel(*TE_itr))
			{
				if ((*map_itr).second == false)
				{
					outputTEs.at(lvl_int*3 + step)[(*map_itr).first] = true;
					matchedTE = (*TE_itr);
					break;
				}
			}
		} // for outputTEs
	}//for vec_TEs

	bool stepPassed = true;
	for (map <TrigConf::HLTTriggerElement *, bool>::iterator itr = outputTEs.at(lvl_int*3 + step).begin(); itr != outputTEs.at(lvl_int*3 + step).end(); itr++)
	{
		if ((*itr).second == false)
			stepPassed = false;
	}
	if (!(matchedTE->getActiveState()))
		stepPassed = false;

	if (stepPassed)
	{
		cd("NavZeeValidation/" + m_chain + "/PreEfficiencies"); 
		hist1("TE_" + lvl + "Step" + realStep.str() + "_Et")->Fill(et); 
		hist1("TE_" + lvl + "Step" + realStep.str() + "_Eta")->Fill(eta);
		hist1("TE_" + lvl + "Step" + realStep.str() + "_Phi")->Fill(phi); 

		if (step < 2)
		{
			vector <TriggerElement *> nextTEs;
			nextTEs.clear();
			nextTEs = m_nav->getDirectSuccessors(matchedTE);

			StatusCode sc = HLTValidation (eta, phi, et, lvl, step+1, nextTEs, outputTEs);
			if (sc.isFailure())
				return sc;
		}
		else
		{
			vector <TriggerElement *> nextTEs;
			nextTEs.clear();
			nextTEs =m_nav->getDirectSuccessors(matchedTE);
			if (lvl == "L2_")
			{
				StatusCode sc = HLTValidation(eta, phi, et, "EF_", 0, nextTEs, outputTEs);
				if (sc.isFailure())
					return sc;
			}
		}
	} //if stepPassed

	return StatusCode::SUCCESS;
}

StatusCode NavZeeValidation::NavigationMatch(float eta, float phi, float et)
{ 
	float errorEta, errorPhi;
	bool foundTE = false;

	vector <vector <const TriggerElement *> > chainTEs;
	TriggerElement *matchedTE = 0;
	vector <map <TrigConf::HLTTriggerElement *, bool> > outputTEs;
	map <TrigConf::HLTTriggerElement *, bool>::iterator itr;
	const TrigRoiDescriptor *matchedRoid;

	errorEta = errorPhi = error;

	const TriggerElement *inode = m_nav->getInitialNode();
	const vector<TriggerElement *> suc_initial = m_nav->getDirectSuccessors(inode); // Initial Node's successor is the Initial RoI
	
	//Finding configured TEs for each chain
	const TrigConf::HLTChainList& vec_chains = fHLTFrame->getHLTChainList();
	if (vec_chains.size() == 0)
		throw "NavigationMatch: vector of chains is empty";

    for (int lvl = 0; lvl < 2; lvl++) {

       TrigConf::HLTChain* compare_chain(0);
       if (lvl == 0) {
          compare_chain = fHLTFrame->getHLTChainList().chain("HLT_" + m_chain);
          if(compare_chain==0)
             compare_chain = fHLTFrame->getHLTChainList().chain("L2_" + m_chain);
       } else {
          compare_chain = fHLTFrame->getHLTChainList().chain("EF_" + m_chain);
       }

       if (compare_chain==0)
          continue;

       vector <TrigConf::HLTSignature *> vec_sigs = compare_chain->signatureList();
       if (vec_sigs.size() == 0)
          throw "NavigationMatch: Vector of signatures is empty";

       for (unsigned step = 0; step < 3; step++)
          {
             for (unsigned sigs = 0; sigs < vec_sigs.size(); sigs++)
                {
                   if (vec_sigs.at(sigs)->signature_counter() == (step +1))
                      {
                         vector <TrigConf::HLTTriggerElement *> sig_TEs = vec_sigs.at(sigs)->outputTEs();
                         if (sig_TEs.size() == 0)
                            throw ("NavigationMatch: vector of outputTEs from " + compare_chain->name() + " is empty");

                         outputTEs.push_back(map <TrigConf::HLTTriggerElement *, bool> ());
                         for (unsigned TEs = 0; TEs < sig_TEs.size(); TEs++)
                            outputTEs.at(lvl*3 + step).insert(pair <TrigConf::HLTTriggerElement *, bool>(sig_TEs.at(TEs), false));
                      } //if found signature
                }// for vec_sigs
          } //for step
    } //for lvl

	chainTEs.clear();

	for (int rois = 0; rois != (int) suc_initial.size(); rois++)
	{
		TriggerElement *roi = (suc_initial.at(rois)); // Initial Roi
		chainTEs.push_back(vector<const TriggerElement *> ());

		const vector<TriggerElement *> TEs_l1 = m_nav->getDirectSuccessors(roi);
		for (vector<TriggerElement *>::const_iterator l1_itr = TEs_l1.begin(); l1_itr != TEs_l1.end(); l1_itr++) 
		{
			const vector<TriggerElement *> L2Step1 = m_nav->getDirectSuccessors(*l1_itr);

			for (vector<TriggerElement *>::const_iterator L2Step1_itr = L2Step1.begin(); L2Step1_itr != L2Step1.end(); L2Step1_itr++) 
			{
				for (itr = outputTEs.at(0).begin(); itr != outputTEs.at(0).end(); itr++)
				{
					if ((*itr).first->name() == getTELabel(*L2Step1_itr))
					{
						chainTEs.back().push_back(const_cast<const TriggerElement *>(*l1_itr));
						chainTEs.back().push_back(const_cast<const TriggerElement *>(*L2Step1_itr));
						outputTEs.at(0)[(*itr).first] = true;
						foundTE = true;
					}
				} //for outputTEs
			}
		}
	}

	matchedRoid = 0;
	if (foundTE)
	{
		for (unsigned i = 0; i < chainTEs.size(); ++i) 
		{
			std::vector<const TrigEMCluster *> trigEMCluster;
			std::vector<const TrigRoiDescriptor *> trigRoiDescriptorVec;
			std::vector<const TrigRoiDescriptor *> trigRoiDescriptorVecLvl1;

			if (chainTEs[i].size() == 0)
				continue;

			if (chainTEs[i][0]) 
			{
				if (!const_cast<NavigationCore *>(m_nav)->getRecentFeatures(chainTEs[i][0], trigRoiDescriptorVecLvl1, "initialRoI"))
					throw ("It was not possible to get TrigEMCluster from the TriggerElement \"" + getTELabel(chainTEs[i][0]));
			}
			 else 
			{
				(*m_log) << MSG::INFO << "chainTEs[" << i << "][0] is null." << endreq;
			}
			
			for (unsigned j = 0; j < trigRoiDescriptorVecLvl1.size(); ++j) 
			{
				if ((fabsf(eta - trigRoiDescriptorVecLvl1[j]->eta()) < errorEta) && (delta(phi - trigRoiDescriptorVecLvl1[j]->phi()) < errorPhi))
				{
					errorPhi = delta(phi - trigRoiDescriptorVecLvl1[j]->phi());
					errorEta = fabsf(eta - trigRoiDescriptorVecLvl1[j]->eta());
					matchedRoid = trigRoiDescriptorVecLvl1[j];
					matchedTE = const_cast <TriggerElement *>(chainTEs[i][1]);
				}
			}
		}
	} //if foundTE

	if (matchedRoid)
	{
		cd("NavZeeValidation/" + m_chain + "/PreEfficiencies"); 
		hist1("TE_L1_Et")->Fill(et);
		hist1("TE_L1_Eta")->Fill(eta);
		hist1("TE_L1_Phi")->Fill(phi);
		cd("NavZeeValidation/" + m_chain + "/Dump");
		hist1("TE_L1_Eta")->Fill(matchedRoid->eta());
		hist1("TE_L1_Phi")->Fill(matchedRoid->phi());

		bool stepPassed = true;
		for (itr = outputTEs.at(0).begin(); itr != outputTEs.at(0).end(); itr++)
		{
			if ((*itr).second == false)
				stepPassed = false;
		}
		if (!matchedTE->getActiveState())
			stepPassed = false;
		
		if (stepPassed)
		{
			cd("NavZeeValidation/" + m_chain + "/PreEfficiencies"); 
			hist1("TE_L2_Step1_Et")->Fill(et);
			hist1("TE_L2_Step1_Eta")->Fill(eta);
			hist1("TE_L2_Step1_Phi")->Fill(phi);

			vector <TriggerElement *> nextTEs = m_nav->getDirectSuccessors(matchedTE);
			try 
			{
				StatusCode sc = HLTValidation(eta, phi, et, "L2_", 1, nextTEs, outputTEs);
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

StatusCode NavZeeValidation::childInitialize() {
   StatusCode sc;
   sc = m_trigdec.retrieve();
   if ( sc.isFailure() ) {
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

      addDirectory("NavZeeValidation/" + vec_chain[i] + "/PreEfficiencies");
      addHistogram(new TH1F("TruthEtHist", "Truth E_{T} for Z->e+e; E_{T} [GeV] ; Count", 80, 0., 80.));
      addHistogram(new TH1F("TruthEtaHist", "Truth \\eta for Z->e+e; \\eta ; Count", 128, -3.2, 3.2));
      addHistogram(new TH1F("TruthPhiHist", "Truth \\phi for Z->e+e; \\phi [rad] ; Count", 128, -3.2, 3.2));

      addHistogram(new TH1F("TE_L1_Et", "Level 1 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L1_Eta", "Level 1 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L1_Phi", "Level 1 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step1_Et", "Level 2 Step1 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step1_Eta", "Level 2 Step1 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step1_Phi", "Level 2 Step1 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step2_Et", "Level 2 Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step2_Eta", "Level 2 Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step2_Phi", "Level 2 Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step3_Et", "Level 2 Step3 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step3_Eta", "Level 2 Step3 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step3_Phi", "Level 2 Step3 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step1_Et", "EF Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step1_Eta", "EF Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step1_Phi", "EF Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step2_Et", "EF Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step2_Eta", "EF Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step2_Phi", "EF Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step3_Et", "EF Step3 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step3_Eta", "EF Step3 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step3_Phi", "EF Step3 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
      //Dump Histograms
      addDirectory ("NavZeeValidation/" + vec_chain[i] + "/Dump");
      addHistogram(new TH1F("TE_L1_Et", "Level 2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L1_Eta", "Level 2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L1_Phi", "Level 2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step1_Et", "Level 2 Step1 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step1_Eta", "Level 2 Step1 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step1_Phi", "Level 2 Step1 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step2_Et", "Level 2 Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step2_Eta", "Level 2 Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step2_Phi", "Level 2 Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step3_Et", "Level 2 Step3 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step3_Eta", "Level 2 Step3 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step3_Phi", "Level 2 Step3 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step1_Et", "EF Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step1_Eta", "EF Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step1_Phi", "EF Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step2_Et", "EF Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step2_Eta", "EF Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step2_Phi", "EF Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step3_Et", "EF Step3 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step3_Eta", "EF Step3 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step3_Phi", "EF Step3 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
   }
   (*m_log) << MSG::DEBUG << "End of NavZeeValidation::initialize()"  << endreq;
   return StatusCode::SUCCESS;
}

StatusCode NavZeeValidation::childExecute()
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

	StatusCode sc;
	for (int i = 0; i < (int)vec_chain.size(); i++)
	{
		m_chain = vec_chain[i];
		sc = chainExecute();
		if (sc.isFailure())
		{
			(*m_log) << MSG::ERROR << "An Error ocurred during the processing of chain: \"" << m_chain << "\"" << endreq;
			return StatusCode::FAILURE;
		}
	}
	
	(*m_log) << MSG::DEBUG << "End of NavZeeValidation::Execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode NavZeeValidation::chainExecute()
{
	StatusCode sc;

	const DataHandle<McEventCollection> themcpartTES,mcpartTESend;
	McEventCollection::const_iterator Itr, ItrE;

  	sc = m_storeGate->retrieve( themcpartTES,mcpartTESend);
 	if (sc.isFailure()) 
	{
    		(*m_log) << MSG::WARNING << "No AOD MC truth particle container found in StoreGate. Aborting!!!" << endreq;
    		return StatusCode::SUCCESS; // Don't stop proccessing events
  	}

	for ( ; themcpartTES != mcpartTESend; ++themcpartTES) 
	{ // For each event collection
    		if (themcpartTES.key() == "TruthEvent") 
		{
      			for (Itr = themcpartTES->begin(), ItrE = themcpartTES->end(); Itr != ItrE; ++Itr) 
			{ //For each event
        			if (!(*Itr)) 
				{
          				(*m_log) << MSG::DEBUG << "*Itr is zero!" << endreq;
          				continue;
        			}

				HepMC::GenEvent::particle_const_iterator itrParticle, itrParticleE;  
        			for (itrParticle = (*Itr)->particles_begin(), itrParticleE = (*Itr)->particles_end(); itrParticle != itrParticleE; ++itrParticle) 
				{// For each particle in the event
					// *Itr is a GenEvent
					// *itrParticle is a GenParticle
					if (!(*itrParticle))
						continue;

					if ( fabsf((*itrParticle)->pdg_id()) == 23) //Finds a Z0
					{
						const HepMC::GenVertex* endVert=(*itrParticle)->end_vertex();
					    	// finds electrons
				    		if ( endVert != 0 ) 
						{
							HepMC::GenVertex::particles_out_const_iterator itrTruthDaughter = endVert->particles_out_const_begin();
							for (; itrTruthDaughter != endVert->particles_out_const_end(); itrTruthDaughter++) 
							{ // For each particle out of the decay
								if (fabs((*itrTruthDaughter)->pdg_id()) == 11) 
								{ // If it is an electron
								  // Fill the Truth Histograms for these particles
									float truthEt = (*itrTruthDaughter)->momentum().e()/cosh((*itrTruthDaughter)->momentum().eta());
									if ( (truthEt > m_lowEtCut) && (fabsf((*itrTruthDaughter)->momentum().eta()) <= m_maxEtaCut) ) 
									{
									// Only if they pass the lowEtCut (don't plot underlying events!) and are inside calo (eta < 2.5)
										cd("NavZeeValidation/" + m_chain + "/PreEfficiencies");
										hist1("TruthEtHist")->Fill(truthEt/1.e3);
										hist1("TruthEtaHist")->Fill((*itrTruthDaughter)->momentum().eta());
										hist1("TruthPhiHist")->Fill((*itrTruthDaughter)->momentum().phi());

										try 
										{
											sc = NavigationMatch((*itrTruthDaughter)->momentum().eta(), (*itrTruthDaughter)->momentum().phi(), truthEt/1.e3);
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
									} //If truth matched
								} //If Z decay is an electron
							} //For each particle of the decay
						}// If endVert
					} //If it is a Z
        			} //for each particle
			} //for event
		} // if it is a Truth Event 
	} //for every collection
	(*m_log) << MSG::DEBUG << "End of NavZeeValidation::execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode NavZeeValidation::childFinalize()
{
	for (int chain = 0; chain < (int) vec_chain.size(); chain++)
	{	
		addDirectory("NavZeeValidation/" + vec_chain[chain] + "/Efficiencies");
	  	std::vector<std::string> PreEffPattern;
		std::vector<std::string> PreEffNotPattern;
	  	PreEffPattern.push_back("NavZeeValidation/" + vec_chain[chain] + "/PreEfficiencies");
  		PreEffNotPattern.push_back("Hist");
  		hist1("TruthEtHist", "NavZeeValidation/" + vec_chain[chain] + "/PreEfficiencies")->Sumw2();
	  	hist1("TruthEtaHist", "NavZeeValidation/" + vec_chain[chain] + "/PreEfficiencies")->Sumw2();
  		hist1("TruthPhiHist", "NavZeeValidation/" + vec_chain[chain] + "/PreEfficiencies")->Sumw2();
	
		std::map<std::string, TH1 *> PreHists;
  		getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);
	  	for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++) 
		{
	    		TH1F *newhist = new TH1F(*( (TH1F *) i->second));
    			addHistogram(newhist);
    			newhist->GetYaxis()->SetTitle("Efficiency");
	    		newhist->Sumw2();
	    		if (i->first.find_last_of("Et") == (i->first.size() - 1)) 
      				newhist->Divide(hist1("TruthEtHist", "NavZeeValidation/" + vec_chain[chain] + "/PreEfficiencies"));
			else if (i->first.find_last_of("Eta") == (i->first.size() - 1)) 	
				newhist->Divide(hist1("TruthEtaHist", "NavZeeValidation/" + vec_chain[chain] + "/PreEfficiencies"));
			else if (i->first.find_last_of("Phi") == (i->first.size() - 1)) 	
     				newhist->Divide(hist1("TruthPhiHist", "NavZeeValidation/" + vec_chain[chain] + "/PreEfficiencies"));
  		}
	}
	(*m_log) << MSG::DEBUG << "End of NavZeeValidation::finalize()" << endreq;
  	return StatusCode::SUCCESS;
}

