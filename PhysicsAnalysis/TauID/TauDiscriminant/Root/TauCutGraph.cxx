/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Class for storing TGraph2D for score flattening
 *
 * Author: Lorenz Hauswald
 */

#include "TauDiscriminant/TauCutGraph.h"
#include <algorithm>

TauCutGraph::TauCutGraph(std::unique_ptr<TGraph2D> graph):
  m_graph(std::move(graph))
{
  m_efficiency = std::stod(static_cast<std::string>(m_graph->GetTitle()));
}

TauCutGraph::TauCutGraph(TauCutGraph&& graph):
  m_graph(std::move(graph.m_graph)),
  m_efficiency(graph.m_efficiency),
  m_cache_pT(graph.m_cache_pT),
  m_cache_nVtx(graph.m_cache_nVtx),
  m_cache_cut(graph.m_cache_cut)
{

}

TauCutGraph& TauCutGraph::operator=(TauCutGraph&& graph){
  if(this != &graph){
    m_graph = std::move(graph.m_graph);
    m_efficiency = graph.m_efficiency;
    m_cache_pT = graph.m_cache_pT;
    m_cache_nVtx = graph.m_cache_nVtx;
    m_cache_cut = graph.m_cache_cut;
  }
  return *this;
}

double TauCutGraph::GetCut(double pT, double nVtx) const {
  pT = std::max(pT, m_graph->GetXmin());
  pT = std::min(pT, m_graph->GetXmax());
  nVtx = std::max(nVtx, m_graph->GetYmin());
  nVtx = std::min(nVtx, m_graph->GetYmax());
  if(pT == m_cache_pT && nVtx == m_cache_nVtx) return m_cache_cut;
  m_cache_pT = pT;
  m_cache_nVtx = nVtx;
  m_cache_cut = m_graph->Interpolate(pT, nVtx);
  return m_cache_cut;
}

double TauCutGraph::GetEfficiency() const {
  return m_efficiency;
}

bool TauCutGraph::operator<(const TauCutGraph& other) const {
  return m_efficiency < other.GetEfficiency();
}
