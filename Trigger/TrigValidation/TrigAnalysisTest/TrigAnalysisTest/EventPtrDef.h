/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigAnalysisTest_EVENTPTRDEF_H
#define TrigAnalysisTest_EVENTPTRDEF_H
/* This is a copy of TrigDecisionTool/EventPtrDef.h
 * should be removed and taken from the original once the
 * tag is propagated
 */

namespace asg {
   class SgTEvent;
}
class StoreGateSvc;

#ifdef ASGTOOL_STANDALONE
    typedef asg::SgTEvent* EventPtr_t;
#elif defined(ASGTOOL_ATHENA)
      typedef StoreGateSvc*  EventPtr_t;
#else
#   error "Wrong environment configuration detected!"
#endif

#endif
