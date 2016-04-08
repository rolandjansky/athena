/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackPatternsHolder.h 689837 2015-08-17 17:22:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_TRACKPATTERNSHOLDER_H
#define RINGERSELECTORTOOLS_TOOLS_TRACKPATTERNSHOLDER_H

#include "RingerSelectorTools/tools/cxx/RingerUseNewCppFeatures.h"

// STL includes:
#include <vector>
#include <cstddef>
#if RINGER_USE_NEW_CPP_FEATURES
#include <array>
#endif

// xAOD framework includes:
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODEgamma/ElectronFwd.h"

// Local includes:
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/RedirectMsgStream.h"

namespace Ringer {

/**
 * @class TrackPatterns
 * @brief Hold Ringer Track needed patterns.
 *
 *  This class will extract the Ringer needed Track patterns from TrackParticle
 *  and Electron (if available). It is used afterwards to extract this patterns
 *  into the containers (std::vector<float>) used by the classifiers.
 **/
class TrackPatternsHolder : public RedirectMsgStream {

  public:

    /**
     * @brief Standard ctor:
     **/
    TrackPatternsHolder(
        const bool useBLOutliers,
        const bool usePIXOutliers,
        const bool useSCTOutliers,
        const bool useTRTOutliers,
        const bool useTRTXenonHits):
      m_useBLOutliers(useBLOutliers),
      m_usePIXOutliers(usePIXOutliers),
      m_useSCTOutliers(useSCTOutliers),
      m_useTRTOutliers(useTRTOutliers),
      m_useTRTXenonHits(useTRTXenonHits),
      m_nBL(0),
      m_nBLOutliers(0),
      m_nPi(0),
      m_nPiOutliers(0),
      m_nSCT(0),
      m_nSCTOutliers(0),
      m_nTRThigh(0),
      m_nTRThighOutliers(0),
      m_nTRT(0),
      m_nTRTOutliers(0),
      m_nTRTXenonHits(0),
      m_expectHitInBLayer(0),
      m_trackd0(0),
      m_deltaeta(0),
      m_deltaphi(0),
      m_ep(0),
      m_energy(0),
      m_eta(0){;}

    /**
     * @brief Extrack Track Patterns from Particles
     **/
    void extractPatternsFrom(
        const xAOD::TrackParticle *track,
        const xAOD::Electron *el );

    /**
     * @brief Export Track Patterns to @name vec end
     *
     * We apply transformations on the raw patterns extracted to obtain the
     * true patterns applied. This method must be called after 
     * extrackPatternsFrom method is called.
     **/
    void exportPatternsTo( std::vector<float> &vec ) const;

    /**
     * @brief Export the number of patterns that will be exported to vector.
     **/
    static size_t numberOfPatterns();

  private:

    /// Private properties (configuration):
    ///@{
    /// Flag whether to use B-layer outliers:
    const bool m_useBLOutliers;
    /// Flag whether to use Pixel outliers:
    const bool m_usePIXOutliers;
    /// Flag whether to use SCT outliers:
    const bool m_useSCTOutliers;
    /// Flag whether to use TRT outliers:
    const bool m_useTRTOutliers;
    /// Flag whether to use TRT Xenon hits:
    const bool m_useTRTXenonHits;
    ///@}

    /// Private Properties:
    ///@{
    /// Number of B-Layer hits:
    uint8_t m_nBL;
    /// Number of B-Layer outliers:
    uint8_t m_nBLOutliers;
    /// Number of Pixel hits:
    uint8_t m_nPi;
    /// Number of Pixel outliers:
    uint8_t m_nPiOutliers;
    /// Number of SCT hits:
    uint8_t m_nSCT;
    /// Number of SCT outliers:
    uint8_t m_nSCTOutliers;
    /// Number of TRT high threshold hits:
    uint8_t m_nTRThigh;
    /// Number of TRT high threshold outliers:
    uint8_t m_nTRThighOutliers;
    /// Number of TRT hits:
    uint8_t m_nTRT;
    /// Number of TRT outliers:
    uint8_t m_nTRTOutliers;
    /// Number of TRT Xenon hits:
    uint8_t m_nTRTXenonHits;
    /// Whether to expect hit in BLayer:
    uint8_t m_expectHitInBLayer;
    /// Track transverse impact param:
    float m_trackd0;
    /// Track Calo deltaEta:
    float m_deltaeta;
    /// Track Calo deltaPhi:
    float m_deltaphi;
    /// Track Calo e/p:
    double m_ep;

    /// Cluster energy:
    double m_energy;
    /// Cluster eta:
    double m_eta;
    ///@}

    /// Static properties:
    ///@{
    /// Coefficients to aproximate Number of TRT hits:
    ///@{
#if RINGER_USE_NEW_CPP_FEATURES
    static constexpr double m_a0 = 33.14; 
    static constexpr double m_b0 = -129.1; 
    static constexpr double m_c0 = 1455.;
    static constexpr double m_a1 = 29.42; 
    static constexpr double m_b1 = 27.93; 
    static constexpr double m_c1 = -89.96; 
    static constexpr double m_d1 = 91.51;
    static constexpr double m_a2 = 196.3; 
    static constexpr double m_b2 = -403.; 
    static constexpr double m_c2 = 230.2;
    static constexpr double m_a3 = -10.59; 
    static constexpr double m_b3 = 37.29;
    static constexpr double m_a4 = -640.9; 
    static constexpr double m_b4 = 1323.; 
    static constexpr double m_c4 = -851.8; 
    static constexpr double m_d4 = 180.8;
    static constexpr double m_a5 = 159.8; 
    static constexpr double m_b5 = -70.9;
    static constexpr std::array<float,6> m_cutBinEta_TRT{{
      0.1, 0.625, 1.07, 1.304, 1.752, 2.0
    }};
#else
    static const double m_a0; 
    static const double m_b0; 
    static const double m_c0;
    static const double m_a1; 
    static const double m_b1; 
    static const double m_c1; 
    static const double m_d1;
    static const double m_a2; 
    static const double m_b2; 
    static const double m_c2;
    static const double m_a3; 
    static const double m_b3;
    static const double m_a4; 
    static const double m_b4; 
    static const double m_c4; 
    static const double m_d4;
    static const double m_a5; 
    static const double m_b5;
    static const std::vector<float> m_cutBinEta_TRT;
#endif
    ///@}
    ///@}

    /// Private Methods:
    ///@{
    /** Reset all values */
    void resetValues();
    /** get Track TRT variables */
    void getTRTVar(double &rTRT, double &estHitsTRT) const;
    ///@}

};

//==============================================================================
inline
size_t TrackPatternsHolder::numberOfPatterns()
{
  return 10;
}

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_TOOLS_TRACKPATTERNSHOLDER_H
