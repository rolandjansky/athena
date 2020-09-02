/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RiddersAlgorithm.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkExAlgs/RiddersAlgorithm.h"
// Trk stuff
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExUtils/TransportJacobian.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
#include "GaudiKernel/SystemOfUnits.h"

//================ Constructor =================================================

Trk::RiddersAlgorithm::RiddersAlgorithm(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_propagator("Trk::RungeKuttaPropagator/RungeKuttaPropagator"),       
  m_useCustomField(true),
  m_useAlignedSurfaces(true),
  m_fieldValue(2.*Gaudi::Units::tesla),
  m_magFieldProperties(0),
  m_sigmaLoc(100.*Gaudi::Units::micrometer),    
  m_sigmaR(0.0),
  m_minPhi(-M_PI),                    
  m_maxPhi(M_PI),    
  m_minEta(-2.5),                    
  m_maxEta(2.5),                    
  m_minP(0.5*Gaudi::Units::GeV),                      
  m_maxP(50000.*Gaudi::Units::GeV),
  m_minimumR(10.),
  m_maximumR(1000.),
  m_localVariations(),
  m_angularVariations(),
  m_qOpVariations(),
  m_validationTree(0),  
  m_validationTreeName("RiddersTree"),
  m_validationTreeDescription("Output of the RiddersAlgorithm"),
  m_validationTreeFolder("/val/RiddersAlgorithm"),
  m_steps(0),
  m_loc1loc1{},  
	m_loc1loc2{},  
	m_loc1phi{},   
	m_loc1theta{}, 
	m_loc1qop{},   
	m_loc1steps{}, 

	m_loc2loc1{},  
	m_loc2loc2{},  
	m_loc2phi{},   
	m_loc2theta{}, 
	m_loc2qop{},   
	m_loc2steps{}, 

	m_philoc1{},   
	m_philoc2{},   
	m_phiphi{},    
	m_phitheta{},  
	m_phiqop{},    
	m_phisteps{},  

	m_thetaloc1{}, 
	m_thetaloc2{}, 
	m_thetaphi{},  
	m_thetatheta{},
	m_thetaqop{},  
	m_thetasteps{},

	m_qoploc1{},   
	m_qoploc2{},   
	m_qopphi{},    
	m_qoptheta{},  
	m_qopqop{},    
	m_qopsteps{},  

  m_gaussDist(0),
  m_flatDist(0)
{
  

  declareProperty("Propagator"                , m_propagator);
  declareProperty("CustomFieldValue"          , m_fieldValue);
  declareProperty("UseCustomMagneticField"    , m_useCustomField);
  declareProperty("UseAlignedSurfaces"        , m_useAlignedSurfaces);

  // TTree handling
  declareProperty("ValidationTreeName",          m_validationTreeName);
  declareProperty("ValidationTreeDescription",   m_validationTreeDescription);
  declareProperty("ValidationTreeFolder",        m_validationTreeFolder);

  declareProperty("StartPerigeeSigmaLoc"      , m_sigmaLoc);
  declareProperty("StartPerigeeMinPhi"        , m_minPhi);
  declareProperty("StartPerigeeMaxPhi"        , m_maxPhi);
  declareProperty("StartPerigeeMinEta"        , m_minEta);
  declareProperty("StartPerigeeMaxEta"        , m_maxEta);
  declareProperty("StartPerigeeMinP"          , m_minP);
  declareProperty("StartPerigeeMaxP"          , m_maxP);

  declareProperty("TargetSurfaceMinR"         , m_minimumR);
  declareProperty("TargetSurfaceMaxR"         , m_maximumR);

  declareProperty("LocalVariations"            , m_localVariations);
  declareProperty("AngularVariations"          , m_angularVariations);
  declareProperty("QopVariations"              , m_qOpVariations);


}

//================ Destructor =================================================

Trk::RiddersAlgorithm::~RiddersAlgorithm()
{

   delete m_gaussDist;
   delete m_flatDist;
   delete m_magFieldProperties;
}


//================ Initialisation =================================================

StatusCode Trk::RiddersAlgorithm::initialize()
{
  // Code entered here will be executed once at program start.
  ATH_MSG_INFO( " initialize()" );

  // Get Extrapolator from ToolService   
  if (m_propagator.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve Tool " << m_propagator << ". Exiting." );
        return StatusCode::FAILURE;
  }

  // Prepare the magnetic field properties 
  if (!m_useCustomField)
      m_magFieldProperties = new Trk::MagneticFieldProperties();
  else {
      // the field
      Amg::Vector3D bField(0.,0.,m_fieldValue);
      // create the custom magnetic field
      m_magFieldProperties = new Trk::MagneticFieldProperties(bField);
  }  

  // intialize the random number generators
  m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
  m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));


   // create the new Tree
   m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

   // the branches for the  start
   m_validationTree->Branch("RiddersSteps", &m_steps, "steps/I");
   // loc 1
   m_validationTree->Branch("Loc1Loc1",   m_loc1loc1,    "loc1loc1[steps]/F");
   m_validationTree->Branch("Loc1Loc2",   m_loc1loc2,    "loc1loc2[steps]/F");
   m_validationTree->Branch("Loc1Phi",    m_loc1phi,     "loc1phi[steps]/F");
   m_validationTree->Branch("Loc1Theta",  m_loc1theta,   "loc1theta[steps]/F");
   m_validationTree->Branch("Loc1qOp",    m_loc1qop,     "loc1qop[steps]/F");
   m_validationTree->Branch("Loc1Steps",  m_loc1steps,   "loc1steps[steps]/F");
   // loc 2
   m_validationTree->Branch("Loc2Loc1",   m_loc2loc1,    "loc2loc1[steps]/F");
   m_validationTree->Branch("Loc2Loc2",   m_loc2loc2,    "loc2loc2[steps]/F");
   m_validationTree->Branch("Loc2Phi",    m_loc2phi,     "loc2phi[steps]/F");
   m_validationTree->Branch("Loc2Theta",  m_loc2theta,   "loc2theta[steps]/F");
   m_validationTree->Branch("Loc2qOp",    m_loc2qop,     "loc2qop[steps]/F");
   m_validationTree->Branch("Loc2Steps",  m_loc2steps,   "loc2steps[steps]/F");
   // phi
   m_validationTree->Branch("PhiLoc1",    m_philoc1,     "philoc1[steps]/F");
   m_validationTree->Branch("PhiLoc2",    m_philoc2 ,    "philoc2[steps]/F");
   m_validationTree->Branch("PhiPhi",     m_phiphi,      "phiphi[steps]/F");
   m_validationTree->Branch("PhiTheta",   m_phitheta,    "phitheta[steps]/F");
   m_validationTree->Branch("PhiqOp",     m_phiqop,      "phiqop[steps]/F");
   m_validationTree->Branch("PhiSteps",   m_phisteps,    "phisteps[steps]/F");
    // Theta
   m_validationTree->Branch("ThetaLoc1",  m_thetaloc1,   "thetaloc1[steps]/F");
   m_validationTree->Branch("ThetaLoc2",  m_thetaloc2,   "thetaloc2[steps]/F");
   m_validationTree->Branch("ThetaPhi",   m_thetaphi,    "thetaphi[steps]/F");
   m_validationTree->Branch("ThetaTheta", m_thetatheta,  "thetatheta[steps]/F");
   m_validationTree->Branch("ThetaqOp",   m_thetaqop,    "thetaqop[steps]/F");
   m_validationTree->Branch("ThetaSteps", m_thetasteps,  "thetasteps[steps]/F");
   // Qop
   m_validationTree->Branch("QopLoc1",    m_qoploc1,     "qoploc1[steps]/F");
   m_validationTree->Branch("QopLoc2",    m_qoploc2,     "qoploc2[steps]/F");
   m_validationTree->Branch("QopPhi",     m_qopphi,      "qopphi[steps]/F");
   m_validationTree->Branch("QopTheta",   m_qoptheta,    "qoptheta[steps]/F");
   m_validationTree->Branch("QopqOp",     m_qopqop,      "qopqop[steps]/F");
   m_validationTree->Branch("QopSteps",   m_qopsteps,    "qopsteps[steps]/F");

   // now register the Tree
   ITHistSvc* tHistSvc = 0;
   if (service("THistSvc",tHistSvc).isFailure()){ 
      ATH_MSG_ERROR( "initialize() Could not find Hist Service -> Switching ValidationMode Off !" );
      delete m_validationTree; m_validationTree = 0;
   }
   if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
      ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
      delete m_validationTree; m_validationTree = 0;
   }

  if (!m_localVariations.size()){
       m_localVariations.push_back(0.01);
       m_localVariations.push_back(0.001);
       m_localVariations.push_back(0.0001);
  }

  if (!m_angularVariations.size()){
       m_angularVariations.push_back(0.01);
       m_angularVariations.push_back(0.001);
       m_angularVariations.push_back(0.0001);
  }

  if (!m_qOpVariations.size()){
       m_qOpVariations.push_back(0.0001);
       m_qOpVariations.push_back(0.00001);
       m_qOpVariations.push_back(0.000001);
  }

  ATH_MSG_INFO( "initialize() successful in " );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::RiddersAlgorithm::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::RiddersAlgorithm::execute()
{
 
   // this is fine
   double p = m_minP + m_flatDist->shoot()*(m_maxP-m_minP);
   double charge = (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;   
   double qOverP = charge/p;

   // for the momentum logging
   // m_startP  = p;   

   // the local start  start
   double loc1  = m_sigmaLoc * m_gaussDist->shoot();
   double loc2  = m_sigmaLoc * m_gaussDist->shoot();
   // are adopted for planar and straight line surfaces
   double phi   = m_minPhi + m_maxPhi * m_flatDist->shoot();
   double eta   = m_minEta + m_flatDist->shoot()*(m_maxEta-m_minEta);
   double theta = 2.*atan(exp(-eta));

   // start
   double startR          = fabs(m_sigmaR * m_gaussDist->shoot());
   double surfacePhi      = M_PI * m_flatDist->shoot();
   surfacePhi            *= (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;
   double startX          = startR*cos(surfacePhi);
   double startY          = startR*sin(surfacePhi);
   double startZ          = m_sigmaLoc * m_gaussDist->shoot();

   // rotate it around Z   
   double alphaZ = M_PI * m_flatDist->shoot();
   alphaZ       *= (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;

   // create the plane surface
   Trk::PlaneSurface startSurface(createTransform(startX,
                                                  startY,
                                                  startZ,
                                                  phi, theta,
                                                  alphaZ),
                                                  10e3,10e3);

   // make a covariance Matrix
   AmgMatrix(5,5)* covMat = new AmgMatrix(5,5);

   // the initial perigee with random numbers
   Trk::AtaPlane startParameters(loc1,
                                         loc2,
                                         phi,
                                         theta,
                                         qOverP,
                                         startSurface,
                                         new AmgMatrix(5,5)(covMat->inverse().eval())); 

   ATH_MSG_VERBOSE( "Start Parameters : " << startParameters );


   // destination position
   double estimationR        = m_minimumR + (m_maximumR-m_minimumR) * m_flatDist->shoot();


   // --------------- propagate to find a first intersection ---------------------
   Trk::CylinderSurface estimationCylinder(new Amg::Transform3D, estimationR, 10e10);

   ATH_MSG_VERBOSE( "Cylinder to be intersected : " << estimationCylinder );

   const Trk::TrackParameters* estimationParameters = m_propagator->propagateParameters(startParameters,
                                                                                        estimationCylinder,
                                                                                        Trk::alongMomentum,
                                                                                        false,
                                                                                        *m_magFieldProperties);
   if (!estimationParameters) {
        ATH_MSG_VERBOSE( "Estimation of intersection did not work - skip event !" );
        return StatusCode::SUCCESS;
   } 

   ATH_MSG_VERBOSE( "Estimation Parameters: " << *estimationParameters );

   const Amg::Vector3D& estimatedPosition = estimationParameters->position();

   double estimationX        = estimatedPosition.x();
   double estimationY        = estimatedPosition.y();
   double estimationZ        = estimatedPosition.z();

   double estimationPhi      = estimatedPosition.phi();
   double estimationTheta    = estimatedPosition.theta();



   double rotateTrans =  M_PI * m_flatDist->shoot();
   rotateTrans       *= (m_flatDist->shoot() > 0.5 ) ? -1. : 1.; 

   Amg::Transform3D* surfaceTransform = 0;

   if (m_useAlignedSurfaces){
      // create a radial vector
      Amg::Vector3D radialVector(estimatedPosition.x(), estimatedPosition.y(), 0.);
      Amg::Vector3D surfaceZdirection(radialVector.unit());
      Amg::Vector3D surfaceYdirection(0.,0.,1.);
      // the x direction
      Amg::Vector3D surfaceXdirection(surfaceYdirection.cross(surfaceZdirection));
      // the rotation
      Amg::RotationMatrix3D surfaceRotation;
      surfaceRotation.col(0) = surfaceXdirection;
      surfaceRotation.col(1) = surfaceYdirection;
      surfaceRotation.col(2) = surfaceZdirection;
      Amg::Transform3D nominalTransform(surfaceRotation, estimatedPosition);   
      surfaceTransform =  new Amg::Transform3D(nominalTransform*Amg::AngleAxis3D(rotateTrans,Amg::Vector3D(0.,0.,1.)));
   } else
      surfaceTransform = createTransform(estimationX,
                                         estimationY,
                                         estimationZ,
                                         estimationPhi,
                                         estimationTheta,
                                         rotateTrans);

   // cleanup for memory reasons
   delete estimationParameters; estimationParameters = 0;

   Trk::PlaneSurface destinationSurface(surfaceTransform,10e5 , 10e5);


   // transport the  start to the destination surface
   Trk::TransportJacobian* transportJacobian = 0;
   AmgMatrix(5,5) testMatrix; testMatrix.setZero();
   Trk::TransportJacobian currentStepJacobian(testMatrix);
   double pathLimit = -1.;

   const Trk::TrackParameters* trackParameters = m_propagator->propagate(startParameters,
                                                                         destinationSurface,
                                                                         Trk::alongMomentum,
                                                                         false,
                                                                         *m_magFieldProperties,
                                                                         transportJacobian,
                                                                         pathLimit);

  // --------------------- check if test propagation was successful ------------------------------
  if (trackParameters && transportJacobian){

      // recSetep = 0
      unsigned int recStep = 0;

      // [0] Transport Jacobian -----------------------------------------------------
      ATH_MSG_VERBOSE( "TransportJacobian : " << *transportJacobian );

      // and now fill the variables
      m_loc1loc1[recStep]   = (*transportJacobian)(0,0);
      m_loc1loc2[recStep]   = (*transportJacobian)(0,1); 
      m_loc1phi[recStep]    = (*transportJacobian)(0,2);
      m_loc1theta[recStep]  = (*transportJacobian)(0,3);
      m_loc1qop[recStep]    = (*transportJacobian)(0,4);
      m_loc1steps[recStep]  = 0.;
      
      m_loc2loc1[recStep]   = (*transportJacobian)(1,0); 
      m_loc2loc2[recStep]   = (*transportJacobian)(1,1); 
      m_loc2phi[recStep]    = (*transportJacobian)(1,2); 
      m_loc2theta[recStep]  = (*transportJacobian)(1,3); 
      m_loc2qop[recStep]    = (*transportJacobian)(1,4); 
      m_loc2steps[recStep]  = 0.;
      
      m_philoc1[recStep]    = (*transportJacobian)(2,0); 
      m_philoc2[recStep]    = (*transportJacobian)(2,1); 
      m_phiphi[recStep]     = (*transportJacobian)(2,2); 
      m_phitheta[recStep]   = (*transportJacobian)(2,3); 
      m_phiqop[recStep]     = (*transportJacobian)(2,4); 
      m_phisteps[recStep]   = 0.;
      
      m_thetaloc1[recStep]  = (*transportJacobian)(3,0); 
      m_thetaloc2[recStep]  = (*transportJacobian)(3,1); 
      m_thetaphi[recStep]   = (*transportJacobian)(3,2); 
      m_thetatheta[recStep] = (*transportJacobian)(3,3); 
      m_thetaqop[recStep]   = (*transportJacobian)(3,4); 
      m_thetasteps[recStep]  = 0.;
      
      m_qoploc1[recStep]    = (*transportJacobian)(4,0); 
      m_qoploc2[recStep]    = (*transportJacobian)(4,1); 
      m_qopphi[recStep]     = (*transportJacobian)(4,2); 
      m_qoptheta[recStep]   = (*transportJacobian)(4,3); 
      m_qopqop[recStep]     = (*transportJacobian)(4,4); 
      m_qopsteps[recStep]  = 0.;

      ++recStep;

      // start the riddlers algorithm
      // [1-2-3] Riddlers jacobians -----------------------------------------
      for (unsigned int istep = 0; istep < m_localVariations.size(); ++istep){
        // --------------------------          
        ATH_MSG_VERBOSE( "Performing step : " << istep );
       // the initial perigee with random numbers
       // for the first row
         Trk::AtaPlane  startLoc1Minus(loc1-m_localVariations[istep],loc2,phi,theta,qOverP,startSurface);
         Trk::AtaPlane  startLoc1Plus(loc1+m_localVariations[istep],loc2,phi,theta,qOverP,startSurface);
       // for the second row
         Trk::AtaPlane  startLoc2Minus(loc1,loc2-m_localVariations[istep],phi,theta,qOverP,startSurface);
         Trk::AtaPlane  startLoc2Plus(loc1,loc2+m_localVariations[istep],phi,theta,qOverP,startSurface);
       // for the third row
         Trk::AtaPlane  startPhiMinus(loc1,loc2,phi-m_angularVariations[istep],theta,qOverP,startSurface);
         Trk::AtaPlane  startPhiPlus(loc1,loc2,phi+m_angularVariations[istep],theta,qOverP,startSurface);
       // for the fourth row
         Trk::AtaPlane  startThetaMinus(loc1,loc2,phi,theta-m_angularVariations[istep],qOverP,startSurface);
         Trk::AtaPlane  startThetaPlus(loc1,loc2,phi,theta+m_angularVariations[istep],qOverP,startSurface);
       // for the fifth row
         Trk::AtaPlane  startQopMinus(loc1,loc2,phi,theta,qOverP-m_qOpVariations[istep],startSurface);
         Trk::AtaPlane  startQopPlus(loc1,loc2,phi,theta,qOverP+m_qOpVariations[istep],startSurface);

        // the propagations --- 10 times
        const Trk::TrackParameters* endLoc1Minus = m_propagator->propagateParameters(startLoc1Minus,
                                                                     destinationSurface,
                                                                     Trk::alongMomentum,
                                                                     false,
                                                                     *m_magFieldProperties);


        const Trk::TrackParameters* endLoc1Plus = m_propagator->propagateParameters(startLoc1Plus,
                                                                     destinationSurface,
                                                                     Trk::alongMomentum,
                                                                     false,
                                                                     *m_magFieldProperties);

        const Trk::TrackParameters* endLoc2Minus = m_propagator->propagateParameters(startLoc2Minus,
                                                                     destinationSurface,
                                                                     Trk::alongMomentum,
                                                                     false,
                                                                     *m_magFieldProperties);

        const Trk::TrackParameters* endLoc2Plus = m_propagator->propagateParameters(startLoc2Plus,
                                                                      destinationSurface,
                                                                      Trk::alongMomentum,
                                                                      false,
                                                                      *m_magFieldProperties);

        const Trk::TrackParameters* endPhiMinus = m_propagator->propagateParameters(startPhiMinus,
                                                                     destinationSurface,
                                                                     Trk::alongMomentum,
                                                                     false,
                                                                     *m_magFieldProperties);

        const Trk::TrackParameters* endPhiPlus = m_propagator->propagateParameters(startPhiPlus,
                                                                      destinationSurface,
                                                                      Trk::alongMomentum,
                                                                      false,
                                                                      *m_magFieldProperties);

        const Trk::TrackParameters* endThetaMinus = m_propagator->propagateParameters(startThetaMinus,
                                                                     destinationSurface,
                                                                     Trk::alongMomentum,
                                                                     false,
                                                                     *m_magFieldProperties);

        const Trk::TrackParameters* endThetaPlus = m_propagator->propagateParameters(startThetaPlus,
                                                                      destinationSurface,
                                                                      Trk::alongMomentum,
                                                                      false,
                                                                      *m_magFieldProperties);

        const Trk::TrackParameters* endQopMinus = m_propagator->propagateParameters(startQopMinus,
                                                                     destinationSurface,
                                                                     Trk::alongMomentum,
                                                                     false,
                                                                     *m_magFieldProperties);

        const Trk::TrackParameters* endQopPlus = m_propagator->propagateParameters(startQopPlus,
                                                                      destinationSurface,
                                                                      Trk::alongMomentum,
                                                                      false,
                                                                      *m_magFieldProperties);
        if (endLoc1Minus
          && endLoc1Plus
          && endLoc2Minus
          && endLoc2Plus
          && endPhiMinus
          && endPhiPlus
          && endThetaMinus
          && endThetaPlus
          && endQopMinus
          && endQopPlus){

            // Loc 1
            const Amg::VectorX& endLoc1MinusPar = endLoc1Minus->parameters();
            const Amg::VectorX& endLoc1PlusPar  = endLoc1Plus->parameters();
            // Loc 2
            const Amg::VectorX& endLoc2MinusPar = endLoc2Minus->parameters();
            const Amg::VectorX& endLoc2PlusPar  = endLoc2Plus->parameters();
            // Phi
            const Amg::VectorX& endPhiMinusPar  = endPhiMinus->parameters();
            const Amg::VectorX& endPhiPlusPar   = endPhiPlus->parameters();
            // Theta
            const Amg::VectorX& endThetaMinusPar = endThetaMinus->parameters();
            const Amg::VectorX& endThetaPlusPar  = endThetaPlus->parameters();
            // qop
            const Amg::VectorX& endQopMinusPar  = endQopMinus->parameters();
            const Amg::VectorX& endQopPlusPar   = endQopPlus->parameters();

            // the deltas
            Amg::VectorX endLoc1Diff(endLoc1PlusPar-endLoc1MinusPar);    
            Amg::VectorX endLoc2Diff(endLoc2PlusPar-endLoc2MinusPar);    
            Amg::VectorX endPhiDiff(endPhiPlusPar-endPhiMinusPar);    
            Amg::VectorX endThetaDiff(endThetaPlusPar-endThetaMinusPar);    
            Amg::VectorX endQopDiff(endQopPlusPar-endQopMinusPar);

            currentStepJacobian(0,0) = endLoc1Diff[0]/(2.*m_localVariations[istep]);
            currentStepJacobian(0,1) = endLoc2Diff[0]/(2.*m_localVariations[istep]);
            currentStepJacobian(0,2) = endPhiDiff[0]/(2.*m_angularVariations[istep]);
            currentStepJacobian(0,3) = endThetaDiff[0]/(2.*m_angularVariations[istep]);
            currentStepJacobian(0,4) = endQopDiff[0]/(2.*m_qOpVariations[istep]);

            m_loc1loc1[recStep]   = currentStepJacobian(0,0);
            m_loc1loc2[recStep]   = currentStepJacobian(0,1);
            m_loc1phi[recStep]    = currentStepJacobian(0,2);
            m_loc1theta[recStep]  = currentStepJacobian(0,3);
            m_loc1qop[recStep]    = currentStepJacobian(0,4);
            m_loc1steps[recStep]  = m_localVariations[istep];

            currentStepJacobian(1,0) = endLoc1Diff[1]/(2.*m_localVariations[istep]);
            currentStepJacobian(1,1) = endLoc2Diff[1]/(2.*m_localVariations[istep]);
            currentStepJacobian(1,2) = endPhiDiff[1]/(2.*m_angularVariations[istep]);
            currentStepJacobian(1,3) = endThetaDiff[1]/(2.*m_angularVariations[istep]);
            currentStepJacobian(1,4) = endQopDiff[1]/(2.*m_qOpVariations[istep]);

            m_loc2loc1[recStep]  = currentStepJacobian(1,0);
            m_loc2loc2[recStep]  = currentStepJacobian(1,1);
            m_loc2phi[recStep]   = currentStepJacobian(1,2);
            m_loc2theta[recStep] = currentStepJacobian(1,3);
            m_loc2qop[recStep]   = currentStepJacobian(1,4);
            m_loc2steps[recStep] = m_localVariations[istep];

            currentStepJacobian(2,0) = endLoc1Diff[2]/(2.*m_localVariations[istep]);
            currentStepJacobian(2,1) = endLoc2Diff[2]/(2.*m_localVariations[istep]);
            currentStepJacobian(2,2) = endPhiDiff[2]/(2.*m_angularVariations[istep]);
            currentStepJacobian(2,3) = endThetaDiff[2]/(2.*m_angularVariations[istep]);
            currentStepJacobian(2,4) = endQopDiff[2]/(2.*m_qOpVariations[istep]);

            m_philoc1[recStep]  = currentStepJacobian(2,0);
            m_philoc2[recStep]  = currentStepJacobian(2,1);
            m_phiphi[recStep]   = currentStepJacobian(2,2);
            m_phitheta[recStep] = currentStepJacobian(2,3);
            m_phiqop[recStep]   = currentStepJacobian(2,4);
            m_phisteps[recStep] = m_angularVariations[istep];

            currentStepJacobian(3,0) = endLoc1Diff[3]/(2.*m_localVariations[istep]);
            currentStepJacobian(3,1) = endLoc2Diff[3]/(2.*m_localVariations[istep]);
            currentStepJacobian(3,2) = endPhiDiff[3]/(2.*m_angularVariations[istep]);
            currentStepJacobian(3,3) = endThetaDiff[3]/(2.*m_angularVariations[istep]);
            currentStepJacobian(3,4) = endQopDiff[3]/(2.*m_qOpVariations[istep]);

            m_thetaloc1[recStep]  = currentStepJacobian(3,0);
            m_thetaloc2[recStep]  = currentStepJacobian(3,1);
            m_thetaphi[recStep]   = currentStepJacobian(3,2);
            m_thetatheta[recStep] = currentStepJacobian(3,3);
            m_thetaqop[recStep]   = currentStepJacobian(3,4);
            m_thetasteps[recStep] = m_angularVariations[istep];

            currentStepJacobian(4,0) = endLoc1Diff[4]/(2.*m_localVariations[istep]);
            currentStepJacobian(4,1) = endLoc2Diff[4]/(2.*m_localVariations[istep]);
            currentStepJacobian(4,2) = endPhiDiff[4]/(2.*m_angularVariations[istep]);
            currentStepJacobian(4,3) = endThetaDiff[4]/(2.*m_angularVariations[istep]);
            currentStepJacobian(4,4) = endQopDiff[4]/(2.*m_qOpVariations[istep]);

            m_qoploc1[recStep]  = currentStepJacobian(4,0);
            m_qoploc2[recStep]  = currentStepJacobian(4,1);
            m_qopphi[recStep]   = currentStepJacobian(4,2);
            m_qoptheta[recStep] = currentStepJacobian(4,3);
            m_qopqop[recStep]   = currentStepJacobian(4,4);
            m_qopsteps[recStep] = m_qOpVariations[istep];

            ATH_MSG_DEBUG( "Current TransportJacobian : " << currentStepJacobian );

            ++recStep;
         }

        delete endLoc1Minus;   endLoc1Minus  = 0;
        delete endLoc1Plus;    endLoc1Plus   = 0;
        delete endLoc2Minus;   endLoc2Minus  = 0;
        delete endLoc2Plus;    endLoc2Plus   = 0;
        delete endPhiMinus;    endPhiMinus   = 0;
        delete endPhiPlus;     endPhiPlus    = 0;
        delete endThetaMinus;  endThetaMinus = 0;
        delete endThetaPlus;   endThetaPlus  = 0;
        delete endQopMinus;    endQopMinus   = 0;
        delete endQopPlus;     endQopPlus    = 0;

      }

      // -------------------------------------------------------------------------------
      if (recStep > 2){
          // The parabolic interpolation -------------------------------------------------------------------------------
          // dL1
          m_loc1loc1[recStep]   = parabolicInterpolation(m_loc1loc1[recStep-1],m_loc1loc1[recStep-2],m_loc1loc1[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_loc1loc2[recStep]   = parabolicInterpolation(m_loc1loc2[recStep-1],m_loc1loc2[recStep-2],m_loc1loc2[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_loc1phi[recStep]    = parabolicInterpolation(m_loc1phi[recStep-1],m_loc1phi[recStep-2],m_loc1phi[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_loc1theta[recStep]  = parabolicInterpolation(m_loc1theta[recStep-1],m_loc1theta[recStep-2],m_loc1theta[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_loc1qop[recStep]    = parabolicInterpolation(m_loc1qop[recStep-1],m_loc1qop[recStep-2],m_loc1qop[recStep-3],
                                                         m_qOpVariations[2], m_qOpVariations[1], m_qOpVariations[0]);
          m_loc1steps[recStep]  = 1;
          // dL2
          m_loc2loc1[recStep]   = parabolicInterpolation(m_loc2loc1[recStep-1],m_loc2loc1[recStep-2],m_loc2loc1[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_loc2loc2[recStep]   = parabolicInterpolation(m_loc2loc2[recStep-1],m_loc2loc2[recStep-2],m_loc2loc2[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_loc2phi[recStep]    = parabolicInterpolation(m_loc2phi[recStep-1],m_loc2phi[recStep-2],m_loc2phi[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_loc2theta[recStep]  = parabolicInterpolation(m_loc2theta[recStep-1],m_loc2theta[recStep-2],m_loc2theta[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_loc2qop[recStep]    = parabolicInterpolation(m_loc2qop[recStep-1],m_loc2qop[recStep-2],m_loc2qop[recStep-3],
                                                         m_qOpVariations[2], m_qOpVariations[1], m_qOpVariations[0]);
          m_loc2steps[recStep]  = 1;
          // dPhi
          m_philoc1[recStep]   = parabolicInterpolation(m_philoc1[recStep-1],m_philoc1[recStep-2],m_philoc1[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_philoc2[recStep]   = parabolicInterpolation(m_philoc2[recStep-1],m_philoc2[recStep-2],m_philoc2[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_phiphi[recStep]    = parabolicInterpolation(m_phiphi[recStep-1],m_phiphi[recStep-2],m_phiphi[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_phitheta[recStep]  = parabolicInterpolation(m_phitheta[recStep-1],m_phitheta[recStep-2],m_phitheta[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_phiqop[recStep]    = parabolicInterpolation(m_phiqop[recStep-1],m_phiqop[recStep-2],m_phiqop[recStep-3],
                                                         m_qOpVariations[2], m_qOpVariations[1], m_qOpVariations[0]);
          m_phisteps[recStep]  = 1;
          // dTheta
          m_thetaloc1[recStep]   = parabolicInterpolation(m_thetaloc1[recStep-1],m_thetaloc1[recStep-2],m_thetaloc1[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_thetaloc2[recStep]   = parabolicInterpolation(m_thetaloc2[recStep-1],m_thetaloc2[recStep-2],m_thetaloc2[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_thetaphi[recStep]    = parabolicInterpolation(m_thetaphi[recStep-1],m_thetaphi[recStep-2],m_thetaphi[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_thetatheta[recStep]  = parabolicInterpolation(m_thetatheta[recStep-1],m_thetatheta[recStep-2],m_thetatheta[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_thetaqop[recStep]    = parabolicInterpolation(m_thetaqop[recStep-1],m_thetaqop[recStep-2],m_thetaqop[recStep-3],
                                                         m_qOpVariations[2], m_qOpVariations[1], m_qOpVariations[0]);
          m_thetasteps[recStep]  = 1;
          // dTheta
          m_qoploc1[recStep]   = parabolicInterpolation(m_qoploc1[recStep-1],m_qoploc1[recStep-2],m_qoploc1[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_qoploc2[recStep]   = parabolicInterpolation(m_qoploc2[recStep-1],m_qoploc2[recStep-2],m_qoploc2[recStep-3],
                                                         m_localVariations[2], m_localVariations[1], m_localVariations[0]);
          m_qopphi[recStep]    = parabolicInterpolation(m_qopphi[recStep-1],m_qopphi[recStep-2],m_qopphi[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_qoptheta[recStep]  = parabolicInterpolation(m_qoptheta[recStep-1],m_qoptheta[recStep-2],m_qoptheta[recStep-3],
                                                         m_angularVariations[2], m_angularVariations[1], m_angularVariations[0]);
          m_qopqop[recStep]    = parabolicInterpolation(m_qopqop[recStep-1],m_qopqop[recStep-2],m_qopqop[recStep-3],
                                                         m_qOpVariations[2], m_qOpVariations[1], m_qOpVariations[0]);
          m_qopsteps[recStep]  = 1;
          
          currentStepJacobian(0,0)= m_loc1loc1[recStep];   
          currentStepJacobian(0,1)= m_loc1loc2[recStep];   
          currentStepJacobian(0,2)= m_loc1phi[recStep];    
          currentStepJacobian(0,3)= m_loc1theta[recStep];  
          currentStepJacobian(0,4)= m_loc1qop[recStep];    
          
          currentStepJacobian(1,0)= m_loc2loc1[recStep];   
          currentStepJacobian(1,1)= m_loc2loc2[recStep];   
          currentStepJacobian(1,2)= m_loc2phi[recStep];    
          currentStepJacobian(1,3)= m_loc2theta[recStep];  
          currentStepJacobian(1,4)= m_loc2qop[recStep];    
          
          currentStepJacobian(2,0)= m_philoc1[recStep];    
          currentStepJacobian(2,1)= m_philoc2[recStep];    
          currentStepJacobian(2,2)= m_phiphi[recStep];     
          currentStepJacobian(2,3)= m_phitheta[recStep];   
          currentStepJacobian(2,4)= m_phiqop[recStep];     
          
          currentStepJacobian(3,0)= m_thetaloc1[recStep];  
          currentStepJacobian(3,1)= m_thetaloc2[recStep]; 
          currentStepJacobian(3,2)= m_thetaphi[recStep];   
          currentStepJacobian(3,3)= m_thetatheta[recStep]; 
          currentStepJacobian(3,4)= m_thetaqop[recStep];   
          
          currentStepJacobian(4,0)= m_qoploc1[recStep];    
          currentStepJacobian(4,1)= m_qoploc2[recStep];    
          currentStepJacobian(4,2)= m_qopphi[recStep];     
          currentStepJacobian(4,3)= m_qoptheta[recStep];   
          currentStepJacobian(4,4)= m_qopqop[recStep];     
    
       }  
            
       ATH_MSG_DEBUG( "Interpolated TransportJacobian : " << currentStepJacobian );
       ++recStep;


       // now fill the results
       TransportJacobian diffMatrix(*transportJacobian-currentStepJacobian);
      
       ATH_MSG_VERBOSE( "Absolute Differences of the TransportJacobian : " << diffMatrix );

       // Absolute differences ----------------------------------------------------
       // (A1) 
       // fill the differences into the last one log (loc1)
       m_loc1loc1[recStep]   = diffMatrix(0,0);
       m_loc1loc2[recStep]   = diffMatrix(0,1);
       m_loc1phi[recStep]    = diffMatrix(0,2);
       m_loc1theta[recStep]  = diffMatrix(0,3);
       m_loc1qop[recStep]    = diffMatrix(0,4);
       m_loc1steps[recStep]  = 2;
       // fill the differences into the last one log (loc2)
       m_loc2loc1[recStep]   = diffMatrix(1,0);
       m_loc2loc2[recStep]   = diffMatrix(1,1);
       m_loc2phi[recStep]    = diffMatrix(1,2);
       m_loc2theta[recStep]  = diffMatrix(1,3);
       m_loc2qop[recStep]    = diffMatrix(1,4);
       m_loc2steps[recStep] = 2;
       // fill the differences into the last one log (phi)
       m_philoc1[recStep]    = diffMatrix(2,0);
       m_philoc2[recStep]    = diffMatrix(2,1);
       m_phiphi[recStep]     = diffMatrix(2,2);
       m_phitheta[recStep]   = diffMatrix(2,3);
       m_phiqop[recStep]     = diffMatrix(2,4);
       m_phisteps[recStep]   = 2;
       // fill the differences into the last one log (theta)
       m_thetaloc1[recStep]  = diffMatrix(3,0);
       m_thetaloc2[recStep]  = diffMatrix(3,1);
       m_thetaphi[recStep]   = diffMatrix(3,2);
       m_thetatheta[recStep] = diffMatrix(3,3);
       m_thetaqop[recStep]   = diffMatrix(3,4);
       m_thetasteps[recStep] = 2;
       // fill the differences into the last one log (qop)
       m_qoploc1[recStep]    = diffMatrix(4,0);
       m_qoploc2[recStep]    = diffMatrix(4,1);
       m_qopphi[recStep]     = diffMatrix(4,2);
       m_qoptheta[recStep]   = diffMatrix(4,3);
       m_qopqop[recStep]     = diffMatrix(4,4);
       m_qopsteps[recStep]   = 2;
       ++recStep;
       
       // (A2) 
       if (recStep > 1){
            // fill the differences into the last one log (loc1)
            m_loc1loc1[recStep]   = fabs(m_loc1loc1[recStep-1]  ) > 1e-50 ?  -log10(fabs(m_loc1loc1[recStep-1] )) : 0.;  
            m_loc1loc2[recStep]   = fabs(m_loc1loc2[recStep-1]  ) > 1e-50 ?  -log10(fabs(m_loc1loc2[recStep-1] )) : 0.;  
            m_loc1phi[recStep]    = fabs(m_loc1phi[recStep-1]   ) > 1e-50 ?  -log10(fabs(m_loc1phi[recStep-1]  )) : 0.;  
            m_loc1theta[recStep]  = fabs(m_loc1theta[recStep-1] ) > 1e-50 ?  -log10(fabs(m_loc1theta[recStep-1])) : 0.;  
            m_loc1qop[recStep]    = fabs(m_loc1qop[recStep-1]   ) > 1e-50 ?  -log10(fabs(m_loc1qop[recStep-1]  )) : 0.;  
            m_loc1steps[recStep]  = 3;                                       
            // fill the differences into the last one log (loc2)                                                      
            m_loc2loc1[recStep]   = fabs(m_loc2loc1[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_loc2loc1[recStep-1] )) : 0.;
            m_loc2loc2[recStep]   = fabs(m_loc2loc2[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_loc2loc2[recStep-1] )) : 0.;
            m_loc2phi[recStep]    = fabs(m_loc2phi[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_loc2phi[recStep-1]  )) : 0.;
            m_loc2theta[recStep]  = fabs(m_loc2theta[recStep-1] ) > 1e-50 ?    -log10(fabs(m_loc2theta[recStep-1])) : 0.;
            m_loc2qop[recStep]    = fabs(m_loc2qop[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_loc2qop[recStep-1]  )) : 0.;
            m_loc2steps[recStep]  = 3;
            // fill the differences into the last one log (phi)
            m_philoc1[recStep]    = fabs(m_philoc1[recStep-1]  ) > 1e-50 ?      -log10(fabs(m_philoc1[recStep-1] )) : 0.;
            m_philoc2[recStep]    = fabs(m_philoc2[recStep-1]  ) > 1e-50 ?      -log10(fabs(m_philoc2[recStep-1] )) : 0.;
            m_phiphi[recStep]     = fabs(m_phiphi[recStep-1]   ) > 1e-50 ?      -log10(fabs(m_phiphi[recStep-1]  )) : 0.;
            m_phitheta[recStep]   = fabs(m_phitheta[recStep-1] ) > 1e-50 ?      -log10(fabs(m_phitheta[recStep-1])) : 0.;
            m_phiqop[recStep]     = fabs(m_phiqop[recStep-1]   ) > 1e-50 ?      -log10(fabs(m_phiqop[recStep-1]  )) : 0.;
            m_phisteps[recStep]   = 3;
            // fill the differences into the last one log (theta)
            m_thetaloc1[recStep]  = fabs(m_thetaloc1[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_thetaloc1[recStep-1] )) : 0.;
            m_thetaloc2[recStep]  = fabs(m_thetaloc2[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_thetaloc2[recStep-1] )) : 0.;
            m_thetaphi[recStep]   = fabs(m_thetaphi[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_thetaphi[recStep-1]  )) : 0.;
            m_thetatheta[recStep] = fabs(m_thetatheta[recStep-1] ) > 1e-50 ?    -log10(fabs(m_thetatheta[recStep-1])) : 0.;
            m_thetaqop[recStep]   = fabs(m_thetaqop[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_thetaqop[recStep-1]  )) : 0.;
            m_thetasteps[recStep] = 3;
            // fill the differences into the last one log (qop)
            m_qoploc1[recStep]    = fabs(m_qoploc1[recStep-1]  ) > 1e-50 ?     -log10(fabs(m_qoploc1[recStep-1] )) : 0.;
            m_qoploc2[recStep]    = fabs(m_qoploc2[recStep-1]  ) > 1e-50 ?     -log10(fabs(m_qoploc2[recStep-1] )) : 0.;
            m_qopphi[recStep]     = fabs(m_qopphi[recStep-1]   ) > 1e-50 ?     -log10(fabs(m_qopphi[recStep-1]  )) : 0.;
            m_qoptheta[recStep]   = fabs(m_qoptheta[recStep-1] ) > 1e-50 ?     -log10(fabs(m_qoptheta[recStep-1])) : 0.;
            m_qopqop[recStep]     = fabs(m_qopqop[recStep-1]   ) > 1e-50 ?     -log10(fabs(m_qopqop[recStep-1]  )) : 0.;
            m_qopsteps[recStep]   = 3;
       }
       ++recStep;


       // Relative differences) ----------------------------------------------------
       // (R1) 
       // fill the differences into the last one log (loc1)
       m_loc1loc1[recStep]   = fabs((*transportJacobian)(0,0)) > 1e-50 ?  diffMatrix(0,0)/((*transportJacobian)(0,0)) : 0.;
       m_loc1loc2[recStep]   = fabs((*transportJacobian)(0,1)) > 1e-50 ?  diffMatrix(0,1)/((*transportJacobian)(0,1)) : 0.;
       m_loc1phi[recStep]    = fabs((*transportJacobian)(0,2)) > 1e-50 ?  diffMatrix(0,2)/((*transportJacobian)(0,2)) : 0.;
       m_loc1theta[recStep]  = fabs((*transportJacobian)(0,3)) > 1e-50 ?  diffMatrix(0,3)/((*transportJacobian)(0,3)) : 0.;
       m_loc1qop[recStep]    = fabs((*transportJacobian)(0,4)) > 1e-50 ?  diffMatrix(0,4)/((*transportJacobian)(0,4)) : 0.;
       m_loc1steps[recStep]  = 4;                                                                                
       // fill the differences into the last one log (loc2)                                                      
       m_loc2loc1[recStep]   = fabs((*transportJacobian)(1,0)) > 1e-50 ?  diffMatrix(1,0)/((*transportJacobian)(1,0)) : 0.;
       m_loc2loc2[recStep]   = fabs((*transportJacobian)(1,1)) > 1e-50 ?  diffMatrix(1,1)/((*transportJacobian)(1,1)) : 0.;
       m_loc2phi[recStep]    = fabs((*transportJacobian)(1,2)) > 1e-50 ?  diffMatrix(1,2)/((*transportJacobian)(1,2)) : 0.;
       m_loc2theta[recStep]  = fabs((*transportJacobian)(1,3)) > 1e-50 ?  diffMatrix(1,3)/((*transportJacobian)(1,3)) : 0.;
       m_loc2qop[recStep]    = fabs((*transportJacobian)(1,4)) > 1e-50 ?  diffMatrix(1,4)/((*transportJacobian)(1,4)) : 0.;
       m_loc2steps[recStep]  = 4;
       // fill the differences into the last one log (phi)
       m_philoc1[recStep]    = fabs((*transportJacobian)(2,0)) > 1e-50 ?  diffMatrix(2,0)/((*transportJacobian)(2,0)) : 0.;
       m_philoc2[recStep]    = fabs((*transportJacobian)(2,1)) > 1e-50 ?  diffMatrix(2,1)/((*transportJacobian)(2,1)) : 0.;
       m_phiphi[recStep]     = fabs((*transportJacobian)(2,2)) > 1e-50 ?  diffMatrix(2,2)/((*transportJacobian)(2,2)) : 0.;
       m_phitheta[recStep]   = fabs((*transportJacobian)(2,3)) > 1e-50 ?  diffMatrix(2,3)/((*transportJacobian)(2,3)) : 0.;
       m_phiqop[recStep]     = fabs((*transportJacobian)(2,4)) > 1e-50 ?  diffMatrix(2,4)/((*transportJacobian)(2,4)) : 0.;
       m_phisteps[recStep]   = 4;
       // fill the differences into the last one log (theta)
       m_thetaloc1[recStep]  = fabs((*transportJacobian)(3,0)) > 1e-50 ?  diffMatrix(3,0)/((*transportJacobian)(3,0)) : 0.;
       m_thetaloc2[recStep]  = fabs((*transportJacobian)(3,1)) > 1e-50 ?  diffMatrix(3,1)/((*transportJacobian)(3,1)) : 0.;
       m_thetaphi[recStep]   = fabs((*transportJacobian)(3,2)) > 1e-50 ?  diffMatrix(3,2)/((*transportJacobian)(3,2)) : 0.;
       m_thetatheta[recStep] = fabs((*transportJacobian)(3,3)) > 1e-50 ?  diffMatrix(3,3)/((*transportJacobian)(3,3)) : 0.;
       m_thetaqop[recStep]   = fabs((*transportJacobian)(3,4)) > 1e-50 ?  diffMatrix(3,4)/((*transportJacobian)(3,4)) : 0.;
       m_thetasteps[recStep] = 4;
       // fill the differences into the last one log (qop)
       m_qoploc1[recStep]    = fabs((*transportJacobian)(4,0)) > 1e-50 ?  diffMatrix(4,0)/((*transportJacobian)(4,0)) : 0.;
       m_qoploc2[recStep]    = fabs((*transportJacobian)(4,1)) > 1e-50 ?  diffMatrix(4,1)/((*transportJacobian)(4,1)) : 0.;
       m_qopphi[recStep]     = fabs((*transportJacobian)(4,2)) > 1e-50 ?  diffMatrix(4,2)/((*transportJacobian)(4,2)) : 0.;
       m_qoptheta[recStep]   = fabs((*transportJacobian)(4,3)) > 1e-50 ?  diffMatrix(4,3)/((*transportJacobian)(4,3)) : 0.;
       m_qopqop[recStep]     = fabs((*transportJacobian)(4,4)) > 1e-50 ?  diffMatrix(4,4)/((*transportJacobian)(4,4)) : 0.;
       m_qopsteps[recStep]   = 4;
       ++recStep;

       // (R2) 
       // Relative differences ----------------------------------------------------
       if (recStep > 0){
         m_loc1loc1[recStep]   = fabs(m_loc1loc1[recStep-1]  ) > 1e-50 ?  -log10(fabs(m_loc1loc1[recStep-1] )) : 0.;  
         m_loc1loc2[recStep]   = fabs(m_loc1loc2[recStep-1]  ) > 1e-50 ?  -log10(fabs(m_loc1loc2[recStep-1] )) : 0.;  
         m_loc1phi[recStep]    = fabs(m_loc1phi[recStep-1]   ) > 1e-50 ?  -log10(fabs(m_loc1phi[recStep-1]  )) : 0.;  
         m_loc1theta[recStep]  = fabs(m_loc1theta[recStep-1] ) > 1e-50 ?  -log10(fabs(m_loc1theta[recStep-1])) : 0.;  
         m_loc1qop[recStep]    = fabs(m_loc1qop[recStep-1]   ) > 1e-50 ?  -log10(fabs(m_loc1qop[recStep-1]  )) : 0.;  
         m_loc1steps[recStep]  = 5;                                       
         // fill the differences into the last one log (loc2)                                                      
         m_loc2loc1[recStep]   = fabs(m_loc2loc1[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_loc2loc1[recStep-1] )) : 0.;
         m_loc2loc2[recStep]   = fabs(m_loc2loc2[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_loc2loc2[recStep-1] )) : 0.;
         m_loc2phi[recStep]    = fabs(m_loc2phi[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_loc2phi[recStep-1]  )) : 0.;
         m_loc2theta[recStep]  = fabs(m_loc2theta[recStep-1] ) > 1e-50 ?    -log10(fabs(m_loc2theta[recStep-1])) : 0.;
         m_loc2qop[recStep]    = fabs(m_loc2qop[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_loc2qop[recStep-1]  )) : 0.;
         m_loc2steps[recStep]  = 5;
         // fill the differences into the last one log (phi)
         m_philoc1[recStep]    = fabs(m_philoc1[recStep-1]  ) > 1e-50 ?      -log10(fabs(m_philoc1[recStep-1] )) : 0.;
         m_philoc2[recStep]    = fabs(m_philoc2[recStep-1]  ) > 1e-50 ?      -log10(fabs(m_philoc2[recStep-1] )) : 0.;
         m_phiphi[recStep]     = fabs(m_phiphi[recStep-1]   ) > 1e-50 ?      -log10(fabs(m_phiphi[recStep-1]  )) : 0.;
         m_phitheta[recStep]   = fabs(m_phitheta[recStep-1] ) > 1e-50 ?      -log10(fabs(m_phitheta[recStep-1])) : 0.;
         m_phiqop[recStep]     = fabs(m_phiqop[recStep-1]   ) > 1e-50 ?      -log10(fabs(m_phiqop[recStep-1]  )) : 0.;
         m_phisteps[recStep]   = 5;
         // fill the differences into the last one log (theta)
         m_thetaloc1[recStep]  = fabs(m_thetaloc1[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_thetaloc1[recStep-1] )) : 0.;
         m_thetaloc2[recStep]  = fabs(m_thetaloc2[recStep-1]  ) > 1e-50 ?    -log10(fabs(m_thetaloc2[recStep-1] )) : 0.;
         m_thetaphi[recStep]   = fabs(m_thetaphi[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_thetaphi[recStep-1]  )) : 0.;
         m_thetatheta[recStep] = fabs(m_thetatheta[recStep-1] ) > 1e-50 ?    -log10(fabs(m_thetatheta[recStep-1])) : 0.;
         m_thetaqop[recStep]   = fabs(m_thetaqop[recStep-1]   ) > 1e-50 ?    -log10(fabs(m_thetaqop[recStep-1]  )) : 0.;
         m_thetasteps[recStep] = 5;
         // fill the differences into the last one log (qop)
         m_qoploc1[recStep]    = fabs(m_qoploc1[recStep-1]  ) > 1e-50 ?     -log10( fabs(m_qoploc1[recStep-1] ) ): 0.;
         m_qoploc2[recStep]    = fabs(m_qoploc2[recStep-1]  ) > 1e-50 ?     -log10( fabs(m_qoploc2[recStep-1] ) ): 0.;
         m_qopphi[recStep]     = fabs(m_qopphi[recStep-1]   ) > 1e-50 ?     -log10( fabs(m_qopphi[recStep-1]  ) ): 0.;
         m_qoptheta[recStep]   = fabs(m_qoptheta[recStep-1] ) > 1e-50 ?     -log10( fabs(m_qoptheta[recStep-1]) ): 0.;
         m_qopqop[recStep]     = fabs(m_qopqop[recStep-1]   ) > 1e-50 ?     -log10( fabs(m_qopqop[recStep-1]  ) ): 0.;
         m_qopsteps[recStep]   = 5;
       }
       ++recStep;

      m_steps = recStep;
      m_validationTree->Fill();

  }

  // memory cleanup
  delete trackParameters;
  delete transportJacobian;

  // Code entered here will be executed once per event
  return StatusCode::SUCCESS;
}

//============================================================================================
Amg::Transform3D* Trk::RiddersAlgorithm::createTransform(double x, double y, double z, double phi, double theta, double alphaZ)
{

 if (phi!=0. && theta != 0.){
   // create the Start Surface
   Amg::Vector3D surfacePosition(x,y,z);
   // z direction
   Amg::Vector3D surfaceZdirection(cos(phi)*sin(theta),
                                sin(phi)*sin(theta),
                                cos(theta));
   // the global z axis
   Amg::Vector3D zAxis(0.,0.,1.);
   // the y direction
   Amg::Vector3D surfaceYdirection(zAxis.cross(surfaceZdirection));
   // the x direction
   Amg::Vector3D surfaceXdirection(surfaceYdirection.cross(surfaceZdirection));
   // the rotation
   Amg::RotationMatrix3D surfaceRotation;
   surfaceRotation.col(0) = surfaceXdirection;
   surfaceRotation.col(1) = surfaceYdirection;
   surfaceRotation.col(2) = surfaceZdirection;
   Amg::Transform3D nominalTransform(surfaceRotation, surfacePosition);   
   return new Amg::Transform3D(nominalTransform*Amg::AngleAxis3D(alphaZ,zAxis));
   
 }

  return new Amg::Transform3D(Amg::Translation3D(x,y,z));
}

double Trk::RiddersAlgorithm::parabolicInterpolation(double y0, double y1, double y2,
                                                     double x0, double x1, double x2)
{
    double Z0 = x1*x2*y0;
    double Z1 = x0*x2*y1;
    double Z2 = x0*x1*y2;
    double N0 = (x0-x1)*(x0-x2);
    double N1 = (x1-x2)*(x1-x0);
    double N2 = (x2-x0)*(x2-x1);
    return Z0/N0 + Z1/N1 + Z2/N2; 
}
