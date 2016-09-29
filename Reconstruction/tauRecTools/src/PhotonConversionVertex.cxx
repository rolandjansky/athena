/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
/********************************************************************
NAME:     PhotonConversionVertex.cxx
PACKAGE:  offline/Reconstruction/tauRec
AUTHORS:  KG Tan <Kong.Guan.Tan@cern.ch>
CREATED:  May 2011

This tool identifies conversion candidates in/near (definable) a
tau decay cone by reconstructing the conversion vertices and
applying a set of cuts optimized for tau conversions on the vertex
parameters.
 ********************************************************************/

#include "PhotonConversionVertex.h"

//#include "tauEvent/TauJetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "AthContainers/ConstDataVector.h"

PhotonConversionVertex::PhotonConversionVertex(const std::string& name) :
TauRecToolBase(name),
m_vertexFinderTool("InDet::InDetConversionFinderTools") {
    declareProperty("TauRecContainer", m_inputTauJetContainerName = "TauJets");
    declareProperty("TrackParticleContainer", m_inputTrackParticleContainerName = "InDetTrackParticles");
    declareProperty("OutputConversionVertexContainerName", m_outputConversionVertexContainerName = "ConversionsVertex_Container");
    declareProperty("MaxTauJetDr", m_maxTauJetDr = 0.5);
    declareProperty("ConversionFinderTool", m_vertexFinderTool);
}

PhotonConversionVertex::~PhotonConversionVertex() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PhotonConversionVertex::initialize() {
    // Get the VertexFinderTool
    if (!retrieveTool(m_vertexFinderTool)) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PhotonConversionVertex::finalize() {
    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PhotonConversionVertex::eventFinalize() {
    // get the jet container from TauEventData, or the StoreGate if it can't find it
    xAOD::TauJetContainer* tauJetCont = tauEventData()->xAODTauContainer;
    if (!tauJetCont) {
        if (!openContainer(tauJetCont, m_inputTauJetContainerName))
            return StatusCode::FAILURE;
    }

    // get the track particle container from StoreGate
    const xAOD::TrackParticleContainer* trackParticleCont = 0;
    if (!openContainer(trackParticleCont, m_inputTrackParticleContainerName)) return StatusCode::FAILURE;

    // Define container to store
    xAOD::VertexContainer* conversionCandidatesVxCont = 0;
    xAOD::VertexAuxContainer* conversionCandidatesVxContAux = 0;
    conversionCandidatesVxCont = new xAOD::VertexContainer(); // No need to run conversion finding if no tracks are found!
    conversionCandidatesVxContAux = new xAOD::VertexAuxContainer(); // No need to run conversion finding if no tracks are found!
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> convContPair;

    // Redo conversion over the tau seed cones, or over entire region
    if (m_maxTauJetDr > 0.) {

        if (tauJetCont->size() > 0) {

            // Create a temporary TrackParticle container that contains tracks within the tau seed cones
            ConstDataVector<xAOD::TrackParticleContainer> tempCont (SG::VIEW_ELEMENTS);
            for (xAOD::TrackParticleContainer::const_iterator tpcItr = trackParticleCont->begin(); tpcItr != trackParticleCont->end(); ++tpcItr) {
                double minTauDr = getMinDrTauDecay(tauJetCont, *tpcItr);
                if (minTauDr < m_maxTauJetDr) {
                    tempCont.push_back(*tpcItr);
                }
            }
            ATH_MSG_VERBOSE("Number of tracks within tau seed cones for conversion finding: " << tempCont.size());

            // Run the conversion finding algorithm over the tau seed cones
            convContPair = m_vertexFinderTool->findVertex(tempCont.asDataVector());
            conversionCandidatesVxCont = convContPair.first;
            conversionCandidatesVxContAux = convContPair.second;
        } 
/**
        else {
            conversionCandidatesVxCont = new xAOD::VertexContainer(); // No need to run conversion finding if no tracks are found!
            conversionCandidatesVxContAux = new xAOD::VertexAuxContainer(); // No need to run conversion finding if no tracks are found!
        }
**/
    } else {
        ATH_MSG_VERBOSE("Running over entire region! Number of tracks: " << trackParticleCont->size());

        // Run the conversion finding algorithm over the entire region!
        convContPair = m_vertexFinderTool->findVertex(trackParticleCont);
        conversionCandidatesVxCont = convContPair.first;
        conversionCandidatesVxContAux = convContPair.second;
    }

    // Perform the storing
    ATH_MSG_VERBOSE("Number of conversion vertices found: " << conversionCandidatesVxCont->size());
    conversionCandidatesVxCont->setStore( conversionCandidatesVxContAux );

    if (!saveContainer(conversionCandidatesVxCont, m_outputConversionVertexContainerName)) return StatusCode::FAILURE;
    if (!saveContainer(conversionCandidatesVxContAux, m_outputConversionVertexContainerName+"Aux")) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

template <class T>
bool PhotonConversionVertex::openContainer(T* &container, std::string containerName) {
    StatusCode sc = evtStore()->retrieve(container, containerName);
    if (!container || sc.isFailure())
        ATH_MSG_FATAL("Container (" << containerName << ") not found in StoreGate");
    return container;
}

template <class T>
bool PhotonConversionVertex::saveContainer(T* &container, std::string containerName) {
  if(!container){
    ATH_MSG_FATAL("Container (" << containerName << ") cannot be saved in StoreGate");
    return false;
  }
  StatusCode sc = evtStore()->record(container, containerName);
  if (sc.isFailure())
    ATH_MSG_FATAL("Container (" << containerName << ") cannot be saved in StoreGate");
  return container;
}

template <class T>
bool PhotonConversionVertex::retrieveTool(T &tool) {
    if (tool.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << tool);
        return false;
    } else {
        ATH_MSG_VERBOSE("Retrieved tool " << tool);
    }
    return true;
}

double PhotonConversionVertex::getMinDrTauDecay(const xAOD::TauJetContainer* tauJetCont, const xAOD::TrackParticle *trackParticle) {
    double minDR = 99999.;
    for (xAOD::TauJetContainer::const_iterator tjcItr = tauJetCont->begin(); tjcItr != tauJetCont->end(); ++tjcItr) {
        const xAOD::TauJet *tauJet = *tjcItr;
        double dR = trackParticle->p4().DeltaR(tauJet->p4());
        if (dR < minDR) minDR = dR;
    }
    return minDR;
}
#endif
