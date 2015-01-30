/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PIXEL_CLUSTER_CACHE_TOOL_H__
#define __PIXEL_CLUSTER_CACHE_TOOL_H__

#include "ByteStreamData/RawEvent.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "TrigOnlineSpacePointTool/FastPixelClusterization.h"
#include "TrigOnlineSpacePointTool/IPixelClusterCacheTool.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "PixelRawDataByteStreamCnv/IPixelRodDecoder.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <string>

class FastPixelClusterization;


typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ;

class PixelClusterCacheTool : public AthAlgTool, virtual public IPixelClusterCacheTool {
public:

  // Constructor 
  PixelClusterCacheTool( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent );
  virtual ~PixelClusterCacheTool(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Convert clusters to space points
  virtual StatusCode m_convertBStoClusters(std::vector<const ROBF*>& robFragments,
					   const std::vector<IdentifierHash> listOfPixIds, std::vector<int>&, bool);

private:

  InDet::PixelClusterContainer* m_clusterContainer;
  std::string m_containerName;
  FastPixelClusterization m_clusterization;
  //PixelIdMapping *m_IdMapping; 
  ServiceHandle<IPixelCablingSvc> m_IdMapping;
  const InDetDD::PixelDetectorManager* p_indet_mgr;
  const PixelID* m_pixel_id;
  IdContext m_cntx_pix;   
  ToolHandle<IPixelRodDecoder> m_offlineDecoder;
  bool m_useOfflineClustering;
  PixelRDO_Container* m_rdoContainer;
  std::string m_rdoContainerName;
  ServiceHandle<IPixelByteStreamErrorsSvc>   m_bsErrorSvc;
  ToolHandle<InDet::IPixelClusteringTool> m_clusteringTool;

  bool m_doBS;
};

#endif
