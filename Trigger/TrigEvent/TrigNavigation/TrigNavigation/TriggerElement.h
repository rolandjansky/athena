// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_TRIGGERELEMENT_H
#define TRIGNAVIGATION_TRIGGERELEMENT_H


#include "TrigNavStructure/TriggerElement.h"

#include "GaudiKernel/MsgStream.h"


MsgStream& operator<< ( MsgStream& m, const HLT::TriggerElement& te );
MsgStream& operator<< ( MsgStream& m, const HLT::TriggerElement::ObjectIndex& i );
#endif //TRIGGERELEMENT_H
