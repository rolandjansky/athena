/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_qjetsplugin_header
#define jetsubstructureutils_qjetsplugin_header

#include "fastjet/JetDefinition.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

namespace JetSubStructureUtils {
  class QjetsPlugin : public fastjet::JetDefinition::Plugin {
    private:
      bool m_rand_seed_set;
      unsigned int m_seed = 0U;
      //int _truncated_length;
      double m_zcut, m_dcut_fctr, m_exp_min, m_exp_max, m_rigidity, m_truncation_fctr;
    public:
      QjetsPlugin(double zcut, double dcut_fctr, double exp_min, double exp_max, double rigidity, double truncation_fctr = 0.);
      void SetRandSeed(unsigned int seed); /* In case you want reproducible behavior */
      double R() const;
      std::string description() const;
      void run_clustering(fastjet::ClusterSequence & cs) const;
  };
}

#endif
