/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMULTIVARHYPO_THRESHOLDS_H
#define TRIGMULTIVARHYPO_THRESHOLDS_H

#include "TrigMultiVarHypo/tools/procedures/IThresholds.h"
#include <vector>
#include <string>



namespace Ringer{

  class Thresholds : public IThresholds{

    public:

      Thresholds(std::vector<double> th, 
                 double etamin,
                 double etamax,
                 double etmin, 
                 double etmax, 
                 double mumin, 
                 double mumax):
                 IThresholds(th,etamin,etamax,etmin,etmax,mumin,mumax)
      {;}
  

      ~Thresholds(){;}
      
      double threshold()             const {return beta0();};
      double threshold(double avgmu) const {return (avgmu*alpha() + beta());};
      double alpha()                 const {return m_values[0];};
      double beta()                  const {return m_values[1];};
      double beta0()                 const {return m_values[2];}; 
  
  };///end of configuration

}

#endif
