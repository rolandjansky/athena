/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TrkToolInterfaces/IBoundaryCheckTool.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetBoundaryCheckTool/InDetBoundaryCheckTool.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

InDet::InDetBoundaryCheckTool::InDetBoundaryCheckTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p
):
    AthAlgTool(t, n, p),
    m_geoModelSvc("GeoModelSvc", n),
    m_pixelLayerTool("InDet::InDetTestPixelLayerTool"),
    m_atlasId(nullptr)
{
    declareInterface<IBoundaryCheckTool>(this);
    declareProperty("GeoModelService", m_geoModelSvc);
    declareProperty("PixelLayerTool", m_pixelLayerTool);
}

StatusCode InDet::InDetBoundaryCheckTool::initialize() {
    StatusCode sc = AlgTool::initialize();

    if (sc.isFailure()) return sc;

    ATH_CHECK(detStore()->retrieve(m_atlasId, "AtlasID"));

    ATH_CHECK(m_pixelLayerTool.retrieve(DisableTool{!m_usePixel.value()}));
    ATH_CHECK(m_sctCondSummaryTool.retrieve(DisableTool{!m_useSCT.value()}));

    if (m_checkBadSCT.value()) {
        /*
         * Check if ITk Strip is used because isBadSCTChipStrip method is
         * valid only for SCT.
         */
        ATH_CHECK(m_geoModelSvc.retrieve());

        if (m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN4 or
            m_geoModelSvc->geoConfig()==GeoModel::GEO_ITk) {
            m_checkBadSCT.set(false);
            ATH_MSG_WARNING("Since ITk Strip is used, m_check_bad_sct is turned off.");
        }
    }

    ATH_MSG_INFO("InDeTBoundaryCheckTool::initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

StatusCode InDet::InDetBoundaryCheckTool::finalize() {
    return AlgTool::finalize();
}

bool InDet::InDetBoundaryCheckTool::isAlivePixel(
    [[maybe_unused]] const InDetDD::SiDetectorElement &element,
    const Trk::TrackParameters &parameters
) const {
    return m_pixelLayerTool->expectHit(&parameters);
}

bool InDet::InDetBoundaryCheckTool::isAliveSCT(
    const InDetDD::SiDetectorElement &element,
    const Trk::TrackParameters &parameters
) const {
    if (m_checkBadSCT.value() && isBadSCTChipStrip(element.identify(), parameters, element)) {
        return false;
    }

    return m_sctCondSummaryTool->isGood(element.identifyHash());
}

Trk::BoundaryCheckResult InDet::InDetBoundaryCheckTool::boundaryCheckSiElement(
    const InDetDD::SiDetectorElement &siElement,
    const Trk::TrackParameters &parameters
) const {
    /*
     * We're supporting SCT and Pixel elements. Some of the checking code can
     * be shared, and some of it is different for these detector types. This
     * method will perform all the shared checks that apply to both SCTs and
     * pixels.
     */
    double phitol;
    double etatol;

    /*
     * We set our \phi and \eta tolerances by scaling the configured value in
     * the tool's parameters by the standard deviation of the relative
     * positions. If we do not have a covariance matrix to gather these \sigma
     * values from, we use default parameters of 2.5 for \phi and 5.0 for
     * \eta.
     */
    if (parameters.covariance() != nullptr) {
        /*
         * From the covariance matrix, we grab the diagonal to get the
         * variance vector. Then, if we take the component-wise square root
         * of that, we get a vector of \sigma. Then all we need to do is use
         * the right indexes to retrieve the values for \eta and \phi.
         */
        Eigen::Matrix<double, 5, 1> var = parameters.covariance()->diagonal().cwiseSqrt();
        etatol = m_etaTol.value() * var(Trk::iEta);
        phitol = m_phiTol.value() * var(Trk::iPhi);
    } else {
        /*
         * If we don't have the covariance matrix, set our defaults. In the
         * future, these two default values could be lifted into separate
         * configuration parameters.
         */
        phitol = 2.5;
        etatol = 5;
    }


    /*
     * Next, we determine whether the hit is in the active region of the
     * element using the SiDetectorElement::inDetector() method followed by
     * the SiIntersect::in() method.
     */
    InDetDD::SiIntersect intersection = siElement.inDetector(
        parameters.localPosition(), phitol, etatol
    );

    /// catch if we are on the edge of an active element
    if (intersection.nearBoundary()){
        
        /*
         * If we are around the boundary, we return a special state which 
         * will not be counted as a hole or missing hit in the pattern,
         * while still being recorded on the trajectory for later 
         * refinement. 
         */
        ATH_MSG_VERBOSE(
            "Track parameter on the module edge"
        );
        return Trk::BoundaryCheckResult::OnEdge; 

    }

    if (intersection.out()) {
        /*
         * In this case, we are _not_ inside the active region of the element.
         */
        ATH_MSG_VERBOSE(
            "Track parameter not inside (active?) detector within " <<
            phitol << " " << etatol 
        );
        return Trk::BoundaryCheckResult::Outside;
    }

    /* Now, to proceed further we need to confirm that the module is actually active 
    */ 

    Identifier id = siElement.identify();

    bool alive;

    /*
     * If we have not yet determined our hit to lie in any inactive region, we
     * check whether the module hit is actually alive. For this, we deletage
     * to two dedicated methods which essentially wrap external tools which
     * know about element states.
     */
    if (m_usePixel.value() && m_atlasId->is_pixel(id)) {
        alive = isAlivePixel(siElement, parameters);
    } else if (m_useSCT.value() && m_atlasId->is_sct(id)) {
        alive = isAliveSCT(siElement, parameters);
    } else {
        ATH_MSG_WARNING("Unsupported identifier type! "+m_atlasId->print_to_string(id));
        return Trk::BoundaryCheckResult::Error;
    }

    /*
     * We now have all the necessary information to make a judgement about the
     * track parameters.
     */
    if (alive) {
        
        /*
        * now, we check whether the local position on the silicon element is
        * near the bonding gap of the module, which is insensitive. For this, we
        * can simply delegate to the SiDetectorElement::newBondGap() method.
        *
        * Keen-eyed readers may note that bond gaps are only relevant for SCT
        * modules and that Pixels do not have them. Therefore, we can technically
        * consider this a check relevant only to SCTs. However, as this logic is
        * abstracted into the SiDetectorElement class, we will treat is as a
        * shared property and a shared check.
        */
        if (siElement.nearBondGap(parameters.localPosition(), etatol )) {
            ATH_MSG_VERBOSE(
                "Track parameter on bond gap within " << etatol << ", so hit " <<
                "is on an insensitive part of the element."
            );
            return Trk::BoundaryCheckResult::Insensitive;
        }
        /*
         * If the module is alive and we hit the active region on it, we know
         * we have a good hit. Note that this is the only way we can return a
         * Candidate result! It's the only success state.
         */
        ATH_MSG_VERBOSE("Module is good, and we're hitting a sensitive part!");
        return Trk::BoundaryCheckResult::Candidate;

    } 
    else {
        /*
         * Finally, if the module is not alive, we simply return a DeadElement
         * result.
         */
        ATH_MSG_VERBOSE("Track is hiting a bad module!");
        return Trk::BoundaryCheckResult::DeadElement;
    }
}

Trk::BoundaryCheckResult InDet::InDetBoundaryCheckTool::boundaryCheck(
    const Trk::TrackParameters &parameters
) const {
    /*
     * Retrieve the detector element associated with our track parameters. If
     * such an element does not exist for whatever reason, return a negative
     * result.
     */
    const Trk::TrkDetElementBase *element = parameters.associatedSurface().associatedDetectorElement();

    if (element == nullptr) {
        return Trk::BoundaryCheckResult::Error;
    }

    /*
     * Try to see if detector element is a silicon element, otherwise return a
     * negative result: this tool is not designed to work on non-silicon
     * elements.
     */
    const InDetDD::SiDetectorElement *siElement = dynamic_cast<const InDetDD::SiDetectorElement *>(element);

    if (siElement != nullptr) {
        return boundaryCheckSiElement(*siElement, parameters);
    } else {
        ATH_MSG_DEBUG("TrackParameters do not belong to a type of element we can process");
        return Trk::BoundaryCheckResult::Error;
    }
}

bool InDet::InDetBoundaryCheckTool::isBadSCTChipStrip(
    const Identifier &waferId,
    const Trk::TrackParameters &parameters,
    const InDetDD::SiDetectorElement &siElement
) const {
    // Check if the track passes through a bad SCT ABCD chip or a bad SCT strip.
    // A chip and a strip are determined by the parameter position.
    // Algorithm is based on InnerDetector/InDetMonitoring/SCT_Monitoring/src/SCTHitEffMonTool.cxx

    // Check the input.
    if (!m_atlasId->is_sct(waferId)) {
        ATH_MSG_ERROR(waferId << " is not an SCT Identifier");
        return true;
    }

    // Get strip id from local position.
    // Due to the limited position resolution, we may pick up a neighboring strip...
    const Amg::Vector2D localPos(parameters.localPosition());
    const Identifier stripIdentifier(siElement.identifierOfPosition(localPos));

    if (!m_atlasId->is_sct(stripIdentifier)) {
        ATH_MSG_WARNING(stripIdentifier << " is not an SCT Identifier");
        return true;
    }

    if (!m_sctCondSummaryTool->isGood(stripIdentifier, InDetConditions::SCT_CHIP)) {
        // The position is on a bad chip.
        return true;
    } else if (!m_sctCondSummaryTool->isGood(stripIdentifier, InDetConditions::SCT_STRIP)) {
        // The position is on a bad strip. (We may need to check neighboring strips.)
        return true;
    }

    return false;
}
