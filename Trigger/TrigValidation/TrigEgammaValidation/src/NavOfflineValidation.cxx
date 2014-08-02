/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/NavOfflineValidation.h"
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

#include "TrigConfHLTData/HLTFrame.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TH1F.h"
#include "TMath.h"

using namespace HLT;
using namespace std;

NavOfflineValidation::NavOfflineValidation(const std::string & name, ISvcLocator * pSvcLocator) 
   : NavAuxValidation(name, pSvcLocator) , m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
   fHLTFrame(0)
{
	declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
	declareProperty("Chains", vec_chain);
	declareProperty("IsemMask", m_isemMask = 0xFFFFFFFF);
	declareProperty("CheckPhotons", m_checkPhotons = true);
	declareProperty("CheckElectrons", m_checkElectrons = true);
	error = 0.4;	
}

NavOfflineValidation::~NavOfflineValidation()
{
}

StatusCode NavOfflineValidation::HLTValidation(float eta, float phi, float et, string lvl, int step, vector <TriggerElement *> &vec_TEs, \
	vector < vector <ConfSeedTE> > &outputTEs)
{ 
	int lvl_int;
	stringstream realStep;
	vector <TriggerElement *>::iterator TEs_itr;
	const TriggerElement * matchedTE = 0;
	vector <ConfSeedTE>::iterator itr;

	if (vec_TEs.size() == 0)
		throw "HLTValidation: vector of TEs is empty";
	if (outputTEs.size() == 0)
		throw "HLTValidation: vetor of outputTEs is empty";

	if (lvl == "L2_")
		lvl_int = 0;
	else
		lvl_int = 1;

	realStep << step +1;

	for (TEs_itr = vec_TEs.begin(); TEs_itr != vec_TEs.end(); TEs_itr++)
	{
		for (itr = outputTEs.at(lvl_int*3 + step).begin(); itr != outputTEs.at(lvl_int*3 + step).end(); itr++)
		{
			(*m_log) << MSG::INFO << "Label2: " << (*itr).m_te->name() << endreq;
			if (getTELabel(*TEs_itr) == (*itr).m_te->name())
			{
				if ((*itr).m_found == false)
				{
					(*itr).m_found = true;
					matchedTE = const_cast <TriggerElement *>(*TEs_itr); 
					break;
				}
			}
		}
	}

	bool stepPassed = true;
	for (itr = outputTEs.at(lvl_int*3 + step).begin(); itr != outputTEs.at(lvl_int*3+ step).end(); itr++)
	{
		if ((*itr).m_found == false)
			stepPassed = false;
	}
	if (!matchedTE->getActiveState()) 
		stepPassed = false;

	if (stepPassed)
	{
		cd("NavOfflineValidation/" + m_chain + "/PreOfflineEfficiencies");
                hist1("TE_" + lvl + "Step" + realStep.str() + "_Et")->Fill(et);
                hist1("TE_" + lvl + "Step" + realStep.str() + "_Eta")->Fill(eta);
                hist1("TE_" + lvl + "Step" + realStep.str() + "_Phi")->Fill(phi);

		if (step < 2)
		{
			vector <TriggerElement *> nextTEs;
			nextTEs.clear();

			nextTEs = m_nav->getDirectSuccessors(matchedTE);

			StatusCode sc = HLTValidation (eta, phi, et, lvl, step +1, nextTEs, outputTEs);
			if (sc.isFailure())
				return sc;
		} //if step <2
		else
		{
			vector <TriggerElement *> nextTEs;
                        nextTEs.clear();

                        nextTEs = m_nav->getDirectSuccessors(matchedTE);
			if (lvl == "L2_")
			{
				StatusCode sc = HLTValidation (eta, phi, et, "EF_", 0, nextTEs, outputTEs);
				if (sc.isFailure())
					return sc;
			}
		}
	} //if stepPassed

	return StatusCode::SUCCESS;
}

