/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_ZEE_VALIDATION
#define NAV_ZEE_VALIDATION

#include "TrigEgammaValidation/NavAuxValidation.h"

#include "GaudiKernel/Algorithm.h"
#include "TH1F.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigNavigation/Navigation.h"

#include <string>
#include <map>

class NavZeeValidation : public NavAuxValidation 
{
 public:	
	NavZeeValidation(const std::string & name, ISvcLocator * pSvcLocator);
	~NavZeeValidation();

	StatusCode HLTValidation(float eta, float phi, float et, std::string lvl, int step, std::vector <HLT::TriggerElement *> &TEs, \
		std::vector <std::map <TrigConf::HLTTriggerElement *, bool> > &outputTEs);

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
	StatusCode NavigationMatch(float eta, float phi, float et);


	/*
	 *Processes "execute" for each chain.
	 *
	 *Returns:
	 * - If the storeGate is not retrieved properly, a warning is gonna be displayed, but SUCCESS will be returned.
	 * - returns from inner functions
	 */
	StatusCode chainExecute();


	StatusCode childInitialize ();
	StatusCode childExecute();
	StatusCode childFinalize();

 private:
	ToolHandle<Trig::TrigDecisionTool> m_trigdec;
	const HLT::NavigationCore *m_nav;

	float error;
	TrigConf::HLTFrame *fHLTFrame;
	std::vector <std::string> vec_chain;

	std::string m_chain;
	bool m_checkElectrons, m_checkPhotons;
	unsigned int m_isemMask;	
//temp
	int contNeg, contPos, found;
};

#endif
