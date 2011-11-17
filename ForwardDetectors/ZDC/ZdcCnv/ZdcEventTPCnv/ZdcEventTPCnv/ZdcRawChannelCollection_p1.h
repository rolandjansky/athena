///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcRawChannelCollection_p1.h 
// Persistent represenation of a ZdcRawChannelCollection
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef ZDCRAWCHANNELCOLLECTION_P1_H
#define ZDCRAWCHANNELCOLLECTION_P1_H

#include "ZdcEventTPCnv/ZdcRawChannel_p1.h"
#include <vector>

class ZdcRawChannelCollection_p1 : public std::vector<ZdcRawChannel_p1>
{
public:
    /// typedefs
    typedef std::vector<ZdcRawChannel_p1> ElemVector;
    typedef ElemVector::const_iterator const_iterator;
    typedef ElemVector::iterator       iterator;

    /// Default constructor
    ZdcRawChannelCollection_p1 () {}

      
private:
};

#endif
