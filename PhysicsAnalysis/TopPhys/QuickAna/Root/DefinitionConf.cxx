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

#include <QuickAna/DefinitionConf.h>

// Suppressing unused typedef warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <AsgTools/StatusCode.h>
#include <QuickAna/AnaToolSelectCombine.h>
#include <QuickAna/AnaToolSelectInit.h>
#include <QuickAna/AnaToolSelectSpread.h>
#include <QuickAna/AnaToolSequence.h>
#include <QuickAna/Configuration.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/EventInfoIsData.h>
#include <QuickAna/EventSelectBaseTool.h>
#include <QuickAna/IAnaTool.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/KinSelectTool.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/ObjectTypeInfo.h>
#include <QuickAna/PileupTool.h>
#include <RootCoreUtils/Assert.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

//
// method implementations
//

namespace ana
{
  namespace
  {
    /// \brief read the list of default names
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   i/o errors
    std::map<std::string,std::string> read_default_list ()
    {
      std::map<std::string,std::string> result;
      result["jets"] = "antikt04";
      result["fatJets"] = "antikt10";
      result["electrons"] = "medium";
      result["photons"] = "tight";
      result["muons"] = "medium";
      result["trigger"] = "skip";
      return result;
    }

    /// \brief resolve the default name (if it is "default")
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   i/o errors
    std::string resolve_default (ObjectType type, const std::string& name)
    {
      if (name != "default")
        return name;
      static const auto list = read_default_list();
      auto iter = list.find (ObjectTypeInfo::name[type]);
      if (iter == list.end())
        return name;
      return iter->second;
    }



    /// \brief make the \ref DefinitionConf from a field in the \ref
    /// Configuration object
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
    StatusCode makeObjectConf (std::vector<DefinitionConf>& confList,
                               ObjectType type,
                               std::string name, const std::string& kine)
    {
      using namespace msgConfiguration;

      if (name == "none")
        name = "";

      // Split the name string into working point names and build a
      // DefinitionConf for each.
      std::vector<std::string> rawnames;
      boost::algorithm::split(rawnames, name, boost::is_any_of("\t "));
      for (auto& subname : rawnames)
      {
        if (!subname.empty())
        {
          DefinitionConf conf =
            DefinitionConf::makeObjectDefinitionConf (type, resolve_default (type, subname));
          confList.push_back (conf);
        }
      }

      // Build a DefinitionConf specifically for the kinematic selection string.
      if (!kine.empty())
      {
        if (confList.empty())
        {
          ANA_MSG_ERROR ("can not specify a kinematic selection " <<
                        "without a definition for " + ObjectTypeInfo::name[type]);
          return StatusCode::FAILURE;
        }
        confList.push_back
          (DefinitionConf::makeKinSelectConf (type, kine));
      }

      return StatusCode::SUCCESS;
    }
  }



  StatusCode DefinitionConf ::
  makeTools (const DefinitionArgs& args,
             AnaToolSequence& sequence)
  {
    using namespace msgConfiguration;

    RCU_REQUIRE (sequence.empty());

    DefinitionArgs myargs = args.cloneConf();
    myargs.setType (m_defType);
    myargs.setName (m_defName);
    myargs.setFirstWP (m_firstWP);
    myargs.addToPrefix (ObjectTypeInfo::name[m_defType]);
    switch (m_mode)
    {
    case Mode::OBJECT_DEFINITION:
      myargs.addToPrefix ("def");
      if (m_multiWP)
        myargs.addToPrefix (m_defName);
      ANA_CHECK (DefinitionMaker::makeDefinition (myargs));
      break;
    case Mode::IPARTICLE_COMMON:
      myargs.addToPrefix ("ipart");
      ANA_CHECK (makeIParticleCommonTools (myargs));
      break;
    case Mode::KINSELECT:
      myargs.addToPrefix ("kine");
      ANA_CHECK (makeKinSelectTool (myargs));
      break;
    case Mode::ISDATA:
      myargs.addToPrefix ("isdata");
      ANA_CHECK (makeIsDataTool (myargs));
      break;
    case Mode::EVENTSELECTBASE:
      myargs.addToPrefix ("eventselectbase");
      ANA_CHECK (makeEventSelectBaseTool (myargs));
      break;
    case Mode::PILEUP:
      myargs.addToPrefix ("pileup");
      ANA_CHECK (makePileupTool (myargs));
      break;
    }
    AnaToolSequence list = myargs.extractSequence ();

    for (auto& tool : list)
    {
      if (tool.first)
      {
        // this drops all the tools from secondary working points
        // that should only exist once
        const bool multiWP = tool.first->step() >= STEP_SELECT || tool.first->step() == STEP_TRIG_DECISION;
        if (m_mode != Mode::OBJECT_DEFINITION ||
            m_firstWP || multiWP)
        {
          std::string workingPoint;
          if (m_mode == Mode::OBJECT_DEFINITION && m_multiWP && multiWP)
            workingPoint = m_defName;
          if (m_mode != Mode::KINSELECT)
            ANA_CHECK (tool.first->setObjectType (m_defType, workingPoint));
          sequence.addTool (std::move(tool.first));
        }
      } else
      {
        sequence.addLabel (tool.second);
      }
    }

    RCU_PROVIDE (!sequence.empty());
    return StatusCode::SUCCESS;
  }



