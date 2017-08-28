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

#include <QuickAna/AnaToolSequence.h>

#include <QuickAna/AnaTool.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionConf.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/ObjectTypeInfo.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  namespace
  {
    // TODO: needs documentation
    StatusCode addDefWP_internal (const DefinitionArgs& masterArgs,
                                  AnaToolSequence& tools,
                                  std::vector<DefinitionConf>& confList)
    {
      using namespace msgConfiguration;

      for (auto& conf : confList)
      {
        DefinitionArgs args = masterArgs.cloneConf();

        // Ask the DefinitionConf to make the tools.
        AnaToolSequence list;
        ANA_CHECK (conf.makeTools (args, list));

        // Loop over the tools to fill the output list.
        for (auto& tool : list)
        {
	  if (tool.first)
	  {
	    tools.addTool (std::move (tool.first));
	  } else
	  {
            // If the tool doesn't exist, we just add the label...?
            // What's going on here?
            tools.addLabel (tool.second);
          }
        }
      }

      return StatusCode::SUCCESS;
    }



    StatusCode addDefWP (const DefinitionArgs& masterArgs,
                         AnaToolSequence& tools,
                         ObjectType type,
                         std::vector<DefinitionConf>& confList)
    {
      using namespace msgConfiguration;

      RCU_REQUIRE (masterArgs.type() == type);

      // Loop over the confs to fill the list of names and check for duplicates.
      // It seems like this could be simplified.
      std::vector<std::string> mynames;
      for (auto& conf : confList)
      {
        std::string wpName = conf.wpName();
        // Under what condition would the name be empty???
        if (!wpName.empty())
        {
          // Check to see if this WP name is already in our list of names.
          // We should use std::find here instead.
          for (auto& other : mynames)
          {
            if (other == wpName)
            {
              ANA_MSG_ERROR ("duplicate object definition name: " << wpName <<
                            " for " << ObjectTypeInfo::name[type]);
              return StatusCode::FAILURE;
            }
          }
          mynames.push_back (wpName);
        }
      }
      // For some reason, every conf needs to know whether it is first
      // and whether it is part of a multi-working point definition.
      bool first = true;
      for (auto& conf : confList)
      {
        RCU_ASSERT (conf.defType() == type);
        conf.setFirstWP (first);
        conf.setMultiWP (mynames.size() > 1);
        first = false;
      }

      // ???
      if (ObjectTypeInfo::supportsIParticle (type))
      {
        DefinitionConf conf
          = DefinitionConf::makeIParticleCommonConf (type, mynames);
        confList.push_back (conf);
      }

      ANA_CHECK (addDefWP_internal (masterArgs, tools, confList));

      return StatusCode::SUCCESS;
    }
  }



  StatusCode
  makeToolSequence (const std::vector<DefinitionConf>& confList,
                    const DefinitionArgs& masterArgs,
                    AnaToolSequence& tools)
  {
    using namespace msgConfiguration;

    // The incoming confList contains all definition info for all object types.
    // Now, we organize them by object type in a map.
    std::map<ObjectType,std::vector<DefinitionConf>> confMap;
    for (auto& conf : confList)
      confMap[conf.defType()].push_back (conf);
    // Now we can loop over the object types and create the tools needed.
    for (auto& iter : confMap)
    {
      DefinitionArgs args = masterArgs.cloneConf();
      args.setType (iter.first);
      ANA_CHECK (addDefWP (args, tools, iter.first, iter.second));
    }

    return StatusCode::SUCCESS;
  }



  void AnaToolSequence ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  AnaToolSequence ::
  AnaToolSequence ()
  {
    RCU_NEW_INVARIANT (this);
  }



  AnaToolSequence ::
  AnaToolSequence (AnaToolSequence&& that)
    : m_tools (std::move (that.m_tools))
  {
    RCU_NEW_INVARIANT (this);
  }



  AnaToolSequence& AnaToolSequence ::
  operator = (AnaToolSequence&& that)
  {
    // no invariant used
    that.swap (*this);
    return *this;
  }



  void AnaToolSequence ::
  swap (AnaToolSequence& that)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_CHANGE_INVARIANT (&that);
    m_tools.swap (that.m_tools);
  }



  void AnaToolSequence ::
  addTool (std::unique_ptr<IAnaTool> val_definition)
  {
    RCU_CHANGE_INVARIANT (this);
    m_tools.push_back (std::make_pair(std::move(val_definition),""));
  }



  void AnaToolSequence ::
  addLabel (const std::string& val_label)
  {
    RCU_CHANGE_INVARIANT (this);
    m_tools.push_back (std::make_pair(std::unique_ptr<IAnaTool>(), val_label));
  }



  bool AnaToolSequence ::
  empty () const
  {
    RCU_READ_INVARIANT (this);
    return m_tools.empty();
  }



  AnaToolSequence::iterator AnaToolSequence ::
  begin ()
  {
    RCU_READ_INVARIANT (this);
    return m_tools.begin();
  }



  AnaToolSequence::iterator AnaToolSequence ::
  end ()
  {
    RCU_READ_INVARIANT (this);
    return m_tools.end();
  }
}
