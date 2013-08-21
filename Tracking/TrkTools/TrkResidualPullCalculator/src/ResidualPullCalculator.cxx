/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ResidualPullCalculator.cxx
//   Source file for class ResidualPullCalculator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "ResidualPullCalculator.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "EventPrimitives/EventPrimitives.h"

//////////////////////////////
/// constructor
//////////////////////////////
Trk::ResidualPullCalculator::ResidualPullCalculator(const std::string& type, const std::string& name, const IInterface* parent)
        : AthAlgTool(type,name,parent),
        m_SCTresidualTool("InDet::SCT_ResidualPullCalculator/SCT_ResidualPullCalculator"),
        m_RPCresidualTool(""),
        m_TGCresidualTool(""),
        m_idHelper(0) {
    declareInterface<IResidualPullCalculator>(this);
    declareProperty("ResidualPullCalculatorForSCT", m_SCTresidualTool,  "Tool to calculate residuals and pulls in the SCT (including module rotation)");
    declareProperty("ResidualPullCalculatorForRPC", m_RPCresidualTool,  "Tool to calculate residuals and pulls in the RPC (including phi/eta detection)");
    declareProperty("ResidualPullCalculatorForTGC", m_TGCresidualTool,  "Tool to calculate residuals and pulls in the SCT (including module rotation)");

}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode Trk::ResidualPullCalculator::initialize() {
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  // ---------------------------------------
  //Set up ATLAS ID helper to be able to identify the RIO's det-subsystem.
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }
  // ----------------------------------
  // get the SCT residual/pull calculator
  if ( ! m_SCTresidualTool.empty() ) {
    sc = m_SCTresidualTool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Could not get res. calculator for SCT measurements: "<< m_SCTresidualTool);
      return sc;
    }
  } else {
    ATH_MSG_DEBUG ("No residual calculator for SCT given, will ignore SCT measurements!");
  }

  // ----------------------------------
  // get the RPC residual/pull calculator
  if ( ! m_RPCresidualTool.empty() ) {
    sc = m_RPCresidualTool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Could not get res. calculator for RPC measurements: "<< m_RPCresidualTool);
      return sc;
    }
  } else {
    ATH_MSG_DEBUG ("No residual calculator for RPC given, will ignore RPC measurements!");
  }

  // ----------------------------------
  // get the TGC residual/pull calculator
  if ( ! m_TGCresidualTool.empty() ) {
    sc = m_TGCresidualTool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Could not get res. calculator for TGC measurements: "<< m_TGCresidualTool);
      return sc;
    }
  } else {
    ATH_MSG_DEBUG ("No residual calculator for TGC given, will ignore TGC measurements!");
  }
  
  return StatusCode::SUCCESS;
}

