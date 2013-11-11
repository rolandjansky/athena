/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetContainer_p1.h
// author: Kyle Cranmer, Sebastien Binet, Lukasz Janyst
// date:   April 2007
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAUJETCONTAINER_P1_H
#define tauEventTPCnv_TAUJETCONTAINER_P1_H

#include <vector>
#include <string>
#include "tauEventTPCnv/TauJet_p1.h"

class TauJetContainer_p1: public std::vector<TauJet_p1>
{
    public:
        std :: string m_ROIauthor;
};

#endif // tauEventTPCnv_TAUJETCONTAINER_P1_H
