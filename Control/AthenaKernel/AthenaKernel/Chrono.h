// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/Chrono.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Exception-safe @c IChronoSvc caller.
 */


#ifndef ATHENAKERNEL_CHRONO_H
#define ATHENAKERNEL_CHRONO_H


#include "GaudiKernel/IChronoSvc.h"
#include <string>


namespace Athena {


/**
 * @brief Exception-safe @c IChronoSvc caller.
 *
 * The way to time something using the Gaudi @c IChronoSvc is like this:
 *
 *@code
 *  chronosvc->chronoStart ("someName");
 *  doSomething();
 *  chronosvc->chronoStop ("someName");
 @endcode
 *
 * This is of course not exception-safe.
 *
 * This wrapper provides an exception-safe way of calling @c IChronoSvc:
 *
 * @code
 *  {
 *    Chrono chrono ("someName", chronosvc);
 *    doSomething();
 *  }
 @endcode
 */
class Chrono
{
public:
  /**
   * @brief Start a @c IChronoSvc timer.
   * @param name Name of the timer.
   * @param svc The @c IChronoSvc.
   *
   * The timer will stop when this object is destroyed.
   */
  Chrono (const std::string& name, IChronoSvc* svc)
    : m_name (name),
      m_svc (svc)
  {
    m_svc->chronoStart (m_name);
  }


  /**
   * @brief Destructor.  Stop the timer.
   */
  ~Chrono()
  {
    m_svc->chronoStop (m_name);
  }


private:
  std::string m_name;
  IChronoSvc* m_svc;
};


} // namespace Athena


#endif // not ATHENA_KERNEL_CHRONO_H
