/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MUONSPSHOWERCONTAINER_P1_H
#define RECTPCNV_MUONSPSHOWERCONTAINER_P1_H

#include <vector>
#include "RecTPCnv/MuonSpShower_p1.h"

/** Persisent representation of the transient Rec::MuonSpShowerContainer class.
@author Edward.Moyse@cern.ch
*/ 
class MuonSpShowerContainer_p1 {

public:

     /** Default Constructor  needed for persistency */
    MuonSpShowerContainer_p1();

    MuonSpShowerContainer_p1(const MuonSpShowerContainer_p1&);
    MuonSpShowerContainer_p1& operator=(const MuonSpShowerContainer_p1&);

     /** Destructor */
    virtual ~MuonSpShowerContainer_p1();

    std::vector<MuonSpShower_p1*> m_showers;
    bool m_ownshowers;
};

#endif

