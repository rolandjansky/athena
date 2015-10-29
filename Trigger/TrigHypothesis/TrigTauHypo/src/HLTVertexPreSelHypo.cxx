/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************
 *
 * NAME:     HLTVertexPreSelHypo.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   Peter McNamara (peter.charles.mcnamara@cern.ch)
 * CREATED:   17th September 2015
 *
 * DESCRIPTION:
 *
 * Hypothesis Algorithm to store the FTK vertices and in the future
 *  make a selection using this information
 *
 *********************************************************************/
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "TrigTauHypo/HLTVertexPreSelHypo.h"
#include "xAODTracking/VertexContainer.h"


// Constructor
HLTVertexPreSelHypo::HLTVertexPreSelHypo(const std::string& name, ISvcLocator* pSvcLocator):
	HLT::HypoAlgo(name, pSvcLocator)
{
	
}

//Destructor
HLTVertexPreSelHypo::~HLTVertexPreSelHypo()
{  
}

//Athena Hooks
HLT::ErrorCode HLTVertexPreSelHypo::hltInitialize()
// ----------------------------------------------------------------------
{
	ATH_MSG_DEBUG("Initialising HLTVertexPreSelHypo");
	return HLT::OK;
}

HLT::ErrorCode HLTVertexPreSelHypo::hltFinalize()
{
	ATH_MSG_DEBUG("Finalising HLTVertexPreSelHypo"); 
	return HLT::OK;
}

HLT::ErrorCode HLTVertexPreSelHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass)
{
	//make no selections yet
	pass = true;
	
	ATH_MSG_DEBUG("Executing HLTVertexPreSelHypo"); 
	
	// get Vertex Container
	std::vector<const xAOD::VertexContainer*> ftkVertices;
 	HLT::ErrorCode hltStatus = getFeatures(inputTE,ftkVertices);
	
	if(hltStatus!=HLT::OK) {
		ATH_MSG_DEBUG("Search for Vertex container went wrong, exiting");
		return hltStatus;
	}
	else {
		if (ftkVertices.size() < 1) {
			ATH_MSG_DEBUG("No Vertex Container Found");
		}
		if(ftkVertices.size() >0) {
			ATH_MSG_DEBUG("found  " << ftkVertices.back()->size() << " vertices");
		}
	}
	
	return HLT::OK;
}
