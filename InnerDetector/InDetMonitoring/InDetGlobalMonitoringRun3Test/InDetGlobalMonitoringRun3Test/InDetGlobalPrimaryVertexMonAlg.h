/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalPrimaryVertexMonAlg.h
 * Implementation of inner detector global primary vertex monitoring tool
 *
 *@author
 * Leonid Serkin <lserkin@cern.ch> @n
 * 
 * based on InDetGlobalPrimaryVertexMonTool.h 
 * 
 ****************************************************************************/

#ifndef InDetGlobalPrimaryVertexMonAlg_H
#define InDetGlobalPrimaryVertexMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

// tracking vertex
#include "StoreGate/ReadHandleKey.h"
#include "xAODTracking/VertexContainer.h"

//Standard c++
#include <string>

//------------------------------

namespace Trk {
  class Vertex;
  class VxCandidate;
}



class InDetGlobalPrimaryVertexMonAlg : public AthMonitorAlgorithm {
  
 public:
  
  InDetGlobalPrimaryVertexMonAlg( const std::string & name, ISvcLocator* pSvcLocator );
  virtual ~InDetGlobalPrimaryVertexMonAlg();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  std::string findComponentString(int bec, int ld) const;
  
  
  
 private:
  
  SG::ReadHandleKey<xAOD::VertexContainer> m_vxContainerName{this,"vxContainerName","PrimaryVertices","Primary Vertices for Global Monitoring"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_vxContainerNameWithoutBeamConstraint{this,"vxContainerNameWithOutBeamConstraint","VxPrimaryCandidateWithBeamConstraint","Vertices without beam constraint for Global Monitoring"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_vxContainerNameSplit{this,"vxContainerNameSplit","VxPrimaryCandidateSplitStream","Split Vertices for Global Monitoring"};
  
  int         m_splitVertexTrkInvFraction; ///< store inverse of the fraction of input tracks used for probe vertex (1:N)
  float       m_distanceSplitVxMatch; ///< store maximum distance for matching split vertices to original non-BC vertex
  /** store metric to be used for split vertex matching in selection efficiency
   * Values currently implemented:
   * 0: dummy metric; returns zero distance
   * 1: simple delta-z metric
   * 2: 3-D distance divided by the error (dominated by Delta z)
   * 3: quadratic sum of distances divided by their errors in the 3 directions. Expected RMS = 1. Default.
   */
  
  int      m_splitMatchingMetric; 
  bool     m_doEnhancedMonitoring; // trigger the enhanced monitoring (to be activated by InDetFlags.doMonitoringPrimaryVertexingEnhanced because split vertexing and vertexing with no beam constraint need to be run as well)
  
  // Helper functions
  /** Returns matching distance between split and original vertex.
   * @param splitVx pointer to split vertex 
   * @param originalVx pointer to original non-BC vertex
   * @return distance for matching split to original Vertex
   */
  double getSplitMatchDistance(const xAOD::Vertex* splitVx, const xAOD::Vertex* originalVx);
  
};
#endif
