/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARPEDESTALP
#define LARRAWCONDITIONS_LARPEDESTALP

#include <vector> 

// persistent c-struct 
class LArPedestalP{
public: 

    LArPedestalP() {}
    bool isEmpty() const { return (m_vPedestal.size() == 0); }
    std::vector<float> m_vPedestal;
    std::vector<float> m_vPedestalRMS;
};

     
#endif 

