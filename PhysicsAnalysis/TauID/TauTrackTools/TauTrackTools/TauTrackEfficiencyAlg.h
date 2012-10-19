/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauTrackEfficiencyAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TAUTRACKTOOLS_TAUTRACKEFFICIENCYALG_H
#define TAUTRACKTOOLS_TAUTRACKEFFICIENCYALG_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
//#include <vector>


//class StoreGateSvc;
class ITHistSvc;
class TruthParticle;
class TH3I;
class TH2I;

namespace Trk {
    class ITrackToVertexIPEstimator;
    class ITrackSelectorTool;
}
namespace Rec {
    class TrackParticle;
}

namespace TauID {
    class TruthTau;

/** @class TauTrackEfficiencyAlg

    This algorithms compares a TruthTau collection with a TrackParticle container and calculates tracking efficiencies
    for tracks from tau decays. Tracks can be selected using a TrackSelectorTool.

    @author  Sebastian Fleischmann <Sebastian.Fleischmann@cern.ch>
*/

class TauTrackEfficiencyAlg : public AthAlgorithm {
public:

    /** Standard Athena-Algorithm Constructor */
    TauTrackEfficiencyAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~TauTrackEfficiencyAlg();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

private:
    enum TruthTauHistType {
        TruthOneProng=0,
        TruthOneProngConversions=1,
        TruthThreeProng=2,
        TruthThreeProngConversions=3,
        TruthKaon=4,
        TruthKaonConversions=5,
        NumberOfTruthTauHistTypes=6
    };

    /** member variables for algorithm properties: */
    std::string m_inputTruthTauContainerName;               //!< SG key of the input truth tau container
    std::string m_inputTrackParticleTruthCollectionName;    //!< SG key of the input track particle truth collection
    std::string m_inputTrackParticleContainerName;          //!< SG key of the input track particle container
    std::string m_inputPrimaryVertexCollection;             //!< SG key of the input primary vertex collection
    std::string m_outputRootTreeName;                       //!< tree and dir name of the output ROOT file

    ITHistSvc*  m_thistSvc;                                 //!< handle on Hist/TTree registration service
    ToolHandle<Trk::ITrackSelectorTool> m_trackSelectorTool;//!< handle to ITrackSelectorTool
    ToolHandle<Trk::ITrackToVertexIPEstimator>   m_trackToVertexIPEstimator; //!< handle to TrackToVertexIPEstimator

    double      m_minTrackMatchProbability;                 //!< minimum probability for valid track to truth match
    bool        m_ignorePionsWithHadronicInteractions;      //!< ignore pions which had hadronic interactions?

    // output histograms:
    TH3I*       m_truthPionEtaPtHist;
    TH3I*       m_matchedPionEtaPtHist;
    TH3I*       m_recoPionEtaPtHist;
    TH3I*       m_truthPionEtaPtVisHist;
    TH3I*       m_matchedPionEtaPtVisHist;
    TH3I*       m_recoPionEtaPtVisHist;
    int         m_ptBins;
    double      m_ptMin;
    double      m_ptMax;
    int         m_ptVisBins;
    double      m_ptVisMin;
    double      m_ptVisMax;
    int         m_etaBins;
    double      m_etaMin;
    double      m_etaMax;

    TH3I*       m_truthPionDRHist;
    TH3I*       m_matchedPionDRHist;
    int         m_dRBins;
    double      m_dRMin;
    double      m_dRMax;

    TH2I*       m_recoD0Hist;
    int         m_d0Bins;
    double      m_d0Min;
    double      m_d0Max;
    TH2I*       m_recoD0SignfHist;
    int         m_d0SignfBins;
    double      m_d0SignfMin;
    double      m_d0SignfMax;

    TH2I*       m_trackNumberMigrationHist;
    int         m_trackNumberMax;
    double      m_tauConeSize;

    StatusCode fillMatchedTrackData(
                                    const TauID::TruthTau& truthTau,
                                    const TruthParticle& truthChargedPion,
                                    const Rec::TrackParticle& track);
    StatusCode fillTruthChargedPionData(
                                    const TauID::TruthTau& truthTau,
                                    const TruthParticle& truthChargedPion);
    double getMinDRPiPi(
                                    const TauID::TruthTau& truthTau,
                                    const TruthParticle& truthChargedPion);
    TruthTauHistType getTruthTauType(const TauID::TruthTau& truthTau);
};
} // end of namespace

#endif //TAUTRACKTOOLS_TAUTRACKEFFICIENCYALG_H
