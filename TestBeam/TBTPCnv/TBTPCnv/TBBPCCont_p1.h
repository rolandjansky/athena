/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBBPCCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBBPCCONT_P1_H
#define TBBPCCONT_P1_H


class TBBPCCont_p1
{

public:

  typedef double signal_type;

  // reconstructed (x,y)
  std::vector< signal_type > m_xPos,m_yPos;
  std::vector< signal_type > m_xErr,m_yErr;

  // associated pulse height sums
  std::vector< signal_type > m_xPulse, m_yPulse;

  // BPC can distinguish between "0" or "1 or more" particle hits
  std::vector< short > m_hitnumber;

  // overflow indicators
  std::vector< bool > m_xPosOverflow,   m_yPosOverflow;
  std::vector< bool > m_xPulseOverflow, m_yPulseOverflow;

  std::vector< bool > m_overflowSetFlag;

  std::vector< std::string > m_tbDetectorName;  // from TBBeamDetector.h
  std::vector< bool > m_overflow;               // from TBBeamDetector.h

};


#endif
