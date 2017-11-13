/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PhotonConversionTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/PhotonConversionTool.h"

// Gaudi Kernel
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"
// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleVector.h"
#include "ISF_Event/ParticleClipboard.h"
#include "ISF_Event/ParticleUserInformation.h"
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFTruthIncident.h"
// iFatras
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"
// Trk inlcude
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkNeutralParameters/NeutralParameters.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Random/RandFlat.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
// STD
#include <math.h>

// temporary
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"

// statics doubles 
Trk::ParticleMasses iFatras::PhotonConversionTool::s_particleMasses;
double  iFatras::PhotonConversionTool::s_alpha         = 1./137.;
double  iFatras::PhotonConversionTool::s_oneOverThree  = 1./3.;
Trk::PdgToParticleHypothesis  iFatras::PhotonConversionTool::s_pdgToHypo;

// constructor
iFatras::PhotonConversionTool::PhotonConversionTool(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_particleBroker("ISF_ParticleParticleBroker", n),
  m_truthRecordSvc("ISF_TruthRecordSvc", n),
  m_processCode(14),
  m_referenceMaterial(true),
  m_minChildEnergy(50.*CLHEP::MeV),
  m_childEnergyScaleFactor(2.),
  m_conversionProbScaleFactor(0.98),
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_recordedConversions(0),
  m_droppedConversions(0),
  m_validationMode(false),
  m_validationTreeName("FatrasPhotonConversions"),
  m_validationTreeDescription("Validation output from the PhotonConversionTool"),
  m_validationTreeFolder("/val/FatrasPhotonConversions"),
  m_validationTree(0),
  m_validationTool(""),
  m_conversionPointX(0.),
  m_conversionPointY(0.),
  m_conversionPointR(0.),
  m_conversionPointZ(0.),
  m_conversionPhotonEnergy(0.),
  m_conversionChildEnergy(0.),
  m_conversionChildAngle(0.)
{
      declareInterface<IPhotonConversionTool>(this);

      // ISF Services and Tools
      declareProperty("ParticleBroker"                      , m_particleBroker        , "ISF ParticleBroker Svc"  );
      declareProperty("TruthRecordSvc"                      , m_truthRecordSvc        , "ISF Particle Truth Svc"  );      
      // MC Truth Properties
      declareProperty("PhysicsProcessCode"                  , m_processCode           , "MCTruth Physics Process Code");
      // the steering - general things
      declareProperty("ReferenceMaterial"                   , m_referenceMaterial);
      declareProperty("MinimumChildEnergy"                  , m_minChildEnergy);
      declareProperty("ChildEnergyScaling"                  , m_childEnergyScaleFactor);
      declareProperty("ConversionProbScaling"               , m_conversionProbScaleFactor);
      declareProperty("RandomNumberService"                 , m_rndGenSvc               , "Random number generator");
      declareProperty("RandomStreamName"                    , m_randomEngineName        , "Name of the random number stream");
      // validation   --------------------------------------------------------------
      declareProperty("ValidationMode"                      , m_validationMode);      
      declareProperty("PhysicsValidationTool"               , m_validationTool);
}

// destructor
iFatras::PhotonConversionTool::~PhotonConversionTool()
{}

// Athena standard methods
// initialize
StatusCode iFatras::PhotonConversionTool::initialize()
{
    ATH_MSG_INFO( "initialize()" );

    // ISF Services
    if (m_particleBroker.retrieve().isFailure()){
        ATH_MSG_FATAL( "Could not retrieve ISF Particle Broker: " << m_particleBroker );
        return StatusCode::FAILURE;
    }
    if (m_truthRecordSvc.retrieve().isFailure()){
        ATH_MSG_FATAL( "Could not retrieve ISF Truth Record Service: " << m_truthRecordSvc );
        return StatusCode::FAILURE;
    }

    // the random service
    if ( m_rndGenSvc.retrieve().isFailure() ){
        ATH_MSG_FATAL( "Could not retrieve " << m_rndGenSvc );
        return StatusCode::FAILURE;
    }
    //Get own engine with own seeds:
    m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
    if (!m_randomEngine) {
        ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
        return StatusCode::FAILURE;
    }
     
    // the validation setup ----------------------------------------------------------------------------------
    if (m_validationMode){

      // retrieve the physics validation tool
      if (m_validationTool.retrieve().isFailure()){
	ATH_MSG_FATAL( "Could not retrieve " << m_validationTool );
	return StatusCode::FAILURE;
      } else
	ATH_MSG_VERBOSE( "Successfully retrieved " << m_validationTool );
      
      
      
      ATH_MSG_VERBOSE(  "Booking conversion validation TTree ... " );
      
      // create the new Tree
      m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());
      
      // conversion positions and information
      m_validationTree->Branch("ConversionPositionX"    ,  &m_conversionPointX, "convX/F");
      m_validationTree->Branch("ConversionPositionY"    ,  &m_conversionPointY, "convY/F");
      m_validationTree->Branch("ConversionPositionR"    ,  &m_conversionPointR, "convR/F");
      m_validationTree->Branch("ConversionPositionZ"    ,  &m_conversionPointZ, "convZ/F");
      m_validationTree->Branch("ConversionPhotonEnergy" ,  &m_conversionPhotonEnergy, "convPhotonE/F");
      m_validationTree->Branch("ConversionChildEnergy"  ,  &m_conversionChildEnergy,  "convChildE/F");
      m_validationTree->Branch("ConversionChildAngle  " ,  &m_conversionChildAngle,   "convChildA/F");
    
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

    } // ------------- end of validation mode -----------------------------------------------------------------
   
    return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::PhotonConversionTool::finalize()
{

    ATH_MSG_INFO( " ---------- Statistics output -------------------------- " );
    ATH_MSG_INFO( "                     Minimum energy cut of conversions into e+e-  : " <<   m_minChildEnergy << " [MeV] " );
    ATH_MSG_INFO( "                     Conversions into e+e-  (above cut, recorded) : " <<   m_recordedConversions );
    ATH_MSG_INFO( "                     Conversions into e+e-  (below cut, dropped)  : " <<   m_droppedConversions );
    ATH_MSG_INFO( "finalize() successful" );

    return StatusCode::SUCCESS;
}

// ----------------- private helper methods ----------------------------------------------------
void iFatras::PhotonConversionTool::recordChilds(double time,
                                                const Amg::Vector3D& vertex,
                                                const Amg::Vector3D& photonDirection,
                                                double childEnergy, double photonEnergy,
                                                const Amg::Vector3D& childDirection,
                                                Trk::ParticleHypothesis childType) const
{
    // get parent particle
    // @TODO: replace by Fatras internal bookkeeping
    const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
    // something is seriously wrong if there is no parent particle
    assert(parent);

    // calculate the child momentum
    double p1 = sqrt(childEnergy*childEnergy-s_particleMasses.mass[childType]*s_particleMasses.mass[childType]);    

    // now properly : energy-momentum conservation
    CLHEP::HepLorentzVector _vtmp;
    CLHEP::Hep3Vector photonDirectionHep( photonDirection.x(), photonDirection.y(), photonDirection.z() );
    CLHEP::Hep3Vector childDirectionHep( childDirection.x(), childDirection.y(), childDirection.z() );
    _vtmp = CLHEP::HepLorentzVector(photonEnergy*photonDirectionHep, photonEnergy) 
      - CLHEP::HepLorentzVector(p1*childDirectionHep, childEnergy);

    double p2 = _vtmp.vect().mag();
    //Amg::Vector3D secondChildDir(_vtmp.vect().unit());

    // charge sampling
    double charge1, charge2;
    charge1 = charge2 = 0.;
    if (CLHEP::RandFlat::shoot(m_randomEngine)>0.5) {
      charge1 = -1.;
      charge2 =  1.;
    }
    else {
      charge1 =  1.;
      charge2 = -1.;
    }

    // add the new secondary states to the ISF particle stack
    double mass = s_particleMasses.mass[childType];
    int    pdg1  = s_pdgToHypo.convert(childType, charge1, false);
    int    pdg2  = s_pdgToHypo.convert(childType, charge2, false);

    // remove soft children
    int nchild = 0;
    if ( p1 > m_minChildEnergy ) nchild++;
    if ( p2 > m_minChildEnergy ) nchild++;

    ISF::ISFParticleVector children(nchild);

    int ichild = 0;
    if (  p1 > m_minChildEnergy ) {
      ISF::ISFParticle* ch1 = new ISF::ISFParticle( vertex,
                                               p1*childDirection,
                                               mass,
                                               charge1,
                                               pdg1,
                                               time,
                                               *parent );
      // in the validation mode, add process info
      if (m_validationMode) {
      	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
        validInfo->setProcess(14);
        if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
      	else validInfo->setGeneration(1);     // assume parent is a primary track
        ch1->setUserInformation(validInfo);
      }
      children[ichild] = ch1;
      if (!ch1->getTruthBinding()) {
	ch1->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
      }
      m_particleBroker->push( ch1, parent);
      ichild++;
    }

    if (  p2 > m_minChildEnergy ) {
      ISF::ISFParticle* ch2  = new ISF::ISFParticle( vertex,
                                               p2*childDirection,
                                               mass,
                                               charge2,
                                               pdg2,
                                               time,
                                               *parent );
      
      // in the validation mode, add process info
      if (m_validationMode) {
      	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
        validInfo->setProcess(14);
	if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
      	else validInfo->setGeneration(1);     // assume parent is a primary track
        ch2->setUserInformation(validInfo);
      }
      children[ichild] = ch2;
      if (!ch2->getTruthBinding()) {
        ch2->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
      }
      m_particleBroker->push( ch2, parent);
    }

    // register TruthIncident
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 children,
                                 m_processCode,
                                 parent->nextGeoID(),
                                 ISF::fKillsPrimary );
    m_truthRecordSvc->registerTruthIncident( truth);

    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nPrim=0;
      m_validationTool->saveISFVertexInfo(14,vertex,*parent,parent->momentum(),nPrim,children);
    }

}

