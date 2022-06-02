/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "StripSpacePointFormationTool.h"
#include "InDetIdentifier/SCT_ID.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_ReadoutGeometry/StripStereoAnnulusDesign.h"
#include "xAODInDetMeasurement/StripClusterAuxContainer.h"
#include "ActsUtils/ContainerAccessor.h"

namespace ActsTrk {

    StripSpacePointFormationTool::StripSpacePointFormationTool(const std::string& type,
                                                               const std::string& name,
                                                               const IInterface* parent)
    : base_class(type, name, parent)
    {}

    StatusCode StripSpacePointFormationTool::initialize() {

        ATH_CHECK(detStore()->retrieve(m_stripId,"SCT_ID"));

        ATH_CHECK(m_lorentzAngleTool.retrieve());

        return StatusCode::SUCCESS;
    }

    void StripSpacePointFormationTool::produceStripSpacePoints(const xAOD::StripClusterContainer& clusterContainer,
                                                               const InDet::SiElementPropertiesTable& properties,
                                                               const InDetDD::SiDetectorElementCollection& elements,
                                                               const Amg::Vector3D& beamSpotVertex,
                                                               ActsTrk::SpacePointContainer& spacePoints,
                                                               ActsTrk::SpacePointData& spacePointData,
                                                               ActsTrk::SpacePointMeasurementDetails& spacePointDetails,
                                                               ActsTrk::SpacePointContainer& overlapSpacePoints,
                                                               ActsTrk::SpacePointData& overlapSpacePointData,
                                                               ActsTrk::SpacePointMeasurementDetails& overlapSpacePointDetails,
                                                               bool processOverlaps) const
    {
        /// Production of ActsTrk::SpacePoint from strip clusters
        /// Strip space points involves a more complex logic since
        /// they are made combining clusters from pairs of
        /// overlapping detectors.

        /// For each trigger element, first evaluate and collect the quantities you need to build the space points.
        /// The detector element array has capacity 6:
        /// [0] is the trigger element
        /// [1] is the opposite element
        /// [2]-[3] are the elements tested for eta overlaps
        /// [4]-[5] are the elements tested for phi overlaps
        /// For each element you save the corresponding cluster collections and the
        /// space point compatibility range as described below.
        ///
        /// For the opposite element and the ones tested for eta overlaps, you have to check
        /// if clusters are compatible with the local position of the trigger cluster
        /// requiring that the distance between the two clusters in phi is withing a specified range.
        /// - For the clusters on the opposite element: [-m_overlapLimitOpposite, m_overlapLimitOpposite]
        ///
        /// - For the eta overlapping clusters : you use m_overlapLimitEtaMin and m_overlapLimitEtaMax
        ///   in different combination depending if you are checking a stereo module or not
        ///
        /// For each element, the extremes of these ranges are saved in the overlapExtents array
        /// which is used later on
        ///   - overlapExtents[0], overlapExtents[1] are filled for the opposite element
        ///   - overlapExtents[2], overlapExtents[3], overlapExtents[4], overlapExtents[5] are filled for the eta overlapping elements
        ///
        /// For the elements tested for phi overlaps, you have to check
        /// if clusters are compatible with the local position of the trigger cluster.
        /// This needs that the trigger cluster is at the edge of the trigger module:
        /// e.g. -hwidth < locX_trigger < -hwidth+m_overlapLimitPhi (or hwidth-m_overlapLimitPhi < locX_trigger < hwidth)
        /// and that the other cluster is on the compatible edge of its module:
        /// e.g. hwidth-m_overlapLimitPhi < locX_other < hwidth (or -hwidth < locX_other < -hwidth+m_overlapLimitPhi)
        ///
        /// For each element, the extremes of these ranges are saved in the overlapExtents array
        /// which is used later on
        ///   - overlapExtents[6], overlapExtents[7], overlapExtents[10], overlapExtents[11]
        ///     overlapExtents[8], overlapExtents[9], overlapExtents[12], overlapExtents[13] are filled for the phi overlapping elements

        /// Access to the cluster from a given detector element is possible
        /// via the ContainerAccessor.

        ContainerAccessor<xAOD::StripCluster, IdentifierHash, 1>
            stripAccessor ( clusterContainer,
                            [](const xAOD::StripCluster& cl) { return cl.identifierHash();},
                            elements.size());

        const auto& allIdHashes = stripAccessor.allIdentifiers();
        for (auto& idHash : allIdHashes) {
            const InDetDD::SiDetectorElement* thisElement = elements.getDetectorElement(idHash);
            if ( not thisElement->isStereo() ) {
                // Retrieve the neighbours of the detector element
                const std::vector<IdentifierHash>* others(properties.neighbours(idHash));
                if (others==nullptr || others->empty() )
                    continue;

                // This flag is use to trigger if the search should be performed.
                // In case there are no clusters on the neighbours of the selected
                // detector element, the flag stays false.
                bool search=false;
                size_t neighbour = 0;
                while (not search and neighbour<others->size() ) {
                    search = stripAccessor.isIdentifierPresent(others->at(neighbour));
                    neighbour++;
                }
                if (not search)
                    continue;

                // prepare clusters, indices and modules for space point formation
                std::array<std::vector<std::pair<const xAOD::StripCluster*, size_t>>, nNeighbours> neighbourClusters{};
                std::array<const InDetDD::SiDetectorElement*, nNeighbours> neighbourElements{};

                auto groupStart = clusterContainer.begin();
                // Get the detector element and range for the idHash
                neighbourElements[0] = thisElement;
                for (auto& this_range : stripAccessor.rangesForIdentifierDirect(idHash)) {
                    for (auto start=this_range.first; start!=this_range.second; start++) {
                        size_t position = std::distance(groupStart, start);
                        neighbourClusters[0].push_back(std::make_pair(*start, position));
                    }
                }

                Identifier thisId = thisElement->identify();

                // define overlap extends before building space points
                std::array<double, 14> overlapExtents{};
                //   Default case: you test the opposite element and the overlapping in phi (total 3 elements)
                int Nmax = 4;

                // In the barrel, test the eta overlaps as well (total 5 elements)
                if (m_stripId->is_barrel(thisId))
                    Nmax = 6;

                // You can remove all the overlaps if requested.
                // Here you test only the opposite element
                if(not processOverlaps) Nmax = 2;

                float hwidth(properties.halfWidth(idHash));
                int n = 0;


                // The order of the elements in others is such that you first get the opposite element,
                // the overlapping in phi and then the overlapping in eta
                // For this reason you need to re-order the indices, since the SiSpacePointMakerTool will process
                // first the eta overlaps and then the phi ones
                const std::array<size_t, nNeighbours> neigbourIndices{ThisOne, Opposite, EtaMinus, EtaPlus, PhiMinus, PhiPlus};

                for (const auto& otherHash : *others) {

                    if(++n==Nmax) break;

                    if(not stripAccessor.isIdentifierPresent(otherHash))
                        continue;

                    const InDetDD::SiDetectorElement* otherElement = elements.getDetectorElement(otherHash);

                    neighbourElements[neigbourIndices[n]] = otherElement;
                    for (auto& this_range : stripAccessor.rangesForIdentifierDirect(otherHash)) {
                        for (auto start=this_range.first; start!=this_range.second; start++) {
                            size_t position = std::distance(groupStart, start);
                            neighbourClusters[neigbourIndices[n]].push_back(std::make_pair(*start, position));
                        }
                    }

                    switch (n) {
                        case Opposite: {
                            overlapExtents[ 0] = -m_overlapLimitOpposite;
                            overlapExtents[ 1] =  m_overlapLimitOpposite;
                            break;
                        }
                        case PhiMinus: {
                            overlapExtents[ 6] =-hwidth;
                            overlapExtents[ 7] =-hwidth+m_overlapLimitPhi;
                            overlapExtents[ 8] = hwidth-m_overlapLimitPhi;
                            overlapExtents[ 9] = hwidth;
                            break;
                        }
                        case PhiPlus: {
                            overlapExtents[10] = hwidth-m_overlapLimitPhi;
                            overlapExtents[11] = hwidth;
                            overlapExtents[12] =-hwidth;
                            overlapExtents[13] =-hwidth+m_overlapLimitPhi;
                            break;
                        }
                        case EtaMinus: {
                            if ((m_stripId->layer_disk(thisId) & 1) == 0) {
                                overlapExtents[ 2] = m_overlapLimitEtaMin;
                                overlapExtents[ 3] = m_overlapLimitEtaMax;
                            } else {
                                overlapExtents[ 2] =-m_overlapLimitEtaMax;
                                overlapExtents[ 3] =-m_overlapLimitEtaMin;
                            }
                            break;
                        }
                        default: {
                            if ((m_stripId->layer_disk(thisId) & 1) == 0) {
                                overlapExtents[ 4] = -m_overlapLimitEtaMax;
                                overlapExtents[ 5] = -m_overlapLimitEtaMin;
                            } else {
                                overlapExtents[ 4] = m_overlapLimitEtaMin;
                                overlapExtents[ 5] = m_overlapLimitEtaMax;
                            }
                            break;
                        }
                    }
                }

                // producing and filling space points
                fillStripSpacePoints(neighbourElements, neighbourClusters, overlapExtents, beamSpotVertex,
                                     spacePoints, spacePointData, spacePointDetails,
                                     overlapSpacePoints, overlapSpacePointData, overlapSpacePointDetails);
            }
        }
    }

