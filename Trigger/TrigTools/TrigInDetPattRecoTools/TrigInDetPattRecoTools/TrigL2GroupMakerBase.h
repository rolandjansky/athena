/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_GROUP_MAKER_BASE_H__
#define __TRIG_L2_GROUP_MAKER_BASE_H__

class TrigL2HoughTransformClass;
class TrigL2HashKeyFunction;
class TrigSiSpacePointBase;
class TrigL2TrackCandidate;

#include <cmath>
#include <vector>


class TrigL2GroupMakerBase {
 public:
  TrigL2GroupMakerBase(TrigL2HoughTransformClass* pHT, TrigL2HashKeyFunction* pHK, 
		       float maxEta,
		       double maxShift,
		       bool enhanceL0=false) :
    m_pHough(pHT), m_pKeyGen(pHK), 
    m_maxEtaIdScan(maxEta), 
    m_maxBeamSpotShift(maxShift), 
    m_enhanceLayer0(enhanceL0), m_roiPhiMin(-M_PI){
    m_zVertex=0.0;
    m_meanEta=0.0;
    m_deltaEta=0.0;
    m_isBoundary=false;
  }

    virtual ~TrigL2GroupMakerBase(){};

    void makeTrackCandidates(std::vector<TrigSiSpacePointBase*>&, std::vector<TrigL2TrackCandidate*>&, 
			     double, //zVertex 
			     double); //layerThreshold_tmp

    void makeTrackCandidates(std::vector<TrigSiSpacePointBase*>&, std::vector<TrigL2TrackCandidate*>&, 
			     double, //zVertex 
			     double, //layerThreshold_tmp
			     double,
			     double,
			     double,
			     double
			     );
    double minPhi() {
      return m_roiPhiMin;
    }


 protected:

    void executeAlgorithm(std::vector<TrigSiSpacePointBase*>&, std::vector<TrigL2TrackCandidate*>&, double);
			     

    TrigL2HoughTransformClass*  m_pHough;
    TrigL2HashKeyFunction* m_pKeyGen;
    float m_maxEtaIdScan;
    float m_maxBeamSpotShift;
    bool m_enhanceLayer0;

			  
    double m_zVertex, m_meanEta, m_deltaEta, m_roiPhiMin;
    bool m_isBoundary;
    
};

#endif
