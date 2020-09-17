/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_FTFROADDEFINER_H
#define  TRIGL2MUONSA_FTFROADDEFINER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRoad.h"

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
    const Trk::TrackParameters* extTrack( const xAOD::TrackParticle* trk, double R, double halflength, int& extFlag );

  protected:

  private:
    ToolHandle<Trk::IExtrapolator> m_extrapolator{
      this, "IOExtrapolator", "Trk::Extrapolator/AtlasExtrapolator"};

  };

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
}

#endif // TRIGL2MUONSA_FTFROADDEFINER_H
