/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   CVFTool
 *
 * @brief Helper tool to calculate CVF. Right now uses a very simple
 * extrapolation to associate clusters to tracks. Longer term TODO item is to
 * use a proper track extrapolation tool.
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_CVFTOOL_H
#define TRIGEFMISSINGET_CVFTOOL_H

#include "TrigEFMissingET/ICVFTool.h"
#include "AsgTools/AsgTool.h"
#include "AthLinks/ElementLink.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <memory>

class CVFTool : public virtual ICVFTool, public asg::AsgTool {
  ASG_TOOL_CLASS(CVFTool, ICVFTool)
  public:
    CVFTool(const std::string& name);

    virtual ~CVFTool() override;

    virtual StatusCode initialize() override;
    /**
     * @brief Calculate CVF for a given calo cluster container
     * @param clusters The cluster container
     * @param tracks The input tracks
     * @param priVtx The identified primary vertex
     *
     * After the calculation is done, the CVF and NCVF cluster moments will be
     * filled.
     */
    virtual StatusCode calculateCVF(
        xAOD::CaloClusterContainer* clusters,
        const xAOD::TrackParticleContainer* tracks,
        const xAOD::Vertex* priVtx) override;

    virtual std::string trackLinkName() const override { return m_trackClusLinkName; }

  private:
    // Properties
    /// Max DR to match clusters to extrapolated tracks
    float m_clusterMatchDR;
    /// Name to use to link tracks to clusters
    std::string m_trackClusLinkName;
    // Track extrapolation parameters
    /// Track extrapolation quartic term
    float m_trackExtrapolationQuartic;
    /// Track extrapolation quadratic term
    float m_trackExtrapolationQuadratic;
    /// Track extrapolation linear term
    float m_trackExtrapolationLinear;
    /// 'Legacy' mode to fit with what was done previously
    bool m_doLegacy;
    /// Only link PV tracks
    bool m_linkOnlyPV;

    // Internals
    using trackLink_t = ElementLink<xAOD::TrackParticleContainer>;
    using vecTrackLink_t = std::vector<trackLink_t>;
    using decTrackLinks_t = SG::AuxElement::Decorator<vecTrackLink_t>;
    /// The link decorator
    std::unique_ptr<decTrackLinks_t> m_trackClusLinks;
    /// Extrapolate a track in DPhi
    float estimateExtrapolation(const xAOD::TrackParticle* track);
}; //> end class CVFTool
#endif //> !TRIGEFMISSINGET_CVFTOOL_H