  ObjectType DefinitionConf ::
  defType () const
  {
    return m_defType;
  }



  void DefinitionConf ::
  setDefType (ObjectType val_defType)
  {
    m_defType = val_defType;
  }



  const std::string& DefinitionConf ::
  defName () const
  {
    return m_defName;
  }



  void DefinitionConf ::
  setDefName (const std::string& val_defName)
  {
    m_defName = val_defName;
  }



  std::string DefinitionConf ::
  wpName () const
  {
    if (m_mode == Mode::OBJECT_DEFINITION)
      return m_defName;
    else
      return "";
  }



  bool DefinitionConf ::
  firstWP () const
  {
    return m_firstWP;
  }



  void DefinitionConf ::
  setFirstWP (bool val_firstWP)
  {
    m_firstWP = val_firstWP;
  }



  bool DefinitionConf ::
  multiWP () const
  {
    return m_multiWP;
  }



  void DefinitionConf ::
  setMultiWP (bool val_multiWP)
  {
    m_multiWP = val_multiWP;
  }



  DefinitionConf DefinitionConf ::
  makeObjectDefinitionConf (ObjectType type, const std::string& name)
  {
    DefinitionConf result;
    result.m_mode = Mode::OBJECT_DEFINITION;
    result.m_defType = type;
    result.m_defName = name;
    RCU_PROVIDE (!result.wpName().empty());
    return result;
  }



  DefinitionConf DefinitionConf ::
  makeIParticleCommonConf (ObjectType type,
                           const std::vector<std::string>& workingPoints)
  {
    DefinitionConf result;
    result.m_mode = Mode::IPARTICLE_COMMON;
    result.m_defType = type;
    result.m_wpNameList = workingPoints;
    RCU_PROVIDE (result.wpName().empty());
    return result;
  }



  DefinitionConf DefinitionConf ::
  makeKinSelectConf (ObjectType type, const std::string& kinSelect)
  {
    DefinitionConf result;
    result.m_mode = Mode::KINSELECT;
    result.m_defType = type;
    result.m_kinSelect = kinSelect;
    RCU_PROVIDE (result.wpName().empty());
    return result;
  }



  DefinitionConf DefinitionConf ::
  makeEventSelectBaseConf ()
  {
    DefinitionConf result;
    result.m_mode = Mode::EVENTSELECTBASE;
    result.m_defType = OBJECT_EVENT_SELECT;
    RCU_PROVIDE (result.wpName().empty());
    return result;
  }



  DefinitionConf DefinitionConf ::
  makeIsDataConf ()
  {
    DefinitionConf result;
    result.m_mode = Mode::ISDATA;
    result.m_defType = OBJECT_NONE;
    RCU_PROVIDE (result.wpName().empty());
    return result;
  }



  DefinitionConf DefinitionConf ::
  makePileupConf (const std::vector<std::string>& muDataFiles,
                  const std::vector<std::string>& muMcFiles)
  {
    DefinitionConf result;
    result.m_mode = Mode::PILEUP;
    result.m_defType = OBJECT_NONE;
    result.m_muDataFiles = muDataFiles;
    result.m_muMcFiles = muMcFiles;
    RCU_PROVIDE (result.wpName().empty());
    return result;
  }



