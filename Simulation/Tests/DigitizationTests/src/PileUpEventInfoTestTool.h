/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_PILEUPEVENTINFOTESTTOOL
#define G4AT_PILEUPEVENTINFOTESTTOOL

#include "DigiTestToolBase.h"

class PixelID;

class PileUpEventInfoTestTool : public DigiTestToolBase {

 public:

  PileUpEventInfoTestTool(const std::string& name,
                            const std::string& type,
                            const IInterface* parent);

  StatusCode initialize();

  StatusCode processEvent();

  StatusCode finalize();

 private:

  StringProperty m_pileupInfo;

};

#endif
