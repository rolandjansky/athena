/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_VERSION_PIXELCLUSTER_V1_H
#define XAODINDETMEASUREMENT_VERSION_PIXELCLUSTER_V1_H

#include "xAODMeasurementBase/versions/UncalibratedMeasurement_v1.h"
#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace xAOD {

    /// @class PixelCluster_v1
    /// Class describing pixel clusters

    class PixelCluster_v1 : public UncalibratedMeasurement_v1 {

    public:
        using ConstVectorMap = Eigen::Map<const Eigen::Matrix<float, 3, 1>>;
        using VectorMap = Eigen::Map<Eigen::Matrix<float, 3, 1>>;

        /// Default constructor
        PixelCluster_v1() = default;
        /// Virtual destructor
        virtual ~PixelCluster_v1() = default;

        /// @name Functions to get pixel cluster properties
        /// @{

        /// Returns the type of the pixel cluster as a simple enumeration
        xAOD::UncalibMeasType type() const final {
            return xAOD::UncalibMeasType::PixelClusterType;
        }

        /// Returns the global position of the pixel cluster
        ConstVectorMap globalPosition() const;
        VectorMap globalPosition();

        /// Returns the list of identifiers of the channels building the cluster
        const std::vector< Identifier > rdoList() const;

        /// Returns the dimensions of the cluster in numbers of channels in phi (x) and eta (y) directions, respectively
        int channelsInPhi() const;
        int channelsInEta() const;

        /// Returns omegax and omegay, i.e. the charge
        /// balance between the first and last rows and colums, respectively,
        /// building the cluster, and are numbers between 0 and 1.
        float omegaX() const;
        float omegaY() const;

        /// Returns the list of ToT of the channels building the cluster
        const std::vector< int >& totList() const;
        /// Returns the sum of the ToTs of the channels building the cluster
        int totalToT() const;

        /// Returns the list of charges of the channels building the cluster
        const std::vector< float >& chargeList() const;
        /// Returns the sum of the charges of the channels building the cluster
        float totalCharge() const;

        /// Return the energy loss in the cluster in MeV
        float energyLoss() const;

        /// Returns if the cluster is split or not
        bool isSplit() const;

        /// Returns the splitting probabilities for the cluster
        float splitProbability1() const;
        float splitProbability2() const;

        /// Return the LVL1 accept
        int lvl1a() const;

        /// @}

        /// @name Functions to set pixel cluster properties
        /// @{

        /// Sets the list of identifiers of the channels building the cluster
        void setRDOlist(const std::vector< Identifier >& rdolist);

        /// Sets the dimensions of the cluster in numbers of channels in phi (x) and eta (y) directions
        void setChannelsInPhiEta(int channelsInPhi, int channelsInEta);

        /// Sets omegax and omegay, i.e. the charge
        /// balance between the first and last rows and colums, respectively,
        /// building the cluster, and are numbers between 0 and 1.
        void setOmegas(float omegax, float omegay);

        /// Sets the list of ToT of the channels building the cluster
        void setToTlist(const std::vector< int >& tots);

        /// Sets the list of charges of the channels building the cluster
        void setChargelist(const std::vector< float >& charges);

        /// Sets the energy loss in the cluster in MeV
        void setEnergyLoss(float dEdX);

        /// Sets if the cluster is split or not
        void setIsSplit(bool isSplit);

        /// Sets the splitting probabilities for the cluster
        void setSplitProbabilities(float prob1, float prob2);

        /// Sets the LVL1 accept
        void setLVL1A(int lvl1a);

        /// @}

    };

}

#endif
