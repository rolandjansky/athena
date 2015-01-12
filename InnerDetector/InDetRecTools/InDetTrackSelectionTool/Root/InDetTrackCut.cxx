// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackCuts.cxx
// implementations and definitions of cut objects

#include "InDetTrackSelectionTool/InDetTrackCut.h"
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

// ---------------- MaxSummaryValueCut ----------------
InDet::MaxSummaryValueCut::MaxSummaryValueCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
{
}

StatusCode InDet::MaxSummaryValueCut::initialize()
{
  // first run the base class version to get access to the tool's accessors
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  for (xAOD::SummaryType type : m_summaryTypes) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    if (getAccessor(summaryName, accessor).isFailure())
      return StatusCode::FAILURE;
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
InDet::MinSummaryValueCut::MinSummaryValueCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_minValue(0)
{
}

StatusCode InDet::MinSummaryValueCut::initialize()
{
  // first run the base class version to get access to the tool's accessors
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  for (xAOD::SummaryType type : m_summaryTypes) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    if (getAccessor(summaryName, accessor).isFailure())
      return StatusCode::FAILURE;
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

// ---------------- MaxSummaryValueCut ----------------
InDet::MaxSummaryValueRatioCut::MaxSummaryValueRatioCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
{
}

StatusCode InDet::MaxSummaryValueRatioCut::initialize()
{
  // first run the base class version to get access to the tool's accessors
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  for (xAOD::SummaryType type : m_summaryTypesNum) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    if (getAccessor(summaryName, accessor).isFailure())
      return StatusCode::FAILURE;
    accessor->setSummaryType( type );
    m_summaryAccessorsNum.push_back( std::move(accessor) );
  }
  for (xAOD::SummaryType type : m_summaryTypesDen) {
    std::string summaryName = "summaryType";
    summaryName += std::to_string(type);
    std::shared_ptr<SummaryAccessor> accessor;
    if (getAccessor(summaryName, accessor).isFailure())
      return StatusCode::FAILURE;
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

// // ---------------- MaxTrtHitCut ----------------
// InDet::MaxTrtHitCut::MaxTrtHitCut(InDet::InDetTrackSelectionTool* tool)
//   : InDet::MaxSummaryValueCut()
//   , m_maxTrtEtaAcceptance(0)
//   , m_etaAccessor(nullptr)
// {
// }

// StatusCode InDet::MaxTrtHitCut::initialize()
// {
//   if (!MaxSummaryValueCut::initialize().isSuccess())
//     return StatusCode::FAILURE;

//   if (getAccessor("eta", m_etaAccessor).isFailure())
//     return StatusCode::FAILURE;
//   return StatusCode::SUCCESS;
// }

// bool InDet::MaxTrtHitCut::result() const
// {
//   if (!m_etaAccessor || !m_etaAccessor->isValid()) {
//      ATH_MSG_WARNING( "Track eta accessor not valid. Track will not pass this cut." );
//     return false;
//   }
//   if (std::fabs(m_etaAccessor->getValue()) <= m_maxTrtEtaAcceptance)
//     return true;
//   else
//     return MaxSummaryValueCut::result();
// }

// ---------------- MinTrtHitCut ----------------
InDet::MinTrtHitCut::MinTrtHitCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , InDet::MinSummaryValueCut(tool)
  , m_maxTrtEtaAcceptance(0)
  , m_etaAccessor(nullptr)
{
}

StatusCode InDet::MinTrtHitCut::initialize()
{
  if (!MinSummaryValueCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  if (getAccessor("eta", m_etaAccessor).isFailure())
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

bool InDet::MinTrtHitCut::result() const
{
  if (!m_etaAccessor || !m_etaAccessor->isValid()) {
    ATH_MSG_WARNING( "Track eta accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (std::fabs(m_etaAccessor->getValue()) <= m_maxTrtEtaAcceptance)
    return true;
  else
    return MinSummaryValueCut::result();
}

// ---------------- MaxTrtHitRatioCut ----------------
InDet::MaxTrtHitRatioCut::MaxTrtHitRatioCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , InDet::MaxSummaryValueRatioCut(tool)
  , m_maxTrtEtaAcceptance(0)
  , m_etaAccessor(nullptr)
{
}

StatusCode InDet::MaxTrtHitRatioCut::initialize()
{
  if (!MaxSummaryValueRatioCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  if (getAccessor("eta", m_etaAccessor).isFailure())
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

bool InDet::MaxTrtHitRatioCut::result() const
{
  if (!m_etaAccessor || !m_etaAccessor->isValid()) {
    ATH_MSG_WARNING( "Track eta accessor not valid. Track will not pass this cut." );
    return false;
  }
  if (std::fabs(m_etaAccessor->getValue()) <= m_maxTrtEtaAcceptance)
    return true;
  else
    return MaxSummaryValueRatioCut::result();
}

// ---------------- D0Cut ----------------
InDet::D0Cut::D0Cut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_paramAccessor(nullptr)
{
}

StatusCode InDet::D0Cut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  if (getAccessor("d0", m_paramAccessor).isFailure())
    return StatusCode::FAILURE;
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
InDet::D0SigmaCut::D0SigmaCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::D0SigmaCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  if (getAccessor("sigmaD0", m_paramCovAccessor).isFailure())
    return StatusCode::FAILURE;
  m_paramCovAccessor->setIndices(0,0);
  return StatusCode::SUCCESS;
}

bool InDet::D0SigmaCut::result() const
{
  if (!m_paramCovAccessor || !m_paramCovAccessor->isValid()) {
    ATH_MSG_WARNING( "Track parameter covariance accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_paramCovAccessor->getValue() <= m_maxValue; // max value is squared in the setting function
}

// ---------------- D0SignificanceCut ----------------
InDet::D0SignificanceCut::D0SignificanceCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_paramAccessor(nullptr)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::D0SignificanceCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  if (getAccessor("d0", m_paramAccessor).isFailure()
      || getAccessor("sigmaD0", m_paramCovAccessor).isFailure())
    return StatusCode::FAILURE;
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
    <= m_maxValue*m_paramCovAccessor->getValue();
}

// ---------------- Z0Cut ----------------
InDet::Z0Cut::Z0Cut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_paramAccessor(nullptr)
{
}

StatusCode InDet::Z0Cut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  if (getAccessor("z0", m_paramAccessor).isFailure())
    return StatusCode::FAILURE;
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
InDet::Z0SigmaCut::Z0SigmaCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::Z0SigmaCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  if (getAccessor("sigmaZ0", m_paramCovAccessor).isFailure())
    return StatusCode::FAILURE;
  m_paramCovAccessor->setIndices(1,1);
  return StatusCode::SUCCESS;
}

bool InDet::Z0SigmaCut::result() const
{
  if (!m_paramCovAccessor || !m_paramCovAccessor->isValid()) {
    ATH_MSG_WARNING( "Track parameter covariance accessor not valid. Track will not pass this cut." );
    return false;
  }
  return m_paramCovAccessor->getValue() <= m_maxValue; // max value is squared in the set function
}

// ---------------- Z0SignificanceCut ----------------
InDet::Z0SignificanceCut::Z0SignificanceCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_paramAccessor(nullptr)
  , m_paramCovAccessor(nullptr)
{
}

StatusCode InDet::Z0SignificanceCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  if (getAccessor("z0", m_paramAccessor).isFailure()
      || getAccessor("sigmaZ0", m_paramCovAccessor).isFailure())
    return StatusCode::FAILURE;
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
    <= m_maxValue*m_paramCovAccessor->getValue();
}


// ---------------- Z0SinThetaCut ----------------
InDet::Z0SinThetaCut::Z0SinThetaCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_Z0Accessor(nullptr)
  , m_ThetaAccessor(nullptr)
{
}

StatusCode InDet::Z0SinThetaCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  if (getAccessor("z0", m_Z0Accessor).isFailure()
      || getAccessor("theta",  m_ThetaAccessor).isFailure())
    return StatusCode::FAILURE;
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
InDet::Z0SinThetaSigmaCut::Z0SinThetaSigmaCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_Z0Accessor(nullptr)
  , m_Z0VarAccessor(nullptr)
  , m_ThetaAccessor(nullptr)
  , m_ThetaVarAccessor(nullptr)
  , m_CovAccessor(nullptr)
{
}

StatusCode InDet::Z0SinThetaSigmaCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  if (getAccessor("z0", m_Z0Accessor).isFailure()
      || getAccessor("sigmaZ0", m_Z0VarAccessor).isFailure()
      || getAccessor("theta",  m_ThetaAccessor).isFailure()
      || getAccessor("sigmaTheta", m_ThetaVarAccessor).isFailure()
      || getAccessor("covZ0Theta", m_CovAccessor).isFailure())
    return StatusCode::FAILURE;
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
	  sinTheta*sinTheta*m_Z0VarAccessor->getValue() ) <= m_maxValue;
}

// ---------------- Z0SinThetaSignificanceCut ----------------
InDet::Z0SinThetaSignificanceCut::Z0SinThetaSignificanceCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_Z0Accessor(nullptr)
  , m_Z0VarAccessor(nullptr)
  , m_ThetaAccessor(nullptr)
  , m_ThetaVarAccessor(nullptr)
  , m_CovAccessor(nullptr)
{
}

StatusCode InDet::Z0SinThetaSignificanceCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  if (getAccessor("z0", m_Z0Accessor).isFailure()
      || getAccessor("sigmaZ0", m_Z0VarAccessor).isFailure()
      || getAccessor("theta",  m_ThetaAccessor).isFailure()
      || getAccessor("sigmaTheta", m_ThetaVarAccessor).isFailure()
      || getAccessor("covZ0Theta", m_CovAccessor).isFailure())
    return StatusCode::FAILURE;
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
    m_maxValue*(z0*z0*cosTheta*cosTheta*m_ThetaVarAccessor->getValue() +
		2*z0*sinTheta*cosTheta*m_CovAccessor->getValue() +
		sinTheta*sinTheta*m_Z0VarAccessor->getValue() );
}

// ---------------- PtCut ----------------
InDet::PtCut::PtCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_minValue(0)
  , m_accessor(nullptr)
{
}

StatusCode InDet::PtCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  return getAccessor("pt", m_accessor);
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
InDet::EtaCut::EtaCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_accessor(nullptr)
{
}

StatusCode InDet::EtaCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  return getAccessor("eta", m_accessor);
}

bool InDet::EtaCut::result() const
{
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "Eta accessor not valid. Track will not pass." );
    return false;
  }
  return std::abs(m_accessor->getValue()) <= m_maxValue;
}

// ---------------- PCut ----------------
InDet::PCut::PCut(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_minValue(0)
  , m_accessor(nullptr)
{
}

StatusCode InDet::PCut::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  return getAccessor("p", m_accessor);
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
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  if (getAccessor("eta", m_etaAccessor).isFailure()
      || getAccessor("fitQuality", m_fitAccessor).isFailure())
    return StatusCode::FAILURE;
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
  if (std::fabs(eta) < 1.9)
    return m_fitAccessor->getChiSq() <= m_fitAccessor->getNumberDoF() * ( 4.4 + 0.32*eta*eta );
  else
    return m_fitAccessor->getChiSq() <= m_fitAccessor->getNumberDoF() * ( 26.9 - 19.6978*std::fabs(eta) + 4.4534*eta*eta );
}

// ---------------- MaxOneSharedModule ----------------
InDet::MaxOneSharedModule::MaxOneSharedModule(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_pixAccessor(nullptr)
  , m_sctAccessor(nullptr)
{
}

StatusCode InDet::MaxOneSharedModule::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  std::string sharedPix = "summaryType";
  sharedPix += std::to_string(xAOD::numberOfPixelSharedHits);
  std::string sharedSct = "summaryType";
  sharedSct += std::to_string(xAOD::numberOfSCTSharedHits);
  if (getAccessor(sharedPix, m_pixAccessor).isFailure()
      || getAccessor(sharedSct, m_sctAccessor).isFailure())
    return StatusCode::FAILURE;
  m_pixAccessor->setSummaryType( xAOD::numberOfPixelSharedHits );
  m_sctAccessor->setSummaryType( xAOD::numberOfSCTSharedHits );
  return StatusCode::SUCCESS;
}

bool InDet::MaxOneSharedModule::result() const
{
  if (!m_pixAccessor || !m_pixAccessor->isValid()) {
    ATH_MSG_WARNING( "Pixel shared hits accessor not valid." );
    return false;
  }
  if (!m_sctAccessor || !m_sctAccessor->isValid()) {
    ATH_MSG_WARNING( "SCT shared hits accessor not valid." );
    return false;
  }
  return m_pixAccessor->getValue() + m_sctAccessor->getValue()/2 <= 1;
}

// ---------------- MinNSiHitsIfSiSharedHits ----------------
InDet::MinNSiHitsIfSiSharedHits::MinNSiHitsIfSiSharedHits(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_minSiHits(0)
  , m_pixHitsAccessor(nullptr)
  , m_sctHitsAccessor(nullptr)
  , m_pixSharedHitsAccessor(nullptr)
  , m_sctSharedHitsAccessor(nullptr)
{
}

StatusCode InDet::MinNSiHitsIfSiSharedHits::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  std::string pixHits = "summaryType";
  pixHits += std::to_string(xAOD::numberOfPixelHits);
  std::string sctHits = "summaryType";
  sctHits += std::to_string(xAOD::numberOfSCTHits);
  std::string sharedPix = "summaryType";
  sharedPix += std::to_string(xAOD::numberOfPixelSharedHits);
  std::string sharedSct = "summaryType";
  sharedSct += std::to_string(xAOD::numberOfSCTSharedHits);
  if (getAccessor(pixHits, m_pixHitsAccessor).isFailure()
      || getAccessor(sctHits, m_sctHitsAccessor).isFailure()
      || getAccessor(sharedPix, m_pixSharedHitsAccessor).isFailure()
      || getAccessor(sharedSct, m_sctSharedHitsAccessor).isFailure())
    return StatusCode::FAILURE;
  m_pixHitsAccessor->setSummaryType( xAOD::numberOfPixelHits );
  m_sctHitsAccessor->setSummaryType( xAOD::numberOfSCTHits );
  m_pixSharedHitsAccessor->setSummaryType( xAOD::numberOfPixelSharedHits );
  m_sctSharedHitsAccessor->setSummaryType( xAOD::numberOfSCTSharedHits );
  return StatusCode::SUCCESS;
}

bool InDet::MinNSiHitsIfSiSharedHits::result() const
{
  if (!m_pixHitsAccessor || !m_pixHitsAccessor->isValid()) {
    ATH_MSG_WARNING( "Pixel hits accessor not valid." );
    return false;
  }
  if (!m_sctHitsAccessor || !m_sctHitsAccessor->isValid()) {
    ATH_MSG_WARNING( "SCT hits accessor not valid." );
    return false;
  }
  if (!m_pixSharedHitsAccessor || !m_pixSharedHitsAccessor->isValid()) {
    ATH_MSG_WARNING( "Pixel shared hits accessor not valid." );
    return false;
  }
  if (!m_sctSharedHitsAccessor || !m_sctSharedHitsAccessor->isValid()) {
    ATH_MSG_WARNING( "SCT shared hits accessor not valid." );
    return false;
  }
  return (m_pixSharedHitsAccessor->getValue() + m_sctSharedHitsAccessor->getValue() >= 0)
    || (m_pixHitsAccessor->getValue() + m_sctHitsAccessor->getValue() >= m_minSiHits);
}

// ---------------- MinNSiHitsAboveEta ----------------
InDet::MinNSiHitsAboveEta::MinNSiHitsAboveEta(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_etaCutoff(0)
  , m_minSiHits(0)
  , m_pixAccessor(nullptr)
  , m_sctAccessor(nullptr)
  , m_etaAccessor(nullptr)
{
}

StatusCode InDet::MinNSiHitsAboveEta::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;

  std::string pixHits = "summaryType";
  pixHits += std::to_string(xAOD::numberOfPixelHits);
  std::string sctHits = "summaryType";
  sctHits += std::to_string(xAOD::numberOfSCTHits);
  if (getAccessor(pixHits, m_pixAccessor).isFailure()
      || getAccessor(sctHits, m_sctAccessor).isFailure()
      || getAccessor("eta", m_etaAccessor).isFailure())
    return StatusCode::FAILURE;
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
  return m_etaAccessor->getValue() <= m_etaCutoff
    || (m_pixAccessor->getValue() + m_sctAccessor->getValue() >= m_minSiHits);
}

// ---------------- MaxChiSq ----------------
InDet::MaxChiSq::MaxChiSq(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_accessor(nullptr)
{
}

StatusCode InDet::MaxChiSq::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  return getAccessor( "fitQuality", m_accessor );
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
InDet::MaxChiSqPerNdf::MaxChiSqPerNdf(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_maxValue(0)
  , m_accessor(nullptr)
{
}

StatusCode InDet::MaxChiSqPerNdf::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
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
InDet::MinProb::MinProb(InDet::InDetTrackSelectionTool* tool)
  : InDet::TrackCut(tool)
  , m_minValue(0)
  , m_accessor(nullptr)
{
}

StatusCode InDet::MinProb::initialize()
{
  if (!TrackCut::initialize().isSuccess())
    return StatusCode::FAILURE;
  return getAccessor( "fitQuality", m_accessor );
}

bool InDet::MinProb::result() const
{
  if (!m_accessor || !m_accessor->isValid()) {
    ATH_MSG_WARNING( "Invalid fit quality accessor. This track will not pass." );
    return false;
  }
  return TMath::Prob(m_accessor->getChiSq(), m_accessor->getNumberDoF()) >= m_minValue;
}
