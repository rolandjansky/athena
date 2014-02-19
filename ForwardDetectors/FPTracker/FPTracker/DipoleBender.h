/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_DIPOLEBENDER_H
#define FPTRACKER_DIPOLEBENDER_H

#include "IBender.h"
#include "FPTracker/FPTrackerConstants.h" // Bendplane enum
#include "boost/shared_ptr.hpp"

namespace FPTracker{
  class IParticle; 
 class DipoleBender: public IBender
  {
  public:
    DipoleBender(
		 Bendplane,
		 double length, 
		 double strength, 
		 double pbeam0, 
		 int side
		 );
    
    void bend(IParticle&)                const;
    std::string label()                  const;
    boost::shared_ptr< IBender > clone() const;
  private:
    
    static const std::string s_label;
    Bendplane     m_bendplane;
    double        m_length;
    double        m_strength;
    double        m_pbeam0;
    double        m_dside; 
  };
}
#endif
