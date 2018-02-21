/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _SingleTrackValidation_h_
#define _SingleTrackValidation_h_

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TH1F.h"

class SingleTrackValidation : public AthAlgorithm {

public:

    SingleTrackValidation(const std::string & name, ISvcLocator *pSvcLocator);
    ~SingleTrackValidation();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

    class Clockwork;
    Clockwork *m_c;

    TH1F* m_histos[162];

    SingleTrackValidation (const SingleTrackValidation&);
    SingleTrackValidation& operator= (const SingleTrackValidation&);
};
 
#endif
