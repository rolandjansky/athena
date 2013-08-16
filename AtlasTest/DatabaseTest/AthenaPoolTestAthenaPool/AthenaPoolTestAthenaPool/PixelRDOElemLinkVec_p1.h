/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVEC_P1_H
#define ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVEC_P1_H

#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"
#include <vector>

class PixelRDOElemLinkVec_p1
{
public:
 
    // Element links
    std::vector<ElementLinkInt_p2> m_pixelRDOs;

    // Element link container name lookup table
    ElementLinkContNames_p2       m_linkNames;

};

#endif