ISF::ISFParticleVector iFatras::PhotonConversionTool::getChilds(const ISF::ISFParticle* parent,
								       double time,
								       const Amg::Vector3D& vertex,
								       const Amg::Vector3D& photonDirection,
								       double childEnergy, double photonEnergy,
								       const Amg::Vector3D& childDirection,
								       Trk::ParticleHypothesis childType) const
{
    static ISF::ISFParticleVector children(2);
 
    // calculate the child momentum
    double p1 = sqrt(childEnergy*childEnergy-s_particleMasses.mass[childType]*s_particleMasses.mass[childType]);    

    // now properly : energy-momentum conservation
    CLHEP::HepLorentzVector _vtmp;
    CLHEP::Hep3Vector photonDirectionHep( photonDirection.x(), photonDirection.y(), photonDirection.z() );
    CLHEP::Hep3Vector childDirectionHep( childDirection.x(), childDirection.y(), childDirection.z() );
    _vtmp = CLHEP::HepLorentzVector(photonEnergy*photonDirectionHep, photonEnergy) 
      - CLHEP::HepLorentzVector(p1*childDirectionHep, childEnergy);

    double p2 = _vtmp.vect().mag();
    //Amg::Vector3D secondChildDir(_vtmp.vect().unit());

    // charge sampling
    double charge1, charge2;
    charge1 = charge2 = 0.;
    if (CLHEP::RandFlat::shoot(m_randomEngine)>0.5) {
      charge1 = -1.;
      charge2 =  1.;
    }
    else {
      charge1 =  1.;
      charge2 = -1.;
    }

    double mass = s_particleMasses.mass[childType];
    int    pdg1  = s_pdgToHypo.convert(childType, charge1, false);
    int    pdg2  = s_pdgToHypo.convert(childType, charge2, false);

    // removal of soft children to be done in layer mat updator

    ISF::ISFParticle* ch1 = new ISF::ISFParticle( vertex,
						  p1*childDirection,
						  mass,
						  charge1,
						  pdg1,
						  time,
						  *parent );
    //if (m_validationMode) {
    //  ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
    //  validInfo->setProcess(14);
    //  if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
    //  else validInfo->setGeneration(1);     // assume parent is a primary track
    //  ch1->setUserInformation(validInfo);
    //}
    children[0] = ch1;

    ISF::ISFParticle* ch2  = new ISF::ISFParticle( vertex,
						   p2*childDirection,
						   mass,
						   charge2,
						   pdg2,
						   time,
						   *parent );
      
    //if (m_validationMode) {
    //  ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
    //  validInfo->setProcess(14);
    //  if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
    //  else validInfo->setGeneration(1);     // assume parent is a primary track
    //  ch2->setUserInformation(validInfo);
    //}
    children[1] = ch2;

    // register TruthIncident
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 children,
                                 m_processCode,
                                 parent->nextGeoID(),
                                 ISF::fKillsPrimary );
    m_truthRecordSvc->registerTruthIncident( truth);

    //Make sure the conversion products get a chance to have correct truth info before pushing into the particle broker
    if (!children[0]->getTruthBinding()) {
        children[0]->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
    }
    if (!children[1]->getTruthBinding()) {
        children[1]->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
    }

    // save info for validation
    //if (m_validationMode && m_validationTool) {
    //  Amg::Vector3D* nPrim=0;
    //  m_validationTool->saveISFVertexInfo(14,vertex,*parent,parent->momentum(),nPrim,children);
    //}

    return children;
}

