/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_EXTRAPOLATIONTOOL
#define  RPC_EXTRAPOLATIONTOOL

#include "GaudiKernel/AlgTool.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataLink.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonTGRecTools/MuonHolesOnTrackTool.h"

class RpcIdHelper;
class IRegSelSvc;
namespace Muon{
  class MuonIdHelperTool;
  class MuonTGMeasurementTool;
}



struct RpcExtrapolationResults{

  Identifier id;
  const Trk::TrackParameters* params;
};

class RpcExtrapolationTool: public AlgTool{


 public:

  RpcExtrapolationTool(const std::string& type, const std::string& name,
			  const IInterface* parent );
  virtual ~RpcExtrapolationTool();

  static const InterfaceID& interfaceID( );
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void getRpcIntersections(TrackCollection::const_iterator theTrack, std::vector<RpcExtrapolationResults> &theResult);

 private:

  bool m_cosmicMode;
  StoreGateSvc* m_eventStore;
  ActiveStoreSvc* m_activeStore;
  const MuonGM::MuonDetectorManager* m_muonMgr;
  ToolHandle<Muon::MuonIdHelperTool>           m_idHelperTool;
  ToolHandle<Trk::IPropagator>                 m_propagator;
  ToolHandle<Trk::INavigator>                       m_navigator;    
  ToolHandle<Trk::IExtrapolator>      m_extrapolator;
  ToolHandle<Muon::IMuonTGMeasTool>                  m_measTool;
  ToolHandle<Trk::ITrackHoleSearchTool>             m_holesTool;
  const RpcIdHelper* m_rpcIdHelper;
  const DataHandle<Trk::TrackingGeometry> m_trackingGeometry;

};


#endif
