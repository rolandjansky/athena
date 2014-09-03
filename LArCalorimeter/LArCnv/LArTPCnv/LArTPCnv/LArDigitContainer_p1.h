/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITCONTAINER_P1_H
#define LARDIGITCONTAINER_P1_H

#include <vector>


class LArDigitContainer_p1   
{

public:

    std::vector<unsigned int> m_channelID;
    std::vector<unsigned char> m_gain;
    std::vector<unsigned short> m_nSamples;
    std::vector<unsigned short> m_samples;
    
};


#endif
