/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionTool/IDecisionUnpacker.h"
namespace Trig{
  IDecisionUnpacker::IDecisionUnpacker() :
    m_unpackedDecision(false), m_unpackedNavigation(false) {
    
  }

  IDecisionUnpacker::~IDecisionUnpacker(){
  }
}
