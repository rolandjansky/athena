/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HasEvt.h"

AANTEvent* HasEvt::p_evt = NULL;

HasEvt::HasEvt() {}

HasEvt::~HasEvt() {}


void HasEvt::setEvt(AANTEvent* pevt) { p_evt = pevt; }

