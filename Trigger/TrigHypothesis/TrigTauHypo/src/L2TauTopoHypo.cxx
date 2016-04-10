/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file
#include <vector> 
#include <cmath>
 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "TrigTauHypo/L2TauTopoHypo.h"
#include "TrigNavStructure/TrigNavStructure.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"

// Constructor
L2TauTopoHypo::L2TauTopoHypo(const std::string& name, ISvcLocator* pSvcLocator):
	HLT::HypoAlgo(name, pSvcLocator)
{
//	declareProperty("useVertices", m_useVertices = true);
//	declareProperty("useLeadingTrackZ0", m_useLeadingTrackZ0 = true);
//	declareProperty("mustUseSameSource", m_mustUseSameSource = false);
//	declareProperty("acceptableZ0Distance", m_acceptableZ0Distance = 2);
}

//Destructor
L2TauTopoHypo::~L2TauTopoHypo()
{  
}

//Athena Hooks
HLT::ErrorCode L2TauTopoHypo::hltInitialize()
// ----------------------------------------------------------------------
{
	ATH_MSG_DEBUG("Initialising L2TauTopoHypo");
//	ATH_MSG_DEBUG("AcceptableZ0Distance: " << m_acceptableZ0Distance);
//	ATH_MSG_DEBUG("useVertices: " << m_useVertices);
//	ATH_MSG_DEBUG("mustUseSameSource: " << m_mustUseSameSource);
//	ATH_MSG_DEBUG("useLeadingTrackZ0: " << m_useLeadingTrackZ0);
	
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoHypo::hltFinalize()
{
	ATH_MSG_DEBUG("Finalising L2TauTopoHypo"); 
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass)
{
	pass = false;
	HLT::ErrorCode status;
        (void)inputTE;





        pass = true;
        return HLT::OK;


}


