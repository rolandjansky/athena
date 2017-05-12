// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackCuts.cxx
// implementations and definitions of cut objects

#include "src/InDetTrackCut.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include <algorithm>


// ---------------- TrackCut ----------------
InDet::TrackCut::TrackCut(InDet::InDetTrackSelectionTool* tool)
  : asg::AsgMessaging(tool)
  , m_trackAccessors(nullptr)
  , m_selectionTool(tool)
{
}

InDet::TrackCut::~TrackCut() = default;

// this function is a friend of the tool
StatusCode InDet::TrackCut::initialize()
{
  if (!m_selectionTool) {
    ATH_MSG_ERROR( "Invalid pointer to InDetTrackSelectionTool." );
    return StatusCode::FAILURE;
  }
  // let the cut have a handle on the tool's accessors
  m_trackAccessors = &(m_selectionTool->m_trackAccessors);
  return StatusCode::SUCCESS;
}

// ---------------- OrCut ----------------
template <size_t N>
InDet::OrCut<N>::OrCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
{
}

template <size_t N>
StatusCode InDet::OrCut<N>::initialize()
{
  for (const auto& cut : m_cuts) {
    if (cut == nullptr) {
      ATH_MSG_ERROR( "Did not set all cuts in OrCut" );
      return StatusCode::FAILURE;
    }
    ASG_CHECK(cut->initialize());
  }
  return StatusCode::SUCCESS;
}

template <size_t N>
bool InDet::OrCut<N>::result() const
{
  for (const auto& cut : m_cuts)
    if (cut->result()) return true;
  return false;
}

template class InDet::OrCut<2>; // instantiate the values that will be used
template class InDet::OrCut<3>;
template class InDet::OrCut<4>;


// ---------------- MaxSummaryValueCut ----------------
InDet::MaxSummaryValueCut::MaxSummaryValueCut(InDet::InDetTrackSelectionTool* tool, Int_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
{
}

InDet::MaxSummaryValueCut::MaxSummaryValueCut(InDet::InDetTrackSelectionTool* tool, Int_t max,
					      const std::vector<xAOD::SummaryType>& sumTypes)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_summaryTypes(sumTypes)
{
}

StatusCode InDet::MaxSummaryValueCut::initialize()
{
  // first run the base class version to get access to the tool's accessors
  ATH_CHECK( TrackCut::initialize() );

  for (xAOD::SummaryType type : m_summaryTypes) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    ATH_CHECK( getAccessor(summaryName, accessor) );
    accessor->setSummaryType( type );
    m_summaryAccessors.push_back( std::move(accessor) );
  }

  return StatusCode::SUCCESS;
}

bool InDet::MaxSummaryValueCut::result() const
{
  Int_t value = 0;
  for (std::shared_ptr<SummaryAccessor> summaryAccessor : m_summaryAccessors) {
    if (!summaryAccessor) {
      ATH_MSG_WARNING( "Track summary accessor not valid. Track will not pass this cut." );
      return false;
    }
    value += summaryAccessor->getValue();
  }
  return value <= m_maxValue;
}

// ---------------- MinSummaryValueCut ----------------
InDet::MinSummaryValueCut::MinSummaryValueCut(InDet::InDetTrackSelectionTool* tool, Int_t min)
  : InDet::TrackCut(tool)
  , m_minValue(min)
{
}

InDet::MinSummaryValueCut::MinSummaryValueCut(InDet::InDetTrackSelectionTool* tool, Int_t min,
					      const std::vector<xAOD::SummaryType>& sumTypes)
  : InDet::TrackCut(tool)
  , m_minValue(min)
  , m_summaryTypes(sumTypes)
{
}

StatusCode InDet::MinSummaryValueCut::initialize()
{
  // first run the base class version to get access to the tool's accessors
  ATH_CHECK( TrackCut::initialize() );

  for (xAOD::SummaryType type : m_summaryTypes) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    ATH_CHECK( getAccessor(summaryName, accessor) );
    accessor->setSummaryType( type );
    m_summaryAccessors.push_back( std::move(accessor) );
  }

  return StatusCode::SUCCESS;
}

bool InDet::MinSummaryValueCut::result() const
{
  Int_t value = 0;
  for (std::shared_ptr<SummaryAccessor> summaryAccessor : m_summaryAccessors) {
    if (!summaryAccessor) {
      ATH_MSG_WARNING( "Track summary accessor not valid. Track will not pass this cut." );
      return false;
    }
    value += summaryAccessor->getValue();
  }
  return value >= m_minValue;
}