StatusCode NavOfflineValidation::NavigationMatch(float eta, float phi, float et)
{
	float errorEta, errorPhi;
	bool foundTE;

	vector <SeedTE> chainTEs;
	vector <SeedTE>::iterator TEs_itr;
	vector < vector <ConfSeedTE> > outputTEs;
	vector <ConfSeedTE>::iterator itr;
	const TrigRoiDescriptor *matchedRoid = 0;
	const TriggerElement *matchedTE = 0;
	vector <const TrigRoiDescriptor*> trigRoiDescriptorVec;

	errorEta = errorPhi = error;

	const TriggerElement *inode = m_nav->getInitialNode();
	const vector<TriggerElement *> suc_initial = m_nav->getDirectSuccessors(inode); // Initial Node's successor is the Initial RoI
	
	foundTE = false;
	chainTEs.clear();

	//Finding TEs for each chain
    if (fHLTFrame->getHLTChainList().size() == 0)
       throw "NavigationMatch: vector of chains is empty";
    

    for (unsigned lvl = 0; lvl < 2; lvl++) {

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
            
       for (unsigned step = 0; step < 3; step++) {
          for (unsigned sigs = 0; sigs < vec_sigs.size(); sigs++) {
             outputTEs.push_back(vector<ConfSeedTE> ());
             if (vec_sigs.at(sigs)->signature_counter() == (step + 1)) {
                vector <TrigConf::HLTTriggerElement *> sig_TEs = vec_sigs.at(sigs)->outputTEs();
                if (sig_TEs.size() == 0)
                   throw ("NavigationMatch: vector of outputTEs from " + compare_chain->name() + " is empty");
                              
                for (unsigned i = 0; i < sig_TEs.size(); i++)
                   outputTEs.at(lvl*3 + step).push_back(ConfSeedTE (sig_TEs.at(i), false));
                              
             } // if found signature
          } //for vec_sigs
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
				} // for outputTEs
			}
		}
	}

	if (foundTE)
	{
		const string label = "initialRoI";

		for (TEs_itr = chainTEs.begin(); TEs_itr != chainTEs.end(); TEs_itr++)
		{
			if (!const_cast<NavigationCore *>(m_nav)->getRecentFeatures((*TEs_itr).m_teL1, trigRoiDescriptorVec, label))
				throw ("An error ocurred while getting the features from the TriggerElement \"" + getTELabel((*TEs_itr).m_teL1) + "\"");

			if (trigRoiDescriptorVec.size() == 0)
				throw ("It was not possible to get the features from the TriggerElement \"" + getTELabel((*TEs_itr).m_teL1) + "\". Vector is empty.");

			for (unsigned i = 0; i < trigRoiDescriptorVec.size(); i++)
			{
				if (!trigRoiDescriptorVec.at(i))
				{
					(*m_log) << MSG::DEBUG << "Null pointer for trigRoiDescriptorVec" << endreq;
					continue;
				}
				if ((fabsf(eta - trigRoiDescriptorVec.at(i)->eta()) < errorEta) && (delta(phi - trigRoiDescriptorVec.at(i)->phi()) < errorPhi))
				{ 
					matchedRoid = trigRoiDescriptorVec.at(i);
					matchedTE = (*TEs_itr).m_teL2;
					errorEta = fabsf(eta - trigRoiDescriptorVec.at(i)->eta());
					errorPhi = delta(phi - trigRoiDescriptorVec.at(i)->phi());
				}
			} // for trigRoiDescriptor
		} // for chainTEs
	} // if foundTE

	if (matchedRoid)
	{ 
		cd("NavOfflineValidation/" + m_chain + "/PreOfflineEfficiencies");
		hist1("TE_L1_Et")->Fill(et);
		hist1("TE_L1_Eta")->Fill(eta);
		hist1("TE_L1_Phi")->Fill(phi);
		cd("NavOfflineValidation/" + m_chain + "/Dump");
		hist1("TE_L1_Eta")->Fill(matchedRoid->eta());
		hist1("TE_L1_Phi")->Fill(matchedRoid->phi());

		//LVL2 Step1
		if (matchedTE->getActiveState())
		{ 
			cd("NavOfflineValidation/" + m_chain + "/PreOfflineEfficiencies");
			hist1("TE_L2_Step1_Et")->Fill(et);
			hist1("TE_L2_Step1_Eta")->Fill(eta);
			hist1("TE_L2_Step1_Phi")->Fill(phi);
	
			vector <TriggerElement *> nextTEs;
			nextTEs.clear();
	
			nextTEs =m_nav->getDirectSuccessors(matchedTE);

			try
			{
				StatusCode sc;
				sc = HLTValidation (eta, phi, et, "L2_", 1, nextTEs, outputTEs);
				if (sc.isFailure())
					return sc;
			}
			catch (string message)
			{
				throw message;
			}
		} //if TE is Active L2Step1
	} //if matchedRoid
	return StatusCode::SUCCESS;
}

