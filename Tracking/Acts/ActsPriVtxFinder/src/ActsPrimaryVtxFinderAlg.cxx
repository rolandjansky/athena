/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsPriVtxFinder/ActsPrimaryVtxFinderAlg.h"

// ATHENA
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ActsGeometry/IActsPropStepRootWriterSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "TrkVertexFitterInterfaces/IVertexMergingTool.h"
#include "TrkVertexFitterInterfaces/IVertexCollectionSortingTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"

// ACTS
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"

// PACKAGE
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsInterop/Logger.h"
//#include "ActsGeometry/IActsMaterialTrackWriterSvc.h"

// OTHER
#include "CLHEP/Random/RandomEngine.h"

// STL
#include <fstream>
#include <string>

using namespace Acts::UnitLiterals;

ActsPrimaryVtxFinderAlg::ActsPrimaryVtxFinderAlg(const std::string &name,
   ISvcLocator *pSvcLocator)
: AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode ActsPrimaryVtxFinderAlg::initialize() {


  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  ATH_CHECK(m_ActsVertexFinderTool.retrieve());
  
  /*Get the Vertex Merginh Tool*/
  if (m_doVertexMerging) {
//    ATH_CHECK(m_VertexMergingTool.retrieve());
} else {
//    m_VertexMergingTool.disable();
}

  /*Get the Vertex Collection Sorting Tool*/
if (m_doVertexSorting) {
//    ATH_CHECK(m_VertexCollectionSortingTool.retrieve());
} else {
//    m_VertexCollectionSortingTool.disable();
}

ATH_CHECK(m_trkTracksName.initialize(!m_useTrackParticles));
ATH_CHECK(m_tracksName.initialize(m_useTrackParticles));
ATH_CHECK(m_vxCandidatesOutputName.initialize());


return StatusCode::SUCCESS;
}

StatusCode ActsPrimaryVtxFinderAlg::execute(const EventContext &ctx) const {

   SG::WriteHandle<xAOD::VertexContainer> outputVertices (m_vxCandidatesOutputName, ctx);

   xAOD::VertexContainer* theXAODContainer = 0;
   xAOD::VertexAuxContainer* theXAODAuxContainer = 0;
   std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theXAODContainers
   = std::make_pair( theXAODContainer, theXAODAuxContainer );

   if(m_useTrackParticles){
      SG::ReadHandle<xAOD::TrackParticleContainer> trackParticleCollection(m_tracksName, ctx);
      if(trackParticleCollection.isValid()){
        theXAODContainers = m_ActsVertexFinderTool->findVertex ( trackParticleCollection.cptr(), ctx );
      }
      else{
          ATH_MSG_DEBUG("No TrackParticle Collection with key "<<m_tracksName.key()<<" exists in StoreGate. No Vertexing Possible");
          return StatusCode::SUCCESS;
      }
    }
    else{
      SG::ReadHandle<TrackCollection> trackCollection(m_trkTracksName, ctx);
      if(trackCollection.isValid()){
        theXAODContainers = m_ActsVertexFinderTool->findVertex ( trackCollection.cptr(), ctx );
    }
    else{
        ATH_MSG_DEBUG("No Trk::Track Collection with key "<<m_trkTracksName.key()<<" exists in StoreGate. No Vertexing Possible");
        return StatusCode::SUCCESS;
    }
}

const std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theXAODContainersConst = theXAODContainers;

    // now  re-merge and resort the vertex container and store to SG
xAOD::VertexContainer* myVertexContainer = 0;
xAOD::VertexAuxContainer* myVertexAuxContainer = 0;
std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> myVxContainers
= std::make_pair( myVertexContainer, myVertexAuxContainer );


if (theXAODContainersConst.first) {
      //sort xAOD::Vertex container

  if(m_doVertexMerging && theXAODContainersConst.first->size() > 1) {
      //const xAOD::VertexContainer bla = *(theXAODContainersConst.first);
//      //myVxContainers = m_VertexMergingTool->mergeVertexContainer( bla );
      // delete theXAODContainersConst.first; //also cleans up the aux store
      // delete theXAODContainersConst.second; 
      // theXAODContainersConst = myVxContainers;
    }

    if (m_doVertexSorting && theXAODContainersConst.first->size() > 1) { 
//    //       myVxContainers = m_VertexCollectionSortingTool->sortVertexContainer(*theXAODContainersConst.first);
    // delete theXAODContainersConst.first; //also cleans up the aux store
    // delete theXAODContainersConst.second; 
    } 
    else {
      myVxContainers.first = theXAODContainersConst.first;
      myVxContainers.second = theXAODContainersConst.second;
  }

if (myVxContainers.first == 0) {
  ATH_MSG_WARNING("Vertex container has no associated store.");
  return StatusCode::SUCCESS;
}

if (not myVxContainers.first->hasStore()) {
  ATH_MSG_WARNING("Vertex container has no associated store.");
  return StatusCode::SUCCESS;
}
}

ATH_CHECK(outputVertices.record(std::unique_ptr<xAOD::VertexContainer>(myVxContainers.first),std::unique_ptr<xAOD::VertexAuxContainer>(myVxContainers.second)));

ATH_MSG_DEBUG( "Recorded Vertices with key: " << m_vxCandidatesOutputName.key() );

ATH_MSG_VERBOSE(name() << " execute done");
return StatusCode::SUCCESS;
}

StatusCode ActsPrimaryVtxFinderAlg::finalize() { return StatusCode::SUCCESS; }
