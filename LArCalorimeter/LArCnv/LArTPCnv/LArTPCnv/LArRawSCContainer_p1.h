/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWSCCONTAINER_P1_H
#define LARRAWSCCONTAINER_P1_H

#include <vector>


class LArRawSCContainer_p1   
{

public:

    std::vector<unsigned int> m_channelID;
    std::vector<unsigned short> m_chan;
    std::vector<unsigned int> m_sourceId;
    std::vector<unsigned short> m_nEnergies;
    std::vector<int> m_energies;
    std::vector<unsigned short> m_bcids;
    std::vector<bool> m_satur;
    
};


#endif
