/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauDiscriBuilder.cxx
// package:     PhysicsAnalysis/TauID/TauDiscriminant
// authors:     M. Wolter, A. Kaczmarska, Noel Dawe
// date:        13 March 2008
//-----------------------------------------------------------------------------

#include "TauDiscriminant/TauDiscriBuilder.h"
#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/FakeTauBits.h"
#include "TauDiscriminant/FakeTauScores.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
// #include "tauEvent/TauCommonDetails.h"
#include "xAODTau/TauDefs.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"

using namespace xAOD;
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauDiscriBuilder::TauDiscriBuilder( const std::string &name,
		ISvcLocator * pSvcLocator ) :
    		AthAlgorithm( name, pSvcLocator ),
    		tauInputContainerName( "TauContainer" ),
    		tools( this ), //make tools private
    		manager(0)
{
	declareProperty( "container", tauInputContainerName );
	declareProperty( "tools", tools, "List of TauDiscriToolBase tools" );
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauDiscriBuilder::~TauDiscriBuilder()
{
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauDiscriBuilder::initialize()
{
	StatusCode sc = StatusCode::SUCCESS;

	this->manager = new TauDetailsManager(&*evtStore());

	//-------------------------------------------------------------------------
	// No tools allocated!
	//-------------------------------------------------------------------------
	if( this->tools.size() == 0 )
	{
		return StatusCode::FAILURE;
	}

	//-------------------------------------------------------------------------
	// Allocate tools
	//-------------------------------------------------------------------------
	ToolHandleArray<TauDiscriToolBase>::iterator tool_it(this->tools.begin());
	ToolHandleArray<TauDiscriToolBase>::iterator tool_end(this->tools.end());
	ATH_MSG_INFO("------------------------------------");
	ATH_MSG_INFO("List of tools in execution sequence:");
	unsigned int tool_count(0);

	for(; tool_it != tool_end; ++tool_it )
	{
		if( tool_it->retrieve().isFailure() )
		{
			ATH_MSG_WARNING("Cannot find tool named <" << *tool_it << ">");
		}
		else
		{
			++tool_count;
			ATH_MSG_INFO(tool_it->name());
			if( (*tool_it)->prepare(*this->manager).isFailure() )
			{
				ATH_MSG_FATAL("Initialization failed in tool " << tool_it->name());
				return sc;
			}
		}
	}
	ATH_MSG_INFO("------------------------------------");

	if(tool_count == 0)
	{
		ATH_MSG_ERROR("Did not allocate any tool!");
		return StatusCode::FAILURE;
	}
	return sc;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauDiscriBuilder::finalize()
{
	delete this->manager;
	return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauDiscriBuilder::execute()
{
	xAOD::TauJetContainer  *tau_container;
	const xAOD::TauJetContainer  *const_tau_container;
	StatusCode sc = evtStore()->retrieve(const_tau_container, this->tauInputContainerName);

	tau_container = const_cast<TauJetContainer *>(const_tau_container);

	if (sc.isFailure() || ! tau_container)
	{
		ATH_MSG_WARNING("No input tau container found!");
		sc = StatusCode::SUCCESS;
		return sc;
	}
	ATH_MSG_VERBOSE("Processing input tau Container Name = " << this->tauInputContainerName);

	FakeTauBits* fakeBits(0);
	FakeTauBitsContainer* fakeBitsContainer(new FakeTauBitsContainer());

	FakeTauScores* fakeScores(0);
	FakeTauScoresContainer* fakeScoresContainer(new FakeTauScoresContainer());

	// Update event-based variables
	if (!this->manager->updateEvent())
	{
		ATH_MSG_WARNING("Updating event-based variables in TauDetailsManager failed! Do not trust discriminant outputs!");
		return StatusCode::SUCCESS;
	}

	xAOD::TauJetContainer::iterator tau_it(tau_container->begin());
	xAOD::TauJetContainer::iterator tau_end(tau_container->end());

	// Loop over tau's:
	for (; tau_it != tau_end; ++tau_it)
	{
		if (!this->manager->update_with_edm(**tau_it))
		{
			ATH_MSG_WARNING("Updating tau-based variables in TauDetailsManager failed! Do not trust discriminant outputs!");
			return StatusCode::SUCCESS;
		}

		ATH_MSG_VERBOSE(*this->manager);

		fakeBits = new FakeTauBits(*tau_it);
		fakeScores = new FakeTauScores(*tau_it);

		//-----------------------------------------------------------------
		// Process the candidate
		//-----------------------------------------------------------------
		ToolHandleArray<TauDiscriToolBase>::iterator tool_it(this->tools.begin());
		ToolHandleArray<TauDiscriToolBase>::iterator tool_end(this->tools.end());

		//-----------------------------------------------------------------
		// Loop stops when Failure indicated by one of the tools
		//-----------------------------------------------------------------
		for(; tool_it != tool_end; ++tool_it )
		{
			ATH_MSG_VERBOSE("Invoking tool " << tool_it->name());
			sc = (*tool_it)->execute( *tau_it, fakeBits, fakeScores);
			if( sc.isFailure() )
			{
				ATH_MSG_FATAL("Execute failed in tool " << tool_it->name());
				return sc;
			}
			// TEMPORARY HACK
			ATH_MSG_VERBOSE("Tool name: "<<tool_it->name());
			if(tool_it->name() == "TauPi0BDT")
			{
				ATH_MSG_VERBOSE("HACK FOR NPI0S");
				float Primary   = fakeScores->getScore(TauScore::BDT_PI0_PRIMARY);
				float Secondary = fakeScores->getScore(TauScore::BDT_PI0_SECONDARY);
				int nPi0s = 0;
				if (Primary < 0.465)	nPi0s += 1;
				if (Secondary < 0.565)	nPi0s += 1;

				this->manager->setNpi0(**tau_it,nPi0s);
			}
			ATH_MSG_VERBOSE(*this->manager);

		}
		fakeBitsContainer->push_back(fakeBits);
		fakeScoresContainer->push_back(fakeScores);
	}

	sc = evtStore()->record(fakeBitsContainer, "FakeTauBitsContainer", false);
	if (sc.isFailure())
	{
		ATH_MSG_WARNING("Could not record FakeTauBitsContainer in StoreGate!");
		return StatusCode::FAILURE;
	}
	sc = evtStore()->record(fakeScoresContainer, "FakeTauScoresContainer", false);
	if (sc.isFailure())
	{
		ATH_MSG_WARNING("Could not record FakeTauScoresContainer in StoreGate!");
	}
	return sc;
}
