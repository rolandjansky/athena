/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSEEDINGTOOL_TRACKPARAMSESTIMATIONTOOL_H
#define ACTSTRKSEEDINGTOOL_TRACKPARAMSESTIMATIONTOOL_H 1

// ATHENA
#include "ActsTrkToolInterfaces/ITrackParamsEstimationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace ActsTrk {
  
  class TrackParamsEstimationTool :
    public extends<AthAlgTool, ActsTrk::ITrackParamsEstimationTool> {
    
  public:
    TrackParamsEstimationTool(const std::string& type, const std::string& name,
			      const IInterface* parent);
    virtual ~TrackParamsEstimationTool() = default;
    
    virtual StatusCode initialize() override;
    
    // Interface
    virtual 
      std::optional<Acts::BoundTrackParameters>
      estimateTrackParameters(const EventContext& ctx,
			      const ActsTrk::Seed& seed,
			      const Acts::GeometryContext& geoContext,
			      const Acts::MagneticFieldContext& magFieldContext,
			      std::function<const Acts::Surface&(const ActsTrk::Seed&)> retrieveSurface) const override;

    virtual
      std::optional<Acts::BoundTrackParameters>
      estimateTrackParameters(const EventContext& ctx,
			      const ActsTrk::Seed& seed,
			      const Acts::GeometryContext& geoContext,
			      const Acts::Surface& surface,
			      const Acts::BoundSymMatrix& covariance,
			      const Acts::Vector3& bField,
			      double bFieldMin) const override;
    
    // *********************************************************************
    // *********************************************************************

  private:
    // Properties
    Gaudi::Property< double > m_bFieldMin {this, "bFieldMin", 0.1 * Acts::UnitConstants::T,
        "The minimum magnetic field to trigger the track parameters estimation"};
    Gaudi::Property< double > m_sigmaLoc0 {this, "sigmaLoc0", 25 * Acts::UnitConstants::um,
        "Constant term of the loc0 resolution"};
    Gaudi::Property< double > m_sigmaLoc1 {this, "sigmaLoc1", 100 * Acts::UnitConstants::um,
        "Constant term of the loc1 resolution"};
    Gaudi::Property< double > m_sigmaPhi {this, "sigmaPhi", 0.02 * Acts::UnitConstants::degree,
        "Phi angular resolution"};
    Gaudi::Property< double > m_sigmaTheta {this, "sigmaTheta", 0.02 * Acts::UnitConstants::degree,
        "Theta angular resolution"};
    Gaudi::Property< double > m_sigmaQOverP {this, "sigmaQOverP", 0.1 / Acts::UnitConstants::GeV,
        "q/p resolution"};
    Gaudi::Property< double > m_sigmaT0 {this, "sigmaT0", 1400 * Acts::UnitConstants::s,
        "Time resolution"};
    Gaudi::Property< std::vector<double> > m_initialVarInflation {this, "initialVarInflation", {1., 1., 1., 1., 1., 1.},
        "Inflate tracks"};

    /// The track parameters covariance (assumed to be the same for all estimated
    /// track parameters for the moment)
    Acts::BoundSymMatrix m_covariance = Acts::BoundSymMatrix::Zero();
  };
  
} // namespace

#endif

