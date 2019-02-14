/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCellBuilderDriver
 * @author Denis Oliveira Damazio Denis.Oliveira.Damazio@cern.ch
 * @brief Raw channel builder adapted for trigger use
 *
 * The conversion LArDigit -> energy is done on demand
 *   */


#ifndef LARROD_LARCELLBUILDERDRIVER_H
#define LARROD_LARCELLBUILDERDRIVER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloGain.h"

#include "LArIdentifier/LArOnlineID.h"

//#include "LArRawUtils/LArRoI_Map.h"

//#include "LArRawUtils/LArRawOrdering.h"

#include "LArROD/LArRawChannelBuilderParams.h"
#include "LArROD/ILArRawChannelBuilderToolBase.h"
#include "LArROD/ILArRawChannelBuilderADC2EToolBase.h"
#include "LArROD/ILArRawChannelBuilderPedestalToolBase.h"

#include <vector>
#include <string>

class LArDigit;
class LArRawChannelContainer;

class HWIdentifier;
class Identifier;

class LArCellBuilderDriver : public AthAlgTool
{
 public:
  
  LArCellBuilderDriver (const std::string& type,
			      const std::string& name,
			      const IInterface* parent);
  virtual ~LArCellBuilderDriver();
  StatusCode initialize();
  StatusCode finalize();
  void initEventTools();
  bool buildLArCell(const LArDigit* digit, int& energy, int& time, CaloGain::CaloGain& gain,
		    MsgStream* pLog = NULL);
  void finishEventTools();
  
  template<typename T>
    StatusCode retrieveStoreGate(const T*& object, const std::string& key);
  
  template<typename T>
    StatusCode retrieveDetectorStore(const T*& object, const std::string& key);

  template<typename T>
    StatusCode regHandleDetectorStore(const T& object, const std::string& key);
  
  template<typename T>
    StatusCode retrieveTool(T*& object, const std::string& key);
  
 private:
  
  float pedestal(MsgStream* pLog);
  
  void ADC2energy(MsgStream* pLog);

  LArRawChannelBuilderParams* m_params;
  
  typedef ToolHandleArray<ILArRawChannelBuilderToolBase>          builderToolVector;
  typedef ToolHandleArray<ILArRawChannelBuilderADC2EToolBase>     adc2eToolVector;
  typedef ToolHandleArray<ILArRawChannelBuilderPedestalToolBase>  pedestalToolVector;
  
  //Services & Tools 
  IToolSvc*                   m_toolSvc;
  
  const LArOnlineID*          m_onlineHelper;
  
  std::string m_DataLocation, m_ChannelContainerName;
  // LArRawChannelContainer*     m_larRawChannelContainer;
  
  builderToolVector           m_buildTools;
  
  adc2eToolVector             m_adc2eTools;
  
  pedestalToolVector          m_pedestalTools;
  
  float                       m_oldPedestal;
  HWIdentifier                m_oldIdentifier;
  
  int                         m_checkSamples;
  
  //LArRoI_Map*                 m_roiMap;
  //LArRawOrdering              m_larRawOrdering; 
  
  std::vector<float>          m_ramps;
  
  bool		              m_buildDiscChannel ; 
  int			      m_defaultShiftTimeSamples;
};

#include "LArROD/LArCellBuilderDriver.icc"

#endif
