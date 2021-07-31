/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_qjets_header
#define jetsubstructureutils_qjets_header

#include <queue>
#include <vector>
#include <list>
#include <algorithm>
#include "fastjet/JetDefinition.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"


namespace JetSubStructureUtils {
  struct jet_distance{
    double dij;
    int j1;
    int j2;
  };

  class JetDistanceCompare {
    public:
      JetDistanceCompare() {};
      bool operator() (const jet_distance& lhs, const jet_distance &rhs) const {return lhs.dij > rhs.dij;};
  };

  class Qjets {
    private:
      bool m_rand_seed_set;
      unsigned int m_seed = 0U;
      double m_zcut, m_dcut, m_dcut_fctr, m_exp_min, m_exp_max, m_rigidity, m_truncation_fctr;
      std::map<int,bool> m_merged_jets;
      std::priority_queue <jet_distance, std::vector<jet_distance>, JetDistanceCompare> m_distances;

      double d_ij(const fastjet::PseudoJet& v1, const fastjet::PseudoJet& v2) const; 
      void ComputeDCut(fastjet::ClusterSequence & cs);

      double Rand();
      bool Prune(jet_distance& jd,fastjet::ClusterSequence & cs);
      bool JetsUnmerged(const jet_distance& jd) const;
      bool JetUnmerged(int num) const;
      void ComputeNewDistanceMeasures(fastjet::ClusterSequence & cs, int new_jet);
      void ComputeAllDistances(const std::vector<fastjet::PseudoJet>& inp);  
      double ComputeMinimumDistance();
      double ComputeNormalization(double dmin);
      jet_distance GetNextDistance();
      bool Same(const jet_distance& lhs, const jet_distance& rhs);

    public:
      Qjets(double zcut, double dcut_fctr, double exp_min, double exp_max, double rigidity, double truncation_fctr);
      void Cluster(fastjet::ClusterSequence & cs);
      void SetRandSeed(unsigned int seed); /* In case you want reproducible behavior */
  };
}
#endif
