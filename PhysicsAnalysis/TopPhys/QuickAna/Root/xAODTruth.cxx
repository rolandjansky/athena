/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/xAODTruth.h>

#include <AsgTools/AsgTool.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <type_traits>

#ifdef ROOTCORE
#include <AsgTools/SgTEvent.h>
#include <xAODRootAccessInterfaces/TActiveEvent.h>
#else
#include <StoreGate/StoreGateSvc.h>
#endif

//
// method implementations
//

namespace ana
{
  namespace
  {
    template<typename Container,typename Store>
    Container *getXAOD (Store& store, const std::string& name, bool allowNull)
    {
      Container *result = nullptr;
      if ((allowNull == false || store.template contains<typename std::decay<Container>::type> (name)) &&
	  store.retrieve (result, name).isFailure())
      {
	std::ostringstream str;
	str << "failed to retrieve " << typeid(Container).name() << " collection with name " << name;
	RCU_THROW_MSG (str.str());
      }
      if (!allowNull && result == nullptr)
      {
	std::ostringstream str;
	str << "failed to retrieve " << typeid(Container).name() << " collection with name " << name;
	RCU_THROW_MSG (str.str());
      }
      return result;
    }
  }



  const xAOD::TruthParticleContainer *
  getCTruth (StoreType& store, const std::string& name, bool allowNull)
  {
    return getXAOD<const xAOD::TruthParticleContainer>(store, name, allowNull);
  }



#ifdef ROOTCORE
  const xAOD::TruthParticleContainer *
  getCTruth (xAOD::TEvent& store, const std::string& name, bool allowNull)
  {
    return getXAOD<const xAOD::TruthParticleContainer>(store, name, allowNull);
  }
#endif



  const xAOD::TruthParticleContainer *
  getCTruth (asg::AsgTool& tool, const std::string& name, bool allowNull)
  {
    return getXAOD<const xAOD::TruthParticleContainer>(*tool.evtStore(), name, allowNull);
  }



#ifdef ROOTCORE
  const xAOD::TruthParticleContainer *
  getCTruth (const std::string& name, bool allowNull)
  {
    // Check if there's an active event:
    xAOD::TVirtualEvent* event = xAOD::TActiveEvent::event();
    if (event == nullptr)
    {
      std::ostringstream str;
      str << "Couldn't find an active event in "
	  << "the job";
      RCU_THROW_MSG (str.str());
    }

    // This should actually be a TEvent:
    xAOD::TEvent *myevent = dynamic_cast<xAOD::TEvent*>(event);
    if (myevent == nullptr)
    {
      std::ostringstream str;
      str << "The active event is not of type "
	  << "xAOD::TEvent?!?";
      RCU_THROW_MSG (str.str());
    }
    return getXAOD<const xAOD::TruthParticleContainer>(*myevent, name, allowNull);
  }
#endif
}
