/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/IValidationAlgo.h"
#include "TrigEgammaValidation/NavAuxValidation.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TMath.h"

using namespace std;
using namespace HLT;

NavAuxValidation::NavAuxValidation (const std::string & name, ISvcLocator * pSvcLocator) 
	: IValidationAlgo(name, pSvcLocator, "Validation")
{
}

NavAuxValidation::~NavAuxValidation ()
{
}

float NavAuxValidation::delta(float a)
{
        return fabsf(TMath::Pi() - fabsf(TMath::Pi() - fabsf(a)));
}

string NavAuxValidation::getTELabel( const HLT::TriggerElement* te ) const
{
        std::string s;
        return (TrigConf::HLTTriggerElement::getLabel(te->getId(), s) ? s : "");
}


