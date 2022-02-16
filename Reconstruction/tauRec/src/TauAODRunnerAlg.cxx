/*
	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TauAODRunnerAlg.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "xAODCore/ShallowCopy.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauAODRunnerAlg::TauAODRunnerAlg(	const std::string &name,
                                 	ISvcLocator *pSvcLocator) : AthAlgorithm(name, pSvcLocator)
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauAODRunnerAlg::~TauAODRunnerAlg()
{
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauAODRunnerAlg::initialize()
{

  //-------------------------------------------------------------------------
  // No tools allocated!
  //-------------------------------------------------------------------------
	if (m_tools_mod.empty())
	{
		ATH_MSG_ERROR("no tools given!");
		return StatusCode::FAILURE;
	}

	ATH_CHECK(m_tauContainer.initialize());
	ATH_CHECK(m_tauTrackInputContainer.initialize());
	ATH_CHECK(m_pi0ClusterInputContainer.initialize());

	ATH_CHECK(m_tauOutContainer.initialize());

	ATH_CHECK(m_pi0Container.initialize());

	ATH_CHECK(m_neutralPFOOutputContainer.initialize());
	ATH_CHECK(m_chargedPFOOutputContainer.initialize());
	ATH_CHECK(m_hadronicPFOOutputContainer.initialize());
	ATH_CHECK(m_tauTrackOutputContainer.initialize());

  //-------------------------------------------------------------------------
  // Allocate tools
  //-------------------------------------------------------------------------
	ATH_CHECK(m_tools_mod.retrieve());
	ATH_CHECK(m_tools_after.retrieve());

	ATH_MSG_INFO("List of tools in execution sequence:");
	ATH_MSG_INFO("------------------------------------");
	unsigned int tool_count = 0;
	for (ToolHandle<ITauToolBase> &tool : m_tools_mod)
	{
		++tool_count;
		ATH_MSG_INFO(tool->type() << " - " << tool->name());
	}
	for (ToolHandle<ITauToolBase> &tool : m_tools_after)
	{
		++tool_count;
		ATH_MSG_INFO(tool->type() << " - " << tool->name());
	}
	ATH_MSG_INFO(" ");
	ATH_MSG_INFO("------------------------------------");

	if (tool_count == 0)
	{
		ATH_MSG_ERROR("could not allocate any tool!");
		return StatusCode::FAILURE;
	}

	return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauAODRunnerAlg::finalize()
{

	StatusCode sc;

	//-----------------------------------------------------------------
	// Loop stops when Failure indicated by one of the tools
	//-----------------------------------------------------------------
	for (ToolHandle<ITauToolBase> &tool : m_tools_mod)
	{
		ATH_MSG_VERBOSE("Invoking tool " << tool->name());
		sc = tool->finalize();
		if (sc.isFailure())
		break;
	}

	for (ToolHandle<ITauToolBase> &tool : m_tools_after)
	{
		ATH_MSG_VERBOSE("Invoking tool " << tool->name());
		sc = tool->finalize();
		if (sc.isFailure())
		break;
	}

	if (sc.isSuccess())
	{
		ATH_MSG_VERBOSE("All the invoded tools are finilized successfully.");
	}

	return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauAODRunnerAlg::execute()
{

	// Read in tau jets
	SG::ReadHandle<xAOD::TauJetContainer> tauInputHandle(m_tauContainer);
	if (!tauInputHandle.isValid())
	{
		ATH_MSG_ERROR("Could not retrieve HiveDataObj with key " << tauInputHandle.key());
		return StatusCode::FAILURE;
	}
	const xAOD::TauJetContainer *pTauContainer = tauInputHandle.cptr();

	// Write the output tau jets, which is a deep copy of the input ones
	SG::WriteHandle<xAOD::TauJetContainer> outputTauHandle(m_tauOutContainer);
	ATH_CHECK(outputTauHandle.record(std::make_unique<xAOD::TauJetContainer>(), std::make_unique<xAOD::TauJetAuxContainer>()));
	xAOD::TauJetContainer *newTauCon = outputTauHandle.ptr();

	for (const xAOD::TauJet *tau : *pTauContainer)
	{
		xAOD::TauJet *newTau = new xAOD::TauJet();
		newTauCon->push_back(newTau);
		*newTau = *tau;
	}

	// READ IN THE official TauTrack container
	SG::ReadHandle<xAOD::TauTrackContainer> tauTrackInputHandle(m_tauTrackInputContainer);
	if (!tauTrackInputHandle.isValid())
	{
		ATH_MSG_ERROR("Could not retrieve HiveDataObj with key " << tauTrackInputHandle.key());
		return StatusCode::FAILURE;
	}
	const xAOD::TauTrackContainer *pTauTrackContainer = tauTrackInputHandle.cptr();

	//Write the output tau track container, which is a deep copy of the original
	SG::WriteHandle<xAOD::TauTrackContainer> outputTauTrackHandle(m_tauTrackOutputContainer);
	ATH_CHECK(outputTauTrackHandle.record(std::make_unique<xAOD::TauTrackContainer>(), std::make_unique<xAOD::TauTrackAuxContainer>()));
	xAOD::TauTrackContainer *newTauTrkCon = outputTauTrackHandle.ptr();

	for (const xAOD::TauTrack *tauTrk : *pTauTrackContainer)
	{
		xAOD::TauTrack *newTauTrk = new xAOD::TauTrack();
		newTauTrkCon->push_back(newTauTrk);
		*newTauTrk = *tauTrk;
	}

	//relink the tautrack to the taujet
	for (xAOD::TauJet *newTau : *newTauCon)
	{
		auto oldlinks =  newTau->allTauTrackLinksNonConst();
		newTau->clearTauTrackLinks();
		for (auto oldlink : oldlinks)
		{
			const xAOD::TauTrack* oldLinkedTrk = *oldlink;
			for (const xAOD::TauTrack * newTrk : *newTauTrkCon)
			{
				if (newTrk->track() == oldLinkedTrk->track())
				{
				ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
				linkToTauTrack.toContainedElement(*newTauTrkCon, newTrk);
				newTau->addTauTrackLink(linkToTauTrack);
				break;
				}
			}
		}
	}


	// Read the CaloClusterContainer created by the CaloClusterMaker
	SG::ReadHandle<xAOD::CaloClusterContainer> pi0ClusterInHandle( m_pi0ClusterInputContainer );
	if (!pi0ClusterInHandle.isValid()) {
		ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << pi0ClusterInHandle.key());
		return StatusCode::FAILURE;
	}
	const xAOD::CaloClusterContainer * pi0ClusterContainer = pi0ClusterInHandle.cptr();

	// write charged PFO container
	SG::WriteHandle<xAOD::PFOContainer> chargedPFOHandle( m_chargedPFOOutputContainer );
	ATH_CHECK(chargedPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
	xAOD::PFOContainer* chargedPFOContainer = chargedPFOHandle.ptr();

	// write neutral PFO container
	SG::WriteHandle<xAOD::PFOContainer> neutralPFOHandle( m_neutralPFOOutputContainer );
	ATH_CHECK(neutralPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
	xAOD::PFOContainer* neutralPFOContainer = neutralPFOHandle.ptr();

	// write pi0 container
	SG::WriteHandle<xAOD::ParticleContainer> pi0Handle(m_pi0Container);
	ATH_CHECK(pi0Handle.record(std::make_unique<xAOD::ParticleContainer>(), std::make_unique<xAOD::ParticleAuxContainer>()));
	xAOD::ParticleContainer *pi0Container = pi0Handle.ptr();

	// write hadronic cluster PFO container
	SG::WriteHandle<xAOD::PFOContainer> hadronicPFOHandle( m_hadronicPFOOutputContainer );
	ATH_CHECK(hadronicPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
	xAOD::PFOContainer* hadronicClusterPFOContainer = hadronicPFOHandle.ptr();


	// iterate over the copy
	int n_tau_modified = 0;
	for (xAOD::TauJet *pTau : *newTauCon)
	{
		//-----------------------------------------------------------------
		// Loop stops when Failure indicated by one of the tools
		//-----------------------------------------------------------------
		StatusCode sc;

		for (ToolHandle<ITauToolBase> &tool : m_tools_mod)
		{
			ATH_MSG_DEBUG("RunnerAlg Invoking tool " << tool->name());
			if (tool->type() == "TauPi0ClusterCreator")
			{
				sc = tool->executePi0ClusterCreator(*pTau, *neutralPFOContainer, *hadronicClusterPFOContainer, *pi0ClusterContainer);
			}
			else if (tool->type() == "TauVertexVariables")
			{
				//sc = tool->executeVertexVariables(*pTau, nullptr);
			}
			else if (tool->type() == "TauPi0ClusterScaler")
			{
				sc = tool->executePi0ClusterScaler(*pTau, *neutralPFOContainer, *chargedPFOContainer);
			}
			else if (tool->type() == "TauPi0ScoreCalculator")
			{
				sc = tool->executePi0nPFO(*pTau, *neutralPFOContainer);
			}
			else if (tool->type() == "TauPi0Selector")
			{
				sc = tool->executePi0nPFO(*pTau, *neutralPFOContainer);
			}
			else if (tool->type() == "PanTau::PanTauProcessor")
			{
				sc = tool->executePanTau(*pTau, *pi0Container);
			}
			else if (tool->type() == "tauRecTools::TauTrackRNNClassifier")
			{
				sc = tool->executeTrackClassifier(*pTau, *newTauTrkCon);
			}
			else
			{
				sc = tool->execute(*pTau);
			}
			if (sc.isFailure())
				break;
		}
		if (sc.isSuccess())
		{
			ATH_MSG_VERBOSE("The tau candidate has been modified successfully by the invoked modification tools.");
		}
		// if tau is not modified by the above tools, never mind running the tool afterward
		if (isTauModified(pTau, pTauContainer->at(pTau->index()))) 
		{
			n_tau_modified++;
			for (ToolHandle<ITauToolBase> &tool : m_tools_after)
			{
				ATH_MSG_DEBUG("RunnerAlg Invoking tool " << tool->name());
				if (tool->type() == "TauPi0ClusterCreator")
				{
					sc = tool->executePi0ClusterCreator(*pTau, *neutralPFOContainer, *hadronicClusterPFOContainer, *pi0ClusterContainer);
				}
				else if (tool->type() == "TauVertexVariables")
				{
					//sc = tool->executeVertexVariables(*pTau, nullptr);
				}
				else if (tool->type() == "TauPi0ClusterScaler")
				{
					sc = tool->executePi0ClusterScaler(*pTau, *neutralPFOContainer, *chargedPFOContainer);
				}
				else if (tool->type() == "TauPi0ScoreCalculator")
				{
					sc = tool->executePi0nPFO(*pTau, *neutralPFOContainer);
				}
				else if (tool->type() == "TauPi0Selector")
				{
					sc = tool->executePi0nPFO(*pTau, *neutralPFOContainer);
				}
				else if (tool->type() == "PanTau::PanTauProcessor")
				{
					sc = tool->executePanTau(*pTau, *pi0Container);
				}
				else if (tool->type() == "tauRecTools::TauTrackRNNClassifier")
				{
					sc = tool->executeTrackClassifier(*pTau, *newTauTrkCon);
				}
				else
				{
					sc = tool->execute(*pTau);
				}
				if (sc.isFailure())
					break;
			}
			if (sc.isSuccess())
			{
				ATH_MSG_VERBOSE("The tau candidate has been modified successfully by the invoked official tools.");
			}
		}
	} 

	ATH_MSG_VERBOSE("The tau candidate container has been modified by the rest of the tools");
	ATH_MSG_INFO(n_tau_modified << " / " << pTauContainer->size() <<" taus were modified");

	return StatusCode::SUCCESS;
}

//helper 
bool TauAODRunnerAlg::isTauModified(const xAOD::TauJet* oritaus, const xAOD::TauJet* modtaus)
{
	bool trkrm = (oritaus->nAllTracks() != modtaus->nAllTracks());
	bool clsrm = (oritaus->nClusters() != modtaus->nClusters());
	return trkrm || clsrm;
}