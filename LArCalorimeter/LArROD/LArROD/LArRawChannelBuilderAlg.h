//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//Event classes
class LArDigitContainer;
class LArRawChannelContainer;

class LArOnlineID;

class LArRawChannelBuilderAlg : public AthReentrantAlgorithm {

 public:
  LArRawChannelBuilderAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute_r(const EventContext& ctx) const override;
  StatusCode finalize() override;


 private:
  //Event input:
  SG::ReadHandleKey<LArDigitContainer> m_digitKey{this, "LArDigitKey","FREE",
      "SG Key of LArDigitContaiiner"};
  //Event output:
  SG::WriteHandleKey<LArRawChannelContainer> m_rawChannelKey{this,"LArRawChannelKey","LArRawChanels",
      "SG key of the LArRawChannelContainer"};

  //Conditions input:
  SG::ReadCondHandleKey<ILArPedestal> m_pedestalKey{this,"PedestalKey","LArPedestal","SG Key of Pedestal conditions object"};
  SG::ReadCondHandleKey<LArADC2MeV> m_adc2MeVKey{this,"ADC2MeVKey","LArADC2MeV","SG Key of ADC2MeV conditions object"};
  SG::ReadCondHandleKey<ILArOFC> m_ofcKey{this,"OFCKey","LArOFC","SG Key of OFC conditions object"};
  SG::ReadCondHandleKey<ILArShape> m_shapeKey{this,"ShapeKey","LArShape","SG Key of Shape conditions object"};

  //Other jobOptions:
  Gaudi::Property<float> m_eCutForQ{this,"ECutForQ",250.0,"Q will be computed only for channels with E above this value"};
  Gaudi::Property<bool> m_useShapeDer{this,"useShapeDer",true,"Use shape derivative in Q-factor computation"};


  //Identifier helper
  const LArOnlineID* m_onlineId;

};



#endif