bool iFatras::PhotonConversionTool::pairProduction(const Trk::MaterialProperties& mprop,
                                                 double pathCorrection,
                                                 double p) const
{

 // use for the moment only Al data - Yung Tsai - Rev.Mod.Particle Physics Vol. 74, No.4, October 1974
 // optainef from a fit given in the momentum range 100 10 6 2 1 0.6 0.4 0.2 0.1 GeV

 //// Quadratic background function
 //  Double_t fitFunction(Double_t *x, Double_t *par) {
 //  return par[0] + par[1]*pow(x[0],par[2]);
 // }
 
 // EXT PARAMETER                                   STEP         FIRST
 // NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE
 //  1  p0          -7.01612e-03   8.43478e-01   1.62766e-04   1.11914e-05
 //  2  p1           7.69040e-02   1.00059e+00   8.90718e-05  -8.41167e-07
 //  3  p2          -6.07682e-01   5.13256e+00   6.07228e-04  -9.44448e-07

 double  p0  =       -7.01612e-03;
 double  p1  =        7.69040e-02;
 double  p2  =       -6.07682e-01;
 // calculate xi
 double xi = p0 + p1*pow(p/1000.,p2);
 // now calculate what's left
 double attenuation = exp( -7.777e-01*pathCorrection*mprop.thicknessInX0()*(1.-xi) );

  ATH_MSG_VERBOSE(  "[ conv ] Propability of conversion    = " << (1.-attenuation) * 100. << " %." );

  /* ST do when the actual conversion happens */
  /*
  // record for the validation mode
  if (m_validationMode){
     // spatical information
     m_conversionPointX = float(vertex.x());     
     m_conversionPointY = float(vertex.y());     
     m_conversionPointR = float(vertex.perp());     
     m_conversionPointZ = float(vertex.z());     
     // photon energyEnergy
     m_conversionPhotonEnergy = float(p);
  }
  */

  return (m_conversionProbScaleFactor*CLHEP::RandFlat::shoot(m_randomEngine) > attenuation) ? true : false;

}


