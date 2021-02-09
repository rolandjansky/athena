/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSCDIGITCONTAINER_P1_H
#define LARSCDIGITCONTAINER_P1_H

#include <vector>


class LArSCDigitContainer_p1   
{

public:

    std::vector<unsigned int> m_channelID;
    std::vector<unsigned char> m_chan;
    std::vector<unsigned int> m_sourceId;
    std::vector<unsigned short> m_nSamples;
    std::vector<unsigned short> m_samples;
    std::vector<unsigned short> m_bcids;
    
};


#endif
