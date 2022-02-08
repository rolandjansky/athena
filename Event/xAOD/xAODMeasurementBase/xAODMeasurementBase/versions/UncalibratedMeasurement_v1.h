/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMEASUREMENTBASE_VERSION_UNCALIBRATEDMEASUREMENT_V1_H
#define XAODMEASUREMENTBASE_VERSION_UNCALIBRATEDMEASUREMENT_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

#include "Identifier/Identifier.h"
#include "EventPrimitives/EventPrimitives.h"

namespace xAOD {

    /// Define the type of the uncalibrated measurement
    enum class UncalibMeasType {
        Other = 0,
        // InDet
        PixelClusterType = 1,
        StripClusterType = 2
    };

    /// @class UncalibratedMeasurement_v1
    /// Class describing uncalibrated measurements
    class UncalibratedMeasurement_v1 : public SG::AuxElement {

    public:

        /// Default constructor
        UncalibratedMeasurement_v1() = default;
        /// Default copy constructors
        UncalibratedMeasurement_v1(const UncalibratedMeasurement_v1&) = default;
        UncalibratedMeasurement_v1& operator=(const UncalibratedMeasurement_v1&) = default;

        /// Virtual destructor
        virtual ~UncalibratedMeasurement_v1() = default;

        UncalibratedMeasurement_v1(UncalibratedMeasurement_v1&&) = delete;
        UncalibratedMeasurement_v1& operator=(UncalibratedMeasurement_v1&&) = delete;

        /// @name Functions to get measurement properties
        /// @{

        /// Returns the identifier of the measurement (corresponds to the detector element identifier)
        const Identifier identifier() const;

        using PosAccessor = const SG::AuxElement::Accessor< std::array< float, 3 > >;
        using CovAccessor = const SG::AuxElement::Accessor< std::array< float, 9 > >;

        /// Returns the local position of the measurement
        template < int N >
        Eigen::Map<const Eigen::Matrix<float,N,1>> localPosition() const {
            static PosAccessor locPosAcc( "localPosition" );
            const auto& values = locPosAcc(*this);
            if(values.size() < N) { 
                throw std::runtime_error{"Storage does not have sufficient size"};
            }
            assert(values.data() != nullptr);
            return Eigen::Map<const Eigen::Matrix<float,N,1>>{values.data()};
        }

        /// Returns the local position as mutable eigen map which can be assigned as well
        template < int N >
        Eigen::Map<Eigen::Matrix<float,N,1>> localPosition() {
            static PosAccessor locPosAcc( "localPosition" );
            auto& values = locPosAcc(*this);
            if(values.size() < N) {
                throw std::runtime_error{"Storage does not have sufficient size. Used size not allowed. Maximum size is 3."};
            }
            assert(values.data() != nullptr);
            return Eigen::Map<Eigen::Matrix<float,N,1>>{values.data()};
        }

        /// Returns the local covariance of the measurement
        template < int N >
        Eigen::Map<const Eigen::Matrix<float,N,N>> localCovariance() const {
            static CovAccessor locCovAcc( "localCovariance" );
            const auto& values = locCovAcc(*this);
            if(values.size() < N*N) { 
                throw std::runtime_error{"Storage does not have sufficient size"};
            }
            assert(values.data() != nullptr);
            return Eigen::Map<const Eigen::Matrix<float,N,N>>{values.data()};
        }

        /// Returns the local covariance as mutable eigen map which can be assigned as well
        template < int N >
        Eigen::Map<Eigen::Matrix<float,N,N>> localCovariance() {
            static CovAccessor locCovAcc( "localCovariance" );
            auto& values = locCovAcc(*this);
            if(values.size() < N*N) {
                throw std::runtime_error{"Storage does not have sufficient size. Used size not allowed. Maximum size is 9."};
            }
            assert(values.data() != nullptr);
            return Eigen::Map<Eigen::Matrix<float,N,N>>{values.data()};
        }


        /// Returns the type of the measurement type as a simple enumeration
        virtual xAOD::UncalibMeasType type() const =0 ;

        /// @}

        /// @name Functions to set measurement properties
        /// @{

        /// Sets the identifier of the measurement (corresponds to the detector element identifier)
        void setIdentifier(Identifier& id);


        /// @}

        /// @name Direct method to set measurement properties
        /// @{

        /// Sets Identifier, local position and local covariance of the measurement
        template < int N >
        void setMeasurement(Identifier& id,
                            Eigen::Matrix<float,N,1>& locPos,
                            Eigen::Matrix<float,N,N>& locCov) {
            setIdentifier(id);
            localPosition<N>() = locPos;
            localCovariance<N>() = locCov;
        }
        /// @}

    };

}

// Finish declaration of SG::AuxElement as a base class of UncalibratedMeasurement_v1:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::UncalibratedMeasurement_v1, SG::AuxElement );

#endif // XAODMEASUREMENTBASE_VERSION_UNCALIBRATEDMEASUREMENT_V1_H
