/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_TRIGVERTEXCOUNTERHYPO_H 
#define TRIGMINBIAS_TRIGVERTEXCOUNTERHYPO_H 

#include <TrigInterfaces/HypoAlgo.h>
#include <xAODTrigMinBias/TrigVertexCounts.h>

/** @class TrigVertexCounter

@author W. H. Bell <W.Bell@cern.ch>

A hypothesis algorithm to test a trigger condition based on online 
reconstructed primary vertices.

*/
class TrigVertexCounterHypo: public HLT::HypoAlgo {
  
 public:
  TrigVertexCounterHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigVertexCounterHypo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  
 private:
  /** Hypo selection criteria */
  bool m_rejectPileup;
  unsigned int m_required_ntrks; //!< for ntrks cut
  unsigned int m_max_ntrks; //!< for pile up rejection
  bool m_cutNumVtx; //!< to select events on the number of vertices
  unsigned int m_min_Nvtx; //!< min number of vertex to select an event

  /** Flag used to bypass hypo selection criteria. */
  bool m_acceptAll;



  /** For monitoring */
  std::vector<unsigned int> m_vtxNtrks;
  std::vector<float> m_vtxTrkPtSqSum;
  int m_vtxNvtx;
  std::vector<unsigned int> m_vtxNtrksAccepted; 
  std::vector<float> m_vtxTrkPtSqSumAccepted; 
  int m_vtxNvtxAccepted;

  /** For accessing retrieved data */
  const xAOD::TrigVertexCounts* m_trigVertexCounts;
  
};

#endif
