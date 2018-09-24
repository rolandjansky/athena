/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMissingET/versions/MissingETAssociationHelper_v1.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODJet/JetAttributes.h"

#include "xAODBase/IParticleContainer.h"
#include "xAODBase/ObjectType.h"

#include <iterator>
#include <cstdio>

using namespace xAOD;

/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

MissingETAssociationHelper_v1::MissingETAssociationHelper_v1()
  : m_map(0)
{  }

MissingETAssociationHelper_v1::MissingETAssociationHelper_v1(const MissingETAssociationMap* map)
  : m_map(map)
{  }

MissingETAssociationHelper_v1::~MissingETAssociationHelper_v1()
{  }

void MissingETAssociationHelper_v1::setObjSelectionFlag(const MissingETAssociation* assoc, size_t objIdx, bool status)
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::setObjSelectionFlag received a null pointer");
  size_t index = assoc->index();
  if(index >= m_useObjectFlags.size()) m_useObjectFlags.resize(index+1, 0);
  if(status) m_useObjectFlags.at(index) |= (1<<objIdx);
  else m_useObjectFlags.at(index) &= ~(1<<objIdx);
}

void MissingETAssociationHelper_v1::setObjSelectionFlag(const MissingETAssociation* assoc, const IParticle* pPart, bool status)
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::setObjSelectionFlag received a null pointer");
  this->setObjSelectionFlag(assoc->findIndex(pPart),status);
}

bool MissingETAssociationHelper_v1::objSelected(const MissingETAssociation* assoc, size_t objIdx) const
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::objSelected received a null pointer");
  return bool(m_useObjectFlags.at(assoc->index()) & (1<<objIdx));
}

bool MissingETAssociationHelper_v1::objSelected(const MissingETAssociation* assoc, const IParticle* pPart) const
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::objSelected received a null pointer");
  return this->objSelected(assoc, assoc->findIndex(pPart));
}

MissingETBase::Types::bitmask_t MissingETAssociationHelper_v1::getObjSelectionFlag(const MissingETAssociation* assoc) const
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::getObjSelectionFlag received a null pointer");
  return m_useObjectFlags.at(assoc->index());
}