    void StripSpacePointFormationTool::fillStripSpacePoints(
        std::array<const InDetDD::SiDetectorElement*, nNeighbours> elements,
        std::array<std::vector<std::pair<const xAOD::StripCluster*, size_t>>, nNeighbours> clusters,
        std::array<double, 14> overlapExtents,
        const Amg::Vector3D& beamSpotVertex,
        ActsTrk::SpacePointContainer& spacePoints,
        ActsTrk::SpacePointData& spacePointData,
        ActsTrk::SpacePointMeasurementDetails& spacePointDetails,
        ActsTrk::SpacePointContainer& overlapSpacePoints,
        ActsTrk::SpacePointData& overlapSpacePointData,
        ActsTrk::SpacePointMeasurementDetails& overlapSpacePointDetails ) const
    {

        // This function is called once all the needed quantities are collected.
        // It is used to build space points checking the compatibility of clusters on pairs of detector elements.
        // Detector elements and cluster collections are elements and clusters, respectively.
        // [0] is the trigger element
        // [1] is the opposite element
        // [2]-[3] are the elements tested for eta overlaps
        // [4]-[5] are the elements tested for phi overlaps
        //
        // To build space points:
        // - For the opposite element and the ones tested for eta overlaps, you have to check
        //   if clusters are compatible with the local position of the trigger cluster
        //   requiring that the distance between the two clusters in phi is withing a specified range.
        //   - overlapExtents[0], overlapExtents[1] are filled for the opposite element
        //   - overlapExtents[2], overlapExtents[3], overlapExtents[4], overlapExtents[5] are filled for the eta overlapping elements
        // - For the elements tested for phi overlaps, you have to check
        //   if clusters are compatible with the local position of the trigger cluster.
        //   This needs that the trigger cluster is at the edge of the trigger module
        //   and that the other cluster is on the compatible edge of its module
        //   - overlapExtents[6], overlapExtents[7], overlapExtents[10], overlapExtents[11]
        //     overlapExtents[8], overlapExtents[9], overlapExtents[12], overlapExtents[13] are filled for the phi overlapping elements

        constexpr int otherSideIndex{1};
        constexpr int maxEtaIndex{3};
        std::array<int, nNeighbours-1> elementIndex{};
        int nElements = 0;

        // For the nNeighbours sides, fill elementIndex with the indices of the existing elements.
        // Same the number of elements in nElements to loop on the later on
        for(int n=1; n!=nNeighbours; ++n) {
            if(elements[n]) {
                elementIndex[nElements++] = n;
            }
        }
        // return if all detector elements are nullptr
        if(!nElements) return;

        // trigger element and clusters
        const InDetDD::SiDetectorElement* element = elements[0];
        bool isEndcap = element->isEndcap();

        std::vector<StripInformationHelper> stripInfos;
        stripInfos.reserve(clusters[0].size());

        // loop on all clusters on the trigger detector element and save the related information
        for (auto& cluster_index : clusters[0]) {
            size_t stripIndex = -1;
            auto ends = getStripEnds(cluster_index.first, element, stripIndex);
            const auto& localPos = cluster_index.first->localPosition<1>();
            StripInformationHelper stripInfo(ends.first, ends.second, beamSpotVertex, localPos(0, 0), cluster_index.second, stripIndex);
            stripInfos.push_back(stripInfo);
        }

        double  limit  = 1. + m_stripLengthTolerance;
        double slimit  = 0.;

        if(not m_allClusters) {
            // Start processing the opposite side and the eta overlapping elements
            int n = 0;
            for(; n < nElements; ++n) {
                int currentIndex = elementIndex[n];
                if(currentIndex > maxEtaIndex) break;

                // get the detector element and the IdentifierHash
                const InDetDD::SiDetectorElement* currentElement  = elements[currentIndex];

                // retrieve the range
                double min = overlapExtents[currentIndex*2-2];
                double max = overlapExtents[currentIndex*2-1];

                size_t minStrip, maxStrip = 0;

                if (m_stripGapParameter != 0.) {
                    updateRange(element, currentElement, slimit, min, max);
                    correctPolarRange(element, min, max, minStrip, maxStrip);
                }

                StripInformationHelper currentStripInfo;
                for (auto& cluster_index : clusters[currentIndex]) {
                    bool processed = false;
                    const auto& currentLocalPos = cluster_index.first->localPosition<1>();

                    for(auto& stripInfo : stripInfos) {
                        double diff = currentLocalPos(0, 0)-stripInfo.locX();

                        if(diff < min || diff > max) continue;

                        if (not processed) {
                            processed = true;
                            size_t currentStripIndex = 0;
                            auto ends = getStripEnds(cluster_index.first, currentElement, currentStripIndex);
                            currentStripInfo.set(ends.first, ends.second, beamSpotVertex, currentLocalPos(0, 0), cluster_index.second, currentStripIndex);
                        }

                        // depending on the index you are processing, you save the space point in the correct container
                        if (currentIndex==otherSideIndex) {
                            std::unique_ptr<ActsTrk::SpacePoint>
                                stripSpacePoint = makeStripSpacePoint(stripInfo, currentStripInfo, isEndcap, limit, slimit, spacePointData, spacePointDetails);
                            if (stripSpacePoint) {
                                spacePoints.push_back( std::move(stripSpacePoint) );
                            }
                        } else {
                            std::unique_ptr<ActsTrk::SpacePoint>
                                stripSpacePoint = makeStripSpacePoint(stripInfo, currentStripInfo, isEndcap, limit, slimit, overlapSpacePointData, overlapSpacePointDetails);
                            if (stripSpacePoint) {
                                overlapSpacePoints.push_back( std::move(stripSpacePoint) );
                            }
                        }
                    }
                }
            }
            // process the phi overlapping elements
            // if possible n starts from 4
            for(; n < nElements; ++n) {
                int currentIndex = elementIndex[n];
                const InDetDD::SiDetectorElement* currentElement = elements[currentIndex];

                double min = overlapExtents[4*currentIndex-10];
                double max = overlapExtents[4*currentIndex- 9];

                size_t minStrip, maxStrip = 0;

                if (m_stripGapParameter != 0.) {
                    updateRange(element, currentElement, slimit, min, max);
                    correctPolarRange(element, min, max, minStrip, maxStrip);
                }

                std::vector<StripInformationHelper*> stripPhiInfos;
                stripPhiInfos.reserve(stripInfos.size());

                for(auto& stripInfo : stripInfos) {
                    auto stripIndex = stripInfo.stripIndex();
                    auto localPosition = stripInfo.locX();
                    auto centralValue = localPosition;
                    auto minValue = min;
                    auto maxValue = max;
                    if (isEndcap) {
                        centralValue = stripIndex;
                        minValue = minStrip;
                        maxValue = maxStrip;
                    }

                    if (minValue <= centralValue and centralValue <= maxValue) {
                        stripPhiInfos.push_back(&stripInfo);
                    }
                }
                // continue if you have no cluster from the phi overlapping region of the trigger element
                if(stripPhiInfos.empty()) continue;

                min = overlapExtents[4*currentIndex-8];
                max = overlapExtents[4*currentIndex-7];

                if (m_stripGapParameter != 0.) {
                    updateRange(element, currentElement, slimit, min, max);
                    correctPolarRange(currentElement, min, max, minStrip, maxStrip);
                }

                for (auto& cluster_index : clusters[currentIndex]) {
                    const auto& currentLocalPos = cluster_index.first->localPosition<1>();

                    size_t currentStripIndex = 0;
                    auto ends = getStripEnds(cluster_index.first, currentElement, currentStripIndex);
                    StripInformationHelper currentStripInfo(ends.first, ends.second, beamSpotVertex, currentLocalPos(0, 0), cluster_index.second, currentStripIndex);
                    auto centralValue = currentLocalPos(0, 0);
                    auto minValue = min;
                    auto maxValue = max;
                    if (isEndcap) {
                        centralValue = currentStripIndex;
                        minValue = minStrip;
                        maxValue = maxStrip;
                    }

                    if (centralValue < minValue or centralValue > maxValue)
                        continue;

                    for(auto& stripInfo : stripPhiInfos) {
                        std::unique_ptr<ActsTrk::SpacePoint>
                            stripSpacePoint = makeStripSpacePoint(*stripInfo, currentStripInfo, isEndcap, limit, slimit, overlapSpacePointData, overlapSpacePointDetails);
                        if (stripSpacePoint) {
                            overlapSpacePoints.push_back( std::move(stripSpacePoint) );
                        }
                    }
                }
            }
            return;
        }

        for(int n=0; n!=nElements; ++n) {

            int currentIndex = elementIndex[n];
            const InDetDD::SiDetectorElement* currentElement = elements[currentIndex];

            if (m_stripGapParameter != 0.) {
                offset(element, currentElement, slimit);
            }

            for (auto& cluster_index : clusters[currentIndex]) {
                size_t currentStripIndex = 0;
                auto ends = getStripEnds(cluster_index.first, element, currentStripIndex);
                const auto& currentLocalPos = cluster_index.first->localPosition<1>();
                StripInformationHelper currentStripInfo(ends.first, ends.second, beamSpotVertex, currentLocalPos(0, 0), cluster_index.second, currentStripIndex);

                for(auto& stripInfo : stripInfos) {
                    // depending on the index you are processing, you save the space point in the correct container
                    if (currentIndex==otherSideIndex) {
                        std::unique_ptr<ActsTrk::SpacePoint>
                            stripSpacePoint = makeStripSpacePoint(stripInfo, currentStripInfo, isEndcap, limit, slimit, spacePointData, spacePointDetails);
                        if (stripSpacePoint)
                            spacePoints.push_back( std::move(stripSpacePoint) );
                    } else {
                        std::unique_ptr<ActsTrk::SpacePoint>
                            stripSpacePoint = makeStripSpacePoint(stripInfo, currentStripInfo, isEndcap, limit, slimit, overlapSpacePointData, overlapSpacePointDetails);
                        if (stripSpacePoint)
                            overlapSpacePoints.push_back( std::move(stripSpacePoint) );
                    }
                }
            }
        }
    }