StatusCode Trk::ResidualPullCalculator::finalize() {
  ATH_MSG_INFO ("starting finalize() in " << name());
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//  calc residuals with determination of detector/MBase type
////////////////////////////////////////////////////////////////////////////////
void Trk::ResidualPullCalculator::residuals(
    std::vector<double>& residuals,
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType resType,
    const Trk::TrackState::MeasurementType detType) const {

  if (residuals.size()<5) residuals.resize(5);
  residuals[0]=residuals[1]=residuals[2]=residuals[3]=residuals[4]=-999;
  Trk::TrackState::MeasurementType measType = detType;
  if (detType == Trk::TrackState::unidentified) {
    Trk::MeasurementTypeID helper = MeasurementTypeID(m_idHelper);
    measType = helper.defineType(measurement);
  }
  switch(measType) {
  case Trk::TrackState::Pixel:
    ATH_MSG_VERBOSE ("Pixel dim calculation ");
    // calc residual and pull for the second coordinate, first coord postponed to 1-dim case
    residuals[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
    residuals[Trk::loc2] = measurement->localParameters()[Trk::loc2] - trkPar->parameters()[Trk::loc2];
    break;
  case Trk::TrackState::TRT:
  case Trk::TrackState::MDT:
  case Trk::TrackState::CSC:
  case Trk::TrackState::MM:
    ATH_MSG_VERBOSE ("One dim calculation ");
    // 1-dim measurement
    // calc residual and pull for the first coordinate
    residuals[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
    break;
  case Trk::TrackState::STGC:
    ATH_MSG_VERBOSE ("One dim calculation ");
    // 1-dim measurement
    // calc residual and pull for the first coordinate
    if( measurement->localParameters().contains(Trk::loc1) )
      residuals[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
    else 
      residuals[Trk::loc2] = measurement->localParameters()[Trk::loc2] - trkPar->parameters()[Trk::loc2];
    break;
  case Trk::TrackState::SCT:
    // special case, has to be handed down to the SCT tool
    if ( ! m_SCTresidualTool.empty() ) {
      ATH_MSG_VERBOSE ("Calling SCT tool ");
      m_SCTresidualTool->residuals(residuals,measurement, trkPar, resType, Trk::TrackState::SCT);
    } else {
      ATH_MSG_WARNING ("No SCT ResidualPullCalculator given, cannot calculate residual and pull for SCT measurement!");
      return;
    }
    break;
  case Trk::TrackState::RPC:
    // special case, has to be handed down to the RPC tool
    if ( ! m_RPCresidualTool.empty() ) {
      ATH_MSG_VERBOSE ("Calling RPC tool ");
      m_RPCresidualTool->residuals(residuals,measurement, trkPar, resType, Trk::TrackState::RPC);
    } else {
      ATH_MSG_WARNING ("No RPC ResidualPullCalculator given, cannot calculate residual and pull for RPC measurement!");
      return;
    }
    break;
  case Trk::TrackState::TGC:
    // special case, has to be handed down to the TGC tool
    if ( ! m_TGCresidualTool.empty() ) {
      ATH_MSG_VERBOSE ("Calling TGC tool ");  
      m_TGCresidualTool->residuals(residuals,measurement, trkPar, resType, Trk::TrackState::TGC);
    } else {
      ATH_MSG_WARNING ("No TGC ResidualPullCalculator given, cannot calculate residual and pull for TGC measurement!");
      return;
    }
    break;
  case Trk::TrackState::Segment:
  case Trk::TrackState::Pseudo:
  case Trk::TrackState::Vertex:
  case Trk::TrackState::SpacePoint:
  default:
    ATH_MSG_VERBOSE ("Bit-key calculation ");
    ParamDefsAccessor PDA;
 
    // PMs, Segments etc: use LocalParameters bit-key scheme
    for (unsigned int i=0; i<5; ++i) {
      Trk::ParamDefs iPar = PDA.pardef[i];
      if (measurement->localParameters().contains(iPar)) {
        residuals[i] = measurement->localParameters()[iPar]
          - trkPar->parameters()[iPar];
      }
    }
    break;
  }

}

////////////////////////////////////////////////////////////////////////////////
//  calc residual and pull with determination of detector/MBase type
////////////////////////////////////////////////////////////////////////////////
const Trk::ResidualPull* Trk::ResidualPullCalculator::residualPull(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType resType,
    const Trk::TrackState::MeasurementType detType) const {

  if (!measurement || !trkPar) return 0;

  // if no covariance for the track parameters is given the pull calculation is not valid
  bool pullIsValid = trkPar->covariance();

  Trk::TrackState::MeasurementType measType = detType;
  if (detType == Trk::TrackState::unidentified) {
    Trk::MeasurementTypeID helper = MeasurementTypeID(m_idHelper);
    measType = helper.defineType(measurement);
  }
  unsigned int dimOfLocPars = (unsigned int)measurement->localParameters().dimension();
  std::vector<double> residual(dimOfLocPars);
  std::vector<double> pull(dimOfLocPars);

  // has to live here as it does not compile if code is switch statement
  ParamDefsAccessor PDA;
  unsigned int iColRow=0;

  ATH_MSG_VERBOSE ("Calculating residual for type " << measType << " dimension " << dimOfLocPars);

  // do the calculations for the different detector types
  switch(measType) {
  case Trk::TrackState::Pixel:
    ATH_MSG_VERBOSE ("Pixel dim calculation ");
    // calc residual and pull for the second coordinate, first coord postponed to 1-dim case
    residual[Trk::loc2] = measurement->localParameters()[Trk::loc2] - trkPar->parameters()[Trk::loc2];
    if (pullIsValid) {
      pull[Trk::loc2] = calcPull(residual[Trk::loc2],
				 measurement->localCovariance()(Trk::loc2,Trk::loc2),
				 (*trkPar->covariance())(Trk::loc2,Trk::loc2),
				 resType);
    } else {
      pull[Trk::loc2] = calcPull(residual[Trk::loc2],
				 measurement->localCovariance()(Trk::loc2,Trk::loc2),
				 0,
				 resType);
    }
    // do not break here, but also fill the first coordinate...
  case Trk::TrackState::TRT:
  case Trk::TrackState::MDT:
  case Trk::TrackState::CSC:
  case Trk::TrackState::STGC:
  case Trk::TrackState::MM:
    ATH_MSG_VERBOSE ("One dim calculation ");
    // 1-dim measurement
    // calc residual and pull for the first coordinate
    residual[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
    if (pullIsValid) {
      pull[Trk::loc1] = calcPull(residual[Trk::loc1],
				 measurement->localCovariance()(Trk::loc1,Trk::loc1),
				 (*trkPar->covariance())(Trk::loc1,Trk::loc1),
				 resType);
    } else {
      pull[Trk::loc1] = calcPull(residual[Trk::loc1],
				 measurement->localCovariance()(Trk::loc1,Trk::loc1),
				 0,
				 resType);
    }
    break;
  case Trk::TrackState::SCT:
    // special case, has to be handed down to the SCT tool
    if ( ! m_SCTresidualTool.empty() ) {
      ATH_MSG_VERBOSE ("Calling SCT tool ");
      return m_SCTresidualTool->residualPull(measurement, trkPar, resType, Trk::TrackState::SCT);
    } else {
      ATH_MSG_WARNING ("No SCT ResidualPullCalculator given, cannot calculate residual and pull for SCT measurement!");
      return 0;
    }
    break;
  case Trk::TrackState::RPC:
    // special case, has to be handed down to the RPC tool
    if ( ! m_RPCresidualTool.empty() ) {
      ATH_MSG_VERBOSE ("Calling RPC tool ");
      return m_RPCresidualTool->residualPull(measurement, trkPar, resType, Trk::TrackState::RPC);
    } else {
      ATH_MSG_WARNING ("No RPC ResidualPullCalculator given, cannot calculate residual and pull for RPC measurement!");
      return 0;
    }
    break;
  case Trk::TrackState::TGC:
    // special case, has to be handed down to the TGC tool
    if ( ! m_TGCresidualTool.empty() ) {
      ATH_MSG_VERBOSE ("Calling TGC tool ");
      return m_TGCresidualTool->residualPull(measurement, trkPar, resType, Trk::TrackState::TGC);
    } else {
      ATH_MSG_WARNING ("No TGC ResidualPullCalculator given, cannot calculate residual and pull for TGC measurement!");
      return 0;
    }
    break;
  case Trk::TrackState::Segment:
  case Trk::TrackState::Pseudo:
  case Trk::TrackState::Vertex:
  case Trk::TrackState::SpacePoint:
    ATH_MSG_VERBOSE ("Bit-key calculation ");

    // PMs, Segments etc: use LocalParameters bit-key scheme
    for (unsigned int i=0; i<5; ++i) {
      Trk::ParamDefs iPar = PDA.pardef[i];
      if (measurement->localParameters().contains(iPar)) {
        residual[iColRow] = measurement->localParameters()[iPar]
          - trkPar->parameters()[iPar];
        if (pullIsValid)
          pull[iColRow]     = calcPull(residual[iColRow],
                                       measurement->localCovariance()(PDA.pardef[iColRow],PDA.pardef[iColRow]),
                                       (*trkPar->covariance())(PDA.pardef[iColRow],PDA.pardef[iColRow]),
                                       resType);
        ++iColRow;
      }
    }
    break;
  default:
    ATH_MSG_VERBOSE ("Default calculation ");
    // use HEPvector to calculate the residual
    // r = m - H.p
    Amg::VectorX HEPresidual = measurement->localParameters() - (measurement->localParameters().expansionMatrix() * trkPar->parameters());
    residual.resize(HEPresidual.rows());
    pull.resize(HEPresidual.rows());
    for (int i = 0; i < HEPresidual.rows(); i++) {
      residual[i] = HEPresidual[i];
    }
    
  }
  return new Trk::ResidualPull(residual, pull, pullIsValid, resType,
			       measurement->localParameters().parameterKey());
}

/////////////////////////////////////////////////////////////////////////////
/// calc pull in 1 dimension
/////////////////////////////////////////////////////////////////////////////
double Trk::ResidualPullCalculator::calcPull(
					     const double residual,
					     const double locMesCov,
					     const double locTrkCov,
					     const Trk::ResidualPull::ResidualType& resType ) const {

  double CovarianceSum = 0.0;
  if (resType == Trk::ResidualPull::Unbiased) {
    CovarianceSum = locMesCov + locTrkCov;
  } else if (resType == Trk::ResidualPull::Biased) {
    CovarianceSum = locMesCov - locTrkCov;
  } else CovarianceSum = locMesCov;

  if (CovarianceSum <= 0.0) {
    ATH_MSG_DEBUG("instable calculation: total covariance is non-positive, MeasCov = "<<
                  locMesCov<<", TrkCov = "<<locTrkCov<<", resType = "<<resType);
    return 0.0;
  }
  return residual/sqrt(CovarianceSum);
}
