/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTVERTEXPRESELHYPO_H
#define HLTVERTEXPRESELHYPO_H

/********************************************************************
 *
 * NAME:     HLTVertexPreSelHypo.h
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

#include "TrigInterfaces/HypoAlgo.h"
#include <vector>
class HLTVertexPreSelHypo : public HLT::HypoAlgo
{

 public:

	HLTVertexPreSelHypo(const std::string& name, ISvcLocator* pSvcLocator);
	~HLTVertexPreSelHypo();

	//Athena Hooks
	HLT::ErrorCode hltInitialize();
	HLT::ErrorCode hltFinalize();
	HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

};
#endif

