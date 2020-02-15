/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTPTRDEF_H
#define EVENTPTRDEF_H

namespace asg {
   class SgTEvent;
}
class StoreGateSvc;

#ifdef XAOD_STANDALONE
    typedef asg::SgTEvent* EventPtr_t;
#elif !defined(XAOD_STANDALONE)
      typedef StoreGateSvc*  EventPtr_t;
#else
#   error "Wrong environment configuration detected!"
#endif

#endif
