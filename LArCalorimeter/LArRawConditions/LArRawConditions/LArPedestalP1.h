/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARPEDESTALP1
#define LARRAWCONDITIONS_LARPEDESTALP1

#include "LArElecCalib/ILArPedestal.h"

// persistent c-struct 
class LArPedestalP1 
{
public: 

  LArPedestalP1():
    m_Pedestal(ILArPedestal::ERRORCODE),
    m_PedestalRMS(ILArPedestal::ERRORCODE) {}

    LArPedestalP1(const float pedestal, const float pedestalRMS) 
            : m_Pedestal(pedestal), m_PedestalRMS(pedestalRMS)  {} 
    bool isEmpty() const; 
    float m_Pedestal;
    float m_PedestalRMS;
};

inline bool LArPedestalP1::isEmpty() const  {
  return (m_Pedestal<=1.0+ILArPedestal::ERRORCODE || 
	  m_PedestalRMS<=1.0+ILArPedestal::ERRORCODE); 
}
     
#endif 

