/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NOISECORRISOL_H
#define NOISECORRISOL_H

//*******************************************************//
// Name: NoiseCorrIsol.h                                 //
// Author: Michel Trottier-McDonald <mtm@cern.ch>        //
// Description: A class to provide isolation based       //
// pileup/underlying event correction to the clusters    //
// of tau decays                                         //
//*******************************************************//

#include <TLorentzVector.h>
#include "TauDiscriminant/EMFCluster.h"
#include <vector>

class NoiseCorrIsol
{
public:
    // Constructors, Destructors
    NoiseCorrIsol();

    NoiseCorrIsol(const TLorentzVector& tau,
		  const std::vector<TLorentzVector>& clusters,
		  double innerDR=0.3,
		  double outerDR=0.4,
		  bool eff=false);

    NoiseCorrIsol(const TLorentzVector& tau,
		  const std::vector<EMFCluster>& clusters,
		  double innerDR=0.3,
		  double outerDR=0.4,
		  bool eff=false);

    ~NoiseCorrIsol();

    //Getters
    std::vector<TLorentzVector> correctedClustersTLV();
    std::vector<EMFCluster> correctedClusters();

private:

    std::vector<TLorentzVector> m_clusters;
    std::vector<EMFCluster> m_EMFClusters;

    TLorentzVector m_tau;

    double m_innerDR;
    double m_outerDR;
    double m_areaRatio;

    double areaRatio();
    std::vector<EMFCluster> effClusters(const std::vector<EMFCluster>& inputClusters);

    bool m_eff;

};

#endif