    std::unique_ptr<ActsTrk::SpacePoint> StripSpacePointFormationTool::makeStripSpacePoint(
        const StripInformationHelper& firstInfo,
        const StripInformationHelper& secondInfo,
        bool isEndcap,
        double limit,
        double slimit,
        ActsTrk::SpacePointData& data,
        ActsTrk::SpacePointMeasurementDetails& details ) const
    {
        double a  =-firstInfo.trajDirection().dot(secondInfo.normal());
        double b  = firstInfo.stripDirection().dot(secondInfo.normal());
        double l0 = firstInfo.oneOverStrip()*slimit+limit ;

        if(std::abs(a) > (std::abs(b)*l0)) {
            return nullptr;
        }

        double c  =-secondInfo.trajDirection().dot(firstInfo.normal());
        double d  = secondInfo.stripDirection().dot(firstInfo.normal());
        double l1 = secondInfo.oneOverStrip()*slimit+limit ;

        if(std::abs(c) > (std::abs(d)*l1)) {
            return nullptr;
        }

        double m = a/b;

        if(slimit!=0.) {
            double n = c/d;
            if (m >  limit || n >  limit) {
                double cs  = firstInfo.stripDirection().dot(secondInfo.stripDirection())*(firstInfo.oneOverStrip()*firstInfo.oneOverStrip());
                double dm  = (m-1);
                double dmn = (n-1.)*cs;
                if(dmn > dm) dm = dmn;
                m-=dm; n-=(dm/cs);
                if(std::abs(m) > limit || std::abs(n) > limit) {
                    return nullptr;
                }
            } else if (m < -limit || n < -limit) {
                double cs  = firstInfo.stripDirection().dot(secondInfo.stripDirection())*(firstInfo.oneOverStrip()*firstInfo.oneOverStrip());
                double dm  = -(1.+m);
                double dmn = -(1.+n)*cs;
                if(dmn > dm) dm = dmn;
                m+=dm; n+=(dm/cs);
                if(std::abs(m) > limit || std::abs(n) > limit) {
                    return nullptr;
                }
            }
        }

        Amg::Vector3D globalPosition(firstInfo.position(m));

        // evaluation of the local covariance
        // Lines taken from SCT_SpacePoint::setupLocalCovarianceSCT()
        double deltaY = 0.0004; // roughly pitch of SCT (80 mu) / sqrt(12)
        float covTerm = 1600.*deltaY;

        Eigen::Matrix<double, 2, 1> variance(0.1, 8.*covTerm);
        // Swap r/z covariance terms for endcap clusters
        if ( isEndcap )
            std::swap( variance(0, 0), variance(1, 0) );

        // evaluation of measurement details
        float topHalfStripLength = 0.5*firstInfo.stripDirection().norm();
        Eigen::Matrix<double, 3, 1> topStripDirection = -firstInfo.stripDirection()/(2.*topHalfStripLength);
        Eigen::Matrix<double, 3, 1> topStripCenter = 0.5*firstInfo.trajDirection();

        float bottomHalfStripLength = 0.5*secondInfo.stripDirection().norm();
        Eigen::Matrix<double, 3, 1> bottomStripDirection = -secondInfo.stripDirection()/(2.*bottomHalfStripLength);

        Eigen::Matrix<double, 3, 1> stripCenterDistance = firstInfo.stripCenter()  - secondInfo.stripCenter();

        boost::container::static_vector<std::size_t, 2> measIndexes({firstInfo.clusterIndex(), secondInfo.clusterIndex()});
        std::unique_ptr<ActsTrk::SpacePoint> spacePoint =
            std::make_unique<ActsTrk::SpacePoint>( globalPosition,
                                                   variance,
                                                   topHalfStripLength,
                                                   topStripDirection,
                                                   bottomHalfStripLength,
                                                   bottomStripDirection,
                                                   stripCenterDistance,
                                                   topStripCenter,
                                                   data,
                                                   details,
                                                   measIndexes );
        return spacePoint;
    }


