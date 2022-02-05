/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// RALHec.h
// JFB Sep 

// Access the HEC parameters from the NOVA database.

#ifndef LARGEORAL_RALHEC_H
#define LARGEORAL_RALHEC_H

#include "LArGeoCode/VDetectorParameters.h"

#include <limits.h>
#include <string>

namespace LArGeo {

  class RALHec : public VDetectorParameters {

  public:

    RALHec();
    virtual ~RALHec();

    virtual double GetValue(const std::string&, 
                            const int i0 = INT_MIN,
                            const int i1 = INT_MIN,
                            const int i2 = INT_MIN,
                            const int i3 = INT_MIN,
                            const int i4 = INT_MIN ) const override;

  private:


    class Clockwork;
    Clockwork *m_c;

    RALHec (const RALHec&);
    RALHec& operator= (const RALHec&);
  };

} // namespace LArGeo

#endif
