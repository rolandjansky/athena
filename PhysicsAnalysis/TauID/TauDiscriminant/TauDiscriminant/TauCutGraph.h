/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This is a class to store TGraph2D for the score flattening
/*!
 * Class for TGraph2D for score flattening
 *
 * Author: Lorenz Hauswald
 */

#ifndef TAUCUTGRAPH_H
#define TAUCUTGRAPH_H

#include "TGraph2D.h"
#include <memory>

class TauCutGraph
{
 public:
  TauCutGraph(std::unique_ptr<TGraph2D> graph);

  TauCutGraph(const TauCutGraph&) = delete;
  TauCutGraph& operator=(const TauCutGraph&) = delete;

  TauCutGraph(TauCutGraph&& graph);
  TauCutGraph& operator=(TauCutGraph&& graph);
  
  double GetCut(double pT, double nVtx) const;
  
  double GetEfficiency() const;

  bool operator<(const TauCutGraph& other) const;

 private:
  std::unique_ptr<TGraph2D> m_graph;
  double m_efficiency = -1.;
  mutable double m_cache_pT = 0.;
  mutable double m_cache_nVtx = 0.;
  mutable double m_cache_cut = 0.;
};

#endif