    double StripSpacePointFormationTool::offset(const InDetDD::SiDetectorElement* element1,
                                                const InDetDD::SiDetectorElement* element2,
                                                double& stripLengthGapTolerance) const
    {
        const Amg::Transform3D& T1 = element1->transform();
        const Amg::Transform3D& T2 = element2->transform();
        Amg::Vector3D           C  = element1->center() ;
        bool isAnnulus = (element1->design().shape() == InDetDD::Annulus);

        double x12 = T1(0,0)*T2(0,0)+T1(1,0)*T2(1,0)+T1(2,0)*T2(2,0);
        double r   = isAnnulus ? std::sqrt(C[0]*C[0]+C[1]*C[1]) : std::sqrt(T1(0,3)*T1(0,3)+T1(1,3)*T1(1,3));
        double s   = (T1(0,3)-T2(0,3))*T1(0,2)+(T1(1,3)-T2(1,3))*T1(1,2)+(T1(2,3)-T2(2,3))*T1(2,2);

        double dm  = (m_stripGapParameter*r)*std::abs(s*x12);
        double d   = isAnnulus ? dm/.04 : dm/std::sqrt((1.-x12)*(1.+x12));

        if (std::abs(T1(2,2)) > 0.7) d*=(r/std::abs(T1(2,3)));

        stripLengthGapTolerance = d;

        return dm;
    }

