/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "RecTPCnv/MissingETComposition_p1.h"
#include "MissingETEvent/MissingETComposition.h"

MissingETComposition_p1::Weight_p1::Weight_p1(const MissingETHandlers::Weight& w)
      : wx(w.wpx()), wy(w.wpy()), wet(w.wet()), statusWord(w.statusWord())
{ }

