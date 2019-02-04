/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/



#include "JetEvent/JetAssociationBase.h"


JetAssociationBase::JetAssociationBase()
  : m_keyIndex(0)
  , m_jetKeyDescr(JetKeyDescriptorInstance::instance())
{ }

void JetAssociationBase::setName(const name_t& name)
{
  m_keyIndex = m_jetKeyDescr->getIndex(JetKeyConstants::AssoCat,name);
}

const JetAssociationBase::name_t& JetAssociationBase::name() const
{
  return m_jetKeyDescr->getKey(JetKeyConstants::AssoCat,m_keyIndex);
} 