    void StripSpacePointFormationTool::updateRange(const InDetDD::SiDetectorElement* element1,
                                                    const InDetDD::SiDetectorElement* element2,
                                                    double& stripLengthGapTolerance,
                                                    double& min, double& max) const
    {
        double dm = offset(element1, element2, stripLengthGapTolerance);
        min -= dm;
        max += dm;
    }

    void StripSpacePointFormationTool::correctPolarRange(const InDetDD::SiDetectorElement* element,
                                                         double& min,
                                                         double& max,
                                                         size_t& minStrip,
                                                         size_t& maxStrip) const
    {
        if (element->isBarrel())
            return;

        // design for endcap modules
        const InDetDD::StripStereoAnnulusDesign *design
            = dynamic_cast<const InDetDD::StripStereoAnnulusDesign *> (&element->design());
        if ( design==nullptr ) {
            ATH_MSG_FATAL( "Invalid strip annulus design for module with identifier/identifierHash " << element->identify() << "/" << element->identifyHash());
            return;
        }

        // converting min and max from cartesian reference frame to polar frame
        auto firstPosition = (design->localPositionOfCell(design->strip1Dim(0, 0))+
            design->localPositionOfCell(design->strip1Dim(design->diodesInRow(0)-1, 0)))*0.5;

        double radius = firstPosition.xEta();

        InDetDD::SiCellId minCellId = element->cellIdOfPosition(InDetDD::SiLocalPosition(radius, min, 0.));
        InDetDD::SiCellId maxCellId = element->cellIdOfPosition(InDetDD::SiLocalPosition(radius, max, 0.));

        if (not minCellId.isValid()) {
            minCellId = InDetDD::SiCellId(0);
        }

        if (not maxCellId.isValid()) {
            maxCellId = InDetDD::SiCellId(design->diodesInRow(0)-1);
        }

        minStrip = minCellId.strip();
        maxStrip = maxCellId.strip();

        // re-evaluate min and max in polar coordinate from the strip index
        min = design->localPositionOfCellPC(minCellId).xPhi();
        max = design->localPositionOfCellPC(maxCellId).xPhi();

        // depends on how the reference frame is oriented. If needed swap min and max
        if (min>max)
            std::swap(min, max);

        min -= 0.5*design->phiPitchPhi();
        max += 0.5*design->phiPitchPhi();

    }

