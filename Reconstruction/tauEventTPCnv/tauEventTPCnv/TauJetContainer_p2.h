/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainer_p2.h
// author: Ilija Vukotic
// date:   April 2007
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINER_P2_H
#define tauEventTPCnv_TAUJETCONTAINER_P2_H

#include <vector>
#include <string>
#include "tauEventTPCnv/TauJet_p2.h"

class TauJetContainer_p2: public std::vector<TauJet_p2>
{
    public:
        std :: string m_ROIauthor;
};

#endif // tauEventTPCnv_TAUJETCONTAINER_P2_H