  StatusCode DefinitionConf ::
  makeConfigList (std::vector<DefinitionConf>& confList,
                  const Configuration& config)
  {
    using namespace msgConfiguration;

    confList.push_back (DefinitionConf::makeEventSelectBaseConf ());
    ANA_CHECK (makeObjectConf (confList, OBJECT_EVENTINFO,
                              config.eventinfoDef, ""));
    ANA_CHECK (makeObjectConf (confList, OBJECT_EVENT_SELECT,
                              config.eventSelectDef, ""));
    ANA_CHECK (makeObjectConf (confList, OBJECT_JET,
                              config.jetDef, config.jetKine));
    ANA_CHECK (makeObjectConf (confList, OBJECT_FAT_JET,
                              config.fatJetDef, config.fatJetKine));
    ANA_CHECK (makeObjectConf (confList, OBJECT_PHOTON,
                              config.photonDef, config.photonKine));
    ANA_CHECK (makeObjectConf (confList, OBJECT_ELECTRON,
                              config.electronDef, config.electronKine));
    ANA_CHECK (makeObjectConf (confList, OBJECT_MUON,
                              config.muonDef, config.muonKine));
    ANA_CHECK (makeObjectConf (confList, OBJECT_TAU,
                              config.tauDef, config.tauKine));
    ANA_CHECK (makeObjectConf (confList, OBJECT_MET,
                              config.metDef, ""));
    ANA_CHECK (makeObjectConf (confList, OBJECT_MET2,
                              config.met2Def, ""));
    ANA_CHECK (makeObjectConf (confList, OBJECT_OVERLAP_REMOVAL,
                              config.orDef, ""));
    ANA_CHECK (makeObjectConf (confList, OBJECT_TRIGGER,
                              config.triggerDef, ""));
    confList.push_back (DefinitionConf::makeIsDataConf ());

    // Need to change this condition for now because we only need data files
    // for the muon SFs.
    //if (config.muDataFiles.empty() != config.muMcFiles.empty())
    //{
    //  ANA_MSG_FATAL("need to set either both or neither of muDataFile and muMcFile");
    //  return StatusCode::FAILURE;
    //}

    // Need both file types configured to trigger pileup tool config.
    if (!config.muDataFiles.empty() && !config.muMcFiles.empty())
    {
      confList.push_back(
        DefinitionConf::makePileupConf(config.muDataFiles, config.muMcFiles)
      );
    }
    return StatusCode::SUCCESS;
  }



  StatusCode DefinitionConf ::
  makeIParticleCommonTools (DefinitionArgs& args)
  {
    using namespace msgConfiguration;

    RCU_REQUIRE (m_mode == Mode::IPARTICLE_COMMON);
    RCU_REQUIRE (!m_wpNameList.empty());
    RCU_REQUIRE (ObjectTypeInfo::supportsIParticle (defType()));

    {
      std::unique_ptr<AnaToolSelectInit> select
        (new AnaToolSelectInit (args.prefix() + "_init"));
      ANA_CHECK (select->setObjectType (m_defType, ""));
      args.add (std::move (select));
    }

    if (m_wpNameList.size() > 1)
    {
      {
        std::unique_ptr<AnaToolSelectSpread> select
          (new AnaToolSelectSpread (args.prefix() + "_split",
                                    m_wpNameList, true));
        ANA_CHECK (select->setObjectType (m_defType, ""));
        args.add (std::move (select));
      }
      {
        std::unique_ptr<AnaToolSelectCombine> select
          (new AnaToolSelectCombine (args.prefix() + "_combine",
                                     m_wpNameList));
        ANA_CHECK (select->setObjectType (m_defType, ""));
        args.add (std::move (select));
      }
      {
        std::unique_ptr<AnaToolSelectSpread> select
          (new AnaToolSelectSpread (args.prefix() + "_spread",
                                    m_wpNameList, false));
        ANA_CHECK (select->setObjectType (m_defType, ""));
        args.add (std::move (select));
      }
    }
    return StatusCode::SUCCESS;
  }



  StatusCode DefinitionConf ::
  makeKinSelectTool (DefinitionArgs& args)
  {
    RCU_REQUIRE (m_mode == Mode::KINSELECT);
    RCU_REQUIRE (!m_kinSelect.empty());

    std::unique_ptr<KinSelectTool> kinSelect (new KinSelectTool (args.prefix() + "_kine"));
    kinSelect->m_objectType = args.type();
    kinSelect->m_selection = m_kinSelect;
    args.add (std::move (kinSelect));
    return StatusCode::SUCCESS;
  }



  StatusCode DefinitionConf ::
  makeEventSelectBaseTool (DefinitionArgs& args)
  {
    args.add (std::unique_ptr<EventSelectBaseTool> (new EventSelectBaseTool (args.prefix() + "_event_select_base")));
    return StatusCode::SUCCESS;
  }



  StatusCode DefinitionConf ::
  makeIsDataTool (DefinitionArgs& args)
  {
    args.add (std::unique_ptr<EventInfoIsData> (new EventInfoIsData (args.prefix() + "_is_data", args.configuration()->isData())));
    return StatusCode::SUCCESS;
  }



  StatusCode DefinitionConf ::
  makePileupTool (DefinitionArgs& args)
  {
    std::unique_ptr<PileupTool> pileupTool (new PileupTool (args.prefix() + "_pileup"));
    pileupTool->muDataFiles = m_muDataFiles;
    pileupTool->muMcFiles = m_muMcFiles;
    args.add (std::move (pileupTool));
    return StatusCode::SUCCESS;
  }
}
