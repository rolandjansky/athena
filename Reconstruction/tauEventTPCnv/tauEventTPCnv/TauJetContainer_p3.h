/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainer_p3.h
// author: Lukasz Janyst
// date:   May 2008
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINER_P3_H
#define tauEventTPCnv_TAUJETCONTAINER_P3_H

#include <vector>
#include <string>
#include "tauEventTPCnv/TauJet_p3.h"

class TauJetContainer_p3: public std::vector<TauJet_p3>
{
    public:
        std :: string m_ROIauthor;
};

#endif // tauEventTPCnv_TAUJETCONTAINER_P3_H
