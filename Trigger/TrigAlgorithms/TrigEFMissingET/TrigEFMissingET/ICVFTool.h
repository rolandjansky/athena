/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   ICVFTool
 *
 * @brief Interface for CVF helper
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_ICVFTOOL_H
#define TRIGEFMISSINGET_ICVFTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"

class ICVFTool : public virtual asg::IAsgTool {
  ASG_TOOL_INTERFACE( ICVFTool )
  public:
    /// Virtual destructor
    virtual ~ICVFTool() {}

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
        const xAOD::Vertex* priVtx) = 0;

    /**
     * @brief Name of the track links
     * If not empty, the tool will link all associated tracks to the clusters.
     */
    virtual std::string trackLinkName() const = 0;
}; //> end class ICVFTool

#endif //> !TRIGEFMISSINGET_ICVFTOOL_H
