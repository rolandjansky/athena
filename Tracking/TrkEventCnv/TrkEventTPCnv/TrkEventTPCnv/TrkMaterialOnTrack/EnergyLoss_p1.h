/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENERGYLOSS_P1_TRK_H
#define ENERGYLOSS_P1_TRK_H

namespace Trk
{
   /** @brief Persistent representation of class EnergyLoss.h
       @author Wolfgang Liebig <lhttp://consult.cern.ch/xwho/people/54608>
   */
   class EnergyLoss_p1
   {
  public:
     EnergyLoss_p1() : m_deltaE(0.0), m_sigmaDeltaE(0.0), m_sigmaMinusDeltaE(0.0), m_sigmaPlusDeltaE(0.0) {}

     float  m_deltaE;
     float  m_sigmaDeltaE;
     float  m_sigmaMinusDeltaE;
     float  m_sigmaPlusDeltaE;

   };
}

#endif