StatusCode NavOfflineValidation::childInitialize() {
   StatusCode sc;

   sc = m_trigdec.retrieve();
   if ( sc.isFailure() ) {
      (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
      return sc;
   }
   m_trigdec->ExperimentalAndExpertMethods()->enable();


   // remove the non-existing ones
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

      addDirectory("NavOfflineValidation/" + vec_chain[i] + "/PreOfflineEfficiencies");
      addHistogram(new TH1F("OffEtHist", "Offline E_{T} for single electron ; E_{T} [GeV] ; Count", 80, 0., 80.));
      addHistogram(new TH1F("OffEtaHist", "Offline \\eta for single electron ; \\eta ; Count", 128, -3.2, 3.2));
      addHistogram(new TH1F("OffPhiHist", "Offline \\phi for single electron ; \\phi [rad] ; Count", 128, -3.2, 3.2));

      addHistogram(new TH1F("TE_L1_Et", "Level 1 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L1_Eta", "Level 1 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L1_Phi", "Level 1 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));
	
      addHistogram(new TH1F("TE_L2_Step1_Et", "Level 2 Step1 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step1_Eta", "Level 2 Step1 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step1_Phi", "Level 2 Step1 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step2_Et", "Level 2 Step2 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step2_Eta", "Level 2 Step2 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step2_Phi", "Level 2 Step2 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step3_Et", "Level 2 Step3 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step3_Eta", "Level 2 Step3 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step3_Phi", "Level 2 Step3 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step1_Et", "EF Step2 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step1_Eta", "EF Step2 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step1_Phi", "EF Step2 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step2_Et", "EF Step2 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step2_Eta", "EF Step2 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step2_Phi", "EF Step2 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step3_Et", "EF Step3 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step3_Eta", "EF Step3 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step3_Phi", "EF Step3 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));
      //Dump Histograms
      addDirectory ("NavOfflineValidation/" + vec_chain[i] + "/Dump");
      addHistogram(new TH1F("TE_L1_Et", "Level 2 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L1_Eta", "Level 2 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L1_Phi", "Level 2 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step1_Et", "Level 2 Step1 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step1_Eta", "Level 2 Step1 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step1_Phi", "Level 2 Step1 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step2_Et", "Level 2 Step2 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step2_Eta", "Level 2 Step2 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step2_Phi", "Level 2 Step2 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_L2_Step3_Et", "Level 2 Step3 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step3_Eta", "Level 2 Step3 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step3_Phi", "Level 2 Step3 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step1_Et", "EF Step2 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step1_Eta", "EF Step2 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step1_Phi", "EF Step2 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step2_Et", "EF Step2 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step2_Eta", "EF Step2 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step2_Phi", "EF Step2 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));

      addHistogram(new TH1F("TE_EF_Step3_Et", "EF Step3 Offline E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step3_Eta", "EF Step3 Offline \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step3_Phi", "EF Step3 Offline \\phi ; \\phi" , 128, -3.2 , 3.2));
   }
   (*m_log) << MSG::DEBUG << "End of NavOfflineValidation::initialize()"  << endreq;
   return StatusCode::SUCCESS;
}

StatusCode NavOfflineValidation::childExecute()
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
	
	(*m_log) << MSG::DEBUG << "End of NavOfflineValidation::Execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode NavOfflineValidation::chainExecute()
{
	StatusCode sc;

	const DataHandle<egammaContainer> egCbegin,egCend;
	sc = m_storeGate->retrieve(egCbegin,egCend);
	if (sc.isFailure()) 
	{
		(*m_log) << MSG::WARNING << "No egammaContainer found in TDS. Aborting!!!" << endreq;
	    	return StatusCode::SUCCESS; // Don't stop proccessing events
  	}
	egammaContainer::const_iterator Itr, ItrE;

	for ( ; egCbegin != egCend; ++egCbegin) 
	{ // For each event collection
    		if ( ((egCbegin.key() == "ElectronCollection") && m_checkElectrons) || ((egCbegin.key() == "PhotonCollection") && (m_checkPhotons)) ) 
		{
      			Itr  = egCbegin->begin();
      			ItrE = egCbegin->end();
      			for (; Itr != ItrE; ++Itr) 
			{ // For each egamma
			// *Itr is an egamma
        			if (!(*Itr)) 
				{
          				(*m_log) << MSG::DEBUG << "*Itr is zero!" << endreq;
          				continue;
        			}
        			if ( ((*Itr)->isem() & m_isemMask) == 0 ) // If it is an electron/positron/photon
				{ 
          				cd("NavOfflineValidation/" + m_chain + "/PreOfflineEfficiencies");
          				hist1("OffEtHist")->Fill((*Itr)->et()/1.e3);
          				hist1("OffEtaHist")->Fill((*Itr)->eta()); 
					hist1("OffPhiHist")->Fill((*Itr)->phi());

					try
					{
						sc = NavigationMatch((*Itr)->eta(), (*Itr)->phi(), (*Itr)->et()/1.e3);
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
				} // isem()
			} //for everyone
		} // if it is an egammaCollection 
	} //for every collection
	(*m_log) << MSG::DEBUG << "End of NavOfflineValidation::execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode NavOfflineValidation::childFinalize()
{
	for (int chain = 0; chain < (int) vec_chain.size(); chain++)
	{	
		addDirectory("NavOfflineValidation/" + vec_chain[chain] + "/OfflineEfficiencies");
	  	std::vector<std::string> PreEffPattern;
		std::vector<std::string> PreEffNotPattern;
	  	PreEffPattern.push_back("NavOfflineValidation/" + vec_chain[chain] + "/PreOfflineEfficiencies");
  		PreEffNotPattern.push_back("Hist");
  		hist1("OffEtHist", "NavOfflineValidation/" + vec_chain[chain] + "/PreOfflineEfficiencies")->Sumw2();
	  	hist1("OffEtaHist", "NavOfflineValidation/" + vec_chain[chain] + "/PreOfflineEfficiencies")->Sumw2();
  		hist1("OffPhiHist", "NavOfflineValidation/" + vec_chain[chain] + "/PreOfflineEfficiencies")->Sumw2();
	
		std::map<std::string, TH1 *> PreHists;
  		getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);
	  	for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++) 
		{
	    		TH1F *newhist = new TH1F(*( (TH1F *) i->second));
    			addHistogram(newhist);
    			newhist->GetYaxis()->SetTitle("Efficiency");
	    		newhist->Sumw2();
	    		if (i->first.find_last_of("Et") == (i->first.size() - 1)) 
      				newhist->Divide(hist1("OffEtHist", "NavOfflineValidation/" + vec_chain[chain] + "/PreOfflineEfficiencies"));
			else if (i->first.find_last_of("Eta") == (i->first.size() - 1)) 	
				newhist->Divide(hist1("OffEtaHist", "NavOfflineValidation/" + vec_chain[chain] + "/PreOfflineEfficiencies"));
			else if (i->first.find_last_of("Phi") == (i->first.size() - 1)) 	
     				newhist->Divide(hist1("OffPhiHist", "NavOfflineValidation/" + vec_chain[chain] + "/PreOfflineEfficiencies"));
  		}
	}
	(*m_log) << MSG::DEBUG << "End of NavOfflineValidation::finalize()" << endreq;
  	return StatusCode::SUCCESS;
}

