/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderDriver
 * @author Rolf Seuster
 * @brief Driver for LArRawChannel building
 * 
 * This driver superseeds LArRawChannelBuilder.h in ATLAS reco. It drives
 * tools for ADC2MeV conversion as well as peak reconstruction using OFCs, parabolic fits, etc...).
 */


#ifndef LARROD_LARRAWCHANNELBUILDERDRIVER_H
#define LARROD_LARRAWCHANNELBUILDERDRIVER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloGain.h"

#include "LArIdentifier/LArOnlineID.h"

//#include "LArRawUtils/LArRoI_Map.h"

//#include "LArRawUtils/LArRawOrdering.h"

#include "LArROD/LArRawChannelBuilderParams.h"
#include "LArROD/ILArRawChannelBuilderToolBase.h"
#include "LArROD/ILArRawChannelBuilderADC2EToolBase.h"
#include "LArROD/ILArRawChannelBuilderPedestalToolBase.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <vector>
#include <string>

class LArDigit;
class LArRawChannelContainer;

class HWIdentifier;
class Identifier;

class LArRawChannelBuilderDriver : public AthAlgorithm
{
 public:
  
  LArRawChannelBuilderDriver (const std::string& name,
			      ISvcLocator* pSvcLocator);

  ~LArRawChannelBuilderDriver() { };
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  //template<typename T>
  //StatusCode regHandleDetectorStore(const T& object, const std::string& key);
  
 private:
  
  float pedestal();
  
  void ADC2energy();
  
  LArRawChannelBuilderParams* m_params;
  
  typedef ToolHandleArray<ILArRawChannelBuilderToolBase>           builderToolVector;
  typedef ToolHandleArray<ILArRawChannelBuilderADC2EToolBase>      adc2eToolVector;
  typedef ToolHandleArray<ILArRawChannelBuilderPedestalToolBase>   pedestalToolVector;
  
  //Services & Tools 
  const LArOnlineID*          m_onlineHelper;
  
  std::string m_DataLocation, m_ChannelContainerName;
  // LArRawChannelContainer*     m_larRawChannelContainer;
  
  builderToolVector           m_buildTools;
  
  adc2eToolVector             m_adc2eTools;
  
  pedestalToolVector          m_pedestalTools;
  
  float                       m_oldPedestal;
  HWIdentifier                m_oldIdentifier;
  
  int                         m_checkSamples;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  
  std::vector<float>          m_ramps;
  
  bool		              m_buildDiscChannel;
  
  int			      m_defaultShiftTimeSamples;
};

#endif
