/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVRINGERVALIDATION_H
#define NAVRINGERVALIDATION_H

#include "TrigEgammaValidation/NavAuxValidation.h"

#include "GaudiKernel/Algorithm.h"
#include "TH1F.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfHLTData/HLTFrame.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigNavigation/Navigation.h"
#include "TrigEgammaValidation/NavAux.h"

#include <string>
#include <map>

class NavRingerValidation : public NavAuxValidation 
{
 public:	
	NavRingerValidation(const std::string & name, ISvcLocator * pSvcLocator);
	~NavRingerValidation();

	StatusCode HLTValidation(const std::string &chain, float eta, float phi, float et, std::string lvl, int step, std::vector <HLT::TriggerElement *> &vec_TEs, std::vector < std::vector <ConfSeedTE> > &outputTEs);

	/**
         * \brief Compares the simulation meta-data information with the cluster position at LVL1 and generates efficiency plots.
	 *
	 * \param eta The eta position of the cluster from the simulation meta-data.
	 * \param phi The phi position of the cluster from the simulation meta-data.
	 * \param et The transverse energy of the cluster from the simulation meta-data in GeV.
	 *
	 */
	StatusCode NavigationMatch(const std::string &chain, float eta, float phi, float et);


	/**
	 * \brief Generate histograms for each chain.
	 */
	StatusCode chainExecute(const std::string &chain);

	StatusCode childInitialize();
	StatusCode childExecute();
	StatusCode childFinalize();

 private:
	ToolHandle<Trig::TrigDecisionTool> m_trigdec;
	const HLT::NavigationCore *m_nav;

	float error;
	TrigConf::HLTFrame *fHLTFrame;
	std::vector <std::string> vec_chain;

	bool m_checkElectrons, m_checkPhotons;
	unsigned int m_isemMask;	
};

#endif
