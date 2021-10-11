/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// RALEmec.h
// JFB Sep 

// Access the HEC parameters from the NOVA database.

#ifndef LARGEORAL_RALEMEC_H
#define LARGEORAL_RALEMEC_H

#include "LArGeoCode/VDetectorParameters.h"

#include <limits.h>
#include <string>

namespace LArGeo {

  class RALEmec : public VDetectorParameters {

  public:

    RALEmec();
    virtual ~RALEmec();

    virtual double GetValue(const std::string&, 
                            const int i0 = INT_MIN,
                            const int i1 = INT_MIN,
                            const int i2 = INT_MIN,
                            const int i3 = INT_MIN,
                            const int i4 = INT_MIN ) const override;

  private:


    class Clockwork;
    Clockwork *m_c;

    RALEmec (const RALEmec&);
    RALEmec& operator= (const RALEmec&);
  };

} // namespace LArGeo

#endif
