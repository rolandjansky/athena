/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
// -*- c++ -*-
// InDetSecVtxTrackCuts.cxx
// implementations and definitions of cut objects

#include "InDetSecVtxTrackCut.h"
#include "InDetSecVtxTrackSelectionTool/InDetSecVtxTrackSelectionTool.h"

#include <algorithm>


// ---------------- SecVtxTrackCut ----------------
InDet::SecVtxTrackCut::SecVtxTrackCut(InDet::InDetSecVtxTrackSelectionTool* tool)
  : asg::AsgMessaging(tool)
  , m_trackAccessors(nullptr)
  , m_selectionTool(tool)
{
}

InDet::SecVtxTrackCut::~SecVtxTrackCut() = default;

// this function is a friend of the tool
StatusCode InDet::SecVtxTrackCut::initialize()
{
  if (!m_selectionTool) {
    ATH_MSG_ERROR( "Invalid pointer to InDetSecVtxTrackSelectionTool." );
    return StatusCode::FAILURE;
  }
  // let the cut have a handle on the tool's accessors
  m_trackAccessors = &(m_selectionTool->m_trackAccessors);
  ATH_MSG_INFO( "SecVtxTrackCut initialized ... " ) ;
  return StatusCode::SUCCESS;
}

// ---------------- D0minCut ----------------
InDet::D0minCut::D0minCut(InDet::InDetSecVtxTrackSelectionTool* tool, Double_t min)
  : InDet::SecVtxTrackCut(tool)
  , m_minValue(min)
  , m_paramAccessor(nullptr)
{
}
	
StatusCode InDet::D0minCut::initialize()
{
  ATH_CHECK( SecVtxTrackCut::initialize() );
  ATH_CHECK( getAccessor("d0", m_paramAccessor) );
  return StatusCode::SUCCESS;
}

bool InDet::D0minCut::result() const
{
  if (!m_paramAccessor) {
    ATH_MSG_WARNING( "Track parameter accessor not valid. Track will not pass this cut." );
    return false;
  }
  ATH_MSG_DEBUG( " trk D0 " << m_paramAccessor->getValue() << " vs D0minCut " << m_minValue ) ;
  return std::fabs( m_paramAccessor->getValue() ) >= m_minValue;
}

// ---------------- FuncSummaryValueCut ----------------
template <size_t N>
InDet::FuncSummaryValueCut<N>::FuncSummaryValueCut(InDet::InDetSecVtxTrackSelectionTool* tool,
             const std::array<xAOD::SummaryType,N>&& sumTypes)  : InDet::SecVtxTrackCut(tool)
	  , m_summaryTypes(sumTypes)
{
}

template <size_t N>
StatusCode InDet::FuncSummaryValueCut<N>::initialize()
{
 // first run the base class version to get access to the tool's accessors
  ATH_CHECK( SecVtxTrackCut::initialize() );

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

template class InDet::FuncSummaryValueCut<3>;
template class InDet::FuncSummaryValueCut<4>;
