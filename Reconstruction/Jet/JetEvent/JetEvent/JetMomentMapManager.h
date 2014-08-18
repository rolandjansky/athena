// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETMOMENTMAPMANAGER_H
#define JETEVENT_JETMOMENTMAPMANAGER_H
/// ////////////////////////////////////////
/// \class JetMomentMapManager
///
/// \brief temporary fix to memory leaks.
///
///
///  The trigger navigation system doesn't work well with
///  the 16.0.0 implementation of JetMomentMap
///  This class avoids memory leak with minimal c++ change to 
///  the core of 16.0.0 (cache compatible).
///
///  This is a temporary class ! It shouldn't been needed anymore when
///  a proper design for JetCollection/JetMomentMap/StoreGate is found.
//// //////////////////////////////////////////////////

#include "JetEvent/JetMomentMap.h"

#include <set>
class JetMomentMap;
class JetMomentMapManager {

public:
  JetMomentMapManager(): m_noRemoveMode(false) {};
  ~JetMomentMapManager();
  void add(JetMomentMap *m);
  void remove(const JetMomentMap *m);
  void clear();
  static JetMomentMapManager s_instance;
protected:
  std::set<JetMomentMap*> m_maps;

  bool m_noRemoveMode;

};
#endif
