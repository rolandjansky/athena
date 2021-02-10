/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ExtrapolationEngineTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg includes
#include "TrkExUnitTests/ExtrapolationEngineTest.h"
#include "TrkGeometry/MaterialProperties.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"

Trk::ExtrapolationEngineTest::ExtrapolationEngineTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkExUnitTestBase(name, pSvcLocator),
 m_extrapolationEngine(""),
 m_idHelper(0),
 m_pixel_ID(0),
 m_sct_ID(0),
 m_hgtd_ID(0),
 m_useHGTD(false),
 m_parametersMode(1),
 m_particleHypothesis(2),
 m_smearProductionVertex(false),
 m_smearFlatOriginT(false),
 m_smearFlatOriginZ(false),
 m_sigmaOriginT(0.),
 m_sigmaOriginZ(0.),
 m_d0Min(0.),
 m_d0Max(0.),
 m_z0Min(0.),
 m_z0Max(0.),
 m_z0Values(0),
 m_etaMin(-3.),
 m_etaMax(3.),
 m_phiMin(-M_PI),
 m_phiMax(M_PI),
 m_ptMin(100.),
 m_ptMax(100000.),
 m_pathLimit(10e10),
 m_collectSensitive(false),
 m_collectPassive(false),
 m_collectBoundary(false),
 m_collectMaterial(false),
 m_backExtrapolation(false),
 m_stepwiseExtrapolation(false),
 m_stepsPhi(1),
 m_currentPhiStep(0),
 m_etaScans(0),
 m_currentEta(0.),
 m_phiScans(0),
 m_currentPhi(0.),
 m_splitCharge(false),
 m_writeTTree(true),
 m_treeName("ExtrapolationEngineTest"),
 m_treeFolder("/val/"),
 m_treeDescription("ExtrapolationEngine test setup"),  
 m_tree(0),
 m_tRandom(0),
 m_startPositionX(0),
 m_startPositionY(0),
 m_startPositionZ(0),
 m_startPositionR(0),
 m_startPhi(0),
 m_startTheta(0),
 m_startEta(0),
 m_startP(0),
 m_startPt(0),  
 m_charge(-1.0),
 m_endSuccessful(0),
 m_endPositionX(0),
 m_endPositionY(0),
 m_endPositionZ(0),
 m_endPositionR(0),
 m_endPhi(0),
 m_endTheta(0),
 m_endEta(0),
 m_endP(0),
 m_endPt(0),
 m_endPathLength(0),
 m_backSuccessful(0),
 m_backPositionX(0),
 m_backPositionY(0),
 m_backPositionZ(0),
 m_backPositionR(0),
 m_backPhi(0),
 m_backTheta(0),
 m_backEta(0),
 m_backP(0),
 m_backPt(0),
 /** lots of vectors, ignore **/
 m_sensitiveLayerIndex(0),
 m_sensitiveLocalPosX(0),
 m_sensitiveLocalPosY(0),
 m_sensitiveCenterPosX(0),
 m_sensitiveCenterPosY(0),
 m_sensitiveCenterPosZ(0),
 m_sensitiveCenterPosR(0),
 m_materialThicknessInX0(0.),
 m_materialThicknessInL0(0.),
 m_materialThicknessZARho(0),
 m_materialEmulatedIonizationLoss(0),
 m_materialThicknessInX0Bwd(0),
 m_materialThicknessInL0Bwd(0),
 m_materialThicknessInX0Sensitive(0.),
 m_materialThicknessInX0Passive(0.),
 m_materialThicknessInX0Boundary(0.),
 m_materialThicknessInX0Cylinder(0.),
 m_materialThicknessInX0Disc(0.),
 m_materialThicknessInX0Plane(0.),
 m_materialThicknessInX0Accumulated(0),
 m_materialThicknessInL0Accumulated(0),
 m_materialThicknessInX0Steps(0),
 m_materialThicknessInL0Steps(0),
 m_materialPositionX(0),
 m_materialPositionY(0),
 m_materialPositionZ(0),
 m_materialPositionR(0),
 m_materialPositionP(0),
 m_materialPositionPt(0),
 m_materialScaling(0),
 m_stepDirection(0),
 m_endStepSuccessful(0),
 m_endStepPositionX(0),
 m_endStepPositionY(0),
 m_endStepPositionZ(0),
 m_endStepPositionR(0),
 m_endStepPhi(0),
 m_endStepTheta(0),         
 m_endStepEta(0),       
 m_endStepP(0),         
 m_endStepPt(0),       
 m_endStepPathLength(0),
 m_endStepThicknessInX0()
{
    // the extrapolation engine
    declareProperty("ExtrapolationEngine",      m_extrapolationEngine);
    // validation ttree for writing
    declareProperty("PositionMomentumWriter",   m_posmomWriter);
    // charged / neutral & other stuff
    declareProperty("ParametersMode",           m_parametersMode);
    declareProperty("ParticleHypothesis",       m_particleHypothesis);
    declareProperty("BackExtrapolation",        m_backExtrapolation);
    declareProperty("StepwiseExtrapolation",    m_stepwiseExtrapolation);
    // configuration 
    declareProperty("PathLimit",                m_pathLimit);    
    declareProperty("CollectSensitive",         m_collectSensitive);
    declareProperty("CollectPassive",           m_collectPassive);
    declareProperty("CollectBoundary",          m_collectBoundary);
    declareProperty("CollectMaterial",          m_collectMaterial);
    declareProperty("UseHGTD",                  m_useHGTD);
    // Mode for scanning in steps
    declareProperty("ScanMode",                 m_scanMode);
    declareProperty("EtaScans",                 m_etaScans);
    declareProperty("PhiSteps",                 m_stepsPhi);
    declareProperty("PhiScans",                 m_phiScans);
    declareProperty("SplitCharge",              m_splitCharge);
    // production vertices
    declareProperty("SmearOrigin",              m_smearProductionVertex);
    declareProperty("SmearFlatOriginD0",        m_smearFlatOriginT);
    declareProperty("SmearFlatOriginZ0",        m_smearFlatOriginZ);
    declareProperty("SimgaOriginD0",            m_sigmaOriginT);
    declareProperty("SimgaOriginZ0",            m_sigmaOriginZ);
    // d0 min / max values for flat smearing
    declareProperty("D0Min",                    m_d0Min);
    declareProperty("D0Max",                    m_d0Max);
    // z0 min / max values for flat smearing
    declareProperty("Z0Min",                    m_z0Min);
    declareProperty("Z0Max",                    m_z0Max);
    declareProperty("Z0Values",                 m_z0Values);
    // eta min / max values
    declareProperty("EtaMin",                   m_etaMin);
    declareProperty("EtaMax",                   m_etaMax);
    // phi min / max values
    declareProperty("PhiMin",                   m_phiMin);
    declareProperty("PhiMax",                   m_phiMax);
    // pt min / max values 
    declareProperty("PtMin",                    m_ptMin);
    declareProperty("PtMax",                    m_ptMax);
    // the properties
    declareProperty("NumTestsPerEvent",         m_numTests=100);
    declareProperty("WriteTTree",               m_writeTTree);
    declareProperty("TreeName",                 m_treeName);
    declareProperty("TreeFolder",               m_treeFolder);
    declareProperty("TreeDescription",          m_treeDescription);   
}