    std::pair<Amg::Vector3D, Amg::Vector3D >
        StripSpacePointFormationTool::getStripEnds(const xAOD::StripCluster* cluster,
                                                   const InDetDD::SiDetectorElement* element,
                                                   size_t& stripIndex) const
    {
        const Eigen::Matrix<float,1,1>& localPos = cluster->localPosition<1>();
        InDetDD::SiLocalPosition localPosition(0., localPos(0, 0), 0.);

        if (element->isEndcap()) {
            // design for endcap modules
            const InDetDD::StripStereoAnnulusDesign *design
                = dynamic_cast<const InDetDD::StripStereoAnnulusDesign *> (&element->design());
            if ( design==nullptr ) {
                ATH_MSG_FATAL( "Invalid strip annulus design for module with identifier/identifierHash " << element->identify() << "/" << element->identifyHash());
                return std::pair<Amg::Vector3D, Amg::Vector3D >();
            }

            auto& rdoList = cluster->rdoList();
            // obtain cluster information to evaluate position on the readout
            const Identifier& firstStripId = rdoList.front();
            int firstStrip = m_stripId->strip(firstStripId);
            int stripRow = m_stripId->row(firstStripId);
            int clusterSizeInStrips = cluster->channelsInPhi();
            // Evaluate position on the readout from first strip and cluster width
            auto clusterPosition = design->localPositionOfCluster(design->strip1Dim(firstStrip, stripRow), clusterSizeInStrips);
            double shift = m_lorentzAngleTool->getLorentzShift(element->identifyHash());
            localPosition = InDetDD::SiLocalPosition(clusterPosition.xEta(), clusterPosition.xPhi()+shift, 0.);

            auto cellid = design->cellIdOfPosition(localPosition);
            stripIndex = cellid.strip();
        }

        std::pair<Amg::Vector3D, Amg::Vector3D > ends(element->endsOfStrip(localPosition));
        return ends;
    }
}
