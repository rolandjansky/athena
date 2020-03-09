/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGONLINESPACEPOINTTOOL_PIXELCLUSTERCACHETOOL_H
#define TRIGONLINESPACEPOINTTOOL_PIXELCLUSTERCACHETOOL_H

#include "ByteStreamData/RawEvent.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "IPixelClusterCacheTool.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelRawDataByteStreamCnv/IPixelRodDecoder.h"
#include "PixelConditionsTools/IPixelByteStreamErrorsTool.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include <vector>
#include <string>

#include "FastPixelClusterization.h"

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
  virtual StatusCode convertBStoClusters(std::vector<const ROBF*>& robFragments,
                                         const std::vector<IdentifierHash>& listOfPixIds, std::vector<int>&, bool);

private:

  InDet::PixelClusterContainer* m_clusterContainer;
  std::string m_containerName;
  FastPixelClusterization m_clusterization;
  //PixelIdMapping *m_IdMapping; 
  const PixelID* m_pixel_id;
  IdContext m_cntx_pix;   
  ToolHandle<IPixelRodDecoder> m_offlineDecoder
  {this, "PixelRodDecoder", "PixelRodDecoder", "Tool for PixelRodDecoder"};
  bool m_useOfflineClustering;
  PixelRDO_Container* m_rdoContainer;
  std::string m_rdoContainerName;
  ToolHandle<IPixelByteStreamErrorsTool> m_bsErrorTool
  {this, "PixelByteStreamErrorsTool", "PixelByteStreamErrorsTool", "Tool for PixelByteStreamError"};

  ToolHandle<InDet::IPixelClusteringTool> m_clusteringTool;
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "PixelLorentzAngleTool", "Tool to retreive Lorentz angle"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

  bool m_doBS;
};

#endif
