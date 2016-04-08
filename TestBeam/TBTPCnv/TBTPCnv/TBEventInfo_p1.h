/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBEventInfo
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTINFO_P1_H
#define TBEVENTINFO_P1_H


class TBEventInfo_p1
{

public:

  int m_ev_number;
  int m_ev_clock;
  int m_ev_type;
  unsigned int m_run_num;
  float m_beam_moment;
  std::string m_beam_part;
  float m_cryoX;
  float m_cryoAngle;
  float m_tableY;
    
};


#endif
