/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////
//
// Z mass constraint fit
//
// original version by:
// Konstantinos Nikolopoulos
// Univ. of Athens
//
////////////////////////////////////////
#ifndef ZMASSCONSTRAINT_CONSTRAINTFIT_H
#define ZMASSCONSTRAINT_CONSTRAINTFIT_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

#include "ZMassConstraint/IConstraintFit.h"

#include "EventPrimitives/EventPrimitives.h"
// #include "TLorentzVector.h"
#include "xAODEgamma/Photon.h"
#include "AsgTools/ToolHandle.h"

// Forward declarations:

namespace CP       {
    class IEgammaCalibrationAndSmearingTool;
    class IMuonCalibrationAndSmearingTool;
}

namespace ZMassConstraint
{
    class ConstraintFit : public virtual IConstraintFit,
                          public asg::AsgTool 
    {

        /// Create a proper constructor for Athena
        ASG_TOOL_CLASS( ConstraintFit, ZMassConstraint::IConstraintFit )
        
    public:

        /// Create a constructor for standalone usage
        ConstraintFit( const std::string& name );     
    
        ~ConstraintFit(void); // default destructor

        /// Initialize constraint fit
        StatusCode initialize();

        /// Perform the constrained mass fit
        StatusCode  doMassFit     (const ConstraintFitInput& input,
                                   ConstraintFitOutput&      output);
        
        /// Calculate the mass error without fit - use just the inputs
        double      getMassError  (const ConstraintFitInput& firstInput,
                                   const ConstraintFitInput& secondInput = ConstraintFitInput());

        /// Calculate the mass error with the result of fit and possible additional input
        /// (4-vecs/cov mat) that has NOT been fit
        double      getMassError  (const ConstraintFitOutput& fitOutput,
                                   const ConstraintFitInput&  extraInput = ConstraintFitInput());

        /// Calculate the mass error with the result of two mass fits        
        double      getMassError  (const ConstraintFitOutput& fitOutput,
                                   const ConstraintFitOutput& secondFitOutput);

        /// Add muon to input, must provide the resolution Scale Factor
        void        addParticle   (const xAOD::Muon&      part,  
                                   ConstraintFitInput&    input, 
                                   MassConstraintMuonType muonType = isCombMCMT);

        /// Add electron to input, must provide the electron energy resolution
        void        addParticle   (const xAOD::Electron& part,
                                   float                 elEnergyRes,
                                   ConstraintFitInput&   input);

        /// Add in FSR photon to input, (energy resolution is obtain in method)
        void        addFSRParticle(const xAOD::IParticle & part,
                                   const TLorentzVector&   fsr4Vec,
                                   ConstraintFitInput&     input);

    private:

        // Enum list for tracking covariance
        enum TrackParameters {
            d0     = 0,
            z0     = 1,
            phi0   = 2,
            theta  = 3,
            qOverP = 4,
            qP     = 4,
            x      = 0,
            y      = 1,
            z      = 2
        };

        int    massFitInterface(const ConstraintFitInput& theInput);
        void   massFitRun(ConstraintFitOutput& output, double zresol = -1.);
        double getMassError(const std::vector<TLorentzVector>& particles,
                            const std::vector<AmgMatrix(3,3)>& covariances);
        double likelihoodMass2(void);
        double likelihoodMass(double);
        void   setIgnore(bool val){ m_ignoreInputChecks=val; }
        bool   doSanityChecksOnCovariance(const TLorentzVector& vector, const AmgMatrix(5,5)& covar) const;
        void   convertCovd0z0PhiThetaPToXYZ(const TLorentzVector& fourVec,
                                            const AmgMatrix(5,5)& covard0z0PhiThetaP,
                                            AmgMatrix(5,5)& covarXYZ) const;
        void   convertCovXYZTod0z0PhiThetaP(const std::vector<TLorentzVector>& particleList,
                                            const Amg::MatrixX&                covarXYZ,
                                            Amg::MatrixX&                      covard0z0PhiThetaP) const;
    private:
        bool     m_conHasWidth;
        double   m_conMass;
        double   m_conWidth;
        double   m_resolution;
        bool     m_ignoreInputChecks;
        std::string m_decorationName;
        bool     m_doCalibCov;
        int      m_stepForLikelihoodFit;

        // Calibration tools
        ToolHandle<CP::IEgammaCalibrationAndSmearingTool>  m_energyRescaler; // electron resolution
        ToolHandle<CP::IMuonCalibrationAndSmearingTool>    m_mu_resolSFTool; // Muon error rescaling

        ConstraintFitInput  m_theInput;
        unsigned int        m_parameters;
        unsigned int        m_nobj;
        std::vector<double> m_objmass;
        Amg::MatrixX        m_parametersInit;
        Amg::MatrixX        m_covarianceInit;
        Amg::MatrixX        m_parametersFinal;
        Amg::MatrixX        m_covarianceFinal;

        // double calculateChi2(const Amg::MatrixX* p0, Amg::MatrixX* var, const double Mass);
        double massFitCalculation(const Amg::MatrixX& var, double mass, Amg::MatrixX& p0);
        double massFit(const Amg::MatrixX& /*p0*/, const Amg::MatrixX& var, double mass, Amg::MatrixX& pOut,
                       Amg::MatrixX& /*varout*/);
        float retrieve_eta_calo(const xAOD::IParticle&  part) const;
    };
}
#endif // ZMASSCONSTRAINT_CONSTRAINTFIT_H
