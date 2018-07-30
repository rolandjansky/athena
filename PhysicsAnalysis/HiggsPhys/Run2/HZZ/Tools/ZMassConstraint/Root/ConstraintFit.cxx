/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// A C++ implementation of Mass constraint fitting
// 23/09/2006
// K. Nikolopoulos
// --- * --- * --- * --- * ---* --- * --- * --- * ---
//
//
//
//
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "ZMassConstraint/ConstraintFit.h"
#include "ZMassConstraint/ConstraintFitInput.h"
#include "ZMassConstraint/ConstraintFitOutput.h"
#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

namespace ZMassConstraint 
{
    ConstraintFit::ConstraintFit( const std::string& name ):
        asg::AsgTool( name ),
        m_conHasWidth(true),
        m_conMass(91187.6),
        m_conWidth(2495.2),
        m_resolution(0.01),
        m_ignoreInputChecks(false),
        m_parameters(3)
    {
        declareProperty( "Z_pdg_mass",                     m_conMass);
        declareProperty( "Z_pdg_width",                    m_conWidth);
        declareProperty( "hasWidth",                       m_conHasWidth);
        declareProperty( "ignoreInputChecks",              m_ignoreInputChecks);
        declareProperty( "EgammaCalibAndSmearingTool",     m_energyRescaler = ToolHandle<CP::IEgammaCalibrationAndSmearingTool>("CP::EgammaCalibrationAndSmearingTool"));
        declareProperty( "MuonCalibrationAndSmearingTool", m_mu_resolSFTool = ToolHandle<CP::IMuonCalibrationAndSmearingTool>("CP::MuonCalibrationAndSmearingTool"));
    }

    ConstraintFit::~ConstraintFit(void)
    {}


