/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationValidation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Tracking
#include "TrkExAlgs/ExtrapolationValidation.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkTrack/TrackStateOnSurface.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
#include "GaudiKernel/SystemOfUnits.h"

//================ Constructor =================================================

Trk::ExtrapolationValidation::ExtrapolationValidation(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_highestVolume(nullptr),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_gaussDist(nullptr),
  m_flatDist(nullptr),
  m_materialCollectionValidation(true),
  m_direct(false),
  m_validationTree(nullptr),  
  m_validationTreeName("ExtrapolationValidation"),
  m_validationTreeDescription("Output of the ExtrapolationValidation Algorithm"),
  m_validationTreeFolder("/val/ExtrapolationValidation"),
  m_maximumR(0.),
  m_maximumZ(0.),
  m_sigmaLoc(10.*Gaudi::Units::micrometer),
  m_sigmaR(17.*Gaudi::Units::micrometer),                   
  m_sigmaZ(50.*Gaudi::Units::millimeter),
  m_minEta(-3.),                    
  m_maxEta(3.),                    
  m_minP(0.5*Gaudi::Units::GeV),                      
  m_maxP(100.*Gaudi::Units::GeV),
  m_particleType(2),
  m_parameters(0),
  m_parameterLoc1{},    //!< start local 1 
  m_parameterLoc2{},    //!< start local 2 
  m_parameterPhi{},     //!< start phi 
  m_parameterTheta{},   //!< start theta
  m_parameterEta{},     //!< start eta
  m_parameterQoverP{},  //!< start qOverP
  m_covarianceLoc1{},    //!< start local 1 
  m_covarianceLoc2{},    //!< start local 2 
  m_covariancePhi{},     //!< start phi 
  m_covarianceTheta{},   //!< start theta
  m_covarianceQoverP{},  //!< start qOverP
  m_covarianceDeterminant{},  //!< start qOverP
  m_destinationSurfaceType(0),
  m_startX{},      //!< startX
  m_startY{},      //!< startX
  m_startR{},      //!< startX
  m_startZ{},      //!< startX
  m_startP{},      //!< startP
  m_estimationX(0.),
  m_estimationY(0.),
  m_estimationR(0.),
  m_estimationZ(0.),
  m_destinationX(0.),
  m_destinationY(0.),
  m_destinationR(0.),
  m_destinationZ(0.),
  m_triesFront(0),
  m_breaksFront(0),
  m_triesBack(0),
  m_breaksBack(0),
  m_collectedLayerFront(0),
  m_collectedLayerBack(0)  
{
  // used algorithms and alg tools
  declareProperty("Extrapolator",                m_extrapolator);
  declareProperty("ValidateMaterialCollection",  m_materialCollectionValidation);
  declareProperty("ExtrapolateDirectly", m_direct);
  // TTree handling
  declareProperty("ValidationTreeName",          m_validationTreeName);
  declareProperty("ValidationTreeDescription",   m_validationTreeDescription);
  declareProperty("ValidationTreeFolder",        m_validationTreeFolder);
  // algorithm steering
  declareProperty("StartPerigeeSigmaLoc"      , m_sigmaLoc);
  declareProperty("StartPerigeeSigmaR"        , m_sigmaR);
  declareProperty("StartPerigeeSigmaZ"        , m_sigmaZ);
  declareProperty("StartPerigeeMinEta"        , m_minEta);
  declareProperty("StartPerigeeMaxEta"        , m_maxEta);
  declareProperty("StartPerigeeMinP"          , m_minP);
  declareProperty("StartPerigeeMaxP"          , m_maxP);
  declareProperty("ParticleType"              , m_particleType);

}

//================ Destructor =================================================

Trk::ExtrapolationValidation::~ExtrapolationValidation()
{
  // clear random number generators
  delete m_gaussDist;
  delete m_flatDist;
}


//================ Initialisation =================================================

StatusCode Trk::ExtrapolationValidation::initialize()
{
  // Code entered here will be executed once at program start.
  ATH_MSG_INFO( " initialize()" );

   // Get Extrapolator from ToolService   
   ATH_CHECK( m_extrapolator.retrieve());
  
   // create the new Tree
   m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

   // the branches for the parameters
   m_validationTree->Branch("Parameters",            &m_parameters,     "params/I");
   m_validationTree->Branch("ParametersLoc1",        m_parameterLoc1,   "paramLoc1[params]/F");
   m_validationTree->Branch("ParametersLoc2",        m_parameterLoc2,   "paramLoc2[params]/F");
   m_validationTree->Branch("ParametersPhi",         m_parameterPhi,    "paramPhi[params]/F");
   m_validationTree->Branch("ParametersTheta",       m_parameterTheta,  "paramTheta[params]/F");
   m_validationTree->Branch("ParametersEta",         m_parameterEta,    "paramEta[params]/F");
   m_validationTree->Branch("ParametersQoverP",      m_parameterQoverP, "paramQoverP[params]/F");
   // for the covariance diagonals
   m_validationTree->Branch("CovarianceLoc1",        m_covarianceLoc1,   "covLoc1[params]/F");
   m_validationTree->Branch("CovarianceLoc2",        m_covarianceLoc2,   "covLoc2[params]/F");
   m_validationTree->Branch("CovariancePhi",         m_covariancePhi,    "covPhi[params]/F");
   m_validationTree->Branch("CovarianceTheta",       m_covarianceTheta,  "covTheta[params]/F");
   m_validationTree->Branch("CovarianceQoverP",      m_covarianceQoverP, "covQoverP[params]/F");
   m_validationTree->Branch("CovarianceDeterminant",      m_covarianceDeterminant, "covDet[params]/F");
   // the start Momentum
   m_validationTree->Branch("StartMomentum",         &m_startP,     "startP/F");
   // for the start surface
   m_validationTree->Branch("StartSurfaceX",         &m_startX,     "startX/F");
   m_validationTree->Branch("StartSurfaceY",         &m_startY,     "startY/F");
   m_validationTree->Branch("StartSurfaceR",         &m_startR,     "startR/F");
   m_validationTree->Branch("StartSurfaceZ",         &m_startZ,     "startZ/F");
   // the estimation of the parameters
   m_validationTree->Branch("EstimationSurfaceX",              &m_estimationX,               "estimateX/F");
   m_validationTree->Branch("EstimationSurfaceY",              &m_estimationY,               "estimateY/F");
   m_validationTree->Branch("EstimationSurfaceR",              &m_estimationR,               "estimateR/F");
   m_validationTree->Branch("EstimationSurfaceZ",              &m_estimationZ,               "estimateZ/F");
   // for the surface type   
   m_validationTree->Branch("DestinationSurfaceType",           &m_destinationSurfaceType,     "surfaceType/I");
   m_validationTree->Branch("DestinationSurfaceX",              &m_destinationX,               "surfaceX/F");
   m_validationTree->Branch("DestinationSurfaceY",              &m_destinationY,               "surfaceY/F");
   m_validationTree->Branch("DestinationSurfaceR",              &m_destinationR,               "surfaceR/F");
   m_validationTree->Branch("DestinationSurfaceZ",              &m_destinationZ,               "surfaceZ/F");

   // now register the Tree
   ITHistSvc* tHistSvc = nullptr;
   if (service("THistSvc",tHistSvc).isFailure()){ 
      ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !" );
      delete m_validationTree; m_validationTree = nullptr;
   }
   if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
      delete m_validationTree; m_validationTree = nullptr;
   }

  // intialize the random number generators
  m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
  m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));

  ATH_MSG_INFO( "initialize() successful");
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::ExtrapolationValidation::finalize()
{
  // Code entered here will be executed once at the end of the program run.
    ATH_MSG_INFO("================== Output Statistics =========================");
    ATH_MSG_INFO("= Navigation : ");
    ATH_MSG_INFO("=  - breaks fwd : " << double(m_breaksFront)/double(m_triesFront) 
        << " (" << m_breaksFront << "/" << m_triesFront << ")");
    ATH_MSG_INFO("=  - breaks bwd : " << double(m_breaksBack)/double(m_triesBack)   
        << " (" << m_breaksBack << "/" << m_triesBack << ")");
    if (m_materialCollectionValidation){
        ATH_MSG_INFO("= Material collection : ");
        ATH_MSG_INFO("=  - layer collected fwd : " << m_collectedLayerFront );
        ATH_MSG_INFO("=  - layer collected bwd : " << m_collectedLayerBack  );
    }

    ATH_MSG_INFO("==============================================================");
  
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::ExtrapolationValidation::execute()
{

  // get the overall dimensions
  if (!m_highestVolume){
     // get TrackingGeometry and highest volume
     const Trk::TrackingGeometry* trackingGeometry = m_extrapolator->trackingGeometry();
     m_highestVolume = trackingGeometry ? trackingGeometry->highestTrackingVolume() : nullptr;
     const Trk::CylinderVolumeBounds* cylBounds = m_highestVolume ? 
         dynamic_cast<const Trk::CylinderVolumeBounds*>(&(m_highestVolume->volumeBounds())) : nullptr;
    // bail out
    if (!cylBounds){
       ATH_MSG_WARNING("No highest TrackingVolume / no VolumeBounds ... pretty useless! ");
       return StatusCode::SUCCESS;   
     }
    // get the numbers
    m_maximumR = cylBounds->outerRadius();
    m_maximumZ = cylBounds->halflengthZ(); 
  }

  // intialize the values
  m_parameters             = 0;
  m_destinationSurfaceType = 0;
  // -----------> start
  m_startX                 = 0.;
  m_startY                 = 0.;
  m_startR                 = 0.;
  m_startZ                 = 0.;
  // -----------> estimation
  m_estimationX            = 0.;
  m_estimationY            = 0.;
  m_estimationR            = 0.;
  m_estimationZ            = 0.;
  // -----------> destination
  m_destinationX           = 0.;
  m_destinationY           = 0.;
  m_destinationR           = 0.;
  m_destinationZ           = 0.;

   // the local start parameters
   m_parameterLoc1[m_parameters]  = m_sigmaLoc * m_gaussDist->shoot();
   m_parameterLoc2[m_parameters]  = m_sigmaLoc * m_gaussDist->shoot();
   // are adopted for planar and straight line surfaces
   m_parameterPhi[m_parameters]   = M_PI * m_flatDist->shoot();
   m_parameterPhi[m_parameters]  *= (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;
   m_parameterEta[m_parameters]   = m_minEta + m_flatDist->shoot()*(m_maxEta-m_minEta);
   m_parameterTheta[m_parameters] = 2.*atan(exp(-m_parameterEta[m_parameters]));


   m_covarianceLoc1[m_parameters] = fabs( m_parameterLoc1[m_parameters] * 0.1);                                          
   m_covarianceLoc2[m_parameters] =  fabs( m_parameterLoc2[m_parameters] * 0.1);                                        
   m_covariancePhi[m_parameters] =   fabs( m_parameterPhi[m_parameters] * 0.1);
   m_covarianceTheta[m_parameters] =  fabs(m_parameterTheta[m_parameters] * 0.1);

   // this is fine
   double p = m_minP + m_flatDist->shoot()*(m_maxP-m_minP);
   double charge = (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;   
   m_parameterQoverP[m_parameters] = charge/p;

   m_covarianceQoverP[m_parameters] =  fabs(m_parameterQoverP[m_parameters] * 0.1);

   // for the momentum logging
   m_startP  = p;   

   // start
   m_startR          = fabs(m_sigmaR * m_gaussDist->shoot());
   double surfacePhi = M_PI * m_flatDist->shoot();
   surfacePhi       *= (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;
   m_startX          = m_startR*cos(surfacePhi);
   m_startY          = m_startR*sin(surfacePhi);
   m_startZ          = m_sigmaZ * m_gaussDist->shoot();

   // rotate it around Z   
   double alphaZ = M_PI * m_flatDist->shoot();
   alphaZ       *= (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;

   // create the plane surface
   Trk::PlaneSurface startSurface(createTransform(m_startX,
                                                  m_startY,
                                                  m_startZ,
                                                  m_parameterPhi[m_parameters],
                                                  m_parameterTheta[m_parameters],
                                                  alphaZ),
                                                  10e3,10e3);



   AmgSymMatrix(5) covariance;
   covariance.setZero();
   ATH_MSG_VERBOSE(m_covarianceLoc1[m_parameters]);
   covariance(0,0) =  m_covarianceLoc1[m_parameters];
   ATH_MSG_VERBOSE(m_covarianceLoc2[m_parameters]);
   covariance(1,1) =  m_covarianceLoc2[m_parameters];
   ATH_MSG_VERBOSE(m_covariancePhi[m_parameters]);
   covariance(2,2) =   m_covariancePhi[m_parameters];
   ATH_MSG_VERBOSE(m_covarianceTheta[m_parameters]);
   covariance(3,3) =  m_covarianceTheta[m_parameters];
   ATH_MSG_VERBOSE(m_covarianceQoverP[m_parameters]);
   covariance(4,4) = m_covarianceQoverP[m_parameters];
   ATH_MSG_VERBOSE("Initial Setting: \n"<<covariance);
   

   m_covarianceDeterminant[m_parameters] = covariance.determinant();

   // the initial perigee with random numbers
   Trk::AtaPlane startParameters(m_parameterLoc1[m_parameters],
                                 m_parameterLoc2[m_parameters],
                                 m_parameterPhi[m_parameters],
                                 m_parameterTheta[m_parameters],
                                 m_parameterQoverP[m_parameters],
                                 startSurface,
                                 std::move(covariance)); 
   
   ATH_MSG_VERBOSE( "Start Parameters : " << startParameters );
   if(startParameters.covariance())ATH_MSG_VERBOSE( "Start Covariance : \n" << *startParameters.covariance() );
 

   // destination position
   m_estimationR        = m_maximumR * m_flatDist->shoot();

   // --------------- propagate to find a first intersection ---------------------
   Amg::Transform3D CylTrf;
   CylTrf.setIdentity();
   Trk::CylinderSurface estimationCylinder(CylTrf, m_estimationR, 10e10);
   const Trk::TrackParameters* estimationParameters = m_extrapolator->extrapolateDirectly(startParameters,
                                                                                          estimationCylinder,
                                                                                          Trk::alongMomentum,
                                                                                          false);
   if (!estimationParameters) {
        ATH_MSG_VERBOSE( "Estimation of intersection did not work - skip event !" );
        return StatusCode::SUCCESS;
   }
   else if (m_highestVolume && estimationParameters && !(m_highestVolume->inside(estimationParameters->position()))){
        ATH_MSG_VERBOSE( "Estimation of intersection is outside the known world - skip event !" );
        delete estimationParameters;
        return StatusCode::SUCCESS;
   }

   ATH_MSG_VERBOSE( "Estimation Parameters: " << *estimationParameters );

   // record the estimation parameters
   ++m_triesFront;
   ++m_parameters;
   m_parameterLoc1[m_parameters]   = estimationParameters->parameters()[Trk::loc1];
   m_parameterLoc2[m_parameters]   = estimationParameters->parameters()[Trk::loc2];
   m_parameterPhi[m_parameters]    = estimationParameters->parameters()[Trk::phi];
   m_parameterEta[m_parameters]    = estimationParameters->eta();
   m_parameterTheta[m_parameters]  = estimationParameters->parameters()[Trk::theta];
   m_parameterQoverP[m_parameters] = estimationParameters->parameters()[Trk::qOverP];
   if(estimationParameters->covariance()){
   m_covarianceLoc1[m_parameters] =  (*estimationParameters->covariance())(0,0);
   m_covarianceLoc2[m_parameters] =  (*estimationParameters->covariance())(1,1);
   m_covariancePhi[m_parameters] =  (*estimationParameters->covariance())(2,2);
   m_covarianceTheta[m_parameters] =  (*estimationParameters->covariance())(3,3);
   m_covarianceQoverP[m_parameters] = (*estimationParameters->covariance())(4,4);
   m_covarianceDeterminant[m_parameters] = (estimationParameters->covariance())->determinant();
   }
   else{
     m_covarianceLoc1[m_parameters] = 0;                                          
     m_covarianceLoc2[m_parameters] =  0;                                        
   m_covariancePhi[m_parameters] =  0;
   m_covarianceTheta[m_parameters] =  0;
   m_covarianceQoverP[m_parameters] = 0;
   m_covarianceDeterminant[m_parameters] = 0;
   }
   // the start Momentum

   // get the estimated position
   const Amg::Vector3D& estimatedPosition = estimationParameters->position();

   m_estimationX        = estimatedPosition.x();
   m_estimationY        = estimatedPosition.y();
   m_estimationZ        = estimatedPosition.z();

   // cleanup for memory reasons
   delete estimationParameters; estimationParameters = nullptr;

   // create the radom surface at the destination point
   Trk::PlaneSurface destinationSurface(createTransform(m_estimationX,
                                                        m_estimationY,
                                                        m_estimationZ,
                                                        m_parameterPhi[m_parameters],
                                                        m_parameterTheta[m_parameters]), 10e5 , 10e5);


   ATH_MSG_VERBOSE( "Extrapolation to Destination Surface: " << destinationSurface );

   // the destination parameters
   const Trk::TrackParameters* destParameters = nullptr; 
   // the standard validation ... 
   if (!m_materialCollectionValidation && !m_direct)
       destParameters = m_extrapolator->extrapolate(startParameters,
                                                    destinationSurface, 
                                                    Trk::alongMomentum,
                                                    false,
                                                    (Trk::ParticleHypothesis)m_particleType,Trk::addNoise);
   else if(!m_direct){ // material collection validation
       // get the vector of TrackStateOnSurfaces back
      const std::vector<const Trk::TrackStateOnSurface*>* 
                     collectedMaterial = m_extrapolator->extrapolateM(startParameters,
                                                                     destinationSurface,
                                                                     Trk::alongMomentum,
                                                                     false,
                                                                     (Trk::ParticleHypothesis)m_particleType);

      // get the last one and clone it  
      if (collectedMaterial && !collectedMaterial->empty()){
        // get the last track state on surface & clone the destination parameters
        const Trk::TrackStateOnSurface* destinationState = collectedMaterial->back();
        destParameters = destinationState->trackParameters() ? destinationState->trackParameters()->clone() : nullptr;
        m_collectedLayerFront += collectedMaterial->size();
        // delete the layers / cleanup
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIter    =  collectedMaterial->begin();
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIterEnd =  collectedMaterial->end();
        for ( ; tsosIter != tsosIterEnd; delete(*tsosIter), ++tsosIter);
     }
   }

   else{
     destParameters = m_extrapolator->extrapolateDirectly(startParameters,
							  destinationSurface, 
							  Trk::alongMomentum,
							  false,
							  (Trk::ParticleHypothesis)m_particleType);
     
   }
   // ----------------------- check if forward call was successful and continue then
   if (destParameters){
       // successful tries
       ++m_triesBack;
       // record the destination parameters 
       ++m_parameters;
       m_parameterLoc1[m_parameters]   = destParameters->parameters()[Trk::loc1];
       m_parameterLoc2[m_parameters]   = destParameters->parameters()[Trk::loc2];
       m_parameterPhi[m_parameters]    = destParameters->parameters()[Trk::phi];
       m_parameterEta[m_parameters]    = destParameters->eta();
       m_parameterTheta[m_parameters]  = destParameters->parameters()[Trk::theta];
       m_parameterQoverP[m_parameters] = destParameters->parameters()[Trk::qOverP];
       if(destParameters->covariance()){
	 m_covarianceLoc1[m_parameters] =  (*destParameters->covariance())(0,0);
	 m_covarianceLoc2[m_parameters] =  (*destParameters->covariance())(1,1);
	 m_covariancePhi[m_parameters] =  (*destParameters->covariance())(2,2);
	 m_covarianceTheta[m_parameters] =  (*destParameters->covariance())(3,3);
	 m_covarianceQoverP[m_parameters] = (*destParameters->covariance())(4,4);
	 m_covarianceDeterminant[m_parameters] = (destParameters->covariance())->determinant();
       }
       else{
	 m_covarianceLoc1[m_parameters] =  0;
	 m_covarianceLoc2[m_parameters] =  0;
	 m_covariancePhi[m_parameters] =  0;
	 m_covarianceTheta[m_parameters] =  0;
	 m_covarianceQoverP[m_parameters] = 0;
	 m_covarianceDeterminant[m_parameters] = 0;
       }
       // record the destination parameters
       const Amg::Vector3D& destinationPosition = destParameters->position();
       m_destinationX = destinationPosition.x();
       m_destinationY = destinationPosition.y();
       m_destinationZ = destinationPosition.z();
       m_destinationR = destinationPosition.perp();

       // now simply go backwards
       const Trk::TrackParameters* backParameters = nullptr;
       // the standard validation ... 
       if (!m_materialCollectionValidation && !m_direct)
            backParameters = m_extrapolator->extrapolate(*destParameters,
                                                          startSurface, 
                                                          Trk::oppositeMomentum,
                                                          false,
							 (Trk::ParticleHypothesis)m_particleType,Trk::removeNoise);
       else if(!m_direct){ // material collection validation
            // get the vector of TrackStateOnSurfaces back
            const std::vector<const Trk::TrackStateOnSurface*>* 
                     collectedBackMaterial = m_extrapolator->extrapolateM(*destParameters,
                                                                     startSurface,
                                                                     Trk::oppositeMomentum,
                                                                     false,
                                                                     (Trk::ParticleHypothesis)m_particleType);
            // get the last one and clone it  
            if (collectedBackMaterial && !collectedBackMaterial->empty()){
                // get the last track state on surface & clone the destination parameters
                const Trk::TrackStateOnSurface* startState = collectedBackMaterial->back();
                // assign the last ones of the call
                backParameters = startState->trackParameters() ? startState->trackParameters()->clone() : nullptr;
                m_collectedLayerBack += collectedBackMaterial->size();
                // delete the layers / cleanup
                std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIter    =  collectedBackMaterial->begin();
                std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosIterEnd =  collectedBackMaterial->end();
                for ( ; tsosIter != tsosIterEnd; delete(*tsosIter), ++tsosIter);
            }
       }

       else{
	 backParameters = m_extrapolator->extrapolateDirectly(*destParameters,
							      startSurface, 
							      Trk::oppositeMomentum,
							      false,
							      (Trk::ParticleHypothesis)m_particleType);
	 
       }
      // ----------------------- check if backward call was successful and continue then
      if (backParameters){

          ATH_MSG_VERBOSE( "Back Parameters : " << *backParameters );

          // record the back extrapolated ones
          ++m_parameters;
          m_parameterLoc1[m_parameters]   = backParameters->parameters()[Trk::loc1];
          m_parameterLoc2[m_parameters]   = backParameters->parameters()[Trk::loc2];
          m_parameterPhi[m_parameters]    = backParameters->parameters()[Trk::phi];
          m_parameterEta[m_parameters]    = backParameters->eta();
          m_parameterTheta[m_parameters]  = backParameters->parameters()[Trk::theta];
          m_parameterQoverP[m_parameters] = backParameters->parameters()[Trk::qOverP];
	  if(backParameters->covariance()){
	  m_covarianceLoc1[m_parameters] =  (*backParameters->covariance())(0,0);
	  m_covarianceLoc2[m_parameters] =  (*backParameters->covariance())(1,1);
	  m_covariancePhi[m_parameters] =  (*backParameters->covariance())(2,2);
	  m_covarianceTheta[m_parameters] =  (*backParameters->covariance())(3,3);
	  m_covarianceQoverP[m_parameters] = (*backParameters->covariance())(4,4);
	  m_covarianceDeterminant[m_parameters] = (backParameters->covariance())->determinant();
	  }
	  else{
	    m_covarianceLoc1[m_parameters] =  0;
	  m_covarianceLoc2[m_parameters] =  0;
	  m_covariancePhi[m_parameters] =  0;
	  m_covarianceTheta[m_parameters] =  0;
	  m_covarianceQoverP[m_parameters] = 0;
	  m_covarianceDeterminant[m_parameters] = 0;
	  }
          // memory cleanup
          delete backParameters;
        } else 
            ++m_breaksBack;
         // memory cleanup
        delete destParameters;
      } else 
        ++m_breaksFront;
    // increase ones more 
    ++m_parameters;
    // memory cleanup

  if (m_validationTree)
    m_validationTree->Fill();
   

  //std::cout<<"Cleaning up..."<<std::endl;
  //delete covariance;

   return StatusCode::SUCCESS;
}

//============================================================================================
Amg::Transform3D 
Trk::ExtrapolationValidation::createTransform(double x, double y, double z, double phi, double theta, double alphaZ)
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
   
   double nx = 1./sqrt(surfaceXdirection[0]*surfaceXdirection[0]+surfaceXdirection[1]*surfaceXdirection[1]+surfaceXdirection[2]*surfaceXdirection[2]);
   double ny = 1./sqrt(surfaceYdirection[0]*surfaceYdirection[0]+surfaceYdirection[1]*surfaceYdirection[1]+surfaceYdirection[2]*surfaceYdirection[2]);
   surfaceXdirection[0]*=nx;
   surfaceXdirection[1]*=nx;
   surfaceXdirection[2]*=nx;
   
   surfaceYdirection[0]*=ny;
   surfaceYdirection[1]*=ny;
   surfaceYdirection[2]*=ny;
   // the rotation
   Amg::RotationMatrix3D surfaceRotation;
   surfaceRotation.col(0) = surfaceXdirection;
   surfaceRotation.col(1) = surfaceYdirection;
   surfaceRotation.col(2) = surfaceZdirection;
   // return it
   if (alphaZ==0.)
     return Amg::Transform3D(surfaceRotation, surfacePosition);   
   Amg::Transform3D nominalTransform(surfaceRotation, surfacePosition);   
   return Amg::Transform3D(nominalTransform*Amg::AngleAxis3D(alphaZ,zAxis));
   
 }

  return Amg::Transform3D(Amg::Translation3D(x,y,z));
}

