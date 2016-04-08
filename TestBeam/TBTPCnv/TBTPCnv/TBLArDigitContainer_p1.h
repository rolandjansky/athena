/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBLArDigitContainer
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBLARDIGITCONTAINER_P1_H
#define TBLARDIGITCONTAINER_P1_H

#include <vector>


class TBLArDigitContainer_p1   
{

public:

    std::vector<unsigned int> m_channelID;
    std::vector<unsigned char> m_gain;
    std::vector<unsigned short> m_nSamples;
    std::vector<unsigned short> m_samples;
    
};


#endif
