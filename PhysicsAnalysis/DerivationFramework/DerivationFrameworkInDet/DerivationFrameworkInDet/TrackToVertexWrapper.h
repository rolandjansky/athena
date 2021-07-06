/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToVertexWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKTOVERTEXWRAPPER_H
#define DERIVATIONFRAMEWORK_TRACKTOVERTEXWRAPPER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include "StoreGate/WriteDecorHandleKey.h"

namespace DerivationFramework {

  class TrackToVertexWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      TrackToVertexWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ToolHandle< Trk::ITrackToVertexIPEstimator > m_tool
        {this, "TrackToVertexIPEstimator", "Trk::TrackToVertexIPEstimator", ""};
      Gaudi::Property<std::string> m_sgName
        {this, "DecorationPrefix", "", ""};
      SG::ReadHandleKey<xAOD::TrackParticleContainer> m_containerName
        {this, "ContainerName", "", ""};
      SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey
        {this, "VertexContainer", "PrimaryVertices", "primary vertex container"};

      enum ETrackFloatDecor{kdecnD0Decor,
                            kdecnZ0Decor,
                            kdecnZ0SinThetaDecor,
                            kdecnD0ErrDecor,
                            kdecnZ0ErrDecor,
                            kdecnZ0SinThetaErrDecor,
                            kdecnPVD0ErrDecor,
                            kdecnPVZ0ErrDecor,
                            kdecnPVZ0SinThetaErrDecor,
                            kdecn_b_D0Decor,
                            kdecn_b_Z0Decor,
                            kdecn_b_Z0SinThetaDecor,
                            kdecn_b_D0ErrDecor,
                            kdecn_b_Z0ErrDecor,
                            kdecn_b_Z0SinThetaErrDecor,
                            kdecn_b_PVD0ErrDecor,
                            kdecn_b_PVZ0ErrDecor,
                            kdecn_b_PVZ0SinThetaErrDecor,
                            kNFloatDecor};
      std::vector<SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> > m_trackFloatDecorKeys;
  };
}

#endif // DERIVATIONFRAMEWORK_TRACKTOVERTEXWRAPPER_H
