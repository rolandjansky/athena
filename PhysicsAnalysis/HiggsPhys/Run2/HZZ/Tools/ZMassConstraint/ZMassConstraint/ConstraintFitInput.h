/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONSTRAINTFITINPUT_H
#define CONSTRAINTFITINPUT_H
#include "EventPrimitives/EventPrimitives.h"
#include "TLorentzVector.h"

namespace ZMassConstraint 
{
    class ConstraintFitInput {

      
    public:

        /// Constructor/destructor
        ConstraintFitInput() : m_isOK(true) {}
        ~ConstraintFitInput()               {}

        /// Number of particles
        unsigned int            getNConstituents() const              { return m_particles.size(); }

        /// Access to individual particle 4-vec
        const TLorentzVector&   getConstituentFourVector(int i) const { return m_particles[i]; }

        /// Access to individual covariance d0z0PhiThetaP (5,5)
        const AmgMatrix(5,5)&   getConstituentCovariance(int i) const { return m_covariances[i]; }

        /// Access to individual covariance PhiThetaP (3,3)
        void                    getConstituentCovariancePhiThetaP(int i, AmgMatrix(3,3)& outMatrix) const 
            { outMatrix = m_covariances[i].block(2, 2, 3, 3); }

        /// Access to individual to full cartesian covariance  (5,5)
        const AmgMatrix(5,5)&   getCovarianceCartesian(int ipart) const { return m_cartCovariances[ipart]; }

        /// Set the inputs
        void addConstituent_FourVector_d0z0PhiThetaP(const TLorentzVector& vector,
                                                     const AmgMatrix(5,5)& covard0z0PhiThetaP,
                                                     const AmgMatrix(5,5)& covarXYZ,
                                                     bool isOK)
            {   m_particles.push_back(vector); m_covariances.push_back(covard0z0PhiThetaP);
                m_cartCovariances.push_back(covarXYZ); if (m_isOK) m_isOK = isOK; }

        /// Check that covariance matrix is OK
        inline bool isOK() { return m_isOK; }

    protected:
        std::vector<TLorentzVector> m_particles;
        std::vector<AmgMatrix(5,5)> m_covariances;    /// d0z0PhiThetaQoverP 
        std::vector<AmgMatrix(5,5)> m_cartCovariances; /// Cartesian Covariance
        bool m_isOK;

    };
}


#endif
