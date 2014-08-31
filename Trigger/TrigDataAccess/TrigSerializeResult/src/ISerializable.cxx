/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// \file ISerializable.cxx
/// \author Gianluca Comune <gianluca.comune@cern.ch>
///

#include "TrigSerializeResult/ISerializable.h"

ISerializable::ISerializable(){
  //m_te=itsTe;
}

ISerializable::~ISerializable() {}

/*
TriggerElement* ISerializable::getTE() const {
    return m_te;
}
void ISerializable::setTE(TriggerElement* theTE) {
    m_te=theTE;
}
*/
