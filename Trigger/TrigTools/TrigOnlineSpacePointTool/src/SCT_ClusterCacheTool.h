/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGONLINESPACEPOINTTOOL_SCTCLUSTERCACHETOOL_H
#define TRIGONLINESPACEPOINTTOOL_SCTCLUSTERCACHETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SiClusterizationTool/ISCT_ClusteringTool.h"

#include "ByteStreamData/RawEvent.h"
#include "Identifier/IdContext.h" 
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "ISCT_ClusterCacheTool.h"
#include "FastSCT_Clusterization.h"
#include "FastSCT_RodDecoder.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class TrigTimer;

namespace InDetDD {
  class SCT_DetectorManager;
}

typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ;

class SCT_ClusterCacheTool : public AthAlgTool, virtual public ISCT_ClusterCacheTool  {
public:

  // Constructor 
  SCT_ClusterCacheTool( const std::string& type, 
			const std::string& name, 
			const IInterface* parent );

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Convert clusters to space points
  virtual StatusCode convertBStoClusters(std::vector<const ROBF*>& robFragments,
                                         const std::vector<IdentifierHash>& listOfSCTIds,
                                         std::vector<int>&, bool);

private:

  InDet::SCT_ClusterContainer* m_clusterContainer;
  FastSCT_Clusterization m_clusterization;

  ToolHandle<ISCT_CablingTool> m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};

  const InDetDD::SCT_DetectorManager * m_indet_mgr;
  const SCT_ID* m_sct_id;
  std::string m_containerName;
  IdContext m_cntx_sct;

  ToolHandle<FastSCT_RodDecoder> m_decoder{this, "FastSCT_RodDecoder", "FastSCT_RodDecoder", "Fast SCT ROD decoder"};
  ToolHandle<ISCT_RodDecoder> m_offlineDecoder;
  bool m_useOfflineDecoder;
  bool m_useOfflineClustering;
  ToolHandle<InDet::ISCT_ClusteringTool> m_clusteringTool;
  SCT_RDO_Container* m_rdoContainer;
  std::string m_rdoContainerName;
  std::string m_bsErrContainerName;
  bool m_doBS;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

#define SCT_CL_CACHE_NTIMERS 5
  TrigTimer* m_timer[SCT_CL_CACHE_NTIMERS];
  bool m_timers;
};

#endif
