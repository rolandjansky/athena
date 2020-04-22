/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_VertexTruthMatching.cxx
 * @author Valentina Cairo
 **/

#include "InDetPerfPlot_VertexTruthMatching.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "InDetPhysValMonitoringUtilities.h"
#include "xAODEventInfo/EventInfo.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"

// xAOD truth object includes:
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthEventBaseContainer.h"
#include "xAODTruth/TruthVertex.h"

using namespace IDPVM;


InDetPerfPlot_VertexTruthMatching::InDetPerfPlot_VertexTruthMatching(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_vx_type_truth(nullptr), m_vx_reco_efficiency(nullptr){
  // nop
}

void
InDetPerfPlot_VertexTruthMatching::initializePlots() {
     
     IDPVM_BOOK(m_vx_type_truth);
     IDPVM_BOOK(m_vx_reco_efficiency);

}


void 
InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex) {

     // fill vertex truth match type

     xAOD::Vertex::Decorator<InDetVertexTruthMatchUtils::VertexMatchType> recoVtxMatchTypeInfo("VertexMatchType");
 
     if (recoVtxMatchTypeInfo.isAvailable(vertex)) {
       try {
         ATH_MSG_DEBUG("VERTEX DECORATOR ======= "<< recoVtxMatchTypeInfo(vertex)<< ", with nTRACKS === " << vertex.nTrackParticles() << ", vertex index = " << vertex.index()<< " AT (" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << ")");
         fillHisto(m_vx_type_truth, recoVtxMatchTypeInfo(vertex));
       }
       catch (SG::ExcBadAuxVar &) {
         ATH_MSG_WARNING("VertexMatchType DECORATOR seems to be available, but may be broken  ===========");
       } 
     }
     else {
       ATH_MSG_WARNING("VertexMatchType DECORATOR is NOT available  ===========");
     } 
 
}//end InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex)