double iFatras::PhotonConversionTool::childEnergyFraction(const Trk::MaterialProperties& mprop, 
                                                        double gammaMom) const {

  // the fraction
  double epsilon0      = s_particleMasses.mass[Trk::electron]/gammaMom;
  // some needed manipolations
  double Z             = mprop.averageZ();
  double oneOverZpow   = 1./pow(Z,s_oneOverThree);
  double alphaZsquare  = (s_alpha*s_alpha*Z*Z);
  // now f(Z) - from Z and s_alpha
  double fZ            = alphaZsquare*(1./(1.+alphaZsquare)+0.20206-0.0369*alphaZsquare+0.0083*alphaZsquare*alphaZsquare);
  // delta_max
  double deltaMax      = exp((42.24-fZ)*.1195)-0.952;
  // delta_min
  double deltaMin      = 4.*epsilon0*136.*oneOverZpow; 
  // the minimum fraction
  double epsilon1      = 0.5-0.5*sqrt(1.-deltaMin/deltaMax);
  double epsilonMin    = epsilon1 > epsilon0 ? epsilon1 : epsilon0;
  // calculate phi1 / phi2 - calculate from deltaMin
  double Phi1          = phi1(deltaMin);
  double Phi2          = phi2(deltaMin);
  // then calculate F10/F20
  double F10           = 3.*Phi1 - Phi2 - fZ;
  double F20           = 1.5*Phi1 - 0.5*Phi2 - fZ;
  // and finally calucate N1, N2
  double N1            = (0.25-epsilonMin+epsilonMin*epsilonMin)*F10;
  double N2            = 1.5*F20;
  // ------------ decide wich one to take 
  if ( N1/(N1+N2) < CLHEP::RandFlat::shoot(m_randomEngine) ) {  
    // sample from f1,g1 distribution
    for ( ; ; ){
      double epsilon = 0.5 - (0.5 - epsilonMin)*pow(CLHEP::RandFlat::shoot(m_randomEngine),s_oneOverThree);
      // prepare for the rejection check
      double delta   = 136.*epsilon0*oneOverZpow/(epsilon-epsilon*epsilon);
      double F1 = 3.*phi1(delta)-phi2(delta)-fZ;   
      // reject ? - or redo the exercise 
      if (F1/F10 > CLHEP::RandFlat::shoot(m_randomEngine)) return m_childEnergyScaleFactor*epsilon;
    }
  } else {
    // sample from f2,g2 distribution
    for ( ; ; ){
      double epsilon = epsilonMin + (0.5-epsilonMin)*CLHEP::RandFlat::shoot(m_randomEngine);
      // prepare for the rejection check
      double delta   = 136.*epsilon0*oneOverZpow/(epsilon-epsilon*epsilon);
      double F2 = 1.5*phi1(delta)-0.5*phi2(delta)-fZ;   
     // reject ? - or redo the exercise 
     if (F2/F20 > CLHEP::RandFlat::shoot(m_randomEngine)) return m_childEnergyScaleFactor*epsilon;  
    }
  }

}



