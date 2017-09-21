/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EMVERTEXBUILDER_H
#define EGAMMAALGS_EMVERTEXBUILDER_H

#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

/**
   @class EMVertexBuilder
   This tool builds ID vertices from a given TrackParticleContainer
   @author Kerstin Tackmann (based on work by many others)
*/

class EMVertexBuilder : public AthAlgorithm {

 public:
  EMVertexBuilder (const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode execute();

 private:
	
  /** Maximum radius accepted for conversion vertices **/
  Gaudi::Property<float> m_maxRadius {this, "MaxRadius", 800., 
      "Maximum radius accepted for conversion vertices"};

  /**  Minimum Pt, less than that TRT track are pileup for double/single track conversion **/
  Gaudi::Property<float> m_minPtCut_DoubleTrack {this, 
      "minPCutDoubleTrackConversion", 2000,  
      "Minimum Pt, less than that TRT tracks pileup for double-track conversion"};

  Gaudi::Property<float> m_minPtCut_SingleTrack {this, 
      "minPCutSingleTrackConversion", 2000,
      "Minimum Pt, less than that TRT track pileup for single-track conversion"};

  /** @brief TrackParticle container input name*/
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTrackParticleContainerKey {this,
      "InputTrackParticleContainerName", "GSFTrackParticles", "Input TrackParticles"};

  /** @brief conversion container output name*/
  SG::WriteHandleKey<xAOD::VertexContainer> m_outputConversionContainerKey {this,
      "OutputConversionContainerName", "GSFConversionVertices", 
      "Output conversion vertices"};

  /** @brief Tool to find vertices (creates double-track conversions) */
  ToolHandle<InDet::IVertexFinder> m_vertexFinderTool {this,
      "VertexFinderTool", "InDetConversionFinderTools",
      "The tool that does the converions finding"};

  /** @brief EMExtrapolationTool */
  ToolHandle<IEMExtrapolationTools>  m_EMExtrapolationTool {this,
      "ExtrapolationTool", "EMExtrapolationTools", "Handle of the extrapolation tool"};
    		
};

#endif // EMVERTEXBUILDER_H 
