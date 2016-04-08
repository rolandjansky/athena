/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBADCRawCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBADCRAWCONT_P1_H
#define TBADCRAWCONT_P1_H


class TBADCRawCont_p1
{

public:

	typedef  unsigned int signal_type;

  std::vector< signal_type > m_adc;      // from TBADCRaw.h

  std::vector< std::string > m_tbDetectorName;  // from TBBeamDetector.h
  std::vector< bool > m_overflow;               // from TBBeamDetector.h

    
};


#endif
