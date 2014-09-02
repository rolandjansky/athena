/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEJETTOOLS_HADRONUTILS_H
#define PARTICLEJETTOOLS_HADRONUTILS_H

#include <utility>

class HadronClass {
 public:
  static std::pair<short,short> type(int);
  static bool HeavyHadron(int);
};

#endif