double iFatras::PhotonConversionTool::childEnergyFraction(double gammaMom) const {

  // the fraction
  double epsilon0      = s_particleMasses.mass[Trk::electron]/gammaMom;
  // some needed manipolations
  //double Z             = mprop.averageZ();
  double Z             = 13.;
  double oneOverZpow   = 1./pow(Z,s_oneOverThree);
  double alphaZsquare  = (s_alpha*s_alpha*Z*Z);
  // now f(Z) - from Z and s_alpha
  double fZ            = alphaZsquare*(1./(1.+alphaZsquare)+0.20206-0.0369*alphaZsquare+0.0083*alphaZsquare*alphaZsquare);
  // delta_max
  double deltaMax      = exp((42.24-fZ)*.1195)-0.952;
  // delta_min
  double deltaMin      = 4.*epsilon0*136.*oneOverZpow; 
  // the minimum fraction
  double epsilon1      = 0.5-0.5*sqrt(1.-deltaMin/deltaMax);
  double epsilonMin    = epsilon1 > epsilon0 ? epsilon1 : epsilon0;
  // calculate phi1 / phi2 - calculate from deltaMin
  double Phi1          = phi1(deltaMin);
  double Phi2          = phi2(deltaMin);
  // then calculate F10/F20
  double F10           = 3.*Phi1 - Phi2 - fZ;
  double F20           = 1.5*Phi1 - 0.5*Phi2 - fZ;
  // and finally calucate N1, N2
  double N1            = (0.25-epsilonMin+epsilonMin*epsilonMin)*F10;
  double N2            = 1.5*F20;
  // ------------ decide wich one to take 
  if ( N1/(N1+N2) < CLHEP::RandFlat::shoot(m_randomEngine) ) {  
    // sample from f1,g1 distribution
    for ( ; ; ){
      double epsilon = 0.5 - (0.5 - epsilonMin)*pow(CLHEP::RandFlat::shoot(m_randomEngine),s_oneOverThree);
      // prepare for the rejection check
      double delta   = 136.*epsilon0*oneOverZpow/(epsilon-epsilon*epsilon);
      double F1 = 3.*phi1(delta)-phi2(delta)-fZ;   
      // reject ? - or redo the exercise 
      if (F1/F10 > CLHEP::RandFlat::shoot(m_randomEngine)) return m_childEnergyScaleFactor*epsilon;
    }
  } else {
    // sample from f2,g2 distribution
    for ( ; ; ){
      double epsilon = epsilonMin + (0.5-epsilonMin)*CLHEP::RandFlat::shoot(m_randomEngine);
      // prepare for the rejection check
      double delta   = 136.*epsilon0*oneOverZpow/(epsilon-epsilon*epsilon);
      double F2 = 1.5*phi1(delta)-0.5*phi2(delta)-fZ;   
     // reject ? - or redo the exercise 
     if (F2/F20 > CLHEP::RandFlat::shoot(m_randomEngine)) return m_childEnergyScaleFactor*epsilon;  
    }
  }

}


