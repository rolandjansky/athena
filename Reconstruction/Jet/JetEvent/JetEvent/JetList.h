/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETLIST_H
#define JETEVENT_JETLIST_H


#include <list>
class Jet;

namespace JetEvt {
  typedef std::list<const Jet*>   const_jet_list_t;
  typedef std::list<Jet*>         jet_list_t;
}
#endif
