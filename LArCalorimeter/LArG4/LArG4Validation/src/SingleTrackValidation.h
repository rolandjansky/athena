/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4VALIDATION_SINGLETRACKVALIDATION_H
#define LARG4VALIDATION_SINGLETRACKVALIDATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TH1F.h"

class SingleTrackValidation : public AthAlgorithm {

public:

    SingleTrackValidation(const std::string & name, ISvcLocator *pSvcLocator);
    ~SingleTrackValidation();
    StatusCode initialize() override;
    StatusCode execute() override;
    StatusCode finalize() override;

private:

    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
	, "CaloDetDescrManager"
	, "CaloDetDescrManager"
	, "SG Key for CaloDetDescrManager in the Condition Store" };

    class Clockwork;
    Clockwork *m_c;

    TH1F* m_histos[162]{};

    SingleTrackValidation (const SingleTrackValidation&);
    SingleTrackValidation& operator= (const SingleTrackValidation&);
};
 
#endif