    StatusCode ConstraintFit::initialize()
    {
        ATH_MSG_INFO ("initialize: retrieve tools" << m_energyRescaler.name() << " and "
                       << m_mu_resolSFTool.name());

        if (!m_energyRescaler.retrieve().isSuccess()) {
            ATH_MSG_ERROR ("initialize: unable to retrieve EgammaCalibrationAndSmearingTool");
            return StatusCode::FAILURE;
        }

        if (!m_mu_resolSFTool.retrieve().isSuccess()) {
            ATH_MSG_ERROR ("initialize: unable to retrieve MuonCalibrationAndSmearingTool");
            return StatusCode::FAILURE;
        }
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    StatusCode
    ConstraintFit::doMassFit (const ConstraintFitInput& input, ConstraintFitOutput& output)
    {

        // For the moment, call the 'old' methods
        if (massFitInterface(input)) {
            ATH_MSG_ERROR( "Could not add in input" );
            return StatusCode::FAILURE;
        }
    
        massFitRun(output);
    
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    // void        
    // ConstraintFit::addParticle   (const xAOD::IParticle & part,    ConstraintFitInput& input)
    // {
    //     if(part.type() == xAOD::Type::Electron) {
    //         const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*> (&part);
    //         return addParticle(*el, input);
    //     } 
    //     else if(part.type()==xAOD::Type::Muon) {
    //         const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*> (&part);
    //         return addParticle(*mu, input);
    //     }
    // }
    
    void
    ConstraintFit::addFSRParticle(const xAOD::IParticle&  part,
                                  const TLorentzVector&   fsr4Vec,
                                  ConstraintFitInput&     input) 
    {

        ATH_MSG_DEBUG ("addFSRParticle: *** 4vecFsr: " << fsr4Vec.Pt() << "/" << fsr4Vec.Eta() << "/" << fsr4Vec.Phi());
        // Get energy resolution - done differently for photons and electrons
        float e_res = 0;
        PATCore::ParticleType::Type phType     = PATCore::ParticleType::Electron;
        float                       cl_etaCalo = retrieve_eta_calo(part);
        if (!m_energyRescaler.empty()) {
            if (xAOD::Type::Photon ==  part.type()) {
                const xAOD::Photon* fsrPf = dynamic_cast<const xAOD::Photon*>(&part);
                phType = (xAOD::EgammaHelpers::isConvertedPhoton(fsrPf)) ?
                    PATCore::ParticleType::ConvertedPhoton : 
                    PATCore::ParticleType::UnconvertedPhoton;
            }
            e_res = m_energyRescaler->resolution( fsr4Vec.E(), fsr4Vec.Eta(), cl_etaCalo, phType)*fsr4Vec.E();
        }
        else {
            ATH_MSG_ERROR( "addFSRParticle:  - cannot find EgammaCalibrationAndSmearingTool. ");
            ATH_MSG_ERROR( "addFSRParticle:  - please set property 'EgammaCalibAndSmearingToolName' with the name of your EgammaCalibrationAndSmearingTool - default is 'EgammaCalibrationAndSmearingTool'. ");
        }
    
        AmgMatrix(5,5) photonCovarianceMatrix;
        photonCovarianceMatrix.setZero();
        photonCovarianceMatrix(d0,d0)         = 0.000001;
        photonCovarianceMatrix(z0,z0)         = 0.000001;
        photonCovarianceMatrix(phi0,phi0)     = 0.000001;
        photonCovarianceMatrix(theta,theta)   = 0.000001;
        photonCovarianceMatrix(qOverP,qOverP) = e_res*e_res;
        photonCovarianceMatrix(d0,z0)         = 0;
        photonCovarianceMatrix(d0,phi0)       = 0;
        photonCovarianceMatrix(d0,theta)      = 0;
        photonCovarianceMatrix(d0,qOverP)     = 0;
        photonCovarianceMatrix(z0,phi0)       = 0;
        photonCovarianceMatrix(z0,theta)      = 0;
        photonCovarianceMatrix(z0,qOverP)     = 0;
        photonCovarianceMatrix(phi0,theta)    = 0;
        photonCovarianceMatrix(phi0,qOverP)   = 0;
        photonCovarianceMatrix(theta,qOverP)  = 0;

        ATH_MSG_DEBUG ("addFSRParticle: FSR calib type,e, e_res  " << phType << " " << fsr4Vec.E() << " " << e_res);

        // symmetrize it
        for (int ii = 0; ii < 5; ii++)
            for (int jj = ii + 1; jj < 5; jj++)
                photonCovarianceMatrix(jj, ii) = photonCovarianceMatrix(ii, jj);

        bool isOK = doSanityChecksOnCovariance(fsr4Vec, photonCovarianceMatrix);
        // Get covariance in cartesian CS
        AmgMatrix(5,5) covarXYZ;
        covarXYZ.setZero();
        convertCovd0z0PhiThetaPToXYZ(fsr4Vec, photonCovarianceMatrix, covarXYZ);
        // Save in input
        input.addConstituent_FourVector_d0z0PhiThetaP(fsr4Vec, photonCovarianceMatrix, covarXYZ, isOK);

        ATH_MSG_DEBUG( "addFSRParticle:  - added fsr");

    }
    
    void
    ConstraintFit::addParticle(const xAOD::Muon& mu,
                               ConstraintFitInput& input,
                               MassConstraintMuonType muonType)
    {

        // For muons, we allow the mass constraint to be done in one of three ways:
        //  1)   standard way - use muon momentum and primary track for cov matrix, or
        //  2,3) ID or MS     - use the ID or MS tracks for both the momentum and cov matrix

        TLorentzVector mu4vec;  // to be filled according to muon type

        // setup accessors for muon decorations - set in MuonCalibrationAndSmearingTool
        static SG::AuxElement::Accessor<float> muonSpectrometerPt ("MuonSpectrometerPt");
        static SG::AuxElement::Accessor<float> innerDetectorPt    ("InnerDetectorPt");

        // For the momentum, 
        // Get the track particle according to the requested muon type for the covariance matrix
        const xAOD::TrackParticle* track = mu.primaryTrackParticle();
        bool set4vec = false;
        if (((isMS_MCMT == muonType) || (isID_MCMT == muonType)) && xAOD::Muon::Combined == mu.muonType()) {
            if (isMS_MCMT  == muonType) { 
                track = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
                if (!muonSpectrometerPt.isAvailable(mu))
                    ATH_MSG_ERROR( "addParticle:  - could not get muonSpectrometerPt from muon. Please applyCorrection with the MuonCalibAndSmearTool");
                if (!track) {
                    ATH_MSG_ERROR( "addParticle:  - Combined muon is missing MS track particle. Using combined track particle");
                    track = mu.primaryTrackParticle();
                }
                mu4vec.SetPtEtaPhiM(muonSpectrometerPt(mu), track->eta(), track->phi(), mu.m());
                set4vec = true;
                ATH_MSG_DEBUG ("addParticle: set muon track to MS ");
            }
            else {
                track = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
                if (!innerDetectorPt.isAvailable(mu))
                    ATH_MSG_ERROR( "addParticle:  - could not get innerDetectorPt from muon. Please applyCorrection with the MuonCalibAndSmearTool");
                mu4vec.SetPtEtaPhiM(innerDetectorPt(mu), track->eta(), track->phi(), mu.m());
                set4vec = true;
                ATH_MSG_DEBUG ("addParticle: set muon track to ID ");
            }
        }

        if (!set4vec) mu4vec.SetPtEtaPhiM(mu.pt(), mu.eta(), mu.phi(), mu.m());
        
        if (!track) {
            ATH_MSG_ERROR( "addParticle:  - could not get track particle for muon");
            return;
        }

        ATH_MSG_DEBUG ("addParticle: *** 4vecMu: " << mu4vec.Pt() << "/" << mu4vec.Eta() << "/" << mu4vec.Phi());


        float muSF = 1;
        int   type = -1;      

        // Comment out use of MuonCalibrationAndSmearingTool until the SF is implemented by Giacomo
        
        // if (m_mu_resolSFTool) {
        //     // Get scale factor for the muon covariance qoverp resolution.
        //     // For muonType == isCombMCMT, we get a SF differently according to the type of muon: Comb/ST/CT/SA
        //     //     muonType == isID_MCMT,  we use the type 2 for ID tracks
        //     //     muonType == isMS_MCMT,  we use the type 3 for MS tracks

        //     if (muonType == isCombMCMT) {
        //         if (xAOD::Muon::Combined            == mu.muonType()) type = 1;
        //         else if (xAOD::Muon::SegmentTagged  == mu.muonType()) type = 2;
        //         else if (xAOD::Muon::CaloTagged     == mu.muonType()) type = 2;
        //         else if (xAOD::Muon::MuonStandAlone == mu.muonType()) type = 3;
        //         else ATH_MSG_ERROR("addParticle: - **** MUON type is not CB/ST/SA/Calo **** ");
        //     }
        //     else if (muonType == isID_MCMT) {
        //         type = 2;
        //         if (xAOD::Muon::MuonStandAlone == mu.muonType()) type = 3;
        //     }
        //     else if (muonType == isMS_MCMT) {
        //         type = 3;
        //         if (xAOD::Muon::SegmentTagged  == mu.muonType() ||
        //             xAOD::Muon::CaloTagged     == mu.muonType()) type = 2;
        //     }

        //     if (muonType < isCombMCMT) {
        //         ATH_MSG_ERROR( "addParticle: - Invalid muon type requested, please use .");
        //     }
        //     else {
        //         muSF = m_mu_resolSF->getResolutionScaleFactor(mu4vec, type);
        //     }

        //     ATH_MSG_DEBUG ("addParticle: 4v,type,errSF  " << mu4vec.Eta() << " " << type << " " << muSF);
            
            
        // }
        // else {
        //     ATH_MSG_ERROR( "addParticle: - MuonResSFTool is NOT set. Please call setMuonResSFTool with MuonResolutionAndMomentumScaleFactors tool.");
        // }

        
        AmgMatrix(5,5) covmatrix;
        covmatrix.setZero();
        covmatrix(d0,d0)         = track->definingParametersCovMatrix()(d0,d0);
        covmatrix(z0,z0)         = track->definingParametersCovMatrix()(z0,z0);
        covmatrix(phi0,phi0)     = track->definingParametersCovMatrix()(phi0,phi0);
        covmatrix(theta,theta)   = track->definingParametersCovMatrix()(theta,theta);
        covmatrix(qOverP,qOverP) = track->definingParametersCovMatrix()(qOverP,qOverP)*muSF*muSF;
        covmatrix(d0,z0)         = track->definingParametersCovMatrix()(d0,z0);
        covmatrix(d0,phi0)       = track->definingParametersCovMatrix()(d0,phi0);
        covmatrix(d0,theta)      = track->definingParametersCovMatrix()(d0,theta);
        covmatrix(d0,qOverP)     = track->definingParametersCovMatrix()(d0,qOverP)*muSF;
        covmatrix(z0,phi0)       = track->definingParametersCovMatrix()(z0,phi0);
        covmatrix(z0,theta)      = track->definingParametersCovMatrix()(z0,theta);
        covmatrix(z0,qOverP)     = track->definingParametersCovMatrix()(z0,qOverP)*muSF;
        covmatrix(phi0,theta)    = track->definingParametersCovMatrix()(phi0,theta);
        covmatrix(phi0,qOverP)   = track->definingParametersCovMatrix()(phi0,qOverP)*muSF;
        covmatrix(theta,qOverP)  = track->definingParametersCovMatrix()(theta,qOverP)*muSF;

        ATH_MSG_DEBUG ("addParticle: type,errSF,mu p,cov  " << type << " " << muSF
                       << " " << mu4vec.E()
                       << " " << track->definingParametersCovMatrix()(d0,    d0)
                       << " " << track->definingParametersCovMatrix()(z0,    z0)
                       << " " << track->definingParametersCovMatrix()(phi0,  phi0)
                       << " " << track->definingParametersCovMatrix()(theta, theta)
                       << " " << track->definingParametersCovMatrix()(qOverP,qOverP)*muSF*muSF
                       << " " << track->definingParametersCovMatrix()(d0,    z0)
                       << " " << track->definingParametersCovMatrix()(d0,    phi0)
                       << " " << track->definingParametersCovMatrix()(d0,    theta)
                       << " " << track->definingParametersCovMatrix()(d0,qOverP)*muSF
                       << " " << track->definingParametersCovMatrix()(z0,    phi0)
                       << " " << track->definingParametersCovMatrix()(z0,    theta)
                       << " " << track->definingParametersCovMatrix()(z0,qOverP)*muSF
                       << " " << track->definingParametersCovMatrix()(phi0,  theta)
                       << " " << track->definingParametersCovMatrix()(phi0,qOverP)*muSF
                       << " " << track->definingParametersCovMatrix()(theta,qOverP)*muSF);

        for (int ii = 0; ii < 5; ii++)
            for (int jj = ii+1; jj < 5; jj++)
                covmatrix(jj,ii) = covmatrix(ii,jj);

        ATH_MSG_VERBOSE( "addParticle: cov matrix \n" << covmatrix );

        // Transform cov of q/p to cov p in covariance matrix
        double P     = mu4vec.P();
        AmgMatrix(5,5) jacobian0;
        jacobian0.setZero();
        jacobian0(0,0) = 1.;
        jacobian0(1,1) = 1.;
        jacobian0(2,2) = 1.;
        jacobian0(3,3) = 1.;
        jacobian0(4,4) = -P*P;

        AmgMatrix(5,5) newmatrix = jacobian0.transpose() * covmatrix * jacobian0;

        ATH_MSG_DEBUG( "addParticle: new matrix \n" << newmatrix );

        bool isOK = doSanityChecksOnCovariance(mu4vec, newmatrix);
        // Get covariance in cartesian CS
        AmgMatrix(5,5) covarXYZ;
        covarXYZ.setZero();
        convertCovd0z0PhiThetaPToXYZ(mu4vec, newmatrix, covarXYZ);
        // Save in input
        input.addConstituent_FourVector_d0z0PhiThetaP(mu4vec, newmatrix, covarXYZ, isOK);

        ATH_MSG_VERBOSE( "addParticle: - added muon");

    }
    
    void
    ConstraintFit::addParticle   (const xAOD::Electron& el,
                                  float                 elEnergyRes,
                                  ConstraintFitInput&   input)
    {
        float el_E_res = elEnergyRes;
        // if (!m_energyRescaler.empty()) {
        //     // Use el.e() which is assumed to be equal to the corrected cluster energy
        //     el_E_res = m_energyRescaler->resolution(el.caloCluster()->e(), el.caloCluster()->eta())*
        //         el.caloCluster()->e();
        // }
        // else ATH_MSG_ERROR( "addParticle: - energyRescaler is NOT AVAILABLE. ");

        const xAOD::TrackParticle* track = el.trackParticle();

        if (!track) {
            ATH_MSG_ERROR( "addParticle: - ERROR could not get track particle for electron");
            return;
        }

        ATH_MSG_DEBUG ("addParticle: *** 4vecEl: " << el.pt() << "/" << el.eta() << "/" << el.phi());

        AmgMatrix(5,5) covmatrix;
        covmatrix.setZero();
        covmatrix(d0,d0)         = track->definingParametersCovMatrix()(d0,d0);
        covmatrix(z0,z0)         = track->definingParametersCovMatrix()(z0,z0);
        covmatrix(phi0,phi0)     = track->definingParametersCovMatrix()(phi0,phi0);
        covmatrix(theta,theta)   = track->definingParametersCovMatrix()(theta,theta);
        covmatrix(qOverP,qOverP) = el_E_res*el_E_res;
        covmatrix(d0,z0)         = track->definingParametersCovMatrix()(d0,z0);
        covmatrix(d0,phi0)       = track->definingParametersCovMatrix()(d0,phi0);
        covmatrix(d0,theta)      = track->definingParametersCovMatrix()(d0,theta);
        covmatrix(d0,qOverP)     = 0;
        covmatrix(z0,phi0)       = track->definingParametersCovMatrix()(z0,phi0);
        covmatrix(z0,theta)      = track->definingParametersCovMatrix()(z0,theta);
        covmatrix(z0,qOverP)     = 0;
        covmatrix(phi0,theta)    = track->definingParametersCovMatrix()(phi0,theta);
        covmatrix(phi0,qOverP)   = 0;
        covmatrix(theta,qOverP)  = 0;

        ATH_MSG_DEBUG( "addParticle: el p,cov" 
                       << " " << el_E_res << " " << el.e()
                       << " " << track->definingParametersCovMatrix()(d0,    d0)
                       << " " << track->definingParametersCovMatrix()(z0,    z0)
                       << " " << track->definingParametersCovMatrix()(phi0,  phi0)
                       << " " << track->definingParametersCovMatrix()(theta, theta)
                       << " " << track->definingParametersCovMatrix()(d0,    z0)
                       << " " << track->definingParametersCovMatrix()(d0,    phi0)
                       << " " << track->definingParametersCovMatrix()(d0,    theta)
                       << " " << track->definingParametersCovMatrix()(z0,    phi0)
                       << " " << track->definingParametersCovMatrix()(z0,    theta)
                       << " " << track->definingParametersCovMatrix()(phi0,  theta));
        


        for(int ii=0;ii<5;ii++)
            for(int jj=ii+1;jj<5;jj++)
                covmatrix(jj,ii) = covmatrix(ii,jj);

        bool isOK = doSanityChecksOnCovariance(el.p4(), covmatrix);
        // Get covariance in cartesian CS
        AmgMatrix(5,5) covarXYZ;
        covarXYZ.setZero();
        convertCovd0z0PhiThetaPToXYZ(el.p4(), covmatrix, covarXYZ);
        // Save in input
        input.addConstituent_FourVector_d0z0PhiThetaP(el.p4(), covmatrix, covarXYZ, isOK);

        ATH_MSG_DEBUG( "addParticle: - el \n" << covmatrix);

        ATH_MSG_VERBOSE( "addParticle: - el\n" << input.getCovarianceCartesian(input.getNConstituents()-1));

        ATH_MSG_VERBOSE( "addParticle: - added electron");
    }
    
    int
    ConstraintFit::massFitInterface(const ZMassConstraint::ConstraintFitInput& theInput)
    {
        m_objmass.clear();
        m_theInput = theInput;
 
        // the m_parameters first parameters of pin are the fitted parameters
        // the next one is the mass of the particle
        if(theInput.getNConstituents() <2 ) {
            ATH_MSG_ERROR( "massFitInterface: Found number of input particles less than 2");
            return 1; 
        }
        else {
            ATH_MSG_DEBUG( "massFitInterface: nparticles " << theInput.getNConstituents());
        }
        
        m_nobj            = theInput.getNConstituents();
        unsigned int dimension    = m_parameters * m_nobj;
        m_parametersInit = Amg::MatrixX(dimension, 1);
        m_parametersInit.setZero();
        m_covarianceInit = Amg::MatrixX(dimension, dimension);
        m_covarianceInit.setZero();
        m_parametersFinal= Amg::MatrixX(dimension, 1);
        m_parametersFinal.setZero();
        m_covarianceFinal= Amg::MatrixX(dimension, dimension);
        m_covarianceFinal.setZero();

        for (unsigned int iobj = 0; iobj < m_nobj; iobj++)
            {
                m_objmass.push_back(theInput.getConstituentFourVector(iobj).M());
                m_parametersInit(0 + iobj*3, 0) = theInput.getConstituentFourVector(iobj).Px();
                m_parametersInit(1 + iobj*3, 0) = theInput.getConstituentFourVector(iobj).Py();
                m_parametersInit(2 + iobj*3, 0) = theInput.getConstituentFourVector(iobj).Pz();

                for (unsigned int i = 0; i < m_parameters; i++)
                    for (unsigned int j = 0; j < m_parameters; j++)
                        m_covarianceInit(i + 3*iobj, j + 3*iobj) = (theInput.getCovarianceCartesian(iobj))(i + 2, j + 2); //keep only the lower right 3x3 part
            }
  
        for(unsigned int i=0;i<dimension;i++)
            {
                m_parametersFinal(i,0) = m_parametersInit(i,0);
                for(unsigned int j=0;j<dimension;j++)
                    m_covarianceFinal(i,j) = m_covarianceInit(i,j);
            }
        ATH_MSG_VERBOSE( "massFitInterface: parameters \n" << m_parametersInit );
        ATH_MSG_VERBOSE( "massFitInterface: cov matrix \n" << m_covarianceInit );
        return 0;
    }

    double
    ConstraintFit::likelihoodMass(double MassResol)
    {
        const int rows =4;
        const int columns = m_nobj;
        double **p = 0;
        p = new double *[rows] ;
        for( int i = 0 ; i < rows ; i++ )
            p[i] = new double[columns];

        for(unsigned int iobj=0; iobj<m_nobj; iobj++)
            {
                p[3][iobj] = m_objmass[iobj]*m_objmass[iobj];
                for(int i=0;i<3;i++)
                    {
                        p[i][iobj] = m_parametersInit(i + 3*iobj, 0);
                        p[3][iobj] += p[i][iobj]*p[i][iobj];
                    }
                p[3][iobj] = sqrt(p[3][iobj]);
            }
        double Etot = 0.;
        double Xtot = 0.;
        double Ytot = 0.;
        double Ztot = 0.;
        for(unsigned int ij=0;ij<m_nobj;ij++)
            {
                Etot += p[3][ij];
                Xtot += p[0][ij];
                Ytot += p[1][ij];
                Ztot += p[2][ij];
            }
        for( int i = 0 ; i < rows ; i++ )
            delete [] p[i];
        delete [] p;

        double initMass =  Etot * Etot - Xtot * Xtot - Ytot * Ytot - Ztot * Ztot;
        initMass = sqrt(initMass);

        double sig = MassResol;
        double xLeft = initMass;
        double xRight = m_conMass;
        if (m_conMass < initMass) {
            xLeft = m_conMass;
            xRight = initMass;
        }
        double dLinitL = (initMass - xLeft) / sig / sig - 2.*(xLeft - m_conMass) / ((xLeft - m_conMass) * (xLeft - m_conMass) + m_conWidth * m_conWidth);
        double dLinitR = (initMass - xRight) / sig / sig - 2.*(xRight - m_conMass) / ((xRight - m_conMass) * (xRight - m_conMass) + m_conWidth * m_conWidth);
        if (dLinitL * dLinitR < 0.) {
            while (xRight - xLeft > 1.) { //1 MeV
                double xM = (xRight + xLeft) / 2.;
                double dL = (initMass - xM) / sig / sig - 2.*(xM - m_conMass) / ((xM - m_conMass) * (xM - m_conMass) + m_conWidth * m_conWidth);
                if (dL * dLinitL < 0.) {
                    xRight = xM;
                    dLinitR = dL;
                } else {
                    xLeft = xM;
                    dLinitL = dL;
                }
            }
            return (xLeft + xRight) / 2.;
        } else {
            if (dLinitL > dLinitR)
                return xLeft;
            else
                return xRight;
        }
    }

    double
    ConstraintFit::likelihoodMass2(void)
    {
        ATH_MSG_VERBOSE( "likelihoodMass2: entering" );
        const int rows =4;
        const int columns = m_nobj;
        double **p = 0;
        p = new double *[rows] ;
        for( int i = 0 ; i < rows ; i++ )
            p[i] = new double[columns];

        for(unsigned int iobj=0; iobj<m_nobj; iobj++) {
            p[3][iobj] = m_objmass[iobj]*m_objmass[iobj];
            for(int i=0;i<3;i++)                 {
                p[i][iobj] = m_parametersInit(i + 3*iobj, 0);
                p[3][iobj] += p[i][iobj]*p[i][iobj];
            }
            p[3][iobj] = sqrt(p[3][iobj]);
        }
        double etot = 0.;
        double xtot = 0.;
        double ytot = 0.;
        double ztot = 0.;
        for(unsigned int ij=0;ij<m_nobj;ij++) {
            etot += p[3][ij];
            xtot += p[0][ij];
            ytot += p[1][ij];
            ztot += p[2][ij];
        }
    
        double initMass =  etot * etot - xtot * xtot - ytot * ytot - ztot * ztot;
        initMass = sqrt(initMass);
        //what is the uncertainty on initMass?
        Amg::MatrixX jacobianMass(3*m_nobj, 1);
        jacobianMass.setZero();
        for(unsigned int ii=0;ii<m_nobj;ii++) {
            jacobianMass(3*ii + 0, 0) = (2.*(etot) * p[0][ii] / p[3][ii] - 2.* xtot);
            jacobianMass(3*ii + 1, 0) = (2.*(etot) * p[1][ii] / p[3][ii] - 2.* ytot);
            jacobianMass(3*ii + 2, 0) = (2.*(etot) * p[2][ii] / p[3][ii] - 2.* ztot);
        }
        ATH_MSG_VERBOSE( "likelihoodMass2: the jacobian mass/matrix \n" << jacobianMass << "\n" << m_covarianceInit );
        

        Amg::MatrixX sigm = jacobianMass.transpose() * m_covarianceInit * jacobianMass;

        ATH_MSG_VERBOSE( "likelihoodMass2: sigm \n" << sigm );

        double sig = sigm(0, 0);
        sig = sqrt(sig);
        m_resolution = sig;
        sig /= 2 * initMass;
        double MassResol = sig;
        double maxmass = initMass;

        ATH_MSG_VERBOSE( "likelihoodMass2: 2 " );

        double max = -(maxmass - initMass) * (maxmass - initMass) / 2. / MassResol / MassResol 
            - log((maxmass * maxmass - m_conMass * m_conMass) * (maxmass * maxmass - m_conMass * m_conMass) + m_conMass * m_conMass * m_conWidth * m_conWidth);

        for (int i = 1; i < 401; i++) {
            double ytest = initMass + (m_conMass - initMass) / 400 * i;

            double val = -(ytest - initMass) * (ytest - initMass) / 2. / MassResol / MassResol 
                - log((ytest * ytest - m_conMass * m_conMass) * (ytest * ytest - m_conMass * m_conMass) + m_conMass * m_conMass * m_conWidth * m_conWidth);

            if (val > max) {
                max = val;
                maxmass = ytest;
            }
        }
  
        ATH_MSG_VERBOSE( "likelihoodMass2: 3 " );
  
        for( int i = 0 ; i < rows ; i++ )
            delete [] p[i];
        delete [] p;

        ATH_MSG_VERBOSE( "likelihoodMass2: maxmass " << maxmass );

        return maxmass;
    }

    void
    ConstraintFit::massFitRun(ConstraintFitOutput& output, double zresol)
    {
        ATH_MSG_VERBOSE( "massFitRun: entering ");

        if(!m_ignoreInputChecks && !m_theInput.isOK()) {
            ATH_MSG_WARNING("ConstraintFit::massFitRun() the input is not OK.");
            ATH_MSG_WARNING("ConstraintFit::massFitRun()(cont) Following H->4l group decision (Feb 1, 2013), this event will not be fit");
            ATH_MSG_WARNING("ConstraintFit::massFitRun()(cont) To bypass this behaviour set the ingore flag");

            std::vector<TLorentzVector> particleList;
            for(unsigned int i=0;i<m_nobj;i++)
                {
                    TLorentzVector particle;
                    particle.SetXYZM(m_parametersInit(0 + i*m_parameters,0),
                                     m_parametersInit(1 + i*m_parameters,0),
                                     m_parametersInit(2 + i*m_parameters,0),
                                     m_objmass[i]);
                    particleList.push_back(particle);
	  
                }
            Amg::MatrixX covOut(5*m_nobj,5*m_nobj);
            covOut.setZero();
            for(unsigned int iobj=0;iobj<m_nobj;iobj++)
                for(int ii=0;ii<5;ii++)
                    for(int jj=0;jj<5;jj++)
                        covOut(ii+5*iobj,jj+5*iobj) = m_theInput.getCovarianceCartesian(iobj)(ii,jj);

            ATH_MSG_VERBOSE( "massFitRun: cov 1 ");

            // convert covariance matrix from cartesian to d0z0PhiThetaP
            Amg::MatrixX covard0z0PhiThetaP;
            covard0z0PhiThetaP.setZero();
            convertCovXYZTod0z0PhiThetaP(particleList, covOut, covard0z0PhiThetaP);
            // save output
            output.setFitOutput(particleList, covOut, covard0z0PhiThetaP);

            // ATH_MSG_VERBOSE( "massFitRun: cov 1 " << covOut);

            return;
        }

        ATH_MSG_VERBOSE( "massFitRun: 1 ");

        Amg::MatrixX parametersFit = m_parametersInit;
        double mass = -1.;
        if (!m_conHasWidth) 
            mass = m_conMass;
        else if(zresol<0.)
            mass = likelihoodMass2();
        else
            mass = likelihoodMass(zresol);

        ATH_MSG_VERBOSE( "massFitRun: resolution/mass " << m_resolution << "/" << mass);

        // do not use chi2:
        // double chi2 = massFit(&m_parametersInit, &m_covarianceInit, mass, &parametersFit, &m_covarianceFinal);
        massFit(m_parametersInit, m_covarianceInit, mass, parametersFit, m_covarianceFinal);

        ATH_MSG_VERBOSE( "massFitRun: 2");

        std::vector<TLorentzVector> particleList;
        for(unsigned int i=0;i<m_nobj;i++) {
            TLorentzVector particle;
            particle.SetXYZM(m_parametersFinal(0 + i*m_parameters,0),
                             m_parametersFinal(1 + i*m_parameters,0),
                             m_parametersFinal(2 + i*m_parameters,0),
                             m_objmass[i]);
            particleList.push_back(particle);
        }

        ATH_MSG_VERBOSE( "massFitRun: 3 ");

        Amg::MatrixX covOut(5*m_nobj,5*m_nobj);
        covOut.setZero();
        for(unsigned int iobj=0;iobj<m_nobj;iobj++)
            for(int ii=0;ii<2;ii++)
                for(int jj=0;jj<2;jj++)
                    covOut(ii+5*iobj,jj+5*iobj) = m_theInput.getCovarianceCartesian(iobj)(ii,jj);
        for(unsigned int iobj=0;iobj<m_nobj;iobj++)
            for(int ii=2;ii<5;ii++)
                for(int jj=2;jj<5;jj++)
                    covOut(ii+5*iobj,jj+5*iobj) = m_covarianceFinal(ii-2+iobj*3,jj-2+iobj*3);

        ATH_MSG_VERBOSE( "massFitRun: 4 ");

        // empirical corrections for constraint width
        if(!m_conHasWidth) {
            // correlation terms
            for(unsigned int iobj1=0;iobj1<m_nobj;iobj1++) {
                for(unsigned int iobj2=0;iobj2<m_nobj;iobj2++) {
                    if(iobj1 == iobj2) continue;
                    for(int ii=2;ii<5;ii++)
                        for(int jj=2;jj<5;jj++)
                            covOut(ii + 5*iobj1, jj+5*iobj2) = m_covarianceFinal(ii-2+iobj1*3,jj-2+iobj2*3);
                }
            }
        }

        // ATH_MSG_DEBUG( "massFitRun: cov 2 ");

        // convert covariance matrix from cartesian to d0z0PhiThetaP
        Amg::MatrixX covard0z0PhiThetaP;
        covard0z0PhiThetaP.setZero();
        convertCovXYZTod0z0PhiThetaP(particleList, covOut, covard0z0PhiThetaP);
        // save output
        output.setFitOutput(particleList, covOut, covard0z0PhiThetaP);

        // ATH_MSG_DEBUG( "massFitRun: cov 2 " << covOut);


        ATH_MSG_VERBOSE( "massFitRun: 5 conHasWidth " << m_conHasWidth);

        // ATH_MSG_DEBUG( "massFitRun: 5.5 conHasWidth " << m_conHasWidth);

        if(m_conHasWidth) {
            TLorentzVector aa;
            for(unsigned int iobj = 0; iobj < m_nobj; iobj++) {
                aa += m_theInput.getConstituentFourVector(iobj);
            }
            double massInit = aa.M();

            // ATH_MSG_DEBUG( "massFitRun: 5.6 massInit " << massInit);

            if(massInit<83.e3) {

                ATH_MSG_VERBOSE( "massFitRun: 6 \n" << covOut);

                for(unsigned int iobj = 0; iobj < m_nobj; iobj++) {
                    for(int ii = 0; ii < 5; ii++) {
                        for(int jj = 0; jj < 5; jj++) {
                            covOut(ii+5*iobj, jj+5*iobj) = m_theInput.getCovarianceCartesian(iobj)(ii, jj);
                        }
                    }
                }

                ATH_MSG_VERBOSE( "massFitRun: 6.1 \n" << covOut);
            }
            else {

                // ATH_MSG_DEBUG( "massFitRun: 7 ");

                std::vector<double> resolInit;
                std::vector<double> resolFinal;
                std::vector<double> resolFinalnew;
                resolInit.reserve(m_nobj);
                resolFinal.reserve(m_nobj);
                resolFinalnew.reserve(m_nobj);
                double sumResolInit=0.;
                const double constraintWidthSquare = /*m_resolution*m_resolution + */m_conWidth*m_conWidth/2.35/2.35;
                for(unsigned int iobj = 0; iobj < m_nobj; iobj++) {

                    // ATH_MSG_DEBUG( "massFitRun: 7 iobj " << iobj << " " << m_theInput.getNConstituents());

                    resolInit.push_back((m_theInput.getConstituentCovariance(iobj))(4,4));

                    // ATH_MSG_DEBUG( "massFitRun: 7 1 ");

                    AmgMatrix(5,5) tmpMtx;
                    tmpMtx.setZero();
                    output.getConstituentCovarianced0z0PhiThetaP(iobj, tmpMtx);
                    resolFinal.push_back(tmpMtx(4,4));
                    // resolFinal.push_back((output.getConstituentCovarianced0z0PhiThetaP(iobj))(4,4));

                    // ATH_MSG_DEBUG( "massFitRun: 7 2 ");

                    sumResolInit += resolInit[iobj];
                    //std::cout << " ### "<< iobj <<"\t"<< resolInit[iobj] <<"\t"<<resolFinal[iobj]<<std::endl;
                }
	  

                ATH_MSG_VERBOSE( "massFitRun: 8 \n" << covOut);

                for(unsigned int iobj=0;iobj<m_nobj;iobj++) {
                    resolFinalnew.push_back( resolFinal[iobj] + constraintWidthSquare * 
                                             resolInit[iobj]*resolInit[iobj]
                                             /sumResolInit/sumResolInit );
                    covOut(4+5*iobj,4+5*iobj) =  covOut(4+5*iobj,4+5*iobj)*resolFinalnew[iobj]/resolFinal[iobj];
                    covOut(3+5*iobj,3+5*iobj) =  covOut(3+5*iobj,3+5*iobj)*resolFinalnew[iobj]/resolFinal[iobj];
                    covOut(2+5*iobj,2+5*iobj) =  covOut(2+5*iobj,2+5*iobj)*resolFinalnew[iobj]/resolFinal[iobj];
                    //std::cout << " ### "<< iobj <<"\t"<< resolInit[iobj] <<"\t"<<resolFinal[iobj]<<"\t"<< resolFinalnew[iobj]<<std::endl;
                }
                ATH_MSG_VERBOSE( "massFitRun: 8.1 \n" << covOut);
            }

            ATH_MSG_VERBOSE( "massFitRun: 9 ");

            // ATH_MSG_DEBUG( "massFitRun: cov 3 ");

            // convert covariance matrix from cartesian to d0z0PhiThetaP
            Amg::MatrixX covard0z0PhiThetaP;
            convertCovXYZTod0z0PhiThetaP(particleList, covOut, covard0z0PhiThetaP);
            // save output
            output.setFitOutput(particleList, covOut, covard0z0PhiThetaP);

            ATH_MSG_VERBOSE( "massFitRun: cov 3 \n" << covOut);

        }

        ATH_MSG_VERBOSE( "massFitRun: end ");
    }

    double
    ConstraintFit::massFitCalculation(const Amg::MatrixX& var, double mass, Amg::MatrixX& p0)
    {
        const int rows =4;
        const int columns = m_nobj;

        double **p = 0;
        p = new double *[rows] ;
        for( int i = 0 ; i < rows ; i++ )
            p[i] = new double[columns];

        for(unsigned int iobj=0; iobj<m_nobj; iobj++) {
            p[3][iobj] = m_objmass[iobj]*m_objmass[iobj];
            for(int i=0;i<3;i++)
                {
                    p[i][iobj] = p0(i+3*iobj, 0);
                    p[3][iobj] += p[i][iobj]*p[i][iobj];
                }
            p[3][iobj] = sqrt(p[3][iobj]);
        }

        double Etot = 0.;
        double Xtot = 0.;
        double Ytot = 0.;
        double Ztot = 0.;
        for(unsigned int ij=0;ij<m_nobj;ij++)
            {
                Etot += p[3][ij];
                Xtot += p[0][ij];
                Ytot += p[1][ij];
                Ztot += p[2][ij];
            }
        double XYZtot[3] = {Xtot, Ytot, Ztot};
        std::vector<double> constraintD;
        for(unsigned int ij=0;ij<m_nobj;ij++)
            for(int ik=0;ik<3;ik++)
                constraintD.push_back((2.*(Etot) * p[ik][ij] / p[3][ij] - 2.* XYZtot[ik]));

        double constraintd =  Etot * Etot - Xtot * Xtot - Ytot * Ytot - Ztot * Ztot;
        constraintd = constraintd - mass * mass;
        //std::cout << "constraint = " <<constraintd << " mass value " <<mass<<std::endl;
        Amg::MatrixX D(1,3*m_nobj);
        for(unsigned int ij=0;ij<3*m_nobj;ij++)
            D(0, ij) = constraintD.at(ij);
        AmgMatrix(1,1) d;
        d(0, 0) = constraintd;

        Amg::MatrixX DVD(D * var * D.transpose());

        Amg::MatrixX DVDinverse(1,1);
        DVDinverse=DVD.inverse();

        Amg::MatrixX VD(DVDinverse);
        Amg::MatrixX lambda(VD * d);
        Amg::MatrixX test(p0 - var * D.transpose() * lambda);
        Amg::MatrixX Vp(var - var * D.transpose() * VD * D * var);
        for(unsigned int ij=0;ij<3*m_nobj;ij++) {
            p0(ij, 0) = test(ij, 0);
            m_parametersFinal(ij,0) = p0(ij,0);
            for(unsigned int jk = 0; jk < 3 * m_nobj; jk++)
                m_covarianceFinal(ij,jk) = Vp(ij,jk);
        }
        double constraintValue = d(0, 0);

        for( int i = 0 ; i < rows ; i++ )
            delete [] p[i];
        delete [] p;
        return constraintValue;
    }

    double
    ConstraintFit::massFit(const Amg::MatrixX& /*p0*/, const Amg::MatrixX& var, double mass, Amg::MatrixX& pOut,
                           Amg::MatrixX& /*varout*/)
    {
        Amg::MatrixX ivar = var.inverse();

        bool doIter = true;
        int  maxIterations = 20;
        int  iIter  = 0;
        double constraintValue = -1.e10;
        ATH_MSG_VERBOSE( "massFit: transpose matrix \n" << pOut.transpose() 
                       << " Iterations " << iIter
                       << " parameters \n" << m_parametersFinal
                       << " cov matrix \n" << m_covarianceFinal
                       << " Iteration  " << iIter);
        while (doIter) {
            Amg::MatrixX p_old(pOut);
            constraintValue = massFitCalculation(var, mass, pOut); //call the fit

            // Convergence criteria
            // 1. the parameters should not change too much (<1.e-6 relative change)
            // 2. the constraint should be satisfied very well (<1.e-6 absolute)
            double maxDiff = 5.e15;
            Amg::MatrixX diff = (pOut - p_old);
            for (unsigned int i = 0; i < m_parameters * m_nobj; i++) {
                diff(i, 0) = diff(i, 0) / p_old(i, 0);
                if (maxDiff > diff(i, 0))
                    maxDiff = diff(i, 0);
            }
            if ((maxDiff < 1.e-4 && fabs(constraintValue) < 5.e-5) || maxIterations <= iIter)
                doIter = false;
            ATH_MSG_VERBOSE( "massFit: transpose matrix \n" << pOut.transpose() 
                           << " Iterations " << iIter
                           << " parameters \n" << m_parametersFinal
                           << " cov matrix \n" << m_covarianceFinal
                           << " \n Iteration  " << iIter << " doIter " << doIter);
            iIter++;

        }
        ATH_MSG_VERBOSE( "massFit: transpose matrix \n" << pOut.transpose() 
                       << " Iterations " << iIter
                       << " parameters \n" << m_parametersFinal
                       << " cov matrix \n" << m_covarianceFinal
                       << " Iteration  " << iIter);
        // double chi2 = 0.;//calculateChi2(p0, var, mass);
        return 0;
    }

    /// Calculate the mass error without fit - use just the inputs
    double
    ConstraintFit::getMassError  (const ConstraintFitInput& firstInput,
                                  const ConstraintFitInput& secondInput)
    {
        unsigned int nPartFirst  = firstInput.getNConstituents();
        unsigned int nPartSecond = secondInput.getNConstituents();
        unsigned int nPart = nPartFirst + nPartSecond;
        std::vector<TLorentzVector> particles(nPart);
        std::vector<AmgMatrix(3,3)> covariances(nPart);
        for ( unsigned int iobj = 0; iobj < firstInput.getNConstituents(); ++iobj) {
            covariances[iobj].setZero();
            particles[iobj] = firstInput.getConstituentFourVector(iobj);
            firstInput.getConstituentCovariancePhiThetaP(iobj, covariances[iobj]);
        }
        for ( unsigned int iobj = 0; iobj < secondInput.getNConstituents(); ++iobj) {
            covariances[iobj + nPartFirst].setZero();
            particles[iobj + nPartFirst] = secondInput.getConstituentFourVector(iobj);
            secondInput.getConstituentCovariancePhiThetaP(iobj, covariances[iobj + nPartFirst]);
        }
        return getMassError( particles, covariances );
    }

    /// Calculate the mass error with the result of fit and possible additional input
    /// (4-vecs/cov mat) that has NOT been fit
    double
    ConstraintFit::getMassError  (const ConstraintFitOutput& fitOutput,
                                  const ConstraintFitInput&  extraInput)
    {
        unsigned int nPartFirst  = fitOutput.getNConstituents();
        unsigned int nPartSecond = extraInput.getNConstituents();
        unsigned int nPart = nPartFirst + nPartSecond;
        std::vector<TLorentzVector> particles(nPart);
        std::vector<AmgMatrix(3,3)> covariances(nPart);
        for ( unsigned int iobj = 0; iobj < fitOutput.getNConstituents(); ++iobj) {
            covariances[iobj].setZero();
            particles[iobj] = fitOutput.getConstituentFourVector(iobj);
            fitOutput.getConstituentCovariancePhiThetaP(iobj, covariances[iobj]);
        }
        for ( unsigned int iobj = 0; iobj < extraInput.getNConstituents(); ++iobj) {
            covariances[iobj + nPartFirst].setZero();
            particles[iobj + nPartFirst] = extraInput.getConstituentFourVector(iobj);
            extraInput.getConstituentCovariancePhiThetaP(iobj, covariances[iobj + nPartFirst]);
        }
        return getMassError( particles, covariances );
    }
    


    /// Calculate the mass error with the result of two mass fits        
    double
    ConstraintFit::getMassError  (const ConstraintFitOutput& fitOutput,
                                  const ConstraintFitOutput& secondFitOutput)
    {
        unsigned int nPartFirst  = fitOutput.getNConstituents();
        unsigned int nPartSecond = secondFitOutput.getNConstituents();
        unsigned int nPart = nPartFirst + nPartSecond;
        std::vector<TLorentzVector> particles(nPart);
        std::vector<AmgMatrix(3,3)> covariances(nPart);
        for ( unsigned int iobj = 0; iobj < fitOutput.getNConstituents(); ++iobj) {
            covariances[iobj].setZero();
            particles[iobj] = fitOutput.getConstituentFourVector(iobj);
            fitOutput.getConstituentCovariancePhiThetaP(iobj, covariances[iobj]);
        }
        for ( unsigned int iobj = 0; iobj < secondFitOutput.getNConstituents(); ++iobj) {
            covariances[iobj + nPartFirst].setZero();
            particles[iobj + nPartFirst] = secondFitOutput.getConstituentFourVector(iobj);
            secondFitOutput.getConstituentCovariancePhiThetaP(iobj, covariances[iobj + nPartFirst]);
        }
        return getMassError( particles, covariances );
    }
    
    double
    ConstraintFit::getMassError(const std::vector<TLorentzVector>& particles,
                                const std::vector<AmgMatrix(3,3)>& covariances)
    {
        double masserror=0;

        if (particles.size() != covariances.size()) {
            ATH_MSG_ERROR( "getMassError: Number of particles is not equal to the number of covariance matrices");
            return 1E11;
        }

        ATH_MSG_DEBUG ("getMassError: 1 " << particles.size() << "/" << covariances.size());
        
        //composite lorentz vector and its invariant mass
        TLorentzVector combv;
        for ( auto lv : particles ) combv += lv;
        double invmass = combv.M();

        ATH_MSG_VERBOSE ("getMassError: 1.1 invmass " << invmass);

        //Calculation of 1x4 jacobian - used in derivation of var(M)
        Amg::MatrixX jacobianNPtoM(1,4);
        jacobianNPtoM.setZero();
        jacobianNPtoM(0,0) = -1.*combv.Px()/invmass;  // dM/dpxN
        jacobianNPtoM(0,1) = -1.*combv.Py()/invmass;  // dM/dpyN
        jacobianNPtoM(0,2) = -1.*combv.Pz()/invmass;  // dM/dpzN
        jacobianNPtoM(0,3) =     combv.E()/invmass;   // dM/dEN

        ATH_MSG_VERBOSE ("getMassError: 2\n" << jacobianNPtoM);

        int iobj = -1;
        for ( auto lv : particles ) {
            ++iobj;

            ATH_MSG_VERBOSE ("getMassError: 1.2 iobj " << iobj);
            
            // Constituent 3*3 covariance matrix (Phi, Theta, P) 
            const AmgMatrix(3,3)& covPhiThetaP = covariances[iobj];

            ATH_MSG_VERBOSE ("getMassError: 1.3 covPhiThetaP\n " << covPhiThetaP);

            double theta = lv.Theta();
            double phi   = lv.Phi();
            double p     = lv.P();
            double e     = lv.E();
            double m     = lv.M(); 

            ATH_MSG_VERBOSE ("getMassError: 2.1 iobj " << iobj << "covPhiThetaP\n" << covPhiThetaP);

            // Convert (Phi, Theta, P) to (Phi, eta, P), as covPhiEtaP
            AmgMatrix(3,3) jacobian1;
            jacobian1.setZero();
            jacobian1(0,0) =  1.;                   // 
            jacobian1(1,1) = -1*(1./sin(theta));    // deta/dtheta
            jacobian1(2,2) =  1.;                   // 
            AmgMatrix(3,3) covPhiEtaP = jacobian1 * covPhiThetaP * jacobian1.transpose();

            ATH_MSG_VERBOSE ("getMassError: 2.2 jacobian1\n" << jacobian1);
            ATH_MSG_VERBOSE ("getMassError: 2.4 covPhiEtaP\n" << covPhiEtaP);

            
            // Rearrange things to get (E, eta, phi, M),  as covEEtaPhiM
            AmgMatrix(4,4) covEEtaPhiM;
            covEEtaPhiM.setZero();
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 3; j++) {
                    covEEtaPhiM(i,j) = covEEtaPhiM(j,i) = covPhiEtaP(2-i,2-j);
                }
            } 
    
            ATH_MSG_VERBOSE ("getMassError: 3 covEEtaPhiM\n" << covEEtaPhiM);

            //convert (E, eta, phi, M) to (px,py,pz,E) representation, as covPxPyPzE
            AmgMatrix(4,4) jacobian2;
            jacobian2.setZero();
            jacobian2(0,0) =  e/p * sin(theta)*cos(phi); // dpx/dE
            jacobian2(1,0) =  e/p * sin(theta)*sin(phi); // dpy/dE
            jacobian2(2,0) =  e/p * cos(theta);          // dpy/dE 
            jacobian2(3,0) =  1;                     // dE/dE
    
            jacobian2(0,1) =  -p * cos(phi) * cos(theta)*sin(theta); // dpx/deta
            jacobian2(1,1) =  -p * sin(phi) * cos(theta)*sin(theta); // dpy/deta
            jacobian2(2,1) =   p * sin(theta)*sin(theta);            // dpz/deta;

            jacobian2(0,2) =  -p * sin(theta) * sin(phi);  // dpx/dphi
            jacobian2(1,2) =   p * sin(theta) * cos(phi);  // dpy/dphi
    
            jacobian2(0,3) =  -m/p * sin(theta)*cos(phi); // dpx/dM
            jacobian2(1,3) =  -m/p * sin(theta)*sin(phi); // dpy/dM
            jacobian2(2,3) =  -m/p * cos(theta);          // dpz/dM

            AmgMatrix(4,4) covPxPyPzE = jacobian2 * covEEtaPhiM * jacobian2.transpose();

            ATH_MSG_VERBOSE ("getMassError: 4 jacobian2\n" << jacobian2);
            ATH_MSG_VERBOSE ("getMassError: 5 covPxPyPzE\n" << covPxPyPzE);

            //Get mass variance
            // (Similarity transform: C = A*B*A^T (from egammaFourMomentumError/GeneralUtils.cxx))
            Amg::MatrixX  em = jacobianNPtoM * covPxPyPzE * jacobianNPtoM.transpose(); 
            masserror += em(0,0);

            ATH_MSG_VERBOSE ("getMassError: 6 em\n" << em);

        }

        //Return square root of variance.
        if (masserror < 0.) {
            ATH_MSG_WARNING( "getMassError: Mass covariance element less than zero! Returning 1E11 ...");
            masserror = 1E11;
        }
        return sqrt(masserror);

    }

