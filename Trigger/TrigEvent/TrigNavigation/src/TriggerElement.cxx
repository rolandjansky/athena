/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/TriggerElement.h"

MsgStream& operator<< ( MsgStream& m, const HLT::TriggerElement::ObjectIndex& i ) {
  m << "SubTypeIdx: " << i.subTypeIndex() << " begin: " << i.objectsBegin() << " end: " << i.objectsEnd();
  return m;    
}

MsgStream& operator<< ( MsgStream& m, const HLT::TriggerElement& te ) {
  m << "TE id: " << te.getId() << " ac: " << te.getActiveState() << " err: " << te.getErrorState();
  return m;
}
