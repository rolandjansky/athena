/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBTDC
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTDC_P1_H
#define TBTDC_P1_H


class TBTDC_p1
{

public:

  int m_tdc;
  int m_tdcmin;
  float m_scale; //Scale of the TDC (in ns)
  int m_phase;

};


#endif