// ---------------- FuncSummaryValueCut ----------------
template <size_t N>
InDet::FuncSummaryValueCut<N>::FuncSummaryValueCut(InDet::InDetTrackSelectionTool* tool,
						   const std::array<xAOD::SummaryType,N>&& sumTypes)
  : InDet::TrackCut(tool)
  , m_summaryTypes(sumTypes)
{
}

template <size_t N>
StatusCode InDet::FuncSummaryValueCut<N>::initialize()
{
  // first run the base class version to get access to the tool's accessors
  ATH_CHECK( TrackCut::initialize() );

  for (size_t i=0; i<N; ++i) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(m_summaryTypes[i]);
    std::shared_ptr<SummaryAccessor> accessor;
    ATH_CHECK( getAccessor(summaryName, accessor) );
    accessor->setSummaryType( m_summaryTypes[i] );
    m_summaryAccessors[i] = std::move(accessor);
  }

  return StatusCode::SUCCESS;
}

template <size_t N>
bool InDet::FuncSummaryValueCut<N>::result() const
{
  for (size_t i=0; i<N; ++i) {
    if (!m_summaryAccessors[i]) {
      ATH_MSG_WARNING( "Track summary accessor not valid. Track will not pass this cut." );
      return false;
    }
    m_results[i] = m_summaryAccessors[i]->getValue();
  }
  return m_func(m_results);
}

template class InDet::FuncSummaryValueCut<2>;
template class InDet::FuncSummaryValueCut<3>;
template class InDet::FuncSummaryValueCut<4>;
template class InDet::FuncSummaryValueCut<6>;

// ---------------- MaxSummaryValueRatioCut ----------------
InDet::MaxSummaryValueRatioCut::MaxSummaryValueRatioCut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
{
}

StatusCode InDet::MaxSummaryValueRatioCut::initialize()
{
  // first run the base class version to get access to the tool's accessors
  ATH_CHECK( TrackCut::initialize() );

  for (xAOD::SummaryType type : m_summaryTypesNum) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    ATH_CHECK( getAccessor(summaryName, accessor) );
    accessor->setSummaryType( type );
    m_summaryAccessorsNum.push_back( std::move(accessor) );
  }
  for (xAOD::SummaryType type : m_summaryTypesDen) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    ATH_CHECK( getAccessor(summaryName, accessor) );
    accessor->setSummaryType( type );
    m_summaryAccessorsDen.push_back( std::move(accessor) );
  }

  return StatusCode::SUCCESS;
}

bool InDet::MaxSummaryValueRatioCut::result() const
{
  Int_t numerator = 0;
  for (std::shared_ptr<SummaryAccessor> summaryAccessor : m_summaryAccessorsNum) {
    if (!summaryAccessor) {
      ATH_MSG_WARNING( "Track summary accessor not valid. Track will not pass this cut." );
      return false;
    }
    numerator += summaryAccessor->getValue();
  }
  Int_t denominator = 0;
  for (std::shared_ptr<SummaryAccessor> summaryAccessor : m_summaryAccessorsDen) {
    if (!summaryAccessor) {
      ATH_MSG_WARNING( "Track summary accessor not valid. Track will not pass this cut." );
      return false;
    }
    denominator += summaryAccessor->getValue();
  }
  return numerator <= m_maxValue*denominator;
}

// ---------------- MinTrtHitCut ----------------
InDet::MinTrtHitCut::MinTrtHitCut(InDet::InDetTrackSelectionTool* tool, Int_t min, Double_t etaLow, Double_t etaHigh)
  : InDet::TrackCut(tool)
  , InDet::MinSummaryValueCut(tool, min)
  , m_maxTrtEtaAcceptance(etaLow)
  , m_maxEtaForCut(etaHigh)
  , m_etaAccessor(nullptr)
{
}

