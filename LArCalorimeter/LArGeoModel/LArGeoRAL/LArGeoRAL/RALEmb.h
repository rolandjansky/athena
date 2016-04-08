/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RALEmb.h
// JFB Sep 

// Access the HEC parameters from the NOVA database.

#ifndef LARGEORAL_RALEMB_H
#define LARGEORAL_RALEMB_H

#include "LArGeoCode/VDetectorParameters.h"

#include <limits.h>
#include <string>

namespace LArGeo {

  class RALEmb : public VDetectorParameters {

  public:

    RALEmb();
    virtual ~RALEmb();

    virtual double GetValue(const std::string&, 
                            const int i0 = INT_MIN,
                            const int i1 = INT_MIN,
                            const int i2 = INT_MIN,
                            const int i3 = INT_MIN,
                            const int i4 = INT_MIN );

  private:


    class Clockwork;
    Clockwork *m_c;
    bool m_oldDB;
    bool m_oldSagging;
    bool m_oldContract;

    RALEmb (const RALEmb&);
    RALEmb& operator= (const RALEmb&);
  };

} // namespace LArGeo

#endif
