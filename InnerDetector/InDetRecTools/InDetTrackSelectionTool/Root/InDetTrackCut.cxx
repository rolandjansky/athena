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

InDet::TrackCut::~TrackCut()
{
}

template <class AccessorType>
StatusCode InDet::TrackCut::getAccessor(const std::string& accessorName,
					std::shared_ptr<AccessorType>& accessor)
{
  // this function looks for the accessor indicated by "name" in the tool's
  // track accessors and assigns "accessor" to it if it exists, creating
  // the accessor if it does not.
  std::unordered_map< std::string, std::shared_ptr<TrackAccessor> >::const_iterator it =
    std::find_if(m_trackAccessors->begin(), m_trackAccessors->end(),
		 [&] (std::pair< std::string, std::shared_ptr<TrackAccessor> > acc)
		 {return acc.first == accessorName;} );
  if (it == m_trackAccessors->end()) {
    // if we can't find the accessor, add one to the tool
    if (!m_selectionTool) {
      ATH_MSG_ERROR( "Must initialize TrackCut with pointer to the selection tool before adding accessors." );
      return StatusCode::FAILURE;
    }
    accessor = std::make_shared<AccessorType>(m_selectionTool);
    (*m_trackAccessors)[accessorName] = accessor;
    ATH_MSG_DEBUG( "Adding accessor " << accessorName );
  } else {
    accessor = std::dynamic_pointer_cast<AccessorType>(it->second);
  }

  if (!accessor) {
    // this could happen if the accessor indicated by "name" is not of type "AccessorType"
    ATH_MSG_ERROR( "Could not instantiate "<< accessorName <<
		   " accessor. Pointer to accessor is to type " <<
		   typeid(AccessorType).name() << ". Is this correct?" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

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

// ---------------- NotCut ----------------
InDet::NotCut::NotCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
{
}

StatusCode InDet::NotCut::initialize()
{
  if (m_cut == nullptr) {
    ATH_MSG_ERROR( "Cut not set in NotCut" );
    return StatusCode::FAILURE;
  }
  ASG_CHECK(m_cut->initialize());
  return StatusCode::SUCCESS;
}

bool InDet::NotCut::result() const
{
  return !m_cut->result();
}

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
    if (!summaryAccessor || !(summaryAccessor->isValid())) {
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
    if (!summaryAccessor || !summaryAccessor->isValid()) {
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
    if (!m_summaryAccessors[i] || !m_summaryAccessors[i]->isValid()) {
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
    if (!summaryAccessor || !(summaryAccessor->isValid())) {
      ATH_MSG_WARNING( "Track summary accessor not valid. Track will not pass this cut." );
      return false;
    }
    numerator += summaryAccessor->getValue();
  }
  Int_t denominator = 0;
  for (std::shared_ptr<SummaryAccessor> summaryAccessor : m_summaryAccessorsDen) {
    if (!summaryAccessor || !(summaryAccessor->isValid())) {
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
  if (!m_etaAccessor || !m_etaAccessor->isValid()) {
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
  if (!m_etaAccessor || !m_etaAccessor->isValid()) {
    ATH_MSG_WARNING( "Track eta accessor not valid. Track will not pass this cut." );
    return false;
  }
  float absEta = std::fabs(m_etaAccessor->getValue());
  if (absEta <= m_maxTrtEtaAcceptance || absEta > m_maxEtaForCut)
    return true;
  return MaxSummaryValueRatioCut::result();
}

// ---------------- MinUsedHitsdEdxCut ----------------
InDet::MinUsedHitsdEdxCut::MinUsedHitsdEdxCut(InDet::InDetTrackSelectionTool* tool, Int_t min)
  : InDet::TrackCut(tool)
  , m_minValue(min)
{
}

StatusCode InDet::MinUsedHitsdEdxCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("usedHitsdEdx", m_accessor) );
  return StatusCode::SUCCESS;
}

bool InDet::MinUsedHitsdEdxCut::result() const
{
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "numberOfUsedHitsdEdx Accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_accessor->getValue() >= m_minValue;
}

// ---------------- MinOverflowHitsdEdxCut ----------------
InDet::MinOverflowHitsdEdxCut::MinOverflowHitsdEdxCut(InDet::InDetTrackSelectionTool* tool, Int_t min)
  : InDet::TrackCut(tool)
  , m_minValue(min)
{
}

StatusCode InDet::MinOverflowHitsdEdxCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("overflowHitsdEdx", m_accessor) );
  return StatusCode::SUCCESS;
}

bool InDet::MinOverflowHitsdEdxCut::result() const
{
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "numberOfOverflowHitsdEdx Accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_accessor->getValue() >= m_minValue;
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
  m_paramAccessor->setIndex(0);
  return StatusCode::SUCCESS;
}

bool InDet::D0Cut::result() const
{
  if (!m_paramAccessor || !m_paramAccessor->isValid()) {
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
  m_paramCovAccessor->setIndices(0,0);
  return StatusCode::SUCCESS;
}

bool InDet::D0SigmaCut::result() const
{
  if (!m_paramCovAccessor || !m_paramCovAccessor->isValid()) {
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
  m_paramAccessor->setIndex(0);
  m_paramCovAccessor->setIndices(0,0);
  return StatusCode::SUCCESS;
}

bool InDet::D0SignificanceCut::result() const
{
  if (!m_paramAccessor || !m_paramAccessor->isValid()) {
    ATH_MSG_WARNING( "Track parameter accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_paramCovAccessor || !m_paramCovAccessor->isValid()) {
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
  m_paramAccessor->setIndex(1);
  return StatusCode::SUCCESS;
}

bool InDet::Z0Cut::result() const
{
  if (!m_paramAccessor || !m_paramAccessor->isValid()) {
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
  m_paramCovAccessor->setIndices(1,1);
  return StatusCode::SUCCESS;
}

bool InDet::Z0SigmaCut::result() const
{
  if (!m_paramCovAccessor || !m_paramCovAccessor->isValid()) {
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
  m_paramAccessor->setIndex(1);
  m_paramCovAccessor->setIndices(1,1);
  return StatusCode::SUCCESS;
}

bool InDet::Z0SignificanceCut::result() const
{
  if (!m_paramAccessor || !m_paramAccessor->isValid()) {
    ATH_MSG_WARNING( "Track parameter accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_paramCovAccessor || !m_paramCovAccessor->isValid()) {
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
  m_Z0Accessor->setIndex(1);
  m_ThetaAccessor->setIndex(3);
  return StatusCode::SUCCESS;
}

bool InDet::Z0SinThetaCut::result() const
{
  if (!m_Z0Accessor || !m_Z0Accessor->isValid()) {
    ATH_MSG_WARNING( "Z0 accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaAccessor || !m_ThetaAccessor->isValid()) {
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
  m_Z0Accessor->setIndex(1);
  m_Z0VarAccessor->setIndices(1,1);
  m_ThetaAccessor->setIndex(3);
  m_ThetaVarAccessor->setIndices(3,3);
  m_CovAccessor->setIndices(1,3);
  return StatusCode::SUCCESS;
}

bool InDet::Z0SinThetaSigmaCut::result() const
{
  if (!m_Z0Accessor || !m_Z0Accessor->isValid()) {
    ATH_MSG_WARNING("Z0 accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_Z0VarAccessor || !m_Z0VarAccessor->isValid()) {
    ATH_MSG_WARNING("Z0 variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaAccessor || !m_ThetaAccessor->isValid()) {
    ATH_MSG_WARNING( "Theta accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaVarAccessor || !m_ThetaVarAccessor->isValid()) {
    ATH_MSG_WARNING( "Theta variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_CovAccessor || !m_CovAccessor->isValid()) {
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
  m_Z0Accessor->setIndex(1);
  m_Z0VarAccessor->setIndices(1,1);
  m_ThetaAccessor->setIndex(3);
  m_ThetaVarAccessor->setIndices(3,3);
  m_CovAccessor->setIndices(1,3);
  return StatusCode::SUCCESS;
}

bool InDet::Z0SinThetaSignificanceCut::result() const
{
  if (!m_Z0Accessor || !m_Z0Accessor->isValid()) {
    ATH_MSG_WARNING( "Z0 accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_Z0VarAccessor || !m_Z0VarAccessor->isValid()) {
    ATH_MSG_WARNING( "Z0 variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaAccessor || !m_ThetaAccessor->isValid()) {
    ATH_MSG_WARNING( "Theta accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_ThetaVarAccessor || !m_ThetaVarAccessor->isValid()) {
    ATH_MSG_WARNING( "Theta variance accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (!m_CovAccessor || !m_CovAccessor->isValid()) {
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

// ---------------- PtCut ----------------
InDet::PtCut::PtCut(InDet::InDetTrackSelectionTool* tool, Double_t min)
  : InDet::TrackCut(tool)
  , m_minValue(min)
  , m_accessor(nullptr)
{
}

StatusCode InDet::PtCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("pt", m_accessor) );
  return StatusCode::SUCCESS;
}

bool InDet::PtCut::result() const
{
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "Pt accessor not valid. Track will not pass." );
    return false;
  }
  return m_accessor->getValue() >= m_minValue;
}

// ---------------- EtaCut ----------------
InDet::EtaCut::EtaCut(InDet::InDetTrackSelectionTool* tool, Double_t max)
  : InDet::TrackCut(tool)
  , m_maxValue(max)
  , m_accessor(nullptr)
{
}

StatusCode InDet::EtaCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("eta", m_accessor) );
  return StatusCode::SUCCESS;
}

bool InDet::EtaCut::result() const
{
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "Eta accessor not valid. Track will not pass." );
    return false;
  }
  return std::fabs(m_accessor->getValue()) <= m_maxValue;
}

// ---------------- PCut ----------------
InDet::PCut::PCut(InDet::InDetTrackSelectionTool* tool, Double_t min)
  : InDet::TrackCut(tool)
  , m_minValue(min)
  , m_accessor(nullptr)
{
}

StatusCode InDet::PCut::initialize()
{
  ATH_CHECK( TrackCut::initialize() );
  ATH_CHECK( getAccessor("p", m_accessor) );
  return StatusCode::SUCCESS;
}

bool InDet::PCut::result() const
{
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "P accessor not valid. Track will not pass." );
    return false;
  }
  return m_accessor->getValue() >= m_minValue;
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
  if (!m_etaAccessor || !m_etaAccessor->isValid()) {
    ATH_MSG_WARNING( "Eta accessor not valid. Track will not pass eta-dependent chi squared cut." );
    return false;
  }
  if (!m_fitAccessor || !m_fitAccessor->isValid()) {
    ATH_MSG_WARNING( "Fit quality accessor not valid. Track will not pass eta-dependent chi squared cut." );
    return false;
  }
  Double_t eta = m_etaAccessor->getValue();
  // this helper function returns true if the track passes a pseudorapidity dependent
  //   chi squared per degree of freedom cut. The current implementation is a working
  //   definition: see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InDetTrackingPerformanceGuidelines
  // TODO: possibly let this cut own something like a TFormula that defines the cut, instead of hard-coding it in. Unfortunately this doesn't have a very clean TFormula expression. Possibly a lambda function/std::function, though that probably wouldn't be trivial in athena.
  // UPDATE: this cut has been removed from the tracking CP recommendations.
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
  , m_sctAccessor(nullptr)
  , m_etaAccessor(nullptr)
{
}

StatusCode InDet::MinNSiHitsAboveEta::initialize()
{
  ATH_CHECK( TrackCut::initialize() );

  std::string pixHits = "summaryType";
  pixHits += std::to_string(xAOD::numberOfPixelHits);
  std::string sctHits = "summaryType";
  sctHits += std::to_string(xAOD::numberOfSCTHits);
  ATH_CHECK( getAccessor(pixHits, m_pixAccessor) );
  ATH_CHECK( getAccessor(sctHits, m_sctAccessor) );
  ATH_CHECK( getAccessor("eta", m_etaAccessor) );
  m_pixAccessor->setSummaryType( xAOD::numberOfPixelHits );
  m_sctAccessor->setSummaryType( xAOD::numberOfSCTHits );
  return StatusCode::SUCCESS;
}

bool InDet::MinNSiHitsAboveEta::result() const
{
  if (!m_pixAccessor || !m_pixAccessor->isValid()) {
    ATH_MSG_WARNING( "Pixel hits accessor not valid." );
    return false;
  }
  if (!m_sctAccessor || !m_sctAccessor->isValid()) {
    ATH_MSG_WARNING( "SCT hits accessor not valid." );
    return false;
  }
  if (!m_etaAccessor || !m_etaAccessor->isValid()) {
    ATH_MSG_WARNING( "Eta accessor not valid." );
    return false;
  }
  return std::fabs(m_etaAccessor->getValue()) <= m_etaCutoff
    || (m_pixAccessor->getValue() + m_sctAccessor->getValue() >= m_minSiHits);
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
  if (!m_accessor || !m_accessor->isValid()) {
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
  if (!m_accessor || !m_accessor->isValid()) {
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
  if (!m_accessor || !m_accessor->isValid()) {
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
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "eProbabilityHT accessor not valid. Track will not pass." );
    return false;
  }
  return m_accessor->getValue() >= m_minValue;
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

  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "Top/bottom Si hits accessor not valid. Track will not pass this cut." );
    return false;
  }
  return (m_accessor->getHitsTop() >= m_minTop)
    && (m_accessor->getHitsBottom() >= m_minBottom);
}

#endif
