/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBTriggerPatternUnit
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTRIGGERPATTERNUNIT_P1
#define TBTRIGGERPATTERNUNIT_P1


class TBTriggerPatternUnit_p1
{

public:

  unsigned int m_triggerWord;
  std::vector<bool> m_triggers;
};


#endif
