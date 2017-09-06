/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGMINBIAS_TRIGVERTEXCOUNTER_H
#define TRIGMINBIAS_TRIGVERTEXCOUNTER_H

#include "TrigInterfaces/FexAlgo.h"
#include <string>
#include <xAODTracking/VertexContainer.h>


/** @class TrigVertexCounter
    @author W. H. Bell <W.Bell@cern.ch>

    @brief A feature extraction algorithm to retrieve online primary vertices 
*/
class TrigVertexCounter: public HLT::FexAlgo {
 public:
  
  TrigVertexCounter(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorithm constructor
  ~TrigVertexCounter(void);

  HLT::ErrorCode hltInitialize();  //!< hlt initialize
  HLT::ErrorCode hltFinalize();    //!< hlt finalize
  HLT::ErrorCode hltExecute(const HLT::TriggerElement *, HLT::TriggerElement* outputTE);
  
 private:
  /** Vertex container name. */
  std::string m_vtxContainerName;

  /** Minimum track pT */
  float  m_pTmin;

  /** Variables used to monitor data stored in TrigVertexCounts */ 
  std::vector<unsigned int> m_vtxNtrks; 
  std::vector<float> m_vtxTrkPtSqSum; 
  int m_vtxNvtx;

  /** Timers */
  TrigTimer *m_formFeaturesTimer;
  TrigTimer *m_getVerticesTimer;

  /** A pointer to the track collection to be retrieved */
  const xAOD::VertexContainer *m_vxContainer;

};

#endif
