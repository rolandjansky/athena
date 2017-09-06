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

#include <QuickAna/OptimizedTool.h>

#include <QuickAna/IAnaTool.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/OptimizedObjects.h>
#include <QuickAna/OptimizedStore.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void OptimizedTool ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
    RCU_INVARIANT (m_tool != nullptr);
  }



  OptimizedTool ::
  OptimizedTool (std::unique_ptr<IAnaTool> val_tool)
    : asg::AsgMessaging(val_tool->name() + "_optWrapper"),
      m_tool (nullptr)
  {
    RCU_REQUIRE (val_tool != nullptr);

    m_tool = std::move (val_tool);

    RCU_NEW_INVARIANT (this);
  }



  OptimizedTool ::
  ~OptimizedTool ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  std::string OptimizedTool ::
  name () const
  {
    RCU_READ_INVARIANT (this);
    return m_tool->name();
  }



  AnalysisStep OptimizedTool ::
  step () const
  {
    RCU_READ_INVARIANT (this);
    return m_tool->step();
  }



  CP::SystematicSet OptimizedTool ::
  affectingSystematics () const
  {
    RCU_READ_INVARIANT (this);
    return m_tool->affectingSystematics();
  }



  CP::SystematicSet OptimizedTool ::
  recommendedSystematics () const
  {
    RCU_READ_INVARIANT (this);
    return m_tool->recommendedSystematics();
  }



  StatusCode OptimizedTool ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);
    ATH_MSG_DEBUG("execute");
    for (auto& object : m_objects)
    {
      try
      {
        ATH_MSG_DEBUG("Executing tool for systematic " << object.first.name());
        ANA_CHECK (object.second->prepare ());
	if (!object.second->passSelection())
	  continue;
        if (m_tool->applySystematicVariation (object.first) != CP::SystematicCode::Ok)
        {
          ATH_MSG_ERROR ("failed to set systematic variation");
          return StatusCode::FAILURE;
        }
        ANA_CHECK (m_tool->execute (*object.second));
      } catch (std::exception& e)
      {
        ATH_MSG_FATAL ("exception while executing tool " << m_tool->name() <<
                             ": " << e.what());
        return StatusCode::FAILURE;
      } catch (std::string& s)
      {
        ATH_MSG_FATAL ("exception while executing tool " << m_tool->name() <<
                             ": " << s);
        return StatusCode::FAILURE;
      } catch (...)
      {
        ATH_MSG_FATAL ("unknown exception while executing tool " << m_tool->name());
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }



  void OptimizedTool ::
  fillEventDataSource (EventData& event) const
  {
    RCU_READ_INVARIANT (this);
    m_tool->fillEventDataSource (event);
  }



  bool OptimizedTool ::
  hasDirectInput (const OptimizedTool& input) const noexcept
  {
    RCU_READ_INVARIANT (this);
    RCU_READ_INVARIANT (&input);
    const OptimizedTool& output = *this;

    AnalysisStep outputStep = output.m_tool->step();
    AnalysisStep inputStep = input.m_tool->step();
    if (inputStep >= outputStep)
      return false;
    unsigned outputInputs = output.m_tool->inputTypes();
    unsigned inputOutputs = input.m_tool->outputTypes();
    return (inputOutputs & outputInputs) != 0;
  }



  bool OptimizedTool ::
  hasOutput (const OptimizedTool *output) const noexcept
  {
    RCU_READ_INVARIANT (this);
    RCU_READ_INVARIANT (output);

#ifndef NDEBUG
    const OptimizedTool *input = this;
#endif

    bool result1 = m_outputs.find (const_cast<OptimizedTool*>(output)) != m_outputs.end();
#ifndef NDEBUG
    bool result2 = output->m_inputs.find (const_cast<OptimizedTool*>(input)) != output->m_inputs.end();
#endif
    RCU_ASSERT (result1 == result2);
    return result1;
  }



  void OptimizedTool ::
  addOutput (OptimizedTool *output)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_CHANGE_INVARIANT (output);
    RCU_ASSERT (step() < output->step());

    OptimizedTool *input = this;

    RCU_ASSERT (m_inputs.find (output) == m_inputs.end());
    RCU_ASSERT (output->m_outputs.find (input) == output->m_outputs.end());

    if (m_outputs.find (output) != m_outputs.end())
    {
      RCU_ASSERT (output->m_inputs.find (input) != output->m_inputs.end());
      return;
    }

    RCU_ASSERT (output->m_inputs.find (input) == output->m_inputs.end());
    m_outputs.insert (output);
    output->m_inputs.insert (input);

    RCU_ASSERT (m_outputs.find (output) != m_outputs.end());
    RCU_ASSERT (output->m_inputs.find (input) != output->m_inputs.end());

    for (auto subinput : m_inputs)
      subinput->addOutput (output);
    for (auto suboutput : output->m_outputs)
      addOutput (suboutput);
  }



  StatusCode OptimizedTool ::
  configureSystematics (OptimizedStore *store)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (store != nullptr);
    ATH_MSG_DEBUG("configureSystematics");

    // TODO: This code needs comments

    CP::SystematicSet affecting = affectingSystematics();
    for (std::size_t objectTypeIter = 0, end = ObjectTypeInfo::numTypes;
	 objectTypeIter != end; ++ objectTypeIter)
    {
      const ObjectType objectType = ObjectType (objectTypeIter);
      if (m_tool->inputTypes() & (1 << objectType))
      {
	affecting.insert (store->objectSystematics(objectType));
      }
    }

    std::vector<OptimizedObjects*> vecObjects;
    m_objects.clear ();
    for (auto& origSys : store->requestedSystematics())
    {
      CP::SystematicSet mySys;
      if (CP::SystematicSet::filterForAffectingSystematics (origSys, affecting, mySys)
          != CP::SystematicCode::Ok)
      {
	ATH_MSG_ERROR ("failed to filter for affecting systematics");
	return StatusCode::FAILURE;
      }
      if (m_objects.find (mySys) == m_objects.end())
      {
	std::unique_ptr<OptimizedObjects> objects
	  (new OptimizedObjects (store, mySys));
	vecObjects.push_back (&*objects);
	m_objects[mySys] = std::move (objects);
      }
    }

    return store->initializeIndices (vecObjects, m_tool->inputTypes(),
				     m_tool->outputTypes(), step());


    /*
	if (!sys.hasObject (ObjectType (objectType)))
	{
	  ATH_MSG_FATAL ("no source for object type: " << ObjectTypeInfo::name[objectType]);
	  return StatusCode::FAILURE;
	}
	affecting.insert (sys.objectSystematics (ObjectType (objectType)));
      } else if (m_tool->inputTypes() & (1 << objectType))
      {
	if (sys.hasObject (ObjectType (objectType)))
	{
	  ATH_MSG_FATAL ("two sources for object type: " << ObjectTypeInfo::name[objectType]);
	  return StatusCode::FAILURE;
	}
      }
    }

    std::set<CP::SystematicSet> targetSystematics;
    for (auto& target : sys.targetSystematics())
    {
      CP::SystematicSet mytarget;
      if (CP::SystematicSet::filterForAffectingSystematics (target, affecting, mytarget) != CP::SystematicCode::Ok)
      {
	ATH_MSG_FATAL ("failed to filter systematics");
	return StatusCode::FAILURE;
      }
      targetSystematics.insert (mytarget);
    }
    std::vector<CP::SystematicSet> m_targetSystematics
      (targetSystematics.begin(), targetSystematics.end());

    std::vector<std::vector<

    // for (auto& input : m_inputs)
    //   mysys.insert (input->affectingSystematcs());
    */

    return StatusCode::SUCCESS;
  }
}
