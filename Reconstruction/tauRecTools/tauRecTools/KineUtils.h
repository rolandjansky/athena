/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_KINEUTILS_H
#define TAURECTOOLS_KINEUTILS_H

#include <string>
#include "TVector2.h"

//!
//! @class Tau1P3PKineUtils
//! @brief Provides methods for simple kinematical calculations
//!
//! Provides methods for simple kinematical calculations: absolute value
//! of difference in pseudorapidity, in phi position, half-opening angle
//! in ( eta, phi ) space
//!

class Tau1P3PKineUtils
{
    public:
        //!
        //! Calculates absolute value for difference in eta position
        //!
        //! @param x eta position of object 1
        //! @param y eta position of object 2
        //!
        static double deltaEta(double eta1, double eta2) { return std::fabs( eta1 - eta2);} 

        //!
        //! Calculates absolute value for difference in phi position,
        //! corrected for 2pi symmetry
        //!
        //! @param x phi  position of object 1
        //! @param y phi  position of object 2
        //!
        
        static double deltaPhi(double phi1, double phi2)
        {
            return TVector2::Phi_mpi_pi(phi1-phi2);
        }

        //!
        //! Calculates half-opening angle in (eta,phi) space
        //!
        //! @param x detphi of two objects 
        //! @param y deteta of two objects
        //!
        static double deltaR(double de,double dp) { return std::sqrt(de*de+dp*dp); }
        static double deltaR(double eta1, double phi1, double eta2, double phi2) {
        	return std::sqrt(deltaEta(eta1,eta2)*deltaEta(eta1,eta2)+deltaPhi(phi1,phi2)*deltaPhi(phi1,phi2));
        }

};

#endif // TAURECTOOLS_KINEUTILS_H 
