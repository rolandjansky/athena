/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARRAWCHANNELBUILDERITERALG_H
#define LARROD_LARRAWCHANNELBUILDERITERALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include "LArElecCalib/ILArPedestal.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "LArRawEvent/LArOFIterResultsContainer.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h" 
#include "LArCabling/LArOnOffIdMapping.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

//Event classes
class LArDigitContainer;
class LArRawChannelContainer;

class LArOnlineID;

class LArRawChannelBuilderIterAlg : public AthReentrantAlgorithm {

 public:
  LArRawChannelBuilderIterAlg(const std::string& name, ISvcLocator* pSvcLocator);

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
  SG::ReadCondHandleKey<LArDSPThresholdsComplete> m_run1DSPThresholdsKey{this, "Run1DSPThresholdsKey","", "SG Key for thresholds to compute time and quality, run 1"};
  SG::ReadCondHandleKey<AthenaAttributeList> m_run2DSPThresholdsKey{this, "Run2DSPThresholdsKey","", "SG Key for thresholds to compute time and quality, run 2"};


  //Other jobOptions:
  Gaudi::Property<float> m_eCutFortQ{this,"ECutFortQ",256.0,"Time and Quality will be computed only for channels with E above this value"};
  //This flag decides if we compute Q and t for cells with negative energy
  Gaudi::Property<bool> m_absECutFortQ{this,"absECut",true,"Cut on fabs(E) for Q and t computation"};
  Gaudi::Property<bool> m_useShapeDer{this,"useShapeDer",true,"Use shape derivative in Q-factor computation"};
  //This flag decides, wheter to use DB or constant threshold
  Gaudi::Property<bool> m_useDBFortQ{this,"useDB",true,"Use DB for cut on t,Q"};

  //The following matters only in the MC case, when we have a 32 sample shapes
  Gaudi::Property<int> m_firstSample{this,"firstSample",0,"first of the 32 sampels of the MC shape to be used"};

  // flags for iterations
  Gaudi::Property<unsigned short> m_AdcMax{this, "ADCMax", 4095, "Saturation cut"};
  Gaudi::Property<bool> m_skipSaturatedCells{this, "Skip", false, "reconstruct saturated cells"};
  Gaudi::Property<float> m_defaultPhase{this, "defaultPhase", 0, "starting phase for iterations"};
  Gaudi::Property<unsigned short> m_minADCforIter{this, "minADCforIter", 30}; 
  Gaudi::Property<float> m_minADCforIterInSigma{this, "minADCforIterInSigma", -1};
  Gaudi::Property<unsigned short> m_minSample{this, "minSample", 0};
  Gaudi::Property<unsigned short> m_maxSample{this, "maxSample",31};
  Gaudi::Property<unsigned short> m_nIterProp{"nIterations", 10};
  Gaudi::Property<int>  m_defaultShiftTimeSamples{this, "DefaultShiftTimeSample", 0};
  Gaudi::Property<bool> m_storeTiming{this, "StoreTiming", false, "Create container of LArOFIterResults in StoreGate"};
  Gaudi::Property<bool> m_forceHighGain{this, "forceHighGain", false, "Force use of high gain for all shapes and OFC (default=false)"};

  SG::WriteHandleKey<LArOFIterResultsContainer> m_timingContKey{this, "TimingContainerKey", "LArOFIterResult", "Key of the LArOFIterResultsContainer in StoreGate"};

  //Identifier helper
  const LArOnlineID* m_onlineId;

  // iterative peak computation
  LArOFIterResults peak(const std::vector<float>& samples, // raw data after pedestal subtraction
                        const HWIdentifier chID,           // online channel id
                        const CaloGain::CaloGain gain,     // gain 
                        const float delayIn,               // initial delay for Shape and OFC  
                        const ILArOFC* ofcs,               // OFC's 
                        const ILArShape* shapes,           // shapes
                        const unsigned  nIter=0,          // number of iteration 
                        const unsigned npeak=2,            // initial peak position.                  
                        unsigned peak_low=2,         // lower limit for peak position
                        unsigned peak_high=2        // upper limit for peak position
                        ) const;

};



#endif
