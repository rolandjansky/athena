/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauEventTPCnv/TauJetContainer_p1_old_cnv.h"
#include "tauEvent/TauJetParameters.h"

void TauJetContainer_p1_old_cnv::Convert (TauJetContainer_p1* newobj,
                                          const TauJetContainer_p1_old* oldobj)
{
  newobj->clear();
  size_t n = oldobj->m_momenta.size();
  newobj->resize (n);
  for (size_t i = 0; i < n; i++)
  {
    TauJet_p1& tau = (*newobj)[i];
    tau.m_momentum = oldobj->m_momenta[i];
    tau.m_particleBase = oldobj->m_particles[i];
    tau.m_cluster = oldobj->m_clusters[i];
    tau.m_tracks = oldobj->m_tracks[i];
    //tau.m_cellCluster = XXX;
    //tau.m_tauDetails = xxx;

    tau.m_flags |= 1;
    tau.m_vetoFlags = 0;

    typedef std::pair<int, double> ppair;
    const std::vector<double>& pold = oldobj->m_parameters[i];
    std::vector<ppair>& pnew = tau.m_params;
    if (oldobj->m_authors[i] == TauJetParameters::tauRec) {
      tau.m_flags |= 0x04;
      pnew.push_back(ppair(TauJetParameters::etHadCalib,          pold[ 0]));
      pnew.push_back(ppair(TauJetParameters::etEMCalib,           pold[ 1]));
      pnew.push_back(ppair(TauJetParameters::emRadius,            pold[ 2]));
      pnew.push_back(ppair(TauJetParameters::isolationFraction,   pold[ 3]));
      pnew.push_back(ppair(TauJetParameters::centralityFraction,  pold[ 4]));
      pnew.push_back(ppair(TauJetParameters::stripWidth2,         pold[ 5]));
      pnew.push_back(ppair(TauJetParameters::nStripCells,         pold[ 6]));
      pnew.push_back(ppair(TauJetParameters::logLikelihoodRatio,  pold[ 7]));
      pnew.push_back(ppair(TauJetParameters::lowPtTauJetDiscriminant,pold[8]));
      pnew.push_back(ppair(TauJetParameters::lowPtTauEleDiscriminant,pold[9]));
      pnew.push_back(ppair(TauJetParameters::tauJetNeuralnetwork, pold[10]));
      pnew.push_back(ppair(TauJetParameters::tauENeuralNetwork,   pold[11]));

    }
    else if (oldobj->m_authors[i] == TauJetParameters::tau1P3P) {
      tau.m_flags |= 0x08;
      const std::vector<double>& pold = oldobj->m_parameters[i];
      pnew.push_back(ppair(TauJetParameters::etHadCalib,         pold[ 0] ));
      pnew.push_back(ppair(TauJetParameters::etEMCalib,          pold[ 1] ));
      pnew.push_back(ppair(TauJetParameters::emRadius,           pold[ 2] ));
      pnew.push_back(ppair(TauJetParameters::isolationFraction,  pold[ 3] ));
      pnew.push_back(ppair(TauJetParameters::centralityFraction, pold[ 4] ));
      pnew.push_back(ppair(TauJetParameters::stripWidth2,        pold[ 5] ));
      pnew.push_back(ppair(TauJetParameters::nStripCells,        pold[ 6] ));
      pnew.push_back(ppair(TauJetParameters::logLikelihoodRatio, pold[ 7] ));
      pnew.push_back(ppair(TauJetParameters::annularIsolationFraction,pold[8]));
      pnew.push_back(ppair(TauJetParameters::etCaloAtEMScale,    pold[ 9] ));
      pnew.push_back(ppair(TauJetParameters::etChargedHadCells,  pold[10] ));
      pnew.push_back(ppair(TauJetParameters::etOtherEMCells,     pold[11] ));
      pnew.push_back(ppair(TauJetParameters::etOtherHadCells,    pold[12] ));
      pnew.push_back(ppair(TauJetParameters::discriminant,       pold[13] ));
    }
  }
}

