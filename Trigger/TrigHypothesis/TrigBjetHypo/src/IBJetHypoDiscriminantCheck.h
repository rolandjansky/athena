/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IBJetHypoDiscriminantCheck_hh
#define IBJetHypoDiscriminantCheck_hh

#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/AuxElement.h"

class IBJetHypoDiscriminantCheck: virtual public ::IAlgTool {
public:
  DeclareInterfaceID( IBJetHypoDiscriminantCheck, 1, 0 );
  virtual bool passThreshold(const SG::AuxElement&) const = 0;
};

#endif
