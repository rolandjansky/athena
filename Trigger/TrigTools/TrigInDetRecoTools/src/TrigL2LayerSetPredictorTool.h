/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_LAYER_SET_PREDICTOR_TOOL_H__
#define __TRIG_L2_LAYER_SET_PREDICTOR_TOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetRecoTools/ITrigL2LayerSetPredictorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/ServiceHandle.h"

//class TrigTimer;
//class IRegSelSvc;
class PixelID;
class SCT_ID;
class AtlasDetectorID;
class ITrigL2LayerNumberTool;

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

typedef struct TrigL2LayerDescriptor {
public:
TrigL2LayerDescriptor(int t) : m_type(t), m_nElements(0), m_refCoordinate(0.0),
    m_maxCoordinate(-10000.0), m_minCoordinate(10000.0) {};
  int m_type, m_nElements;
  double m_refCoordinate, m_maxCoordinate, m_minCoordinate;
private:
  TrigL2LayerDescriptor(){};
} TRIG_L2_LAYER_DESCRIPTOR;

typedef struct LayerImpactPoint {
public:
  struct compare {
    bool operator()(const struct LayerImpactPoint& p1, const struct LayerImpactPoint& p2) {
      return (p1.m_L < p2.m_L);
    }
  };
LayerImpactPoint(int lId, double l) : m_layerId(lId), m_L(l) {};
  int m_layerId;
  double m_L;
private:
LayerImpactPoint() : m_layerId(-100), m_L(0.0){};
} LAYER_IMPACT_POINT;

class TrigL2LayerSetPredictorTool: public AthAlgTool, virtual public ITrigL2LayerSetPredictorTool { 
  
 public: 
  TrigL2LayerSetPredictorTool(const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2LayerSetPredictorTool() {}; 
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode createLUT();
  virtual const TrigL2LayerSetLUT* getLUT() {return m_pLUT;}

 private:

  //ServiceHandle<IRegSelSvc>     m_regionSelector;
  ToolHandle<ITrigL2LayerNumberTool>  m_numberingTool;
  TrigL2LayerSetLUT* m_pLUT;

  double m_maxEta, m_maxZ0;
  int m_nBinsZ, m_nBinsTheta;
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;
  const InDetDD::PixelDetectorManager* m_pixelManager;
  const InDetDD::SCT_DetectorManager* m_SCT_Manager;
};



#endif


 
