/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBScintillatorCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBSCINTILLATORCONT_P1_H
#define TBSCINTILLATORCONT_P1_H


class TBScintillatorCont_p1
{

public:

  std::vector< float > m_signal, m_time_signal;            // from TBScintillator.h
  std::vector< bool > m_signal_overflow, m_time_overflow;  // from TBScintillator.h

  std::vector< std::string > m_tbDetectorName;  // from TBBeamDetector.h
  std::vector< bool > m_overflow;               // from TBBeamDetector.h

};


#endif
