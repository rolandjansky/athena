/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//implementation

#ifndef __TRIG_FTK_CLUSTER_CONVERTER_TOOL_H__
#define __TRIG_FTK_CLUSTER_CONVERTER_TOOL_H__

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigFTKSim/FTKTrack.h"
#include "TrigFTKToolInterfaces/ITrigFTKClusterConverterTool.h"

#include "TrkFitterInterfaces/ITrackFitter.h" 
#include "TrkFitterUtils/FitterTypes.h" 
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

class StoreGateSvc;

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class Identifier;
class IdentifierHash;

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace InDet {
  class PixelCluster;
  class SCT_Cluster;
}

class PRD_MultiTruthCollection;
class McEventCollection;


class TrigFTKClusterConverterTool : public AthAlgTool, virtual public ITrigFTKClusterConverterTool
{
 public:
  TrigFTKClusterConverterTool( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigFTKClusterConverterTool(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

 protected:

  InDet::SCT_Cluster*  createSCT_Cluster(IdentifierHash , float , int);
  InDet::PixelCluster* createPixelCluster(IdentifierHash , float , float , float , float , float );
  StatusCode getMcTruthCollections();
  void createSCT_Truth(Identifier id, const MultiTruth& mt, PRD_MultiTruthCollection *sctTruth, const McEventCollection*  m_mcEventCollection, StoreGateSvc* m_evtStore, std::string m_mcTruthName);
  void createPixelTruth(Identifier id, const MultiTruth& mt, PRD_MultiTruthCollection *pxlTruth, const McEventCollection*  m_mcEventCollection, StoreGateSvc* m_evtStore, std::string m_mcTruthName);

  InDet::SCT_ClusterCollection*  getCollection(InDet::SCT_ClusterContainer*, IdentifierHash); 
  InDet::PixelClusterCollection*  getCollection(InDet::PixelClusterContainer*, IdentifierHash); 
  StatusCode getMcTruthCollections(  StoreGateSvc* m_evtStore, const McEventCollection*  m_mcEventCollection,  std::string m_ftkPixelTruthName,   std::string m_ftkSctTruthName, std::string m_mcTruthName);

private:

  ServiceHandle<IPixelOfflineCalibSvc> m_offlineCalibSvc;

  bool m_usePixelCalibSvc;

  StoreGateSvc* m_evtStore;

  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  
  const InDetDD::PixelDetectorManager* m_pixelManager;
  const InDetDD::SCT_DetectorManager* m_SCT_Manager;

  ToolHandle<Trk::ITrackFitter> m_trackFitter;
  bool m_doFit;
  bool m_doTruth;
  std::string m_ftkPixelTruthName;
  std::string m_ftkSctTruthName;
  std::string m_mcTruthName;

  PRD_MultiTruthCollection* m_ftkPixelTruth;
  PRD_MultiTruthCollection* m_ftkSctTruth;
  const AtlasDetectorID* m_idHelper;
  bool m_collectionsReady;
};

#endif
