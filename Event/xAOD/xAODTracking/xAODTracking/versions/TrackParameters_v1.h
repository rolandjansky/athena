/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKPARAMETERS_V1_H
#define XAODTRACKING_VERSIONS_TRACKPARAMETERS_V1_H
#include <cstdint>
#include "AthContainers/AuxElement.h"
namespace xAOD {
    /**
     * @brief Track Parameters for Acts MultiTrajectory
     **/

    class TrackParameters_v1 : public SG::AuxElement {
    public:
        TrackParameters_v1() = default;
        /**
         * access track parameters vector of const element
         **/
        const std::vector<double>& parameters() const;
        /**
         * access parameters of non const element
         * MultiTrajectory API requires access to the storage to present that dat as Eigen types
         **/
        std::vector<double>& parameters();
        /**
         * @brief Set the Parameters vector
         **/
        void setParameters(const std::vector<double>&);

        /**
         * access track covariance matrix (flattened, rows layout) of const element
         **/
        const std::vector<double>& covariance() const;

        /**
         * access track covariance matrix (flattened, rows layout)
         * MultiTrajectory API requires access to the storage to present that dat as Eigen types
         **/
        std::vector<double>& covariance();
        /**
         * @brief Set the covariance matrix (flattened rows layout)
         */
        void setCovariance(const std::vector<double>&);

        /**
         * @brief expands sizes of internal vectors for the data storage
         * ( by default this is 6 for parameters and 6x6 for covariance)
         */
        void resize(size_t sz = 6);
    };
}
#endif