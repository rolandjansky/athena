/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/OptimizedStore.h>

#include <PATInterfaces/SystematicSet.h>
#include <QuickAna/EventData.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/OptimizedObjects.h>
#include <QuickAna/xAODHelpers.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace ana
{
  // for documentation see header file/doxygen
  struct OptimizedStore::InfoObject
  {
    /// \brief the data pointer
  public:
    void *data;

    /// \brief the name inside the data store
  public:
    std::string name;

    /// \brief standard constructor
  public:
    InfoObject ()
      : data (nullptr)
    {}
  };



  // for documentation see header file/doxygen
  struct OptimizedStore::InfoType
  {
    /// \brief the object information
  public:
    std::vector<InfoObject> objects;

    /// \brief all the systematics affecting this object type (up to
    ///   this point)
  public:
    CP::SystematicSet systematics;

    /// \brief a mapping from systematics points to actual systematics
    ///   used
  public:
    std::unordered_map<CP::SystematicSet,std::size_t> systematicsIndex;
  };



  // for documentation see header file/doxygen
  struct OptimizedStore::Pimpl
  {
    /// \brief the type information
  public:
    InfoType types [ObjectTypeInfo::numTypes];

    /// \brief the value of \ref OptimizedStore::requestedSystematics
  public:
    std::vector<CP::SystematicSet> requestedSystematics;

    /// \brief the store we are operating on
  public:
    StoreType *store;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    Pimpl (const std::vector<CP::SystematicSet>& val_requestedSystematics,
	   StoreType *val_store)
      : requestedSystematics (val_requestedSystematics), store (val_store)
    {}
  };



  // for documentation see header file/doxygen
  const std::size_t OptimizedStore::noIndex;



  void OptimizedStore ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
    RCU_INVARIANT (pimpl != nullptr);
  }



  OptimizedStore ::
  OptimizedStore (const std::vector<CP::SystematicSet>& val_requestedSystematics,
		  StoreType *val_store)
    : pimpl (new Pimpl (val_requestedSystematics, val_store))
  {
    RCU_NEW_INVARIANT (this);
  }



  OptimizedStore ::
  ~OptimizedStore ()
  {
    RCU_DESTROY_INVARIANT (this);
    delete pimpl;
  }



  const std::vector<CP::SystematicSet>& OptimizedStore ::
  requestedSystematics () const
  {
    RCU_READ_INVARIANT (this);
    return pimpl->requestedSystematics;
  }



  StatusCode OptimizedStore ::
  prepareCopy (ObjectType type, std::size_t index, std::size_t origIndex)
  {
    using namespace msgStore;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);
    if (index == noIndex)
    {
      ANA_MSG_ERROR ("trying to use unregistered object");
      return StatusCode::FAILURE;
    }

    // TODO: This should have comments
    InfoType& infoType = pimpl->types[type];
    RCU_ASSERT (index < infoType.objects.size());
    RCU_ASSERT (origIndex < infoType.objects.size());
    InfoObject& infoObjectFrom = infoType.objects[origIndex];
    InfoObject& infoObjectTo = infoType.objects[index];
    RCU_ASSERT (infoObjectFrom.data != nullptr);
    RCU_ASSERT (infoObjectTo.data == nullptr);

    ANA_CHECK (xAODHelpers::makeShallow (*pimpl->store, infoObjectTo.name,
					       type, infoObjectTo.data,
					       infoObjectFrom.name));
    return StatusCode::SUCCESS;
  }



  void *OptimizedStore ::
  getVoid (ObjectType type, const std::size_t index) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);
    InfoType& infoType = pimpl->types[type];

    if (index == noIndex)
    {
      if (infoType.objects.empty())
	return nullptr;
      RCU_THROW_MSG ("trying to use unregistered object of type " + ObjectTypeInfo::name[type]);
    }

    RCU_ASSERT (index < infoType.objects.size());
    InfoObject& infoObject = infoType.objects[index];
    if (infoObject.data == nullptr)
      RCU_THROW_MSG ("trying to read un-initialized object of type " + ObjectTypeInfo::name[type]);

    return infoObject.data;
  }



  StatusCode OptimizedStore ::
  addCopy (ObjectType type, std::size_t index,
	   const std::string& name)
  {
    using namespace msgStore;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);
    if (index == noIndex)
    {
      ANA_MSG_ERROR ("trying to use unregistered object");
      return StatusCode::FAILURE;
    }

    // TODO: This should have comments
    InfoType& infoType = pimpl->types[type];
    RCU_ASSERT (index < infoType.objects.size());
    InfoObject& infoObject = infoType.objects[index];
    if (infoObject.data)
    {
      ANA_MSG_ERROR ("trying to initialize object twice");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (xAODHelpers::makeShallow (*pimpl->store, infoObject.name,
					       type, infoObject.data, name));
    return StatusCode::SUCCESS;
  }



  StatusCode OptimizedStore ::
  addNew (ObjectType type, std::size_t index)
  {
    using namespace msgStore;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);
    if (index == noIndex)
    {
      ANA_MSG_ERROR ("trying to use unregistered object");
      return StatusCode::FAILURE;
    }

    // TODO: This should have comments
    InfoType& infoType = pimpl->types[type];
    RCU_ASSERT (index < infoType.objects.size());
    InfoObject& infoObject = infoType.objects[index];
    if (infoObject.data)
    {
      ANA_MSG_ERROR ("trying to initialize object twice");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (xAODHelpers::makeNew (*pimpl->store, infoObject.name,
					   type, infoObject.data));
    return StatusCode::SUCCESS;
  }



  //
  // legacy code
  //













  const CP::SystematicSet& OptimizedStore ::
  objectSystematics (ObjectType type) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);

    const InfoType& infoType = pimpl->types[type];

    return infoType.systematics;
  }



  StatusCode OptimizedStore ::
  initializeIndices (const std::vector<OptimizedObjects*>& objects,
		     unsigned inputTypes, unsigned outputTypes,
		     AnalysisStep step)
  {
    using namespace msgStore;

    RCU_CHANGE_INVARIANT (this);

    for (unsigned iter = 0, end = ObjectTypeInfo::numTypes;
	 iter != end; ++ iter)
    {
      const ObjectType objectType = ObjectType (iter);
      InfoType& infoType = pimpl->types[objectType];
      const bool isInput = (inputTypes & (1 << objectType));
      const bool isOutput = (outputTypes & (1 << objectType));

      // input objects are only used if present, and ignored otherwise
      if (isInput && !infoType.objects.empty())
      {
	for (auto& obj : objects)
	{
	  CP::SystematicSet mySys;
	  if (CP::SystematicSet::filterForAffectingSystematics (obj->systematics(), infoType.systematics, mySys) != CP::SystematicCode::Ok)
	  {
	    ANA_MSG_ERROR ("failed to filter for systematics");
	    return StatusCode::FAILURE;
	  }
	  auto iter = infoType.systematicsIndex.find (mySys);
	  if (iter == infoType.systematicsIndex.end())
	  {
	    ANA_MSG_ERROR ("unknown systematics point: \"" << mySys.name() << "\"");
	    return StatusCode::FAILURE;
	  }
	  obj->m_index[objectType] = obj->m_inputIndex[objectType]
	    = iter->second;
	}
      }

      // output objects are only used if the input objects are
      // present, or if we don't use any input objects (indicating
      // that we create the objects)
      if (isOutput &&
	  !(isInput && infoType.objects.empty()))
      {
	if (!isInput && !infoType.objects.empty())
	{
	  ANA_MSG_ERROR ("trying to fill object type twice: " << ObjectTypeInfo::name[objectType]);
	  return StatusCode::FAILURE;
	}

	std::set<std::size_t> duplicates;
	{
	  std::set<std::size_t> used;
	  used.insert (noIndex);
	  for (auto& obj : objects)
	  {
	    const std::size_t index = obj->m_index[objectType];
	    if (used.find (index) != used.end())
	      duplicates.insert (index);
	    used.insert (index);
	  }
	}

	infoType.systematicsIndex.clear ();
	for (auto& obj : objects)
	{
	  const std::size_t index = obj->m_index[objectType];
	  if (duplicates.find (index) != duplicates.end())
	  {
	    std::ostringstream name;
	    name << "ana_" << ObjectTypeInfo::name[objectType] << "_";
	    name << step << "_" << obj->systematics().name();

	    InfoObject infoObject;
	    infoObject.name = name.str();
	    infoType.objects.push_back (infoObject);
	    obj->m_index[objectType] = infoType.objects.size() - 1;
	  }
	  infoType.systematics.insert (obj->systematics());
	  RCU_ASSERT (infoType.systematicsIndex.find (obj->systematics()) == infoType.systematicsIndex.end());
	  infoType.systematicsIndex[obj->systematics()]
	    = obj->m_index[objectType];
	}
      }
    }
    return StatusCode::SUCCESS;
  }



  unsigned OptimizedStore ::
  objectTypes () const
  {
    RCU_READ_INVARIANT (this);

    unsigned result = 0;
    for (unsigned iter = 0, end = ObjectTypeInfo::numTypes;
	 iter != end; ++ iter)
    {
      const ObjectType objectType = ObjectType (iter);
      InfoType& infoType = pimpl->types[objectType];
      if (!infoType.objects.empty())
	result |= 1 << objectType;
    }
    return result;
  }



  void OptimizedStore ::
  clearData ()
  {
    RCU_CHANGE_INVARIANT (this);
    for (auto& infoType : pimpl->types)
    {
      for (auto& infoObject : infoType.objects)
      {
	infoObject.data = nullptr;
      }
    }
  }



  void OptimizedStore ::
  fillEventData (ObjectType type, unsigned index,
		 const CP::SystematicSet& sys,
		 EventData& eventData) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (type < ObjectTypeInfo::numTypes);

    InfoType& infoType = pimpl->types[type];
    RCU_ASSERT (index < infoType.objects.size());
    InfoObject& infoObject = infoType.objects[index];
    eventData.setName (type, sys, infoObject.name);
  }
}
