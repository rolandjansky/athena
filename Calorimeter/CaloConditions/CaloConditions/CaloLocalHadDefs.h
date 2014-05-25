/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDITIONS_CALOLOCALHADDEFS_H
#define CALOCONDITIONS_CALOLOCALHADDEFS_H

/**
 * @struct CaloLocalHadCoeff
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 18-August-2009
 * @brief defines enums and data types of @c CaloLocalHadCoeff
 *
 */


struct CaloLocalHadDefs {
    /** @brief enums to identify different area types corrected with different approaches */
    enum LocalHadAreaType {
      AREA_STD         = 0,   // Standard area, lookup table 
      AREA_DMFIT       = 101, // dead material area corrected with polynom fit
      AREA_DMLOOKUP    = 102, // dead material area corrected with lookup table approach
      AREA_DMSMPW      = 103  // dead material area corrected with new calo sampling weights
    };

    /** @brief enums to identify different types of correction coefficients */
    enum LocalHadCoeffType {
      BIN_WEIGHT   = 0,  // weight stored in one lookup table bin
      BIN_ENTRIES  = 1,  // number of entries in one lookup table bin
      BIN_ERROR    = 2,  // error stored in one lookup table bin
      BIN_P0       = 0,  // polynomial coefficients in p0+p1*x+p2*x*x
      BIN_P1       = 1,  // polynomial coefficients in p0+p1*x+p2*x*x
      BIN_P2       = 2   // polynomial coefficients in p0+p1*x+p2*x*x
    };

    /** @brief enums to identify user dimensions types */
    enum LocalHadDimensionType {
      DIM_LOG      = 1<<0, // logarithmic scale
      DIM_WRAP     = 1<<1, // phi-like dimension
      DIM_EQUI     = 1<<2  // equidistant binning
    };

    /** @brief enums to identify user dimensions id number 
      * DIMC_* - classification, DIMW_*-weighting, DIMO_* - out-of-cluser, DIMD_* - dead material */
    enum LocalHadDimensionId {
      // classification
      DIMC_SIDE   = 0,
      DIMC_ETA    = 1,
      DIMC_PHI    = 2,
      DIMC_ENER   = 3,
      DIMC_EDENS  = 4,
      DIMC_LAMBDA = 5,
      // weighting
      DIMW_SIDE   = 0,
      DIMW_ETA    = 1,
      DIMW_PHI    = 2,
      DIMW_ENER   = 3,
      DIMW_EDENS  = 4,
      // out-of-cluster
      DIMO_SIDE   = 0,
      DIMO_PHI    = 1,
      DIMO_ENER   = 2,
      DIMO_ETA    = 3,
      DIMO_LAMBDA = 4,
      // dead material
      DIMD_EMFRAC = 0,
      DIMD_SIDE   = 1, 
      DIMD_ETA    = 2, 
      DIMD_PHI    = 3, 
      DIMD_ENER   = 4, 
      DIMD_LAMBDA = 5, 
      // unknown
      DIMU_UNKNOWN = 99
    };

};


#endif
