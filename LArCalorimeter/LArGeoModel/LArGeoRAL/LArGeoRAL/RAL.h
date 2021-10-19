/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// RAL.h
//
// A doomed routine.  Do not develop this any further.  Muddle along
// until we drop the NOVA database entirely and then call the Relational
// abstract layer directly. --JFB.

#ifndef _LArGeo_RAL_h_
#define _LArGeo_RAL_h_

#include "LArGeoCode/VDetectorParameters.h"
#include "boost/scoped_ptr.hpp"

#include <limits.h>

namespace LArGeo {

  class RALExperimentalHall;
  class RALEmec;
  class RALEmb;
  class RALHec;


  class RAL : public VDetectorParameters {

  public:
    RAL();
    virtual ~RAL();
    
    virtual double GetValue(const std::string&, 
                            const int i0 = INT_MIN,
                            const int i1 = INT_MIN,
                            const int i2 = INT_MIN,
                            const int i3 = INT_MIN,
                            const int i4 = INT_MIN ) const override;

  private:

    // Pointers to the individual database access routines.  (If
    // there's a 'void*' below, it means that the corresponding
    // routine has not yet been implemented.)

    boost::scoped_ptr<RALExperimentalHall> m_ExpHall;
    boost::scoped_ptr<RALEmec>             m_Emec;
    boost::scoped_ptr<RALEmb>              m_Emb;
    boost::scoped_ptr<RALHec>              m_Hec;

  };

} // namespace LArGeo

#endif
