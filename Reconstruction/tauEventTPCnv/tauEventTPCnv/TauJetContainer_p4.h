/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainer_p4.h
// author: Lukasz Janyst
// date:   May 2008
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINER_P4_H
#define tauEventTPCnv_TAUJETCONTAINER_P4_H

#include <vector>
#include <string>
#include "tauEventTPCnv/TauJet_p4.h"

class TauJetContainer_p4: public std::vector<TauJet_p4>
{
    public:
        std :: string m_ROIauthor;
};

#endif // tauEventTPCnv_TAUJETCONTAINER_P4_H