Amg::Vector3D iFatras::PhotonConversionTool::childDirection(const Amg::Vector3D& gammaMom,
                                                                 double childE) const
{
    // --------------------------------------------------
    // Following the Geant4 approximation from L. Urban
    // the azimutal angle
    double psi    =  2.*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
    
    // the start of the equation
    double theta = s_particleMasses.mass[Trk::electron]/childE;
    // follow 
    double a = 0.625; // 5/8
    //double d = 27.;

    double r1 = CLHEP::RandFlat::shoot(m_randomEngine);
    double r2 = CLHEP::RandFlat::shoot(m_randomEngine);
    double r3 = CLHEP::RandFlat::shoot(m_randomEngine);

    double u =  -log(r2*r3)/a;
    
    theta *= (r1 < 0.25 ) ? u : u*s_oneOverThree; // 9./(9.+27) = 0.25

     ATH_MSG_VERBOSE( "[ conv ] Simulated angle to photon    = " << theta << "." );

    // more complex but "more true"
    CLHEP::Hep3Vector gammaMomHep( gammaMom.x(), gammaMom.y(), gammaMom.z() );
    CLHEP::Hep3Vector newDirectionHep(gammaMomHep.unit());
    double x = -newDirectionHep.y();
    double y = newDirectionHep.x();
    double z = 0.;
    // if it runs along the z axis - no good ==> take the x axis
    if (newDirectionHep.z()*newDirectionHep.z() > 0.999999)       
        x = 1.;
    // deflector direction
    CLHEP::Hep3Vector deflectorHep(x,y,z);
    // rotate the new direction for scattering
    newDirectionHep.rotate(theta, deflectorHep);
    // and arbitrarily in psi             
    newDirectionHep.rotate(psi,gammaMomHep);

    // record for the validation mode
    if (m_validationMode){
       // child energy and direction
       m_conversionChildEnergy = float(childE);  
       m_conversionChildAngle  = float(theta);
    }

    // assign the new values
    Amg::Vector3D newDirection( newDirectionHep.x(), newDirectionHep.y(), newDirectionHep.z() );
    return newDirection;

}

bool iFatras::PhotonConversionTool::doConversion(double time, const Trk::NeutralParameters& parm,
						const Trk::ExtendedMaterialProperties* /*extMatProp*/) const {
  double p = parm.momentum().mag();
  
  // get the energy
  double childEnergy = p*childEnergyFraction(p);
  // count the conversion 
  ++m_recordedConversions;
  // now get the deflection
  Amg::Vector3D childDir(childDirection(parm.momentum(), childEnergy));
  // verbose output
  ATH_MSG_VERBOSE(  "[ conv ] Child energy simulated as : " << childEnergy << " MeV" );
  // calculate the second child direction
  recordChilds(time,
	       parm.position(),
               parm.momentum().unit(),
	       childEnergy, p,
	       childDir,
	       Trk::electron);
  // fill the TTree ----------------------------
  if (m_validationTree)
    m_validationTree->Fill();
  
  return true;
}

/** interface for processing of the presampled nuclear interactions on layer*/
ISF::ISFParticleVector iFatras::PhotonConversionTool::doConversionOnLayer(const ISF::ISFParticle* parent, 
									 double time, const Trk::NeutralParameters& parm,
									 const Trk::ExtendedMaterialProperties* /*ematprop*/) const {
  double p = parm.momentum().mag();

  // get the energy
  double childEnergy = p*childEnergyFraction(p);

  // count the conversion 
  ++m_recordedConversions;

  // now get the deflection
  Amg::Vector3D childDir(childDirection(parm.momentum(), childEnergy));
  // verbose output
  ATH_MSG_VERBOSE(  "[ conv ] Child energy simulated as : " << childEnergy << " MeV" );
  // fill the TTree ----------------------------
  if (m_validationTree)
    m_validationTree->Fill();
  // calculate the second child direction and return
  return getChilds(parent,
		   time,
		   parm.position(),
		   parm.momentum().unit(),
		   childEnergy, p,
		   childDir,
		   Trk::electron);

}

