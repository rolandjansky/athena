/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_RATE_EXPERT
#define NAV_RATE_EXPERT

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

class NavRateExpert : public NavAuxValidation
{
 public:	
	NavRateExpert(const std::string & name, ISvcLocator * pSvcLocator);
	~NavRateExpert();

	StatusCode HLTRates(std::string lvl, int step, std::vector <HLT::TriggerElement *> &TEs, \
		std::vector <std::vector <ConfSeedTE> > &outputTEs);

	/*
	 * Checks the efficiency of a given chain, step by step, comparing with offline electrons.
	 * If the step which is being checked passed, eta, phi and et from the offline electron are filled in histograms.
	 *
	 *Arguments:
	 * -eta, phi and et from the offline electron
	 *
	 *This function only return StatusCode::SUCCESS
	 *In case of error, it throws Exceptions.
	 */
	StatusCode NavigationRates();

	StatusCode childInitialize ();
	StatusCode childExecute();
	StatusCode childFinalize();

 private:
	ToolHandle<Trig::TrigDecisionTool> m_trigdec;
	const HLT::NavigationCore *m_nav;

	TrigConf::HLTFrame *fHLTFrame;
	std::vector <std::string> vec_chain;

	std::string m_chain;
	bool m_checkElectrons, m_checkPhotons;
	unsigned int m_isemMask;	

	bool ElectronFound[7];
};

#endif
