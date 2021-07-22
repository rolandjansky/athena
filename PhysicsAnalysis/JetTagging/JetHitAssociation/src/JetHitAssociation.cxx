/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetHitAssociation/JetHitAssociation.h"

//Include some helpful ROOT objects here.
#include <TVector3.h>

// Constructor
JetHitAssociation::JetHitAssociation(const std::string& name, ISvcLocator* svcloc)
  : AthAlgorithm(name, svcloc),
    m_nStoredPixHits(0),
    m_nStoredSCTHits(0),
    m_nTotalPixHits(0),
    m_nTotalSCTHits(0) {
    
  declareProperty("jetCollectionName", m_jetCollectionName = "AntiKt4EMPFlowJets", 
    		  "Jet collection that will be used for matching the Hits");
  declareProperty("vertexCollectionName", m_vertexCollectionName = "PrimaryVertices", 
    		  "Name of PV collection. Default is PrimaryVertices");
  declareProperty("outputPixHitCollectionName", m_outputPixHitCollectionName = "JetAssociatedPixelClusters", 
    		  "Name of output pixel hit collection. JetAssociatedPixelClusters is default.");
  declareProperty("outputSCTHitCollectionName", m_outputSCTHitCollectionName = "JetAssociatedSCTClusters", 
    		  "Name of output SCT hit collection. JetAssociatedSCTClusters is default.");
  declareProperty("inputPixHitCollectionName", m_inputPixHitCollectionName = "PixelClusters", 
    		  "Name of input pixel hit collection. PixelClusters is default");
  declareProperty("inputSCTHitCollectionName", m_inputSCTHitCollectionName = "SCT_Clusters", 
    		  "Name of input SCT hit collection. SCT_Clusters is default");
  declareProperty("jetPtThreshold", m_jetPtThreshold = 300000.0, 
    		  "Hits are saved only if they match to jets with pT > jetPtThreshold [in MeV]");
  declareProperty("dRmatchHitToJet", m_dRmatchHitToJet = 0.4, 
    		  "The radius used for matching hits to jets");
		  
  declare(m_outputPixHitCollectionName);
  declare(m_outputSCTHitCollectionName);	  
}

StatusCode JetHitAssociation::initialize() {

  ATH_CHECK( AthAlgorithm::initialize() );
  
  ATH_MSG_DEBUG("jetCollectionName           = " << m_jetCollectionName);
  ATH_MSG_DEBUG("vertexCollectionName        = " << m_vertexCollectionName);
  ATH_MSG_DEBUG("inputPixHitCollectionName   = " << m_inputPixHitCollectionName);
  ATH_MSG_DEBUG("inputSCTHitCollectionName   = " << m_inputSCTHitCollectionName);
  ATH_MSG_DEBUG("outputPixHitCollectionName  = " << m_outputPixHitCollectionName);
  ATH_MSG_DEBUG("outputSCTHitCollectionName  = " << m_outputSCTHitCollectionName);
  ATH_MSG_DEBUG("jetPtThreshold [MeV]         = " << m_jetPtThreshold);
  ATH_MSG_DEBUG("dRmatchHitToJet             = " << m_dRmatchHitToJet);
  
  ATH_CHECK(m_inputPixHitCollectionName.initialize());
  ATH_CHECK(m_inputSCTHitCollectionName.initialize());
  ATH_CHECK(m_jetCollectionName.initialize());
  ATH_CHECK(m_vertexCollectionName.initialize());
  ATH_CHECK(m_outputPixHitCollectionName.initialize());
  ATH_CHECK(m_outputSCTHitCollectionName.initialize());

  ATH_MSG_DEBUG("JetHitAssociation: Initialized");
  
  return StatusCode::SUCCESS;
}


