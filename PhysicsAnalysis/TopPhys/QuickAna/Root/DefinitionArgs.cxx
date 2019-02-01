/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/DefinitionArgs.h>

#include <QuickAna/AnaTool.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void DefinitionArgs ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
    RCU_INVARIANT (!m_prefix.empty());
    RCU_INVARIANT (m_configuration != nullptr);
  }



  DefinitionArgs ::
  DefinitionArgs (const std::string& val_prefix,
		  InternalConfiguration *val_configuration)
    : m_prefix (val_prefix), m_configuration (val_configuration)
  {
    RCU_NEW_INVARIANT (this);
  }



  DefinitionArgs DefinitionArgs ::
  cloneConf () const
  {
    DefinitionArgs result (m_prefix, m_configuration);
    result.m_type = m_type;
    result.m_name = m_name;
    result.m_firstWP = m_firstWP;
    return result;
  }



  ObjectType DefinitionArgs ::
  type () const
  {
    RCU_READ_INVARIANT (this);
    return m_type;
  }



  const std::string& DefinitionArgs ::
  name () const
  {
    RCU_READ_INVARIANT (this);
    return m_name;
  }



  const std::string& DefinitionArgs ::
  prefix () const
  {
    RCU_READ_INVARIANT (this);
    return m_prefix;
  }



  void DefinitionArgs ::
  addToPrefix (const std::string& extension)
  {
    RCU_CHANGE_INVARIANT (this);
    m_prefix += "_" + extension;
  }



  InternalConfiguration *DefinitionArgs ::
  configuration () const
  {
    RCU_READ_INVARIANT (this);
    return m_configuration;
  }



  bool DefinitionArgs ::
  firstWP () const
  {
    RCU_READ_INVARIANT (this);
    return m_firstWP;
  }



  void DefinitionArgs ::
  setFirstWP (bool val_firstWP)
  {
    RCU_CHANGE_INVARIANT (this);
    m_firstWP = val_firstWP;
  }



  void DefinitionArgs ::
  add (std::unique_ptr<IAnaTool> val_definition)
  {
    RCU_CHANGE_INVARIANT (this);
    m_sequence.addTool (std::move (val_definition));
  }



  AnaToolSequence&& DefinitionArgs ::
  extractSequence ()
  {
    RCU_CHANGE_INVARIANT (this);
    return std::move (m_sequence);
  }



  bool DefinitionArgs ::
  filled () const
  {
    RCU_READ_INVARIANT (this);
    return !m_sequence.empty();
  }



  void DefinitionArgs ::
  setType (ObjectType val_type)
  {
    RCU_CHANGE_INVARIANT (this);
    m_type = val_type;
  }



  void DefinitionArgs ::
  setName (const std::string& val_name)
  {
    RCU_CHANGE_INVARIANT (this);
    m_name = val_name;
  }
}
