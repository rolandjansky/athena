//-*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIBTOOLS_LARCOMPLETETOFLAT_H
#define LARCALIBTOOLS_LARCOMPLETETOFLAT_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArSingleFloatP.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"

class LArOnlineID;
class CondAttrListCollection;
class AthenaAttributeList;
class ILArPedestal;
class ILArOFC;
class ILArRamp;
class LArShapeComplete;
class ILArDAC2uA;
class ILAruA2MeV;
class LArDSPThresholdsComplete;

class LArCompleteToFlat: public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  /// Constructor with parameters: 
  LArCompleteToFlat( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArCompleteToFlat(); 

  // Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute() {return StatusCode::SUCCESS;}
  StatusCode  finalize() {return StatusCode::SUCCESS;}
  virtual StatusCode  stop();

 private: 
  /// Default constructor: 
  LArCompleteToFlat();


  CondAttrListCollection* singleFloatFlat(const char* blobName, const LArConditionsContainer<LArSingleFloatP>* input, 
					  const std::string& outputName, const unsigned nGain, const bool withFCAL=true);
  CondAttrListCollection* DAC2uAFlat(const ILArDAC2uA* input, const std::string& outputName);
  CondAttrListCollection* uA2MeVFlat(const ILAruA2MeV* input, const std::string& outputName);
  CondAttrListCollection* pedestalFlat(const ILArPedestal* input, const std::string& outputName);
  CondAttrListCollection* rampFlat(const ILArRamp* input, const std::string& outputName);
  CondAttrListCollection* ofcFlat(const ILArOFC* input, const std::string& outputName);
  CondAttrListCollection* shapeFlat(const LArShapeComplete* input, const std::string& outputName);
  AthenaAttributeList* DSPThresholdsFlat(const LArDSPThresholdsComplete* input, const std::string& outputName);


  void errIfConnected(const HWIdentifier chid, const int gain, const char* objName, const char* message=0) const;

  unsigned m_hashMax;
  const LArOnlineID* m_onlineID;


  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  ///InputSGKeys
  std::string m_uA2MeVInput;
  std::string m_DAC2uAInput;
  std::string m_HVScaleCorrInput;
  std::string m_PedestalInput;
  std::string m_RampInput;
  std::string m_MphysOverMcalInput;
  std::string m_OFCInput;
  std::string m_ShapeInput;
  std::string m_DSPThresholdsInput;

  // DSPThreshold set name
  std::string m_nameOfSet;

  bool m_forceStop;
  bool m_fakeEMBPSLowGain;
}; 

#endif //> !LARCALIBTOOLS_LARCOMPLETETOFLAT_H
