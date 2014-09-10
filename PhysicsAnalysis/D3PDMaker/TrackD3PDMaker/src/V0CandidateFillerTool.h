/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_V0CANDIDATEFILLERTOOL_H
#define TRACKD3PDMAKER_V0CANDIDATEFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TrkV0Vertex/V0Container.h"
//#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

namespace Trk{
  class V0Candidate;
}

namespace D3PD {

/* *************************
 * Getter class            *
 * Not needed in principle * 
 ***************************/
class V0ContainerGetterTool
  : public SGCollectionGetterTool<V0Container>
{
public:
  V0ContainerGetterTool (const std::string& type,
			 const std::string& name,
			 const IInterface* parent)
    : SGCollectionGetterTool<V0Container> 
    (type, name, parent){}
};
/***************************/

class V0CandidateFillerTool : public BlockFillerTool<Trk::V0Candidate>
{
 public:
  typedef BlockFillerTool<Trk::V0Candidate> Base;
  V0CandidateFillerTool (const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
  
  StatusCode initialize();
  virtual StatusCode book();
  
  virtual StatusCode fill (const Trk::V0Candidate& p);

 private:
  
  void clearData();

  //ToolHandle<Trk::V0Tools> m_v0Tools;
  IBeamCondSvc* m_iBeamCondSvc;
  std::string   m_beamCondSvcName;

  float *m_ksMass;
  float *m_lambda1Mass;
  float *m_lambda2Mass;
  float *m_vertexProb;
  float *m_vertexChi2;
  float *m_ksPt;
  float *m_ksPx;
  float *m_ksPy;
  float *m_ksEta;
  float *m_ksPhi;
  float *m_ksMomentum;

  float *m_flightX;
  float *m_flightY;
  float *m_cosThetaPointing;
  float *m_totalFlightDistance;
  float *m_properDecayTime;
  float *m_properFlightDist;

  float *m_flightX_BS;
  float *m_flightY_BS;
  float *m_cosThetaPointing_BS;
  float *m_totalFlightDistance_BS;
  float *m_properDecayTime_BS;
  float *m_properFlightDist_BS;

  float *m_radius;

  float *m_thetaPiPlus;
  float *m_thetaPiMinus;
    
  std::vector<float> *m_trk_L;
  std::vector<float> *m_trk_T;

  float *m_thetaStarPiPlus;
  float *m_thetaStarPiMinus;

  std::vector<float> *m_trkPt_SV;
  std::vector<float> *m_trkEta_SV;
  std::vector<float> *m_trkPhi_SV;

}; // class V0CandidateFillerTool

} // namespace D3PD

#endif 
