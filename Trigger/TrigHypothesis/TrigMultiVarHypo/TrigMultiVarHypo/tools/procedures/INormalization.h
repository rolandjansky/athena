/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMULTIVARHYPO_INORMALIZATION_H
#define TRIGMULTIVARHYPO_INORMALIZATION_H

#include <vector>



namespace Ringer{


  class INormalization{
  
    private:
  
      double m_etmin;
      double m_etmax;
      double m_etamin;
      double m_etamax;
      double m_mumin;
      double m_mumax;
    
    public:
      
      
      // Constructor
      INormalization( double etmin, double etmax, double etamin, double etamax, double mumin, double mumax):
        m_etmin(etmin),m_etmax(etmax),m_etamin(etamin),m_etamax(etamax),m_mumin(mumin),m_mumax(mumax)
      {;};
     
      // Destructor
      virtual ~INormalization()
      {;};

      virtual void execute( std::vector<float> & ) const=0;
  
      double etamin() const{ return m_etamin;};
      double etamax() const{ return m_etamax;};
      double etmin()  const{  return m_etmin;};
      double etmax()  const{  return m_etmax;};
      double mumin()  const{  return m_mumin;};
      double mumax()  const{  return m_mumax;};
   
  };

}
#endif