// Function for saving hits matched to jets
StatusCode JetHitAssociation::execute() {
  
  const EventContext& ctx = Gaudi::Hive::currentContext();
  
  // All jets
  SG::ReadHandle<xAOD::JetContainer> jetReadHandle(m_jetCollectionName, ctx);
  if ( !jetReadHandle.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve jet container with key " << m_jetCollectionName.key() );
    return StatusCode::FAILURE;
  }
  
  // Keep only jets above threashold
  std::vector<const xAOD::Jet*> jets;
  jets.reserve( jetReadHandle->size() );
  for (const xAOD::Jet *jet : *jetReadHandle) {
    if (jet->pt() > m_jetPtThreshold) jets.push_back(jet);
  }
  
// Create write handles
  SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> outputPixHits(m_outputPixHitCollectionName, ctx);
  SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> outputSCTHits(m_outputSCTHitCollectionName, ctx);

  
  // Vertex collection
  SG::ReadHandle<xAOD::VertexContainer> vertexReadHandle(m_vertexCollectionName, ctx);
  if ( !vertexReadHandle.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve PrimaryVertices container" );
    return StatusCode::FAILURE;
  }
  
  // Find primary vertex
  const xAOD::Vertex *primVtx = nullptr;
  for (const xAOD::Vertex *vertex : *vertexReadHandle) {
    if (vertex->vertexType() == xAOD::VxType::PriVtx) { 
      primVtx = vertex;
      break;
    }
  }
  
  // Read pixel hits
  SG::ReadHandle<xAOD::TrackMeasurementValidationContainer> pixHitReadHandle(m_inputPixHitCollectionName, ctx);
  if ( !pixHitReadHandle.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve pixel hit container with key " << m_inputPixHitCollectionName);
    return StatusCode::FAILURE;
  }
  
  // Read SCT hits
  SG::ReadHandle<xAOD::TrackMeasurementValidationContainer> sctHitReadHandle(m_inputSCTHitCollectionName, ctx);
  if ( !sctHitReadHandle.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve SCT hit container with key " << m_inputSCTHitCollectionName);
    return StatusCode::FAILURE;
  }
  
  // These subroutines check for viable jets and write out hits if any exist. 
  // A JetAssociatedPixelCluster or JetAssociatedSCTCluster container is created for each event regardless
  ATH_CHECK( saveHits(jets, primVtx, pixHitReadHandle, outputPixHits, m_nStoredPixHits, m_nTotalPixHits) );
  ATH_CHECK( saveHits(jets, primVtx, sctHitReadHandle, outputSCTHits, m_nStoredSCTHits, m_nTotalSCTHits) );
  
  return StatusCode::SUCCESS;
}


StatusCode JetHitAssociation::saveHits(const std::vector<const xAOD::Jet*> &jets, 
				       const xAOD::Vertex* const vertex,
				       SG::ReadHandle<xAOD::TrackMeasurementValidationContainer> &hits,
				       const SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> &outputHits,
				       unsigned long long int &nStoredHits,
				       unsigned long long int &nTotalHits) {

  // Collection where output hits are stored
  auto outputHitCollection = std::make_unique<xAOD::TrackMeasurementValidationContainer>();
  auto outputHitCollectionAux = std::make_unique<xAOD::TrackMeasurementValidationAuxContainer>();
  outputHitCollection->setStore(outputHitCollectionAux.get());
    
    
  // if there are no jets you need to skip this loop but you still need to write out the 
  // if the vertex is a null pointer (no primary vertex in the event) do the same
  // JetAssociatedSCTClusters and JetAssociatedPixelClusters
  if (!jets.empty() && vertex!=nullptr) {

    // Get the x,y,z of the primary vertex
    TVector3 PVposition(vertex->x(), vertex->y(), vertex->z());   
    ATH_MSG_DEBUG("JetHitAssociation: PrimaryVertex Z Position = " << PVposition.Z());

    // Loop over hits
    for (const xAOD::TrackMeasurementValidation *hit : *hits) {
      // Get pixel globalX,Y,Z
      float x = hit->globalX();
      float y = hit->globalY();
      float z = hit->globalZ();

      // Put the x,y,z in a TVector3
      TVector3 hitPosition(x,y,z);

      // Correct for the PV position
      hitPosition = hitPosition - PVposition;

      // Loop over jets
      for (const xAOD::Jet *jet : jets) {
	// Calculate dR(hit,jet)
	float dR = hitPosition.DeltaR(jet->p4().Vect());
      
	// if the dR is smaller than the dR association threshold save the hit
	if (dR < m_dRmatchHitToJet) {
	  outputHitCollection->push_back(std::make_unique<xAOD::TrackMeasurementValidation>(*hit));
	  *outputHitCollection->back() = *hit; //Without this line. entries are filled with zero instead of the correct values
	  ++nStoredHits;
	  break;
	}
      }   
    }
  }  
  // count the total number of hits run
  nTotalHits += hits->size(); 
    
  // Write output container
  if ( ! outputHits.put( std::move(outputHitCollection), std::move(outputHitCollectionAux) ) ) {
    ATH_MSG_ERROR("Could not write output hit containers");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode JetHitAssociation::finalize() {

  ATH_MSG_DEBUG("JetHitAssociation: Total number of Pix Hits tested = " << m_nTotalPixHits);
  ATH_MSG_DEBUG("JetHitAssociation: Total number of Pix Hits Stored = " << m_nStoredPixHits);
  ATH_MSG_DEBUG("JetHitAssociation: Total number of SCT Hits tested = " << m_nTotalSCTHits);
  ATH_MSG_DEBUG("JetHitAssociation: Total number of SCT Hits Stored = " << m_nStoredSCTHits);
  
  return StatusCode::SUCCESS;
}
