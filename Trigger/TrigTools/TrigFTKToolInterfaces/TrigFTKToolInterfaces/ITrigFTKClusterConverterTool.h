/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//abstract interface

#ifndef __ITRIG_FTK_CLUSTER_CONVERTER_TOOL_H__
#define __ITRIG_FTK_CLUSTER_CONVERTER_TOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include <vector>


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

class FTKHit;
class IdentifierHash;
class MultiTruth;
class PRD_MultiTruthCollection;
class McEventCollection;

static const InterfaceID IID_ITrigFTKClusterConverterTool("ITrigFTKClusterConverterTool",1,0);

/** @class ITrigFTKClusterConverterTool

    provides the abstract interface for FTK cluster conversion into Pixel/SCT clusters

    @author Z.Jiang, L.Tompkins  <http://consult.cern.ch/xwho>
*/

class ITrigFTKClusterConverterTool : virtual public IAlgTool { 

 public:
  /** other standard AlgTool methods */

  static const InterfaceID& interfaceID ()   //!< the Tool's interface
  {  return IID_ITrigFTKClusterConverterTool; }  	

  virtual InDet::SCT_Cluster*   createSCT_Cluster(IdentifierHash , float , int) = 0;
  virtual InDet::PixelCluster*  createPixelCluster(IdentifierHash , float , float, float, float, float) = 0;
  virtual InDet::PixelClusterCollection* getCollection(InDet::PixelClusterContainer*, IdentifierHash) =0; 
  virtual InDet::SCT_ClusterCollection*  getCollection(InDet::SCT_ClusterContainer*, IdentifierHash) =0; 

  virtual StatusCode getMcTruthCollections( StoreGateSvc*, const McEventCollection*,  std::string,  std::string, std::string ) = 0;
  virtual void createSCT_Truth(Identifier, const MultiTruth& , PRD_MultiTruthCollection*, const McEventCollection*, StoreGateSvc*,  std::string)=0;
  virtual void createPixelTruth(Identifier, const MultiTruth& , PRD_MultiTruthCollection*, const McEventCollection*, StoreGateSvc*, std::string)=0;

};

#endif
