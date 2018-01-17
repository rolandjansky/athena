/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBTDC_H
#define TBEVENT_TBTDC_H

#include "AthenaKernel/CLASS_DEF.h"

/* Author: Walter Lampl, Remi Lafaye */
/* Contains information from the TDC 
 * in the Beam Crate. The Phase is 
 * is given in ns (0 to 24)
 */

class TBTDC 
{
 public:
  TBTDC(); //Default constructor

  TBTDC(int tdc_value, int tdcmin_value, float scale_value=0.05);
  TBTDC(int phase_value, float scale_value=0.05);

  inline int phase() const
    {return m_phase;}

  inline int tdc() const
    {return m_tdc;}

  inline int tdcmin() const
    {return m_tdcmin;}

  float scale() const { return m_scale; }

 private:
  int m_tdc;
  int m_tdcmin;
  float m_scale; //Scale of the TDC (in ns)
  int m_phase;
};

CLASS_DEF(TBTDC,182194156,0)

#endif
