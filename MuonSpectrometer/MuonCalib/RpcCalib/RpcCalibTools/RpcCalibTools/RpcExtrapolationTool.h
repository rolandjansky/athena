/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_EXTRAPOLATIONTOOL
#define  RPC_EXTRAPOLATIONTOOL

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "AthLinks/DataLink.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonTGRecTools/MuonHolesOnTrackTool.h"

namespace Muon{
  class MuonTGMeasurementTool;
}

struct RpcExtrapolationResults{

  Identifier id;
  const Trk::TrackParameters* params;
};

class RpcExtrapolationTool: public AthAlgTool{


 public:

  RpcExtrapolationTool(const std::string& type, const std::string& name,
			  const IInterface* parent );
  virtual ~RpcExtrapolationTool() {};

  static const InterfaceID& interfaceID( );
  
  virtual StatusCode initialize();

  void getRpcIntersections(TrackCollection::const_iterator theTrack, std::vector<RpcExtrapolationResults> &theResult);

 private:

  bool m_cosmicMode;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<Trk::IPropagator>                 m_propagator;
  ToolHandle<Trk::INavigator>                       m_navigator;    
  ToolHandle<Trk::IExtrapolator>      m_extrapolator;
  ToolHandle<Muon::IMuonTGMeasTool>                  m_measTool;
  ToolHandle<Trk::ITrackHoleSearchTool>             m_holesTool;
  const DataHandle<Trk::TrackingGeometry> m_trackingGeometry;

};


#endif
