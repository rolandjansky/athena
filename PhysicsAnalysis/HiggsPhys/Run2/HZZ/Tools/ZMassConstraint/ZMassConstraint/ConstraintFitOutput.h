/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONSTRAINTFITOUTPUT_H
#define CONSTRAINTFITOUTPUT_H
#include "EventPrimitives/EventPrimitives.h"
#include "TLorentzVector.h"

namespace ZMassConstraint 
{
    class ConstraintFitOutput
    {
    public:

        /// Constructor/destructor
        ConstraintFitOutput(){}
        ~ConstraintFitOutput() {}

        /// Number of particles
        unsigned int             getNConstituents() const { return m_particleList.size(); }

        /// Access to individual particle 4-vec
        const TLorentzVector&    getConstituentFourVector(int index) const { return m_particleList.at(index); }

        /// Access to combined 4-vec
        void                     getCompositeFourVector(TLorentzVector& lv) const
            { lv = TLorentzVector(); for( auto lv1 : m_particleList ) lv += lv1; }

        /// Access to individual covariance PhiThetaP (3,3)
        Amg::MatrixX             getConstituentCovariancePhiThetaP(int index) const 
            { return m_covariancePhiThetaP.block(5*index +2, 5*index+2, 3, 3); }

        /// Access to individual covariance PhiThetaP (3,3)
        void                     getConstituentCovariancePhiThetaP(int index, AmgMatrix(3,3)& outMatrix) const 
            { outMatrix = m_covariancePhiThetaP.block(5*index +2, 5*index+2, 3, 3); }

        /// Access to individual covariance d0z0PhiThetaP (5,5)
        Amg::MatrixX             getConstituentCovarianced0z0PhiThetaP(int index) const
            { return m_covariancePhiThetaP.block(5*index, 5*index, 5, 5); }

        /// Access to individual covariance d0z0PhiThetaP (5,5)
        void                     getConstituentCovarianced0z0PhiThetaP(int index, AmgMatrix(5,5)& outMatrix) const 
            { outMatrix = m_covariancePhiThetaP.block(5*index, 5*index, 5, 5); }

        /// Access to full covariance d0z0PhiThetaP (5 * nparticle, 5 * nparticle)
        const Amg::MatrixX&      getConstituentCovariancePhiThetaP() const 
            { return m_covariancePhiThetaP; }

        /// Set output
        void setFitOutput(const std::vector<TLorentzVector>& particleList,
                          const Amg::MatrixX& covarXYZ,
                          const Amg::MatrixX& covard0z0PhiThetaP)
            { m_particleList = particleList; m_covariance   = covarXYZ; m_covariancePhiThetaP = covard0z0PhiThetaP; }
        
    protected:
        std::vector<TLorentzVector> m_particleList;
        Amg::MatrixX                m_covariance;
        Amg::MatrixX                m_covariancePhiThetaP;
    };
}

#endif
