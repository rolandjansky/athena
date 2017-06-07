/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/QuickAna.h>

#include <QuickAna/EventData.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/OptimizedScheduler.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/ToolScheduler.h>
#include <RootCoreUtils/Assert.h>

#ifndef ROOTCORE
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#include "boost/algorithm/string.hpp"
#endif

//
// method implementations
//

namespace ana
{
  namespace
  {
    double make_weight (const InternalConfiguration& configuration,
                        xAOD::IParticleContainer *container)
    {
      SG::AuxElement::Accessor<SelectType> accessor
        (configuration.selectionName (SelectionStep::ANALYSIS));

      double result = 1;
      if (container)
      {
        for (auto object : *container)
        {
          if (accessor (*object) &&
              object->isAvailable<float> ("ana_weight"))
          {
            result *= object->auxdata<float> ("ana_weight");
          }
        }
      }
      return result;
    }
  }



  struct QuickAna::Pimpl
  {
    /// description: the objects we use
  public:
    IEventObjects *objects;

    /// description: the cached weight
  public:
    float weight;

    /// \brief the internal configuration object
  public:
    InternalConfiguration internalConfiguration;

    Pimpl () : objects (0), weight (-1) {}
  };



  void QuickAna ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    RCU_INVARIANT (pimpl != 0);
  }



  QuickAna ::
  QuickAna (const std::string& name)
    : AsgTool (name), pimpl (new Pimpl)
  {
    declareConfigProperties (*this);

    RCU_NEW_INVARIANT (this);
  }



  QuickAna ::
  ~QuickAna ()
  {
    delete pimpl;
  }



  StatusCode QuickAna ::
  initialize()
  {
    RCU_CHANGE_INVARIANT (this);
    ATH_MSG_DEBUG("initialize");

#ifndef ROOTCORE
    {
      std::string project_name;
      ATH_CHECK(AthAnalysisHelper::retrieveMetadata ("/TagInfo", "project_name", project_name));
      isDataFlag = (project_name != "IS_SIMULATION");
      ATH_MSG_INFO ("project_name: \"" << project_name << "\"");
      ATH_MSG_INFO ("isDataFlag: " << isDataFlag);

      if(!isDataFlag) {
         std::string simulation_flavour;
         ATH_CHECK(AthAnalysisHelper::retrieveMetadata ("/Simulation/Parameters",
                                                        "SimulationFlavour",
                                                        simulation_flavour));
         ATH_MSG_INFO ("simulation_flavour: \"" << simulation_flavour << "\"");

         boost::to_upper(simulation_flavour);
         isAFIIFlag = simulation_flavour.find("ATLFASTII") != std::string::npos;
         //isAFIIFlag = (!simulation_flavour.empty() && simulation_flavour != "default");
         ATH_MSG_INFO ("isAFIIFlag: " << isAFIIFlag);
      }
      else isAFIIFlag = false;
    }
#endif
    pimpl->internalConfiguration.fetchConfiguration (*this);

    if (schedulerDef == "basic")
      toolScheduler.reset (new ToolScheduler (name() + "_scheduler"));
    else if (schedulerDef == "optimized")
      toolScheduler.reset (new OptimizedScheduler (name() + "_scheduler"));
    else
    {
      ATH_MSG_ERROR ("unknown scheduler: " << schedulerDef);
      return StatusCode::FAILURE;
    }
    ATH_CHECK (toolScheduler->addTools (*this, pimpl->internalConfiguration, name()));
    ATH_CHECK (toolScheduler->initialize());

    return StatusCode::SUCCESS;
  }



  bool QuickAna ::
  isAffectedBySystematic (const CP::SystematicVariation& systematic) const
  {
    RCU_READ_INVARIANT (this);
    CP::SystematicSet sysList = affectingSystematics();
    return sysList.find (systematic) != sysList.end();
  }



  CP::SystematicSet QuickAna ::
  affectingSystematics() const
  {
    RCU_READ_INVARIANT (this);
    return toolScheduler->affectingSystematics();
  }



  CP::SystematicSet QuickAna ::
  recommendedSystematics() const
  {
    RCU_READ_INVARIANT (this);
    return toolScheduler->recommendedSystematics();
  }



  CP::SystematicCode QuickAna ::
  applySystematicVariation (const CP::SystematicSet& systConfig)
  {
    RCU_CHANGE_INVARIANT (this);
    ANA_CHECK_SET_TYPE (CP::SystematicCode);
    ANA_CHECK (toolScheduler->applySystematicVariation(systConfig));
    return CP::SystematicCode::Ok;
  }



  StatusCode QuickAna ::
  process ()
  {
    RCU_CHANGE_INVARIANT (this);
    pimpl->weight = -1;
    pimpl->objects = 0;
    return toolScheduler->fillEventObjects (pimpl->objects);
  }



  float QuickAna ::
  weight ()
  {
    RCU_CHANGE_INVARIANT (this);

    if (pimpl->weight == -1)
    {
      // Rely on local function to do most of the heavy lifting
      double weight = eventWeight();

      auto select = pimpl->objects->eventSelect();
      if (select &&
          select->auxdata<ana::SelectType> ("ana_select") == false)
      {
        weight = 0;
      } else
      {
        weight *= make_weight (pimpl->internalConfiguration, pimpl->objects->muons());
        weight *= make_weight (pimpl->internalConfiguration, pimpl->objects->taus());
        weight *= make_weight (pimpl->internalConfiguration, pimpl->objects->electrons());
        weight *= make_weight (pimpl->internalConfiguration, pimpl->objects->photons());
        weight *= make_weight (pimpl->internalConfiguration, pimpl->objects->jets());
      }

      pimpl->weight = weight;
    }
    return pimpl->weight;
  }



  float QuickAna ::
  eventWeight ()
  {
    RCU_CHANGE_INVARIANT (this);
    float weight = pimpl->objects->eventWeight();
    // Add the JVT scale factor
    if (pimpl->objects->eventinfo()->auxdata<float>("JVT_SF"))
    { 
      weight *= pimpl->objects->eventinfo()->auxdata<float>("JVT_SF");
    }
    // Add the pileup weight if we had a random run number (PRW was enabled)
    if (pimpl->objects->eventinfo()->auxdata<unsigned int>( "RandomRunNumber" ))
    { 
      weight *= pimpl->objects->eventinfo()->auxdata<float>("PileupWeight");
    }
    // In the case of MC, add the MC weight
    if(pimpl->objects->eventinfo()->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
      weight *= pimpl->objects->eventinfo()->mcEventWeight();
    }
    return weight;
  }



  const IEventObjects& QuickAna ::
  objects () const
  {
    RCU_READ_INVARIANT (this);
    return *pimpl->objects;
  }



  const InternalConfiguration& QuickAna ::
  internalConfiguration ()
  {
    RCU_READ_INVARIANT (this);
    return pimpl->internalConfiguration;
  }



  //xAOD::EventInfoContainer& QuickAna ::
  xAOD::EventInfo *QuickAna ::
  eventinfo () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->eventinfo(), "eventinfo is not defined in this analysis");
    return pimpl->objects->eventinfo();
  }



  xAOD::MuonContainer *QuickAna ::
  muons () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->muons(), "muons are defined in this analysis");
    return pimpl->objects->muons();
  }



  xAOD::ElectronContainer *QuickAna ::
  electrons () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->electrons(), "electrons are defined in this analysis");
    return pimpl->objects->electrons();
  }

  xAOD::PhotonContainer *QuickAna ::
  photons () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->photons(), "photons are defined in this analysis");
    return pimpl->objects->photons();
  }


  xAOD::TauJetContainer *QuickAna ::
  taus () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->taus(), "taus are defined in this analysis");
    return pimpl->objects->taus();
  }



  xAOD::JetContainer *QuickAna ::
  jets () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->jets(), "jets are defined in this analysis");
    return pimpl->objects->jets();
  }

  xAOD::JetContainer *QuickAna ::
  fat_jets () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->fat_jets(), "fat jets are defined in this analysis");
    return pimpl->objects->fat_jets();
  }


  xAOD::MissingET *QuickAna ::
  met () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->met(), "met is not defined in this analysis");
    xAOD::MissingET *result = (*pimpl->objects->met())["Final"];
    RCU_ASSERT (result != 0);
    return result;
  }

  xAOD::MissingET *QuickAna ::
  met2 () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (pimpl->objects->met2(), "met2 is not defined in this analysis");
    xAOD::MissingET *result = (*pimpl->objects->met2())["Final"];
    RCU_ASSERT (result != 0);
    return result;
  }



  const std::vector<CP::SystematicSet>& QuickAna ::
  systematics () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (toolScheduler != nullptr, "initialize() has been called");
    return toolScheduler->systematics();
  }



  StatusCode QuickAna ::
  setSystematics (const std::vector<CP::SystematicSet>& val_systematics)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2 (toolScheduler != nullptr, "initialize() has been called");
    return toolScheduler->setSystematics (val_systematics);
  }



  EventData QuickAna ::
  getEventData () const
  {
    RCU_READ_INVARIANT (this);
    return toolScheduler->getEventData ();
  }
}