    bool
    ConstraintFit::doSanityChecksOnCovariance(const TLorentzVector& vector,
                                              const AmgMatrix(5,5)& covar) const
    {
        bool isOK = true;
        if ( sqrt(covar(qP,qP))/vector.P()>1.0 ) { 
            isOK = false;
            ATH_MSG_WARNING("ZMassConstraint::ConstraintFit::doSanityChecksOnCovariance:: Fractional uncertainty on P = "
                            << sqrt(covar(qP,qP))/vector.P() << " is > 1 ...this is not ok...");
            
        }
 
        if( covar(theta,theta) > 1.e-2) {
            isOK = false;
            ATH_MSG_WARNING("ZMassConstraint::ConstraintFit::doSanityChecksOnCovariance:: Uncertainty on Theta = "
                            << covar(theta,theta) << " is > 10^-2 ...this is not ok");
        }
        if( covar(phi0,phi0) > 1.e-2) {									
            isOK = false;
            ATH_MSG_WARNING("ZMassConstraint::ConstraintFit::doSanityChecksOnCovariance:: Uncertainty on Phi = "
                            << covar(phi0,phi0) << " is > 10^-2 ...this is not ok");
        }

        for(int i=0;i<5;i++) {
            for(int j=0;j<5;j++) {
                if(i==j) continue;
                if(fabs(covar(i,j))/sqrt(covar(i,i))/sqrt(covar(j,j)) > 1.) {
                    ATH_MSG_WARNING( "ZMassConstraint::ConstraintFit::doSanityChecksOnCovariance:: Off-diagonal term "
                                     << i << " " << j << " is > 1 - doesn't look ok");
                    isOK = false;
                }
            }
        }

        return isOK;
    }

