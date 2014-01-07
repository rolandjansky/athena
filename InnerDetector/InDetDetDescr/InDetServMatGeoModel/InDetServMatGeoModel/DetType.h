/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetType_H
#define DetType_H

#include <string>

namespace DetType {
  //public:

  enum Type { Pixel, ShortStrip, LongStrip};
  enum Part { Barrel, Endcap};

  inline std::string name( Type t) {
    if (t == Pixel) return "Pixel";
    else            return "Strip";
  }

  inline std::string name( Part p) {
    if (p == Barrel) return "Barrel";
    else             return "Endcap";
  }

  inline std::string name( Type t, Part p) { return name(p) + name(t);}

}

#endif
