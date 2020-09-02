/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
Class to write useful tag bits for cosmic ray commissioning
(like if there is an ID track in this event...)
Jamie Boyd 21 Jan 2008 (Jamie.Boyd@cern.ch)
*/


#ifndef COMMISSIONREC_RAWINFOSUMMARYFORTAGWRITER
#define COMMISSIONREC_RAWINFOSUMMARYFORTAGWRITER


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ServiceHandle.h"
// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

#include <string>
#include <vector>
#include <map>
#include "TrkSegment/SegmentCollection.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkTrack/TrackCollection.h"
#include "TileEvent/TileCellContainer.h"
#include "TileEvent/MBTSCollisionTime.h"
#include "TagEvent/RawInfoSummaryForTag.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "CommissionEvent/ComTime.h" 

class TileTBID;


class RawInfoSummaryForTagWriter : public AthAlgorithm
{

 public:

  RawInfoSummaryForTagWriter(const std::string& name, ISvcLocator* pSvcLocator);
  ~RawInfoSummaryForTagWriter();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  /** member variables for algorithm properties: */
  //std::string m_sgKeyIDtrack; //!< Name of the Track Collection to read in
  SG::ReadHandleKey<TrackCollection> m_sgKeyIDtrack;
  SG::ReadHandleKey<CaloCellContainer> m_cellContKey;
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusterKey;
  SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_TRT_DriftCircleName;
  SG::ReadHandleKey<MBTSCollisionTime> m_MBTSCollTimeKey;
  SG::ReadHandleKey<SpacePointContainer> m_sctSpacePointName;
  SG::ReadHandleKey<SpacePointContainer> m_pixSpacePointName;
  SG::ReadHandleKey<TileCellContainer> m_mbtsName;
  SG::ReadHandleKey<LArCollisionTime> m_larCollTimeName;
  SG::ReadHandleKey<ComTime> m_trtPhaseName;
  SG::ReadHandleKey<BCM_RDO_Container> m_bcmRDOName{
      this, "BCM_RDOKey", "BCM_RDOs", "SG key for BCM RDOs"};
  SG::WriteHandleKey<RawInfoSummaryForTag> m_RISFTKey;

  bool m_doClusterSums;//!< turn on/off the cluster sum in the tag (as it crashes if Tile is off)
  
  std::string m_mbtsContainerName;
  float m_mbts_threshold;


  std::string m_vertexCollectionName;

  const TileTBID* m_tileTBID;
       
  //coral::AttributeListSpecification* m_attribListSpec;

  // Read handle for conditions object to get the field cache
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
};

#endif