StatusCode Trk::ExtrapolationEngineTest::finalize() {

    // memory clean up
    for (size_t ip = 0; ip < m_parameterNames.size(); ++ip){
        // create
        delete m_pPositionX[ip];
        delete m_pPositionY[ip];
        delete m_pPositionZ[ip];
        delete m_pPositionR[ip];
        delete m_pPhi[ip];
        delete m_pTheta[ip];
        delete m_pEta[ip];
        delete m_pP[ip];
        delete m_pPt[ip];
    }  
    delete m_sensitiveLayerIndex;
    delete m_sensitiveSurfaceType;
    delete m_sensitiveCenterPosX;
    delete m_sensitiveCenterPosY;
    delete m_sensitiveCenterPosZ;
    delete m_sensitiveCenterPosR;
    delete m_sensitiveLocalPosX;
    delete m_sensitiveLocalPosY;
    delete m_sensitiveLocalPosR;
    delete m_sensitiveLocalPosPhi;
    delete m_sensitiveDetector;
    delete m_sensitiveIsInnermost;
    delete m_sensitiveIsNextToInnermost;
    delete m_sensitiveBarrelEndcap;
    delete m_sensitiveLayerDisc;
    delete m_sensitiveEtaModule;
    delete m_sensitivePhiModule;
    delete m_sensitiveSide;
    delete m_sensitiveNearBondGap;
    delete m_sensitiveisInside;
    delete m_sensitiveisInsideBound;
    delete m_materialtInX0AccumulatedUpTo;
    delete m_materialtInL0AccumulatedUpTo;
    delete m_materialThicknessInX0Accumulated;
    delete m_materialThicknessInL0Accumulated;
    delete m_materialThicknessInX0Steps;
    delete m_materialThicknessInL0Steps;
    delete m_materialPositionX;
    delete m_materialPositionY;
    delete m_materialPositionZ;
    delete m_materialPositionR;
    delete m_materialPositionP;
    delete m_materialPositionPt;
    delete m_materialScaling;
    delete m_stepDirection;
    
    return StatusCode::SUCCESS;
}

