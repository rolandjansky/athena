/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMULTIVARHYPO_ITHRESHOLDS_H
#define TRIGMULTIVARHYPO_ITHRESHOLDS_H

#include <vector>
#include <string>


namespace Ringer{
  
  class IThresholds{

    protected:
      double m_etamin;
      double m_etamax;
      double m_etmin;
      double m_etmax;
      double m_mumin;
      double m_mumax;
      std::vector<double> m_values;
  
    public:

      IThresholds(std::vector<double> th, 
                  double etamin,
                  double etamax,
                  double etmin, 
                  double etmax, 
                  double mumin, 
                  double mumax):
        m_etamin(etamin),m_etamax(etamax),m_etmin(etmin),m_etmax(etmax),m_mumin(mumin),m_mumax(mumax),m_values(th)
      {;}
  
      virtual ~IThresholds()
      {;}
      
      virtual double threshold() const=0;
      virtual double threshold(double) const=0;
      
      double etamin() const {return m_etamin;};
      double etamax() const {return m_etamax;};
      double etmin()  const {return m_etmin; };
      double etmax()  const {return m_etmax; };
      double mumin()  const {return m_mumin; };
      double mumax()  const {return m_mumax; };

      std::vector<double> values() const {return m_values;};
  
  
  };

}

#endif
