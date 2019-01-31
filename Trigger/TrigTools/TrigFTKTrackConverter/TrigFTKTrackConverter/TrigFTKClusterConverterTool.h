/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//implementation

#ifndef TRIGFTKTRACKCONVERTER_TRIGFTKCLUSTERCONVERTERTOOL_H
#define TRIGFTKTRACKCONVERTER_TRIGFTKCLUSTERCONVERTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigFTKToolInterfaces/ITrigFTKClusterConverterTool.h"

#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrigFTKSim/FTKTrack.h"
#include "TrkFitterInterfaces/ITrackFitter.h" 
#include "TrkFitterUtils/FitterTypes.h" 

#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <mutex>
#include <vector>

class StoreGateSvc;

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class Identifier;
class IdentifierHash;

namespace InDetDD {
  class PixelDetectorManager;
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
  void createSCT_Truth(Identifier id, const MultiTruth& mt, PRD_MultiTruthCollection *sctTruth, const McEventCollection*  mcEventCollection, StoreGateSvc* evtStore, std::string mcTruthName);
  void createPixelTruth(Identifier id, const MultiTruth& mt, PRD_MultiTruthCollection *pxlTruth, const McEventCollection*  mcEventCollection, StoreGateSvc* evtStore, std::string mcTruthName);

  InDet::SCT_ClusterCollection*  getCollection(InDet::SCT_ClusterContainer*, IdentifierHash); 
  InDet::PixelClusterCollection*  getCollection(InDet::PixelClusterContainer*, IdentifierHash); 
  StatusCode getMcTruthCollections(  StoreGateSvc* evtStore, const McEventCollection*  mcEventCollection,  std::string ftkPixelTruthName,   std::string ftkSctTruthName, std::string mcTruthName);

private:

  bool m_usePixelCalibSvc;

  StoreGateSvc* m_evtStore;

  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  
  const InDetDD::PixelDetectorManager* m_pixelManager;

  ToolHandle<ISiLorentzAngleTool> m_pixelLorentzAngleTool{this, "PixelLorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retreive Lorentz angle of Pixel"};
  ToolHandle<ISiLorentzAngleTool> m_sctLorentzAngleTool{this, "SCTLorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle of SCT"};
  ToolHandle<Trk::ITrackFitter> m_trackFitter;

  SG::ReadCondHandleKey<PixelCalib::PixelOfflineCalibData> m_clusterErrorKey{this, "PixelOfflineCalibData", "PixelOfflineCalibData", "Output key of pixel cluster"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  bool m_doFit;
  bool m_doTruth;
  std::string m_ftkPixelTruthName;
  std::string m_ftkSctTruthName;
  std::string m_mcTruthName;

  PRD_MultiTruthCollection* m_ftkPixelTruth;
  PRD_MultiTruthCollection* m_ftkSctTruth;
  const AtlasDetectorID* m_idHelper;
  bool m_collectionsReady;

  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cacheSCTElements;
  // Pointer of InDetDD::SiDetectorElementCollection
  mutable Gaudi::Hive::ContextSpecificPtr<const InDetDD::SiDetectorElementCollection> m_SCTDetectorElements;

  const InDetDD::SiDetectorElement* getSCTDetectorElement(const IdentifierHash hash) const;
};

#endif
