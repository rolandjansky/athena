/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigHIL2VtxMultHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHIHypo
 *
 * AUTHOR:   M. Sutton
 * CREATED:  Jan 2013
 * MODIFIED: 
 *
 *********************************************************************/

#ifndef TRIGHIL2VTXMULT_H
#define TRIGHIL2VTXMULT_H

#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;
class TriggerElement;


/**
 * \class TrigHIL2VtxMultHypo
 * \brief TrigHIL2VtxMultkHypo is a Trigger Hypothesis Algorithm that retrieves the TrigVertex 
 * container created by the special HI mode of the L2 ID algorithm
 * and simply cuts on the multiplicity of candidates
 */

class TrigHIL2VtxMultHypo : public HLT::HypoAlgo {
  
 public:
  
  TrigHIL2VtxMultHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHIL2VtxMultHypo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltFinalize();
  

 private:
  
  // define the properties:
  //----------------------------
  
  /// accept all events
  bool m_acceptAll;
  
  /// minimum required number of vertices above threshold 
  int m_minVertices;
  
  /// weight threshold for each vertex
  int m_weightThreshold;

  int m_numVertices;
  
  std::vector<double> m_vertexWeight;

  double m_vertexWeightMax;

};
#endif

