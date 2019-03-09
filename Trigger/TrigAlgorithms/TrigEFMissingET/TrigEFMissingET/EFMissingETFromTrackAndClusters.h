/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMissingETFromTrackAndClusters_H
#define TRIGEFMISSINGET_EFMissingETFromTrackAndClusters_H

/********************************************************************

NAME:     EFMissingETFromTrackAndClusters.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Renjie Wang (renjie.wang@cern.ch)
          Use tracks and vertices to remove pile-up clusters by Constituent-level pile-up mitigation techniques
          (cluster vertex fraction, Voronoi Subtraction, SofterKiller )
          the track-based soft term is also added to MET

CREATED:  Aug 05, 2018

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"



#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include <fastjet/tools/Subtractor.hh>
#include "fastjet/contrib/SoftKiller.hh"


/**
  $class EFMissingETFromTrackAndClusters
  Updates transient helper object with topo. clusters
 **/

class EFMissingETFromTrackAndClusters : public EFMissingETBaseTool {
public:

    EFMissingETFromTrackAndClusters(const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);

    ~EFMissingETFromTrackAndClusters();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute();

    virtual StatusCode execute(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const xAOD::CaloClusterContainer *caloCluster,
                               const xAOD::JetContainer *jets,
                               const xAOD::TrackParticleContainer *track,
                               const xAOD::VertexContainer *vertex,
                               const xAOD::MuonContainer *muon);


    void setCVF(std::vector<xAOD::CaloCluster>, const xAOD::TrackParticleContainer*, std::vector<const xAOD::TrackParticle*>, std::vector<float>&);
    float ExtrapolationEstimate(float,float,float);

    StatusCode makeVoronoiClusters(std::vector<fastjet::PseudoJet>& clusters, std::vector< std::pair<fastjet::PseudoJet,std::vector<float> > >&) const;
    void spreadPt(std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec, float spreadr=0.4, float alpha=2) const;

    int calculateWeight(xAOD::CaloCluster cl) const;
    int calculateSplitWeight(xAOD::CaloCluster cl) const;
    void RunClusters(std::vector<xAOD::CaloCluster> m_clust);
    void RunSplitClusters(std::vector<xAOD::CaloCluster> m_clust);



private:

    double findMinPt(std::vector<fastjet::PseudoJet> *clustSK) const;
    std::vector<fastjet::PseudoJet> makeSKClust(std::vector<fastjet::PseudoJet> clustPJ) const;


    bool  m_saveUncalibrated;
    int   m_metHelperPosition;
    xAOD::CaloCluster_v1::State m_clusterstate;

    bool m_debug = false;
    bool m_doLC = true;
    bool m_doSpread = true;
    int m_nSigma = 0;


    //mutable int m_initCount;
    float m_lambdaCalDivide;

    // Properties.
    bool m_isCaloSplit;
    float m_gridSpacing;
    float m_eCalGrid;
    float m_hCalGrid;
    float m_rapmin;
    float m_rapmax;
    float m_rapminApplied;
    float m_rapmaxApplied;
    double m_minPt;
    double m_minPtECal;
    double m_minPtHCal;


   float m_deltaR;
};

#endif // TRIGEFMISSINGET_EFMissingETFromTrackAndClusters_H
