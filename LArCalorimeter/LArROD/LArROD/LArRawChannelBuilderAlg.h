//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARRAWCHANNELBUILDERALG_H
#define LARROD_LARRAWCHANNELBUILDERALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include "LArElecCalib/ILArPedestal.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h" 
#include "LArCabling/LArOnOffIdMapping.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

//Event classes
class LArDigitContainer;
class LArRawChannelContainer;

class LArOnlineID;

class LArRawChannelBuilderAlg : public AthReentrantAlgorithm {

 public:
  LArRawChannelBuilderAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;


 private:
  //Event input:
  SG::ReadHandleKey<LArDigitContainer> m_digitKey{this, "LArDigitKey","FREE",
      "SG Key of LArDigitContaiiner"};
  //Event output:
  SG::WriteHandleKey<LArRawChannelContainer> m_rawChannelKey{this,"LArRawChannelKey","LArRawChannels",
      "SG key of the LArRawChannelContainer"};

  //Conditions input:
  SG::ReadCondHandleKey<ILArPedestal> m_pedestalKey{this,"PedestalKey","LArPedestal","SG Key of Pedestal conditions object"};
  SG::ReadCondHandleKey<LArADC2MeV> m_adc2MeVKey{this,"ADC2MeVKey","LArADC2MeV","SG Key of ADC2MeV conditions object"};
  SG::ReadCondHandleKey<ILArOFC> m_ofcKey{this,"OFCKey","LArOFC","SG Key of OFC conditions object"};
  SG::ReadCondHandleKey<ILArShape> m_shapeKey{this,"ShapeKey","LArShape","SG Key of Shape conditions object"};


  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<AthenaAttributeList> m_thresholdsKey{this, "DSPThresholdsKey","/LAR/NoiseOfl/DSPThresholds", "SG Key for thresholds to compute time and quality"};


  //Other jobOptions:
  Gaudi::Property<float> m_eCutFortQ{this,"ECutFortQ",256.0,"Time and Quality will be computed only for channels with E above this value"};
  //This flag decides if we compute Q and t for cells with negative energy
  Gaudi::Property<bool> m_absECutFortQ{this,"absECut",true,"Cut on fabs(E) for Q and t computation"};
  Gaudi::Property<bool> m_useShapeDer{this,"useShapeDer",true,"Use shape derivative in Q-factor computation"};
  //This flag decides, wheter to use DB or constant threshold
  Gaudi::Property<bool> m_useDBFortQ{this,"useDB",true,"Use DB for cut on t,Q"};

  //The following matters only in the MC case, when we have a 32 sample shapes
  Gaudi::Property<int> m_firstSample{this,"firstSample",0,"first of the 32 sampels of the MC shape to be used"};

  

  //Identifier helper
  const LArOnlineID* m_onlineId;

};



#endif