    void
    ConstraintFit::convertCovd0z0PhiThetaPToXYZ(const TLorentzVector& fourVec,
                                                const AmgMatrix(5,5)& covard0z0PhiThetaP,
                                                AmgMatrix(5,5)& covarXYZ) const
    {
        //going from d0,z0,phi,theta,P --> d0,z0,px,py,pz
        double phi   = fourVec.Phi();
        double theta = fourVec.Theta();
        double P     = fourVec.P();
        //std::cout << ":::::L \t"<<P<<"\t"<< phi<<"\t"<<theta<<std::endl;
        AmgMatrix(5,5) jacobian;
        jacobian.setZero();
        jacobian(0,0) = 1.;
        jacobian(1,1) = 1.;
        jacobian(2,2) = -P * TMath::Sin(theta) * TMath::Sin(phi);
        jacobian(2,3) =  P * TMath::Sin(theta) * TMath::Cos(phi);
        jacobian(3,2) =  P * TMath::Cos(theta) * TMath::Cos(phi);
        jacobian(3,3) =  P * TMath::Cos(theta) * TMath::Sin(phi);
        jacobian(3,4) = -P * TMath::Sin(theta);
        jacobian(4,2) =      TMath::Sin(theta) * TMath::Cos(phi);
        jacobian(4,3) =      TMath::Sin(theta) * TMath::Sin(phi);
        jacobian(4,4) =      TMath::Cos(theta);
	
        covarXYZ = jacobian.transpose() * covard0z0PhiThetaP * jacobian;

        // std::cout << "initial\n"<< covar << "final \n" << newcovariance << "jac\n"
        //         << jacobian << std::endl;

    }


