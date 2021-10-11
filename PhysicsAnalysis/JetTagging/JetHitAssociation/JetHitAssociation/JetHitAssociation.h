/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETHITASSOCIATION_H
#define JETHITASSOCIATION_H

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Containers
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"

// Read and write handles
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class JetHitAssociation : public AthAlgorithm
{
  public:
    /// Constructor with parameters: 
    JetHitAssociation (const std::string& name,
                       ISvcLocator* svcloc);


    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode execute() override;


  private:
    // functions
    StatusCode saveHits(const std::vector<const xAOD::Jet*> &jets, 
	 		const xAOD::Vertex* const vertex,
	 		SG::ReadHandle<xAOD::TrackMeasurementValidationContainer> &hits,
			const SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> &writeHandle,
			unsigned long long int &nStoredHits,
			unsigned long long int &nTotalHits);
    
    // Read and write handles
    SG::ReadHandleKey<xAOD::JetContainer> m_jetCollectionName;
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexCollectionName;
    SG::ReadHandleKey<xAOD::TrackMeasurementValidationContainer>  m_inputPixHitCollectionName;
    SG::ReadHandleKey<xAOD::TrackMeasurementValidationContainer>  m_inputSCTHitCollectionName;
    SG::WriteHandleKey<xAOD::TrackMeasurementValidationContainer> m_outputPixHitCollectionName;
    SG::WriteHandleKey<xAOD::TrackMeasurementValidationContainer> m_outputSCTHitCollectionName;
    
    // algorithm parameters, counters
    float m_jetPtThreshold;
    float m_dRmatchHitToJet;
    unsigned long long int m_nStoredPixHits;
    unsigned long long int m_nStoredSCTHits;
    unsigned long long int m_nTotalPixHits;
    unsigned long long int m_nTotalSCTHits;
    
};

#endif
