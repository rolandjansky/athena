/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigVertexCounts.h"

//---------------------------------------------------------------

TrigVertexCounts::TrigVertexCounts(): m_vtxNtrks(),
                                      m_vtxTrkPtSqSum() {}

//---------------------------------------------------------------

TrigVertexCounts::TrigVertexCounts(const std::vector<unsigned int>& vtxNtrks,
				   const std::vector<float>& vtxTrkPtSqSum)
  : m_vtxNtrks(vtxNtrks),
    m_vtxTrkPtSqSum(vtxTrkPtSqSum)
{
}

//---------------------------------------------------------------

TrigVertexCounts::TrigVertexCounts(std::vector<unsigned int>&& vtxNtrks,
				   std::vector<float>&& vtxTrkPtSqSum)
  : m_vtxNtrks(std::move(vtxNtrks)),
    m_vtxTrkPtSqSum(std::move(vtxTrkPtSqSum))
{
}

//---------------------------------------------------------------

TrigVertexCounts::~TrigVertexCounts() {
}

//---------------------------------------------------------------
