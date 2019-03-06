/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/EventData.h>

#include <QuickAna/ObjectTypeInfo.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace ana
{
  void EventData ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  EventData ::
  EventData ()
  {
    RCU_NEW_INVARIANT (this);
  }



  const std::set<ObjectType>& EventData ::
  getObjectTypes () const
  {
    RCU_READ_INVARIANT (this);
    return m_objectTypes;
  }



  const std::set<CP::SystematicSet>& EventData ::
  getSystematics () const
  {
    RCU_READ_INVARIANT (this);
    return m_systematics;
  }



  const std::string& EventData ::
  getName (ObjectType type, const CP::SystematicSet& sys) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (unsigned (type) < EnumSize<ObjectType>::dataSize);
    const auto iter = m_names[type].find (sys);
    if (iter == m_names[type].end())
    {
      if (m_names[type].empty())
      {
        static std::string empty;
        return empty;
      }
      RCU_THROW_MSG ("unsupported systematic \"" + sys.name() + "\" for type " + ObjectTypeInfo::name[type]);
    }
    return iter->second;
  }



  void EventData ::
  setName (ObjectType type, const CP::SystematicSet& sys,
           const std::string& val_name)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (unsigned (type) < EnumSize<ObjectType>::dataSize);
    RCU_REQUIRE (!val_name.empty());

    m_objectTypes.insert (type);
    m_systematics.insert (sys);
    m_names[type][sys] = val_name;
  }



  std::vector<std::string> EventData ::
  getNames (ObjectType type) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (unsigned (type) < EnumSize<ObjectType>::dataSize);
    // Fill a set first to identify the unique elements
    std::set<std::string> uniqueNames;
    for(auto& sys : m_systematics)
      uniqueNames.insert( getName(type, sys) );
    // Now convert to a vector
    return std::vector<std::string>( uniqueNames.begin(), uniqueNames.end() );
  }



  const std::string& EventData ::
  getSourceName (ObjectType type, const CP::SystematicSet& sys) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (unsigned (type) < EnumSize<ObjectType>::dataSize);
    const auto iter = m_sourceNames[type].find (sys);
    if (iter == m_sourceNames[type].end())
    {
      if (m_sourceNames[type].empty())
      {
        static std::string empty;
        return empty;
      }
      RCU_THROW_MSG ("unsupported systematic \"" + sys.name() + "\" for type " + ObjectTypeInfo::name[type]);
    }
    return iter->second;
  }



  void EventData ::
  setSourceName (ObjectType type, const CP::SystematicSet& sys,
                 const std::string& val_sourceName)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (unsigned (type) < EnumSize<ObjectType>::dataSize);
    RCU_REQUIRE (!val_sourceName.empty());

    m_objectTypes.insert (type);
    m_systematics.insert (sys);
    m_sourceNames[type][sys] = val_sourceName;
  }



  std::vector<std::string> EventData ::
  getSourceNames (ObjectType type) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (unsigned (type) < EnumSize<ObjectType>::dataSize);
    // Fill a set first to identify the unique elements
    std::set<std::string> uniqueNames;
    for(auto& sys : m_systematics) {
      // There may not be a source container, so we skip if the name is empty.
      auto sourceName = getSourceName(type, sys);
      if(!sourceName.empty())
        uniqueNames.insert( getSourceName(type, sys) );
    }
    // Now convert to a vector
    return std::vector<std::string>( uniqueNames.begin(), uniqueNames.end() );
  }
}
