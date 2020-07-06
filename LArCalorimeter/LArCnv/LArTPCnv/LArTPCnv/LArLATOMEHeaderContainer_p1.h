/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARLATOMEHEADERCONTAINER_P1_H
#define LARLATOMEHEADERCONTAINER_P1_H

#include <vector>


class LArLATOMEHeaderContainer_p1   
{

public:

    std::vector<unsigned int> m_sourceID;
    std::vector<unsigned int> m_latomeID;
    std::vector<unsigned short> m_nChannels;
    std::vector<unsigned short> m_BCID;
    std::vector<unsigned int> m_l1ID;
    
};


#endif