    void
    ConstraintFit::convertCovXYZTod0z0PhiThetaP(const std::vector<TLorentzVector>& particleList,
                                                const Amg::MatrixX&                covarXYZ,
                                                Amg::MatrixX&                      covard0z0PhiThetaP) const
    {
        unsigned int matrixSize = 5 * particleList.size();
        Amg::MatrixX Jacobian(matrixSize, matrixSize);
        Jacobian.setZero();
        /// build the Jacobian of the phi,theta,P --> px,py,pz
        for(unsigned int ii = 0; ii < particleList.size(); ii++) {
            double phi   = particleList.at(ii).Phi();
            double theta = particleList.at(ii).Theta();
            double P     = particleList.at(ii).P();
            Jacobian(    5*ii,     5*ii) = 1.;
            Jacobian(1 + 5*ii, 1 + 5*ii) = 1.;

            Jacobian(2 + 5*ii, 2 + 5*ii) = -P * TMath::Sin(theta) * TMath::Sin(phi);
            Jacobian(2 + 5*ii, 3 + 5*ii) =  P * TMath::Sin(theta) * TMath::Cos(phi);
            Jacobian(3 + 5*ii, 2 + 5*ii) =  P * TMath::Cos(theta) * TMath::Cos(phi);
            Jacobian(3 + 5*ii, 3 + 5*ii) =  P * TMath::Cos(theta) * TMath::Sin(phi);
            Jacobian(3 + 5*ii, 4 + 5*ii) = -P * TMath::Sin(theta);
            Jacobian(4 + 5*ii, 2 + 5*ii) =      TMath::Sin(theta) * TMath::Cos(phi);
            Jacobian(4 + 5*ii, 3 + 5*ii) =      TMath::Sin(theta) * TMath::Sin(phi);
            Jacobian(4 + 5*ii, 4 + 5*ii) =      TMath::Cos(theta);
        }

        Amg::MatrixX Jacobianinverse(matrixSize, matrixSize);
        Jacobianinverse=Jacobian.inverse();
        covard0z0PhiThetaP = Jacobianinverse.transpose() * covarXYZ * Jacobianinverse;
    }

    float
    ConstraintFit::retrieve_eta_calo(const xAOD::IParticle&  part) const
    {
        double eta_calo = 0;
        const xAOD::Egamma* eg = dynamic_cast<const xAOD::Egamma*> (&part);
        if (!eg) {
            ATH_MSG_ERROR("retrieve_eta_calo - unable to cast to Egamma");
            return eta_calo;
        }
        const xAOD::CaloCluster& cluster = *eg->caloCluster();
        if (cluster.retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,
                                   eta_calo)) { }
        else if (cluster.isAvailable<float>("etaCalo")) {
            eta_calo = cluster.auxdata<float>("etaCalo");
        }
        else {
            ATH_MSG_ERROR("retrieve_eta_calo - etaCalo not available as auxilliary variable");
            ATH_MSG_WARNING("retrieve_eta_calo - using eta as etaCalo");
            eta_calo = cluster.eta();
        }
        return eta_calo;
    }
}