StatusCode Trk::ExtrapolationEngineTest::initializeTest() 
{
    
    if (m_extrapolationEngine.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not retrieve ExtrapolationEngine.");
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO("Successfully retrieved ExtrapolationEngine.");
    
    if (!m_posmomWriter.empty() && m_posmomWriter.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not retrieve position/momentum writer.");
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO("Successfully retrieved ExtrapolationEngine.");
    
    //ID Helper
   if (detStore()->retrieve(m_idHelper, "AtlasID" ).isFailure()) {
     ATH_MSG_ERROR ( "Could not get ATLAS ID helper" );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO("Successfully retrieved ATLAS ID Helper.");

    if (detStore()->retrieve(m_pixel_ID, "PixelID").isFailure()) {
        ATH_MSG_ERROR ( "Could not get Pixel ID helper" );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO("Successfully retrieved Pixel ID Helper.");
    
    if (detStore()->retrieve(m_sct_ID, "SCT_ID").isFailure()) {
        ATH_MSG_ERROR ( "Could not get SCT ID helper" );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO("Successfully retrieved SCT ID Helper.");
    
    if (m_useHGTD) {
      if (detStore()->retrieve(m_hgtd_ID, "HGTD_ID").isFailure()) {
        ATH_MSG_ERROR ( "Could not get HGTD ID helper" );
        return StatusCode::FAILURE;
      } else 
        ATH_MSG_INFO("Successfully retrieved HGTD ID Helper.");
    }
        
    // success return
    return StatusCode::SUCCESS;    
}

StatusCode Trk::ExtrapolationEngineTest::bookTree()
{
    ATH_MSG_VERBOSE("Booking the Extrapolation test Tree.");
    
    // ------------------------------> OUTPUT NTUPLE (geometry validation)
    m_tree = new TTree(m_treeName.c_str(), m_treeDescription.c_str());
    // add the Branches
    m_tree->Branch("StartPosX",     &m_startPositionX);
    m_tree->Branch("StartPosY",     &m_startPositionY);
    m_tree->Branch("StartPosZ",     &m_startPositionZ);
    m_tree->Branch("StartPosR",     &m_startPositionR);
    m_tree->Branch("StartPhi",      &m_startPhi);
    m_tree->Branch("StartEta",      &m_startEta);
    m_tree->Branch("StartTheta",    &m_startTheta);
    m_tree->Branch("StartP",        &m_startP);
    m_tree->Branch("StartPt",       &m_startPt);
    if (m_parametersMode) m_tree->Branch("StartCharge",   &m_charge);
    
    m_tree->Branch("EndSuccessful", &m_endSuccessful);
    m_tree->Branch("EndPosX",       &m_endPositionX);
    m_tree->Branch("EndPosY",       &m_endPositionY);
    m_tree->Branch("EndPosZ",       &m_endPositionZ);
    m_tree->Branch("EndPosR",       &m_endPositionR);
    m_tree->Branch("EndPhi",        &m_endPhi);
    m_tree->Branch("EndEta",        &m_endEta);
    m_tree->Branch("EndTheta",      &m_endTheta);
    m_tree->Branch("EndP",          &m_endP);
    m_tree->Branch("EndPt",         &m_endPt);
    // also add the path length
    m_tree->Branch("EndPathLength", &m_endPathLength);
    
    if (m_backExtrapolation){
        m_tree->Branch("BackSuccessful", &m_backSuccessful);
        m_tree->Branch("BackPosX",       &m_backPositionX);
        m_tree->Branch("BackPosY",       &m_backPositionY);
        m_tree->Branch("BackPosZ",       &m_backPositionZ);
        m_tree->Branch("BackPosR",       &m_backPositionR);
        m_tree->Branch("BackPhi",        &m_backPhi);
        m_tree->Branch("BackEta",        &m_backEta);
        m_tree->Branch("BackTheta",      &m_backTheta);
        m_tree->Branch("BackP",          &m_backP);
        m_tree->Branch("BackPt",         &m_backPt);
    }
    
    // this fixes the parameters to order
    m_parameterNames.push_back("Sensitive");
    m_parameterNames.push_back("Passive");
    m_parameterNames.push_back("Boundary");
    for (size_t ip = 0; ip < m_parameterNames.size(); ++ip){
        // create
        m_pPositionX.push_back( new std::vector<float> );
        m_pPositionY.push_back( new std::vector<float> );
        m_pPositionZ.push_back( new std::vector<float> );
        m_pPositionR.push_back( new std::vector<float> );
        m_pPhi.push_back( new std::vector<float> );      
        m_pTheta.push_back( new std::vector<float> );
        m_pEta.push_back( new std::vector<float> );
        m_pP.push_back( new std::vector<float> );
        m_pPt.push_back( new std::vector<float> );
	
        // define the branches    
        m_tree->Branch(m_parameterNames[ip]+"PosX",       m_pPositionX[ip]);
        m_tree->Branch(m_parameterNames[ip]+"PosY",       m_pPositionY[ip]);
        m_tree->Branch(m_parameterNames[ip]+"PosZ",       m_pPositionZ[ip]);
        m_tree->Branch(m_parameterNames[ip]+"PosR",       m_pPositionR[ip]);
        m_tree->Branch(m_parameterNames[ip]+"Phi",        m_pPhi[ip]      );
        m_tree->Branch(m_parameterNames[ip]+"Eta",        m_pTheta[ip]    );
        m_tree->Branch(m_parameterNames[ip]+"Theta",      m_pEta[ip]      );
        m_tree->Branch(m_parameterNames[ip]+"P",          m_pP[ip]        );
        m_tree->Branch(m_parameterNames[ip]+"Pt",         m_pPt[ip]       );
    }
    
    if (m_collectSensitive){
        m_sensitiveLayerIndex      = new std::vector< int >;
        m_sensitiveSurfaceType     = new std::vector< int >;
        m_sensitiveCenterPosX      = new std::vector< float >;
        m_sensitiveCenterPosY      = new std::vector< float >;
        m_sensitiveCenterPosZ      = new std::vector< float >;
        m_sensitiveCenterPosR      = new std::vector< float >;
        m_sensitiveLocalPosX       = new std::vector< float >;
        m_sensitiveLocalPosY       = new std::vector< float >;
        m_sensitiveLocalPosR       = new std::vector< float >;
        m_sensitiveLocalPosPhi     = new std::vector< float >;
        m_sensitiveDetector         = new std::vector< int >; 
        m_sensitiveIsInnermost     = new std::vector< int >;
        m_sensitiveIsNextToInnermost = new std::vector< int >;
        m_sensitiveBarrelEndcap    = new std::vector< int >;
        m_sensitiveLayerDisc       = new std::vector< int >;
        m_sensitiveEtaModule       = new std::vector< int >;
        m_sensitivePhiModule       = new std::vector< int >;
        m_sensitiveSide            = new std::vector< int >;
        m_sensitiveNearBondGap     = new std::vector< int >;
        m_sensitiveisInside        = new std::vector< int >;
        m_sensitiveisInsideBound   = new std::vector< int >;        
        m_materialtInX0AccumulatedUpTo  = new std::vector< float >;
        m_materialtInL0AccumulatedUpTo  = new std::vector< float >;

        m_tree->Branch("SensitiveLayerIndex"  ,  m_sensitiveLayerIndex);
        m_tree->Branch("SensitiveSurfaceType" ,  m_sensitiveSurfaceType);
        m_tree->Branch("SensitiveCenterPosX"  ,  m_sensitiveCenterPosX);
        m_tree->Branch("SensitiveCenterPosY"  ,  m_sensitiveCenterPosY);
        m_tree->Branch("SensitiveCenterPosZ"  ,  m_sensitiveCenterPosZ);
        m_tree->Branch("SensitiveCenterPosR"  ,  m_sensitiveCenterPosR);
        m_tree->Branch("SensitiveLocalPosX"   ,  m_sensitiveLocalPosX);
        m_tree->Branch("SensitiveLocalPosY"   ,  m_sensitiveLocalPosY);
        m_tree->Branch("SensitiveLocalPosR"   ,  m_sensitiveLocalPosR);
        m_tree->Branch("SensitiveLocalPosPhi" ,  m_sensitiveLocalPosPhi);
        m_tree->Branch("SensitiveDetector"    ,  m_sensitiveDetector);
        m_tree->Branch("SensitiveIsInnermost" ,  m_sensitiveIsInnermost);
        m_tree->Branch("SensitiveIsNextToInnermost" ,  m_sensitiveIsNextToInnermost);
        m_tree->Branch("SensitiveBarrelEndcap",  m_sensitiveBarrelEndcap);
        m_tree->Branch("SensitiveLayerDisc"   ,  m_sensitiveLayerDisc);
        m_tree->Branch("SensitiveEtaModule"   ,  m_sensitiveEtaModule);
        m_tree->Branch("SensitivePhiModule"   ,  m_sensitivePhiModule);
        m_tree->Branch("SensitiveSide"        ,  m_sensitiveSide);
        m_tree->Branch("SensitiveNearBondGap"  , m_sensitiveNearBondGap);
        m_tree->Branch("SensitiveisInside"     , m_sensitiveisInside);
        m_tree->Branch("SensitiveisInsideBound", m_sensitiveisInsideBound);
        m_tree->Branch("MaterialtInX0AccumulatedUpTo"        ,  m_materialtInX0AccumulatedUpTo);
        m_tree->Branch("MaterialtInL0AccumulatedUpTo"        ,  m_materialtInL0AccumulatedUpTo);
    }
    
    
    // collect the material, you need branches for this
    if (m_collectMaterial){
        m_materialThicknessInX0Accumulated = new std::vector<float>;
        m_materialThicknessInL0Accumulated = new std::vector<float>;
        m_materialThicknessInX0Steps       = new std::vector<float>;
        m_materialThicknessInL0Steps       = new std::vector<float>;     
        m_materialPositionX                = new std::vector<float>;
        m_materialPositionY                = new std::vector<float>; 
        m_materialPositionZ                = new std::vector<float>;
        m_materialPositionR                = new std::vector<float>; 
        m_materialPositionP                = new std::vector<float>; 
        m_materialPositionPt               = new std::vector<float>; 
        m_materialScaling                  = new std::vector<float>;   
        m_stepDirection                    = new std::vector<int>;     
        m_tree->Branch("MaterialThicknessInX0",                &m_materialThicknessInX0);
        m_tree->Branch("MaterialThicknessInL0",                &m_materialThicknessInL0);
        m_tree->Branch("MaterialThicknessZARho",               &m_materialThicknessZARho);
        m_tree->Branch("MaterialEmulatedEloss",                &m_materialEmulatedIonizationLoss);
        m_tree->Branch("MaterialThicknessSensitiveInX0",       &m_materialThicknessInX0Sensitive);
        m_tree->Branch("MaterialThicknessPassiveInX0",         &m_materialThicknessInX0Passive  );
        m_tree->Branch("MaterialThicknessBoundaryInX0",        &m_materialThicknessInX0Boundary );
        m_tree->Branch("MaterialThicknessCylinderInX0",        &m_materialThicknessInX0Cylinder);
        m_tree->Branch("MaterialThicknessDiscInX0",            &m_materialThicknessInX0Disc );
        m_tree->Branch("MaterialThicknessPlaneInX0",           &m_materialThicknessInX0Plane );
        m_tree->Branch("MaterialThicknessAccumulatedX0",       m_materialThicknessInX0Accumulated );
        m_tree->Branch("MaterialThicknessAccumulatedL0",       m_materialThicknessInL0Accumulated );
        m_tree->Branch("MaterialThicknessStepsInX0",           m_materialThicknessInX0Steps);
        m_tree->Branch("MaterialThicknessStepsInL0",           m_materialThicknessInL0Steps);
        m_tree->Branch("MaterialPosX",                         m_materialPositionX);
        m_tree->Branch("MaterialPosY",                         m_materialPositionY);
        m_tree->Branch("MaterialPosZ",                         m_materialPositionZ);
        m_tree->Branch("MaterialPosR",                         m_materialPositionR);
        m_tree->Branch("MaterialPosP",                         m_materialPositionP);
        m_tree->Branch("MaterialPosPt",                        m_materialPositionPt);        
        m_tree->Branch("MaterialScaling",                      m_materialScaling);
        m_tree->Branch("StepDirection",                        m_stepDirection);
    }

    if (m_stepwiseExtrapolation){
        m_tree->Branch("EndStepSuccessful",             &m_endStepSuccessful);
        m_tree->Branch("EndStepPosX",                   &m_endStepPositionX);
        m_tree->Branch("EndStepPosY",                   &m_endStepPositionY);
        m_tree->Branch("EndStepPosZ",                   &m_endStepPositionZ);
        m_tree->Branch("EndStepPosR",                   &m_endStepPositionR);
        m_tree->Branch("EndStepPhi",                    &m_endStepPhi);
        m_tree->Branch("EndStepEta",                    &m_endStepEta);
        m_tree->Branch("EndStepTheta",                  &m_endStepTheta);
        m_tree->Branch("EndStepP",                      &m_endStepP);
        m_tree->Branch("EndStepPt",                     &m_endStepPt);
        m_tree->Branch("EndStepPathLength",             &m_endStepPathLength);
        m_tree->Branch("EndStepMaterialThicknessInX0",  &m_endStepThicknessInX0);

    }
    
    // now register the Tree
    ITHistSvc* tHistSvc = 0;
    if (service("THistSvc",tHistSvc).isFailure()) {
      ATH_MSG_ERROR( "initialize() Could not find Hist Service  -> Switching Tree output off !" );
        delete m_tree; m_tree = 0;
    }
    if (tHistSvc && ((tHistSvc->regTree(m_treeFolder+m_treeName, m_tree)).isFailure()) ) {
        ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching Tree output off !" );
        delete m_tree; m_tree = 0;
    }    
    return StatusCode::SUCCESS;
    
}

StatusCode Trk::ExtrapolationEngineTest::runTest()
{
  ATH_MSG_VERBOSE("Running the ExtrapolationEngineTest Test in parameters mode : " << m_parametersMode);
  if (m_parametersMode == 0) {
    return runTestT<Trk::NeutralParameters,Trk::NeutralPerigee>();
  }
  return runTestT<Trk::TrackParameters,Trk::Perigee>();
}


StatusCode Trk::ExtrapolationEngineTest::runScan()
{

  ATH_MSG_VERBOSE("Running the ExtrapolationEngineTest Test in scanning mode");

  if (m_phiScans.size()==0) {
    ATH_MSG_VERBOSE( "Running in scan mode: Phi scans not given. Automatically calculated." );
    m_currentPhi = m_phiMin + double(m_currentPhiStep+1)*(m_phiMax-m_phiMin)/double(m_stepsPhi);
  } else {
    ATH_MSG_VERBOSE( "Running in scan mode: Phi scans given." );
    if (m_currentPhiStep >= int(m_phiScans.size()))
      m_currentPhiStep = 0;
    m_currentPhi = m_phiScans.at(m_currentPhiStep);
  }
  m_currentPhiStep++;
  
  if (m_etaScans.size()==0) {
    ATH_MSG_ERROR( "Running in scan mode without specified eta values" );
    return StatusCode::FAILURE;
  }

  for (auto etaValue : m_etaScans) {
    m_currentEta = etaValue;
    ATH_MSG_VERBOSE("Running the ExtrapolationEngineTest Test in parameters mode : " << m_parametersMode);
    if (m_parametersMode == 0) {
      if(runTestT<Trk::NeutralParameters,Trk::NeutralPerigee>().isFailure())
	return StatusCode::FAILURE;
    }
    if(runTestT<Trk::TrackParameters,Trk::Perigee>().isFailure())
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;

}




  
