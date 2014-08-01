/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @version: $Id: T2Timer.h 361213 2011-04-23 18:26:44Z bartoldu $
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2Timer
 *
 * @brief Helper class that uses scope to start and stop TrigTimer objects.
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2TIMER_H
#define TRIGT2BEAMSPOT_T2TIMER_H

/// Externals
#include "TrigTimeAlgs/TrigTimer.h"


namespace PESA
{

  class T2Timer
  {
  public:

    // Constructor
    T2Timer( TrigTimer* timer )
      : m_timer( timer )
      {
        if ( m_timer ) m_timer->start();
      }

    // Destructor
    ~T2Timer()
      {
        if ( m_timer ) m_timer->stop();
      }

  private:

    // Data members
    TrigTimer* m_timer;
  };

} // end namespace

#endif
