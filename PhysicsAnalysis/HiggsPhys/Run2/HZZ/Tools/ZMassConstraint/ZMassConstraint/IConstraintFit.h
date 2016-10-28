// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IConstraintFit.h 670486 2015-05-28 09:29:21Z schaffer $
/////////////////////////////////////////
//
// Z mass constraint fit
//
// original version by:
// Konstantinos Nikolopoulos
// Univ. of Athens
//
////////////////////////////////////////
#ifndef ZMASSCONSTRAINT_ICONSTRAINTFIT_H
#define ZMASSCONSTRAINT_ICONSTRAINTFIT_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODBase/IParticle.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/Electron.h"

// ZMassConstraint I/O includes
#include "ZMassConstraint/ConstraintFitInput.h"
#include "ZMassConstraint/ConstraintFitOutput.h"

namespace ZMassConstraint 
{

    enum MassConstraintMuonType {
        isCombMCMT,      // 0
        isID_MCMT,       // 1
        isMS_MCMT        // 2
    };

    class IConstraintFit  : public virtual asg::IAsgTool 
    {
        /// Declare the interface that the class provides
        ASG_TOOL_INTERFACE( ZMassConstraint::IConstraintFit )
        
        public:

        /// Perform the constrained mass fit
        virtual StatusCode  doMassFit     (const ConstraintFitInput& input, 
                                           ConstraintFitOutput&      output) = 0;

        /// Calculate the mass error without fit - use just the inputs
        virtual double      getMassError  (const ConstraintFitInput& firstInput,
                                           const ConstraintFitInput& secondInput = ConstraintFitInput()) = 0;

        /// Calculate the mass error with the result of fit and possible additional input
        /// (4-vecs/cov mat) that has NOT been fit
        virtual double      getMassError  (const ConstraintFitOutput& fitOutput,
                                           const ConstraintFitInput&  extraInput = ConstraintFitInput()) = 0;

        /// Calculate the mass error with the result of two mass fits        
        virtual double      getMassError  (const ConstraintFitOutput& fitOutput,
                                           const ConstraintFitOutput& secondFitOutput) = 0;

        /// Add muon to input, must provide the resolution Scale Factor
        virtual void        addParticle   (const xAOD::Muon&       mu, 
                                           ConstraintFitInput&     input, 
                                           MassConstraintMuonType  muonType = isCombMCMT) = 0;

        /// Add electron to input, must provide the electron energy resolution
        virtual void        addParticle   (const xAOD::Electron&   el,
                                           float                   elEnergyRes,
                                           ConstraintFitInput&     input) = 0;

        /// Add in FSR photon to input, must provide BOTH the IParticle AND 4vec of FSR photon
        ///   The IParticle is used to identify type of particle: 
        ///      electron/unconverted photon/converted photon
        //    The four vector is used in the mass fit and comes from the FsrTool
        virtual void        addFSRParticle(const xAOD::IParticle & part,
                                           const TLorentzVector&   fsr4Vec,
                                           ConstraintFitInput&     input) = 0;
        
    };

} // namespace ZMassConstraint 

#endif // ZMASSCONSTRAINT_ICONSTRAINTFIT_H
