/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKDetectorTool_h
#define FTKDetectorTool_h

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigFTKSim/FTKDetectorToolI.h"
#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRawHit.h"
#include "TrigFTKSim/FTKHit.h"
#include "TrigFTKSim/FTKRegionMap.h"

#include <string>
#include <vector>
#include <list>
#include <set>
#include <fstream>

#include "GaudiKernel/IPartPropSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/DataHandle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

class AtlasDetectorID;
class ITruthParameters;
class TruthSelector;
class PixelID;
class SCT_ID;
class EventID;

namespace HepPDT {
  class ParticleDataTable;
}
namespace HepMC {
  class GenParticle;
}


class FTKDetectorTool :  virtual public FTKDetectorToolI, 
			 public AthAlgTool   {
  
 private: 

  mutable MsgStream m_log;
  
  std::string m_dumppath;  
  
  const InDet::SiClusterContainer*  m_pixelContainer;
  const InDet::SiClusterContainer*  m_sctContainer;
  
  ToolHandle<IInDetConditionsTool>        m_pixelCondSummaryTool; // tool to retrieve pixel conditions db 
  ToolHandle<IInDetConditionsTool>        m_sctCondSummaryTool{this, "SctSummaryTool",
      "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions Summary"}; // tool to retrieve SCT conditions db

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  
  const PixelID*   m_pixelId;
  const SCT_ID*    m_sctId;
  
  std::string    m_pixelClustersName;
  std::string    m_sctClustersName;
  std::string    m_FTK_BadModuleMapPath;
  std::string    m_ATLAS_BadModuleMapPath;
  std::string   m_FTK_ModuleIDMapPath;
  bool      m_doBadModuleMap;

  std::string m_pmap_path;
  FTKPlaneMap *m_pmap;

  std::vector< std::set<int> > m_WCSS;
  std::list< FTKHit >  *m_bad_module_map;
  
  // variable to create the module ID map
  std::string m_global2local_path;
  std::string m_sram_path_pix;
  std::string m_sram_path_sct;
  std::string m_rmap_path;
  FTKRegionMap *m_rmap;
  bool m_dumpAllModules;
public:
  
  
private:
  
  void outputTxt_badmodule();

public:
  FTKDetectorTool(const std::string&,const std::string&,const IInterface*);
  virtual ~FTKDetectorTool();
  
  StatusCode initialize();
  StatusCode finalize();
  
  virtual void dumpDeadModuleSummary()  ;
  virtual void makeBadModuleMap()  ;
  
  virtual void dumpModuleIDMap();

  virtual std::set<int> *getWildcardList(int plane) { return &m_WCSS[plane]; }

  virtual void dumpGlobalToLocalModuleMap();

  virtual void dumpIDMap();

  virtual void dumpModulePositions();
};

#endif // FTKDetectorTool_h
