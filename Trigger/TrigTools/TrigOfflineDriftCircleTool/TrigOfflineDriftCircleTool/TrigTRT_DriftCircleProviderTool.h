/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigTRT_DriftCircleProviderTool.h
//		 
// Description: produces LVL2 data: TRT Drift Circles
// 
// -------------------------------
// Cooked from D.Emelyanov's OnlineSpacePointProvider by S.Sivoklokov
////////////////////////////////////////////////////////////////////////////////


#ifndef __TRT_DRIFTCIRCLEPROVIDER_TOOL_H__
#define __TRT_DRIFTCIRCLEPROVIDER_TOOL_H__

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigTRT_DriftCircleProvider.h"
#include "GaudiKernel/ToolHandle.h" 
#include "GaudiKernel/ServiceHandle.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"

#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"

class TrigTRT_DCCollByteStreamTool;
class TRT_ID;
class TrigTimer;
class IRoiDescriptor;

class TrigTRT_DriftCircleProviderTool: public AthAlgTool, virtual public ITrigTRT_DriftCircleProviderTool 
{
 public:
 
  TrigTRT_DriftCircleProviderTool( const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigTRT_DriftCircleProviderTool(){};
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
  StatusCode fillCollections( const IRoiDescriptor &roi);
  StatusCode fillCollections(std::vector<IdentifierHash> DetElemList, std::vector<const ROBF*> v);

  std::string trtContainerName(); 

  inline double getTRTTotalTime(void) const {return (double) m_time_total;}
  inline double getTRTRobProvTime(void) const {return (double) m_time_robprov;}
  inline double getTRTRegSelTime(void) const {return (double) m_time_regsel;}
  inline double getTRTTrtCnvTime(void) const {return (double) m_time_trtcnv;}

  const std::vector<int>* fillTRT_DataErrors();

 private:  // data members

  std::string m_ROBDataProviderSvcName;
  std::string m_regionSelectorToolName;
  std::vector<unsigned int> m_uIntListOfTRT_Robs;
  //std::string m_trtDriftCircleContainerName;

  ServiceHandle<IRegSelSvc> m_regionSelector; 
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<TrigTRT_DCCollByteStreamTool>  m_TRT_DCCollBSTool;
  ServiceHandle<ITRT_ByteStream_ConditionsSvc> m_trtByteStreamErrorService;
                                    
  //  double m_zMin, m_zMax;

  std::vector<int> m_trtDataErrors;

#define SP_NTIMERS 12
  TrigTimer* m_timer[SP_NTIMERS];
  bool m_timers;
  float m_time_total;
  float m_time_robprov;
  float m_time_regsel;
  float m_time_trtcnv;
};
 

#endif

