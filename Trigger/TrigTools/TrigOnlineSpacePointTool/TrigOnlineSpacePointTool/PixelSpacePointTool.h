/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelSpacePointTool_H
#define PixelSpacePointTool_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigOnlineSpacePointTool/PixelGCBuilder.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>

class TrigSiSpacePoint;
class PixelID;
class TrigTimer;
class IBeamCondSvc;
class ITrigL2LayerNumberTool;

class PixelSpacePointTool : public AthAlgTool {
public:
  typedef std::vector<const InDet::PixelClusterCollection*> ClusterCollectionData;
  typedef std::vector<TrigSiSpacePoint*> SiSpacePointData;

  // Constructor 
  PixelSpacePointTool( const std::string& type, 
                       const std::string& name, 
		       const IInterface* parent );

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();

  virtual StatusCode finalize();
 
  StatusCode fillCollections(ClusterCollectionData& clusterCollData);

private:

  PixelGCBuilder* m_builder;
  const PixelID* m_id_pixel;
  IdContext m_cntx_pixel;
  TrigSiSpacePointContainer* m_spacepointContainer;
  std::string m_spacepointContainerName;

  double m_xVertex;
  double m_yVertex;
  double m_zVertex;

  double m_xCenter;
  double m_yCenter;

  IBeamCondSvc* m_iBeamCondSvc; 
  bool m_useBeamSpot;
  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;

#define PIXSP_NTIMERS 5
  TrigTimer* m_timer[PIXSP_NTIMERS];
  bool m_timers;
};


#endif 
