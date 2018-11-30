/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VTUNE_PROFILERUNNER_H
#define VTUNE_PROFILERUNNER_H

// VTune include(s)
#include <ittnotify.h>

class VTuneProfileRunner {

  public:

      /// Standard constructor uses the API to resume collection
      VTuneProfileRunner() { __itt_resume(); }
      /// Standard destructor uses the API to pause collection
      ~VTuneProfileRunner() { __itt_pause(); }

};

#endif // VTUNE_PROFILERUNNER_H 
