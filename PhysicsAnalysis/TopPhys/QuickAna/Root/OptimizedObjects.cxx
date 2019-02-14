/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/OptimizedObjects.h>

#include <AsgTools/StatusCode.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/OptimizedStore.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void OptimizedObjects ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
    RCU_INVARIANT (m_master != nullptr);
  }



  OptimizedObjects ::
  OptimizedObjects (OptimizedStore *val_master,
		    const CP::SystematicSet& val_systematics)
    : m_master (val_master), m_systematics (val_systematics)
  {
    for (std::size_t type = 0, end = ObjectTypeInfo::numTypes;
	 type != end; ++ type)
    {
      m_index[type] = OptimizedStore::noIndex;
      m_inputIndex[type] = OptimizedStore::noIndex;
    }

    RCU_NEW_INVARIANT (this);
  }



  StatusCode OptimizedObjects ::
  prepare () const
  {
    using namespace msgStore;

    RCU_READ_INVARIANT (this);
    for (unsigned mytype = 0, end = ObjectTypeInfo::numTypes;
	 mytype != end; ++ mytype)
    {
      const ObjectType type = ObjectType (mytype);
      if (m_inputIndex[type] != OptimizedStore::noIndex &&
	  m_index[type] != m_inputIndex[type])
      {
	ANA_CHECK (m_master->prepareCopy (type, m_index[type], m_inputIndex[type]));
      }
      if (type == OBJECT_EVENT_SELECT &&
	  !passSelection())
	return StatusCode::SUCCESS;
    }
    return StatusCode::SUCCESS;
  }



  bool OptimizedObjects ::
  passSelection () const
  {
    using namespace msgStore;

    RCU_READ_INVARIANT (this);
    if (m_inputIndex[OBJECT_EVENT_SELECT] != OptimizedStore::noIndex)
    {
      auto select = eventSelect();
      if (select && select->isAvailable<ana::SelectType> ("ana_select"))
	return select->auxdata<ana::SelectType> ("ana_select");
    }
    return true;
  }



  void *OptimizedObjects ::
  getVoid (ObjectType type) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);
    return m_master->getVoid (type, m_index[type]);
  }



  StatusCode OptimizedObjects ::
  addCopy (ObjectType type, const std::string& name)
  {
    using namespace msgStore;

    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);
    if (m_inputIndex[type] != OptimizedStore::noIndex)
    {
      ANA_MSG_ERROR ("trying to create new object instead of \"decorating\" the existing one");
      return StatusCode::FAILURE;
    }
    ANA_CHECK (m_master->addCopy (type, m_index[type], name));
    return StatusCode::SUCCESS;
  }



  StatusCode OptimizedObjects ::
  addNew (ObjectType type)
  {
    using namespace msgStore;

    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);
    if (m_inputIndex[type] != OptimizedStore::noIndex)
    {
      ANA_MSG_ERROR ("trying to create new object instead of \"decorating\" the existing one");
      return StatusCode::FAILURE;
    }
    ANA_CHECK (m_master->addNew (type, m_index[type]));
    return StatusCode::SUCCESS;
  }



  float OptimizedObjects ::
  eventWeight () const
  {
    return 1;
  }



  const CP::SystematicSet& OptimizedObjects ::
  systematics () const
  {
    RCU_READ_INVARIANT (this);
    return m_systematics;
  }



  void OptimizedObjects ::
  fillEventData (const CP::SystematicSet& sys,
		 EventData& eventData) const
  {
    RCU_READ_INVARIANT (this);
    for (unsigned typeIter = 0; typeIter < EnumSize<ObjectType>::dataSize; ++ typeIter)
    {
      const ObjectType type = ObjectType (typeIter);
      if (m_inputIndex[type] != OptimizedStore::noIndex)
      {
	m_master->fillEventData (type, m_inputIndex[type], sys, eventData);
      }
    }
  }
}
