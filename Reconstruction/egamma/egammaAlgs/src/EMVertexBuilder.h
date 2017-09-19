/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EMVERTEXBUILDER_H
#define EGAMMAALGS_EMVERTEXBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

namespace InDet
{
  class IVertexFinder;
  class SingleTrackConversionTool;
}

class IEMExtrapolationTools;

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
  float m_maxRadius;
  /**  Minimum Pt, less than that TRT track are pileup for double/single track conversion **/
  float m_minPtCut_DoubleTrack;
  float m_minPtCut_SingleTrack;

  /** @brief TrackParticle container input name*/
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTrackParticleContainerKey;

  /** @brief conversion container output name*/
  SG::WriteHandleKey<xAOD::VertexContainer> m_outputConversionContainerKey;

  /** @brief Tool to find vertices (creates double-track conversions) */
  ToolHandle<InDet::IVertexFinder>    m_vertexFinderTool;

  /** @brief EMExtrapolationTool */
  ToolHandle<IEMExtrapolationTools>  m_EMExtrapolationTool;
    		
};

#endif // EMVERTEXBUILDER_H 
