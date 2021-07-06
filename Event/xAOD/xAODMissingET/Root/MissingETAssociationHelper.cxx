/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMissingET/MissingETAssociationHelper.h"

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

MissingETAssociationHelper::MissingETAssociationHelper()
  : m_map(nullptr)
{  }

MissingETAssociationHelper::MissingETAssociationHelper(const MissingETAssociationMap_v1* map)
  : m_map(map)
{  }

MissingETAssociationHelper::~MissingETAssociationHelper()
{  }

void MissingETAssociationHelper::setObjSelectionFlag(const MissingETAssociation_v1* assoc, size_t objIdx, bool status)
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::setObjSelectionFlag received a null pointer");
  size_t index = assoc->index();
  if(index >= m_useObjectFlags.size()) m_useObjectFlags.resize(index+1, 0);
  if(status) m_useObjectFlags.at(index) |= (1<<objIdx);
  else m_useObjectFlags.at(index) &= ~(1<<objIdx);
}

void MissingETAssociationHelper::setObjSelectionFlag(const MissingETAssociation_v1* assoc, const IParticle* pPart, bool status)
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::setObjSelectionFlag received a null pointer");
  this->setObjSelectionFlag(assoc,assoc->findIndex(pPart),status);
}

bool MissingETAssociationHelper::objSelected(const MissingETAssociation_v1* assoc, size_t objIdx) const
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::objSelected received a null pointer");
  size_t index = assoc->index();
  if(index >= m_useObjectFlags.size()) return false; // No flag for this association has been set to 1 yet
  return bool(m_useObjectFlags.at(index) & (1<<objIdx));
}

bool MissingETAssociationHelper::objSelected(const MissingETAssociation_v1* assoc, const IParticle* pPart) const
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::objSelected received a null pointer");
  return this->objSelected(assoc, assoc->findIndex(pPart));
}

MissingETBase::Types::bitmask_t MissingETAssociationHelper::getObjSelectionFlags(const MissingETAssociation_v1* assoc) const
{
  if(!assoc) throw std::runtime_error("MissingETAssociationHelper::getObjSelectionFlags received a null pointer");
  if(assoc->index() >= m_useObjectFlags.size()) return 0; // No flag for this association has been set to 1 yet
  return m_useObjectFlags.at(assoc->index());
}


