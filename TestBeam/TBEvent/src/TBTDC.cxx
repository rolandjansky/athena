/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBTDC.h"


TBTDC::TBTDC() : m_tdc(0), m_tdcmin(0), m_scale(0),m_phase(0)
{
}

TBTDC::TBTDC(int tdc_value, int tdcmin_value, float scale_value) : m_tdc(tdc_value), m_tdcmin(tdcmin_value), m_scale(scale_value)
{m_phase=(int)(m_scale*(m_tdc-m_tdcmin));
}

TBTDC::TBTDC(int phase_value, float scale_value) : m_tdcmin(0),  m_scale(scale_value), m_phase(phase_value)
{
  m_tdc=(int)(m_phase/m_scale);
}
