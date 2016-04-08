/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGTRT_CLUSTERCOLLRAWEVENTTOOL_H
#define TRIGTRT_CLUSTERCOLLRAWEVENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "TRT_Cabling/ITRT_IdMapping.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "InDetRawData/TRT_RDORawData.h"
#include "Identifier/IdContext.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "ByteStreamData/ROBData.h"
#include "TrigTimeAlgs/TrigTimerSvc.h" 

class IOpaqueAddress; 
class StatusCode;
class IAddressCreator;
class TRT_DriftCircleCollection;
class TRT_ID;
class ITRT_RodDecoder;

namespace InDet { class ITRT_DriftCircleTool; }

#include <string>

class TrigTRT_DCCollByteStreamTool: public AthAlgTool
{
public:

  typedef InDet::TRT_DriftCircle RDI;
 
  typedef InDet::TRT_DriftCircleCollection TRT_ClusterCollection;
  typedef InDet::TRT_DriftCircleCollection COLLECTION;

  typedef  std::vector<ROBData> VROBDATA;

  typedef  VROBDATA::const_iterator ROB_ConstIT;

  typedef InDetRawDataCollection< TRT_RDORawData > TRTRawCollection;
    // Constructor
  TrigTRT_DCCollByteStreamTool(const std::string& type,
    const std::string& name, const IInterface* parent);

    // Destructor
  virtual ~TrigTRT_DCCollByteStreamTool();

    // AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
 
  StatusCode convert(VROBDATA& r,const std::vector<IdentifierHash> i);

    // Drift time in bins
  int driftTime(unsigned word) const;

    // Time over threshold in ns for valid digits; zero otherwise
  double timeOverThreshold(unsigned word) const;

    // High level threshold
  bool highLevel(unsigned word) const;

  const std::string trtContainerName() const {return m_trtDriftCircleContainerName; }
  
private:
  ServiceHandle<ITRT_CablingSvc> m_TRT_IdConvTool;
  const TRT_ID* m_trt_id;
  IdContext m_cntx;

  ToolHandle<ITRT_RodDecoder> m_decoder;
  ToolHandle<InDet::ITRT_DriftCircleTool>  m_driftCircleTool;
  bool m_trtBadChannels;
  std::string m_trtDriftCircleContainerName;
  InDet::TRT_DriftCircleContainer* m_trtDriftCircleContainer;
  TRT_RDO_Container* m_rdoContainer;
  std::string m_rdoContainerName;

  bool m_timers; 
#define NTIMERS 14 
  TrigTimer* m_timer[NTIMERS]; 

};

#endif

