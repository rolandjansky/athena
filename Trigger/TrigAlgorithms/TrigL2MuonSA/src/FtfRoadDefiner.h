/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_FTFROADDEFINER_H
#define  TRIGL2MUONSA_FTFROADDEFINER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

/* #include "RpcData.h" */
/* #include "RpcPatFinder.h" */
#include "MuonRoad.h"
/* #include "MdtRegion.h" */
/* #include "RpcFitResult.h" */
/* #include "BarrelRoadData.h" */
/* #include "RecMuonRoI.h" */

/* #include "IRegionSelector/IRegSelTool.h" */
#include "xAODTracking/TrackParticleContainer.h"

#include "TrkExInterfaces/IExtrapolator.h"

using Amg::Transform3D;

namespace TrigL2MuonSA {

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------

  class FtfRoadDefiner: public AthAlgTool
  {
  public:

    FtfRoadDefiner(const std::string& type,
		   const std::string& name,
		   const IInterface*  parent);

    virtual StatusCode initialize() override;

  public:
    StatusCode defineRoad( const xAOD::TrackParticle* idtrack,
			   TrigL2MuonSA::MuonRoad&    muonRoad);
    /* void setMdtGeometry( const ServiceHandle<IRegSelSvc>& regionSelector, */
    /* 			 const Muon::MuonIdHelperTool* muonIdHelperTool); */
    /* void unsetRpc(bool insideOut, Gaudi::Property<bool>* use_rpc); */
    const Trk::TrackParameters* extTrack( const xAOD::TrackParticle* trk, double R, double halflength, int& extFlag );

  protected:

  private:
    /* const BarrelRoadData*  m_roadData; */
    ToolHandle<Trk::IExtrapolator> m_extrapolator{
      this, "IOExtrapolator", "Trk::Extrapolator/AtlasExtrapolator"};

    /* bool m_use_ftf; */

    /* ToolHandle<IRegSelTool> m_regionSelector; */
    /* const Muon::MuonIdHelperTool* m_muonIdHelperTool; */
  };

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_FTFROADDEFINER_H
