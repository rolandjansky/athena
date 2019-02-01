/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/DefinitionMaker.h>

// Suppressing unused typedef warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/IAnaTool.h>
#include <QuickAna/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <mutex>

//
// method implementations
//

namespace ana
{
  namespace
  {
    /// \brief the global database of \ref DefinitionMaker objects
    struct Data
    {
    public:
      std::mutex mutex;
    public:
      std::vector<DefinitionMaker> list;
    };
    Data& data ()
    {
      static Data data;
      return data;
    }
  }



  void DefinitionMaker ::
  addDefinition (DefinitionMaker val_definition)
  {
    Data& mydata (data());
    std::lock_guard<std::mutex> lock (mydata.mutex);
    mydata.list.push_back (val_definition);
  }



  StatusCode DefinitionMaker ::
  makeDefinition (DefinitionArgs& args)
  {
    using namespace msgConfiguration;

    Data& mydata (data());
    std::lock_guard<std::mutex> lock (mydata.mutex);
    AnaToolSequence result;
    bool foundName = false;
    for (auto& maker : mydata.list)
    {
      DefinitionArgs myargs = args.cloneConf();
      bool myfoundName = false;
      ANA_CHECK (maker.doMakeDefinition (myargs, myfoundName));
      if (myfoundName)
      {
        if (foundName)
        {
          ANA_MSG_ERROR ("duplicate object definition: " + args.name());
          return StatusCode::FAILURE;
        }
        result = myargs.extractSequence ();
        foundName = true;
      }
    }
    if (!foundName)
    {
      ANA_MSG_ERROR ("unknown object definition: " + args.name());
      return StatusCode::FAILURE;
    }
    for (auto&& def : result)
    {
      if (def.first)
	args.add (std::move (def.first));
    }
    return StatusCode::SUCCESS;
  }



  void DefinitionMaker ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  DefinitionMaker ::
  DefinitionMaker (ObjectType val_type, const std::string& val_name,
                   const std::function<StatusCode (DefinitionArgs&)>& val_function)
    : m_type (val_type), m_function (val_function)
  {
    boost::algorithm::split(m_names, val_name, boost::is_any_of("\t "));

    RCU_NEW_INVARIANT (this);
  }



  StatusCode DefinitionMaker ::
  doMakeDefinition (DefinitionArgs& args, bool& foundName) const
  {
    using namespace msgConfiguration;

    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (!args.filled());
    ObjectType type = args.type();
    if (type == OBJECT_PFLOW_JET)
      type = OBJECT_JET;
    if (type == OBJECT_FAT_JET)
      type = OBJECT_JET;
    if (type == OBJECT_MET2)
      type = OBJECT_MET;
    if (type == m_type &&
	std::find (m_names.begin(), m_names.end(), args.name()) != m_names.end())
    {
      ANA_CHECK (m_function (args));
      if (!args.filled())
      {
        ANA_MSG_ERROR ("definition " + args.name() + " didn't do anything");
        return StatusCode::FAILURE;
      }
      foundName = true;
    } else foundName = false;
    return StatusCode::SUCCESS;
  }
}
