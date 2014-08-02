/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_AUX_VALIDATION
#define NAV_AUX_VALIDATION

#include "TrigEgammaValidation/IValidationAlgo.h"
#include "TrigNavigation/Navigation.h"

#include <string>

class NavAuxValidation : public IValidationAlgo
{
public:
	NavAuxValidation (const std::string & name, ISvcLocator * pSvcLocator);
	~NavAuxValidation ();

	float delta (float);

      	std::string getTELabel( const HLT::TriggerElement* te ) const;

	StatusCode FillDumpHistograms(const HLT::TriggerElement *te, std::string chain, std::string LVL, int step);

	const HLT::Navigation *m_nav;
private:

};

#endif
