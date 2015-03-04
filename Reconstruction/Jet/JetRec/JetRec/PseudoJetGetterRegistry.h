/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetGetterRegistry.h

#ifndef PseudoJetGetterRegistry_H
#define PseudoJetGetterRegistry_H

// David Adams
// February 2015
//
// Class to hold pseudojet getters indexed by label.
// Each pseudojet getter is expected to register itself here.
// JetIsolationtool uses this to find the appropriate getter
// for a given jet.

#include <string>
#include <map>
#include "JetInterface/IPseudoJetGetter.h"

class PseudoJetGetterRegistry {

public:

  typedef std::string Label;
  typedef std::map<std::string, const IPseudoJetGetter*> Map;

  // Add a getter.
  // Overwrites existing value.
  // Returns 0 for success.
  static int add(Label lab, const IPseudoJetGetter* ptool);
  static int add(const IPseudoJetGetter* ptool);

  // Return if a getter is registered.
  static bool has(Label lab);

  // Return the getter.
  static const IPseudoJetGetter* find(Label lab);

private:

  static Map m_map;

};
  
#endif