StatusCode InDet::MinTrtHitCut::initialize()
{
  ATH_CHECK( MinSummaryValueCut::initialize() );
  ATH_CHECK( getAccessor("eta", m_etaAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::MinTrtHitCut::result() const
{
  if (!m_etaAccessor) {
    ATH_MSG_WARNING( "Track eta accessor not valid. Track will not pass this cut." );
    return false;
  }
  float absEta = std::fabs(m_etaAccessor->getValue());
  if (absEta <= m_maxTrtEtaAcceptance || absEta > m_maxEtaForCut)
    return true;
  return MinSummaryValueCut::result();
}

// ---------------- MaxTrtHitRatioCut ----------------
InDet::MaxTrtHitRatioCut::MaxTrtHitRatioCut(InDet::InDetTrackSelectionTool* tool, Double_t ratio,
					    Double_t etaLow, Double_t etaHigh)
  : InDet::TrackCut(tool)
  , InDet::MaxSummaryValueRatioCut(tool, ratio)
  , m_maxTrtEtaAcceptance(etaLow)
  , m_maxEtaForCut(etaHigh)
  , m_etaAccessor(nullptr)
{
}

StatusCode InDet::MaxTrtHitRatioCut::initialize()
{
  ATH_CHECK( MaxSummaryValueRatioCut::initialize() );
  ATH_CHECK( getAccessor("eta", m_etaAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::MaxTrtHitRatioCut::result() const
{
  if (!m_etaAccessor) {
    ATH_MSG_WARNING( "Track eta accessor not valid. Track will not pass this cut." );
    return false;
  }
  float absEta = std::fabs(m_etaAccessor->getValue());
  if (absEta <= m_maxTrtEtaAcceptance || absEta > m_maxEtaForCut)
    return true;
  return MaxSummaryValueRatioCut::result();
}


// ---------------- D0Cut ----------------
InDet::D0Cut::D0Cut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_paramAccessor(nullptr)
{
}

StatusCode InDet::D0Cut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("d0", m_paramAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::D0Cut::result() const
{
  if (!m_paramAccessor) {
    ATH_MSG_WARNING( "Track parameter accessor not valid. Track will not pass this cut." );
    return false;
  }
  return std::fabs(m_paramAccessor->getValue()) <= m_maxValue;
}

// ---------------- D0SigmaCut ----------------
InDet::D0SigmaCut::D0SigmaCut(InDet::InDetTrackSelectionTool* tool, Double_t maxSigma)
  : InDet::TrackCut(tool)
  , m_maxValueSq(maxSigma*maxSigma)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::D0SigmaCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("sigmaD0", m_paramCovAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::D0SigmaCut::result() const
{
  if (!m_paramCovAccessor) {
    ATH_MSG_WARNING( "Track parameter covariance accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_paramCovAccessor->getValue() <= m_maxValueSq; // max value is squared in the setting function
}

// ---------------- D0SignificanceCut ----------------
InDet::D0SignificanceCut::D0SignificanceCut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValueSq(max*max)
  , m_paramAccessor(nullptr)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::D0SignificanceCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("d0", m_paramAccessor) );
  ATH_CHECK( getAccessor("sigmaD0", m_paramCovAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::D0SignificanceCut::result() const
{
  if (!m_paramAccessor) {
    ATH_MSG_WARNING( "Track parameter accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_paramCovAccessor) {
    ATH_MSG_WARNING( "Track parameter covariance accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_paramAccessor->getValue()*m_paramAccessor->getValue()
    <= m_maxValueSq*m_paramCovAccessor->getValue();
}

// ---------------- Z0Cut ----------------
InDet::Z0Cut::Z0Cut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_paramAccessor(nullptr)
{
}

StatusCode InDet::Z0Cut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("z0", m_paramAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::Z0Cut::result() const
{
  if (!m_paramAccessor) {
    ATH_MSG_WARNING( "Track parameter accessor not valid. Track will not pass this cut." );
    return false;
  }
  return std::fabs(m_paramAccessor->getValue()) <= m_maxValue;
}


// ---------------- Z0SigmaCut ----------------
InDet::Z0SigmaCut::Z0SigmaCut(InDet::InDetTrackSelectionTool* tool, Double_t maxSigma)
  : InDet::TrackCut(tool)
  , m_maxValueSq(maxSigma*maxSigma)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::Z0SigmaCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("sigmaZ0", m_paramCovAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::Z0SigmaCut::result() const
{
  if (!m_paramCovAccessor) {
    ATH_MSG_WARNING( "Track parameter covariance accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_paramCovAccessor->getValue() <= m_maxValueSq; // max value is squared in the set function
}

// ---------------- Z0SignificanceCut ----------------
InDet::Z0SignificanceCut::Z0SignificanceCut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValueSq(max*max)
  , m_paramAccessor(nullptr)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::Z0SignificanceCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("z0", m_paramAccessor) );
  ATH_CHECK( getAccessor("sigmaZ0", m_paramCovAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::Z0SignificanceCut::result() const
{
  if (!m_paramAccessor) {
    ATH_MSG_WARNING( "Track parameter accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_paramCovAccessor) {
    ATH_MSG_WARNING( "Track parameter covariance accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_paramAccessor->getValue()*m_paramAccessor->getValue()
    <= m_maxValueSq*m_paramCovAccessor->getValue();
}


// ---------------- Z0SinThetaCut ----------------
InDet::Z0SinThetaCut::Z0SinThetaCut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_Z0Accessor(nullptr)
  , m_ThetaAccessor(nullptr)
{
}

StatusCode InDet::Z0SinThetaCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("z0", m_Z0Accessor) );
  ATH_CHECK( getAccessor("theta",  m_ThetaAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::Z0SinThetaCut::result() const
{
  if (!m_Z0Accessor) {
    ATH_MSG_WARNING( "Z0 accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaAccessor) {
    ATH_MSG_WARNING( "Theta accessor not valid. Track will not pass this cut." );
    return false;
  }
  return std::fabs(m_Z0Accessor->getValue() * std::sin(m_ThetaAccessor->getValue())) <= m_maxValue;
}

// ---------------- Z0SinThetaSigmaCut ----------------
InDet::Z0SinThetaSigmaCut::Z0SinThetaSigmaCut(InDet::InDetTrackSelectionTool* tool, Double_t maxSigma)
  : InDet::TrackCut(tool)
  , m_maxValueSq(maxSigma*maxSigma)
  , m_Z0Accessor(nullptr)
  , m_Z0VarAccessor(nullptr)
  , m_ThetaAccessor(nullptr)
  , m_ThetaVarAccessor(nullptr)
  , m_CovAccessor(nullptr)
{
}

StatusCode InDet::Z0SinThetaSigmaCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("z0", m_Z0Accessor) );
  ATH_CHECK( getAccessor("sigmaZ0", m_Z0VarAccessor) );
  ATH_CHECK( getAccessor("theta",  m_ThetaAccessor) );
  ATH_CHECK( getAccessor("sigmaTheta", m_ThetaVarAccessor) );
  ATH_CHECK( getAccessor("covZ0Theta", m_CovAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::Z0SinThetaSigmaCut::result() const
{
  if (!m_Z0Accessor) {
    ATH_MSG_WARNING("Z0 accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_Z0VarAccessor) {
    ATH_MSG_WARNING("Z0 variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaAccessor) {
    ATH_MSG_WARNING( "Theta accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaVarAccessor) {
    ATH_MSG_WARNING( "Theta variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_CovAccessor) {
    ATH_MSG_WARNING( "Covariance of z0 and theta accessor not valid. Track will not pass this cut." );
    return false;
  }
  
  Double_t z0 = m_Z0Accessor->getValue();
  Double_t sinTheta = std::sin(m_ThetaAccessor->getValue());
  Double_t cosTheta = std::cos(m_ThetaAccessor->getValue());

  return (z0*z0*cosTheta*cosTheta*m_ThetaVarAccessor->getValue() +
	  2*z0*sinTheta*cosTheta*m_CovAccessor->getValue() +
	  sinTheta*sinTheta*m_Z0VarAccessor->getValue() ) <= m_maxValueSq;
}

// ---------------- Z0SinThetaSignificanceCut ----------------
InDet::Z0SinThetaSignificanceCut::Z0SinThetaSignificanceCut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValueSq(max*max)
  , m_Z0Accessor(nullptr)
  , m_Z0VarAccessor(nullptr)
  , m_ThetaAccessor(nullptr)
  , m_ThetaVarAccessor(nullptr)
  , m_CovAccessor(nullptr)
{
}

StatusCode InDet::Z0SinThetaSignificanceCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("z0", m_Z0Accessor) );
  ATH_CHECK( getAccessor("sigmaZ0", m_Z0VarAccessor) );
  ATH_CHECK( getAccessor("theta",  m_ThetaAccessor) );
  ATH_CHECK( getAccessor("sigmaTheta", m_ThetaVarAccessor) );
  ATH_CHECK( getAccessor("covZ0Theta", m_CovAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::Z0SinThetaSignificanceCut::result() const
{
  if (!m_Z0Accessor) {
    ATH_MSG_WARNING( "Z0 accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_Z0VarAccessor) {
    ATH_MSG_WARNING( "Z0 variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaAccessor) {
    ATH_MSG_WARNING( "Theta accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaVarAccessor) {
    ATH_MSG_WARNING( "Theta variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_CovAccessor) {
    ATH_MSG_WARNING( "Covariance of z0 and theta accessor not valid. Track will not pass this cut." );
    return false;
  }
  
  Double_t z0 = m_Z0Accessor->getValue();
  Double_t sinTheta = std::sin(m_ThetaAccessor->getValue());
  Double_t cosTheta = std::cos(m_ThetaAccessor->getValue());
  
  return z0*z0*sinTheta*sinTheta <=
    m_maxValueSq*(z0*z0*cosTheta*cosTheta*m_ThetaVarAccessor->getValue() +
		  2*z0*sinTheta*cosTheta*m_CovAccessor->getValue() +
		  sinTheta*sinTheta*m_Z0VarAccessor->getValue() );
}


// ---------------- EtaDependentChiSqCut ----------------
InDet::EtaDependentChiSqCut::EtaDependentChiSqCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_etaAccessor(nullptr)
  , m_fitAccessor(nullptr)
{
}

StatusCode InDet::EtaDependentChiSqCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("eta", m_etaAccessor) );
  ATH_CHECK( getAccessor("fitQuality", m_fitAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::EtaDependentChiSqCut::result() const
{
  if (!m_etaAccessor) {
    ATH_MSG_WARNING( "Eta accessor not valid. Track will not pass eta-dependent chi squared cut." );
    return false;
  }
  if (!m_fitAccessor) {
    ATH_MSG_WARNING( "Fit quality accessor not valid. Track will not pass eta-dependent chi squared cut." );
    return false;
  }
  Double_t eta = m_etaAccessor->getValue();
  // this helper function returns true if the track passes a pseudorapidity dependent
  //   chi squared per degree of freedom cut. The current implementation is a working
  //   definition: see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InDetTrackingPerformanceGuidelines
  if (std::fabs(eta) < 1.9)
    return m_fitAccessor->getChiSq() <= m_fitAccessor->getNumberDoF() * ( 4.4 + 0.32*eta*eta );
  else
    return m_fitAccessor->getChiSq() <= m_fitAccessor->getNumberDoF() * ( 26.9 - 19.6978*std::fabs(eta) + 4.4534*eta*eta );
}

// ---------------- MinNSiHitsAboveEta ----------------
InDet::MinNSiHitsAboveEta::MinNSiHitsAboveEta(InDet::InDetTrackSelectionTool* tool, Int_t minSi, Double_t eta)
  : InDet::TrackCut(tool)
  , m_etaCutoff(eta)
  , m_minSiHits(minSi)
  , m_pixAccessor(nullptr)
  , m_pixDeadAccessor(nullptr)
  , m_sctAccessor(nullptr)
  , m_sctDeadAccessor(nullptr)
  , m_etaAccessor(nullptr)
{
}

StatusCode InDet::MinNSiHitsAboveEta::initialize()
{
  ATH_CHECK( TrackCut::initialize() );

  std::string pixHits = "summaryType";
  pixHits += std::to_string(xAOD::numberOfPixelHits);
  std::string pixDead = "summaryType";
  pixDead += std::to_string(xAOD::numberOfPixelDeadSensors);
  std::string sctHits = "summaryType";
  sctHits += std::to_string(xAOD::numberOfSCTHits);
  std::string sctDead = "summaryType";
  sctDead += std::to_string(xAOD::numberOfSCTDeadSensors);
  ATH_CHECK( getAccessor(pixHits, m_pixAccessor) );
  ATH_CHECK( getAccessor(pixDead, m_pixDeadAccessor) );
  ATH_CHECK( getAccessor(sctHits, m_sctAccessor) );
  ATH_CHECK( getAccessor(sctDead, m_sctDeadAccessor) );
  ATH_CHECK( getAccessor("eta", m_etaAccessor) );
  m_pixAccessor->setSummaryType( xAOD::numberOfPixelHits );
  m_pixDeadAccessor->setSummaryType( xAOD::numberOfPixelDeadSensors );
  m_sctAccessor->setSummaryType( xAOD::numberOfSCTHits );
  m_sctDeadAccessor->setSummaryType( xAOD::numberOfSCTDeadSensors );
  return StatusCode::SUCCESS;
}

bool InDet::MinNSiHitsAboveEta::result() const
{
  if (!m_pixAccessor) {
    ATH_MSG_WARNING( "Pixel hits accessor not valid." );
    return false;
  }
  if (!m_pixDeadAccessor) {
    ATH_MSG_WARNING( "Pixel dead sensors accessor not valid." );
    return false;
  }
  if (!m_sctAccessor) {
    ATH_MSG_WARNING( "SCT hits accessor not valid." );
    return false;
  }
  if (!m_sctDeadAccessor) {
    ATH_MSG_WARNING( "SCT dead sensor accessor not valid." );
    return false;
  }
  if (!m_etaAccessor) {
    ATH_MSG_WARNING( "Eta accessor not valid." );
    return false;
  }
  //  ATH_MSG_INFO(" in MinNSiHitsAboveEta" );
  return std::fabs(m_etaAccessor->getValue()) <= m_etaCutoff
    || (m_pixAccessor->getValue() + m_pixDeadAccessor->getValue()
	+ m_sctAccessor->getValue() + m_sctDeadAccessor->getValue() >= m_minSiHits);
}

// ---------------- MaxChiSq ----------------
InDet::MaxChiSq::MaxChiSq(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_accessor(nullptr)
{
}

StatusCode InDet::MaxChiSq::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor( "fitQuality", m_accessor ) );
  return StatusCode::SUCCESS;
}

bool InDet::MaxChiSq::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( "Invalid fit quality accessor. This track will not pass." );
    return false;
  }
  return m_accessor->getChiSq() <= m_maxValue;
}

// ---------------- MaxChiSqPerNdf ----------------
InDet::MaxChiSqPerNdf::MaxChiSqPerNdf(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_accessor(nullptr)
{
}

StatusCode InDet::MaxChiSqPerNdf::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  return getAccessor( "fitQuality", m_accessor );
}

bool InDet::MaxChiSqPerNdf::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( "Invalid fit quality accessor. This track will not pass." );
    return false;
  }
  return m_accessor->getChiSq() <= m_accessor->getNumberDoF() * m_maxValue;
}

// ---------------- MinProb ----------------
InDet::MinProb::MinProb(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_minValue(max)
  , m_accessor(nullptr)
{
}

StatusCode InDet::MinProb::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor( "fitQuality", m_accessor ) );
  return StatusCode::SUCCESS;
}

bool InDet::MinProb::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( "Invalid fit quality accessor. This track will not pass." );
    return false;
  }
  return TMath::Prob(m_accessor->getChiSq(), m_accessor->getNumberDoF()) >= m_minValue;
}

// ---------------- MinEProbHTCut ----------------
InDet::MinEProbHTCut::MinEProbHTCut(InDet::InDetTrackSelectionTool* tool, Double_t min)
  : InDet::TrackCut(tool)
  , m_minValue(min)
  , m_accessor(nullptr)
{
}

StatusCode InDet::MinEProbHTCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("eProbHT", m_accessor) );
  return StatusCode::SUCCESS;
}

bool InDet::MinEProbHTCut::result() const
{
  if (!m_accessor) {
    ATH_MSG_WARNING( "eProbabilityHT accessor not valid. Track will not pass." );
    return false;
  }
  return m_accessor->getValue() >= m_minValue;
}
// ---------------- EtaDependentSiliconHitsCut ----------------
InDet::EtaDependentSiliconHitsCut::EtaDependentSiliconHitsCut(InDet::InDetTrackSelectionTool* tool,
						    std::vector<Double_t> eta, std::vector<Int_t> hits)
  : InDet::TrackCut(tool)
  , m_etaCutoffs(eta)
  , m_siHitCuts(hits)
  , m_etaAccessor(nullptr)
  , m_sctAccessor(nullptr)
  , m_pixAccessor(nullptr)
  , m_sctDeadAccessor(nullptr)
  , m_pixDeadAccessor(nullptr)
{
}

StatusCode InDet::EtaDependentSiliconHitsCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );

  if (m_etaCutoffs.size() != m_siHitCuts.size()) {
    ATH_MSG_ERROR( "Eta cutoffs and Silicon hit cuts must be the same size" );
    return StatusCode::FAILURE;
  }
  for (size_t i_cuts = 0; i_cuts<m_etaCutoffs.size()-1; ++i_cuts) {
    if (std::fabs(m_etaCutoffs.at(i_cuts)) >= std::fabs(m_etaCutoffs.at(i_cuts+1))) {
      ATH_MSG_ERROR( "Eta cutoffs must be increasing" );
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( getAccessor("eta", m_etaAccessor) );
  std::string sctHits = "summaryType" + std::to_string(xAOD::numberOfSCTHits);
  std::string pixHits = "summaryType" + std::to_string(xAOD::numberOfPixelHits);
  std::string sctDead = "summaryType" + std::to_string(xAOD::numberOfSCTDeadSensors);
  std::string pixDead = "summaryType" + std::to_string(xAOD::numberOfPixelDeadSensors);

  ATH_CHECK( getAccessor(sctHits, m_sctAccessor) );
  m_sctAccessor->setSummaryType(xAOD::numberOfSCTHits);
  ATH_CHECK( getAccessor(pixHits, m_pixAccessor) );
  m_pixAccessor->setSummaryType(xAOD::numberOfPixelHits);
  ATH_CHECK( getAccessor(sctDead, m_sctDeadAccessor) );
  m_sctDeadAccessor->setSummaryType(xAOD::numberOfSCTDeadSensors);
  ATH_CHECK( getAccessor(pixDead, m_pixDeadAccessor) );
  m_pixDeadAccessor->setSummaryType(xAOD::numberOfPixelDeadSensors);
  return StatusCode::SUCCESS;
}
bool InDet::EtaDependentSiliconHitsCut::result() const
{
  if (!m_sctAccessor) {
    ATH_MSG_WARNING( "SCT hits accessor not valid. Track will not pass." );
    return false;
  }
  if (!m_pixAccessor) {
    ATH_MSG_WARNING( "Pixel hits accessor not valid. Track will not pass." );
    return false;
  }
  if (!m_sctDeadAccessor) {
    ATH_MSG_WARNING( "SCT Deadsensors accessor not valid. Track will not pass." );
    return false;
  }
  if (!m_pixDeadAccessor) {
    ATH_MSG_WARNING( "Pixel Deadsensors accessor not valid. Track will not pass." );
    return false;
  }
  if (!m_etaAccessor) {
    ATH_MSG_WARNING( "eta accessor not valid. Track will not pass." );
    return false;
  }
  //  ATH_MSG_INFO("in eta dependent hit cut");
  static int cutVecSize = m_etaCutoffs.size();
  //  ATH_MSG_INFO("cut vec size "<<cutVecSize);

  float trketa = std::fabs(m_etaAccessor->getValue());
  int trknHits =   m_sctAccessor->getValue() + m_pixAccessor->getValue() + m_sctDeadAccessor->getValue() + m_pixDeadAccessor->getValue();
  //  ATH_MSG_INFO("eta of track "<<trketa<<" Si hits on track "<<trknHits);
  //  ATH_MSG_INFO(" ");

  // loop over eta cutoff vector

  for (int i_etabin = cutVecSize-1; i_etabin >= 0; --i_etabin) {

    //    ATH_MSG_INFO("etabin "<<i_etabin<<" eta cutoff "<<std::fabs(m_etaCutoffs.at(i_etabin))<<" Hit cut value "<<m_siHitCuts.at(i_etabin) );
    //
    float etaMax =5.0;
    if(i_etabin<cutVecSize-1) etaMax = std::fabs(m_etaCutoffs.at(i_etabin+1));
    //

    if (  (trketa >= std::fabs(m_etaCutoffs.at(i_etabin)) && trketa < etaMax )) {

      if ( trknHits < m_siHitCuts.at(i_etabin)) {  
	//     ATH_MSG_INFO("returning false");
           return false;
      }
      else {
	//      ATH_MSG_INFO("returning true");
	   return true;
      }
    }   
  }

  //  ATH_MSG_INFO("returning true");
  return true;
}

InDet::EtaDependentPtCut::EtaDependentPtCut(InDet::InDetTrackSelectionTool* tool,
                                                    std::vector<Double_t> eta, std::vector<Double_t> pt)
  : InDet::TrackCut(tool)
  , m_etaCutoffs(eta)
  , m_ptCuts(pt)
  , m_etaAccessor(nullptr)
  , m_ptAccessor(nullptr)
{
}

StatusCode InDet::EtaDependentPtCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );

  if (m_etaCutoffs.size() != m_ptCuts.size()) {
    ATH_MSG_ERROR( "Eta cutoffs and transverse momentum cuts must be the same size" );
    return StatusCode::FAILURE;
  }
  for (size_t i_cuts = 0; i_cuts<m_etaCutoffs.size()-1; ++i_cuts) {
    if (std::fabs(m_etaCutoffs.at(i_cuts)) >= std::fabs(m_etaCutoffs.at(i_cuts+1))) {
      ATH_MSG_ERROR( "Eta cutoffs must be increasing" );
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( getAccessor("eta", m_etaAccessor) );
  ATH_CHECK( getAccessor("pt", m_ptAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::EtaDependentPtCut::result() const
{

  if (!m_etaAccessor) {
    ATH_MSG_WARNING( "eta accessor not valid. Track will not pass." );
    return false;
  }
  if (!m_ptAccessor) {
    ATH_MSG_WARNING( "pt accessor not valid. Track will not pass." );
    return false;
  }
  float trkpt = std::fabs(m_ptAccessor->getValue());
  float trketa = std::fabs(m_etaAccessor->getValue());
  static int cutVecSize = m_etaCutoffs.size();

  for (int i_etabin = cutVecSize-1; i_etabin >= 0; --i_etabin) {

    float etaMax =5.0;
    if(i_etabin<cutVecSize-1) etaMax = std::fabs(m_etaCutoffs.at(i_etabin+1));

    if (  (trketa >= std::fabs(m_etaCutoffs.at(i_etabin)) && trketa < etaMax )) {

      if ( trkpt < m_ptCuts.at(i_etabin)) {
        //     ATH_MSG_INFO("returning false");                                                                                                                                                                  
	return false;
      }
      else {
        //      ATH_MSG_INFO("returning true");                                                                                                                                                                  
	return true;
      }
    }
  }

  //  ATH_MSG_INFO("returning true");                                                                                                                                                                            
  return true;
}


// ---------------- PtDependentSctHitsCut ----------------
InDet::PtDependentSctHitsCut::PtDependentSctHitsCut(InDet::InDetTrackSelectionTool* tool,
						    std::vector<Double_t> pt, std::vector<Int_t> sct)
  : InDet::TrackCut(tool)
  , m_ptCutoffs(pt)
  , m_sctHitCuts(sct)
  , m_ptAccessor(nullptr)
  , m_sctAccessor(nullptr)
  , m_sctDeadAccessor(nullptr)
{
}

StatusCode InDet::PtDependentSctHitsCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );

  if (m_ptCutoffs.size() != m_sctHitCuts.size()) {
    ATH_MSG_ERROR( "Pt cutoffs and SCT hit cuts must be the same size" );
    return StatusCode::FAILURE;
  }
  for (size_t i_cuts = 0; i_cuts<m_ptCutoffs.size()-1; ++i_cuts) {
    if (m_ptCutoffs.at(i_cuts) >= m_ptCutoffs.at(i_cuts+1)) {
      ATH_MSG_ERROR( "Pt cutoffs must be increasing" );
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( getAccessor("pt", m_ptAccessor) );
  std::string sctHits = "summaryType" + std::to_string(xAOD::numberOfSCTHits);
  std::string sctDead = "summaryType" + std::to_string(xAOD::numberOfSCTDeadSensors);
  ATH_CHECK( getAccessor(sctHits, m_sctAccessor) );
  m_sctAccessor->setSummaryType(xAOD::numberOfSCTHits);
  ATH_CHECK( getAccessor(sctDead, m_sctDeadAccessor) );
  m_sctDeadAccessor->setSummaryType(xAOD::numberOfSCTDeadSensors);

  return StatusCode::SUCCESS;
}

bool InDet::PtDependentSctHitsCut::result() const
{
  if (!m_sctAccessor) {
    ATH_MSG_WARNING( "SCT hits accessor not valid. Track will not pass." );
    return false;
  }
  if (!m_sctDeadAccessor) {
    ATH_MSG_WARNING( "SCT dead hits accessor not valid. Track will not pass." );
    return false;
  }
  if (!m_ptAccessor) {
    ATH_MSG_WARNING( "pt accessor not valid. Track will not pass." );
    return false;
  }
  static int cutVecSize = m_ptCutoffs.size();
  for (int i_ptbin = cutVecSize-1; i_ptbin >= 0; --i_ptbin) {
    if (m_ptAccessor->getValue() >= m_ptCutoffs.at(i_ptbin)
	&& (m_sctAccessor->getValue() + m_sctDeadAccessor->getValue()
	    < m_sctHitCuts.at(i_ptbin))) {
      return false;
    }
  }  
  return true;
}


#ifndef XAOD_ANALYSIS
// ---------------- MinSiHitsModTopBottomCut ----------------
InDet::MinSiHitsModTopBottomCut::MinSiHitsModTopBottomCut(InDet::InDetTrackSelectionTool* tool, Int_t top, Int_t bottom)
  : InDet::TrackCut(tool)
  , m_minTop(top)
  , m_minBottom(bottom)
{
}

StatusCode InDet::MinSiHitsModTopBottomCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("siHitsMod", m_accessor) );
  return StatusCode::SUCCESS;
}

bool InDet::MinSiHitsModTopBottomCut::result() const
{
  // note that this cut uses an alternate definition of Si hits,
  // where pixel hits count twice (this is accounted for in the accessor).

  if (!m_accessor) {
    ATH_MSG_WARNING( "Top/bottom Si hits accessor not valid. Track will not pass this cut." );
    return false;
  }
  return (m_accessor->getHitsTop() >= m_minTop)
    && (m_accessor->getHitsBottom() >= m_minBottom);
}

#endif
