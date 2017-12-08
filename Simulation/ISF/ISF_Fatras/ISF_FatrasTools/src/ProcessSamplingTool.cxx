/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ProcessSamplingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/ProcessSamplingTool.h"

// Tracking
#include "TrkParameters/TrackParameters.h"
// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFTruthIncident.h"
#include "ISF_Interfaces/ITruthSvc.h"
// iFatras
#include "ISF_FatrasInterfaces/IHadronicInteractionProcessor.h"
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"
#include "ISF_FatrasInterfaces/IPhotonConversionTool.h"
#include "ISF_FatrasInterfaces/IParticleDecayHelper.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"
//
 
#include <iostream>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::ProcessSamplingTool::ProcessSamplingTool( const std::string& t,
                                      const std::string& n,
                                      const IInterface*  p )
  : AthAlgTool(t,n,p),
    m_rndGenSvc("AtDSFMTGenSvc", n),
    m_randomEngine(0),
    m_randomEngineName("FatrasRnd"),
    m_truthRecordSvc("ISF_ValidationTruthService", n),
    m_hadInt(true),
    m_hadIntProcessor("iFatras::G4HadIntProcessor/FatrasG4HadIntProcessor"),
    m_particleDecayer(),
    m_conversionTool("iFatras::McConversionCreator/FatrasConversionCreator"),
    m_validationMode(false),
    m_validationTool("")
{
  declareInterface<IProcessSamplingTool>(this);
  // service handles
  declareProperty( "RandomNumberService"                , m_rndGenSvc                );
  // hadronic interactions
  declareProperty( "HadronicInteraction"                , m_hadInt);
  declareProperty( "HadronicInteractionProcessor"       , m_hadIntProcessor);
  declareProperty( "ParticleDecayHelper"                , m_particleDecayer      );
  declareProperty( "PhotonConversionTool"               , m_conversionTool);
  // validation mode ------------------------------------------------------------
  declareProperty("ValidationMode"                      , m_validationMode);
  declareProperty("PhysicsValidationTool"               , m_validationTool);
  declareProperty("TruthRecordSvc"                      , m_truthRecordSvc        , "ISF Particle Truth Svc");      
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
iFatras::ProcessSamplingTool::~ProcessSamplingTool()
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode
iFatras::ProcessSamplingTool::initialize()
{
  
  ATH_MSG_INFO( "initialize()" );

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

  if (m_truthRecordSvc.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_truthRecordSvc );
    return StatusCode::FAILURE;
  }

  // retrieve the photon conversion tool
  if (m_conversionTool.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_conversionTool );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_VERBOSE( "Successfully retrieved " << m_conversionTool );
  
  // retrieve the hadronic interaction tool
  if (m_hadInt) {
    if (m_hadIntProcessor.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_hadIntProcessor );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_VERBOSE( "Successfully retrieved " << m_hadIntProcessor );                
  }

  // Particle decayer
  if (m_particleDecayer.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_particleDecayer );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_VERBOSE( "Successfully retrieved " << m_particleDecayer );                
   
  // retrieve the physics validation tool
  if (m_validationMode) {
    if (m_validationTool.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_validationTool );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_VERBOSE( "Successfully retrieved " << m_validationTool );
  }  

  return StatusCode::SUCCESS;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
StatusCode iFatras::ProcessSamplingTool::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

Trk::PathLimit iFatras::ProcessSamplingTool::sampleProcess(double momentum,double charge,Trk::ParticleHypothesis particle) const
{
  int process=0;
  double x0Max = -1.;

  if ( particle == Trk::electron && charge<0. ) return Trk::PathLimit(x0Max,process);

  if ( particle == Trk::muon || particle == Trk::nonInteractingMuon ) return Trk::PathLimit(x0Max,process);

  double rndx = CLHEP::RandFlat::shoot(m_randomEngine);

  if ( particle == Trk::photon ) {

    // conversion (14), compton effect (13), photo-effect (12)

    double  p0  =       -7.01612e-03;
    double  p1  =        7.69040e-02;
    double  p2  =       -6.07682e-01;
    // calculate xi
    double xi = p0 + p1*pow(momentum/1000.,p2);
    double attenuation = -7.777e-01*(1.-xi);

    x0Max = log(rndx)/attenuation ;

    // comment additional interactions till properly implemented
    /*
    // first estimate : compton ~ 4% , use attenuation factor as for conversion (till parameterized )
    double rndp = CLHEP::RandFlat::shoot(m_randomEngine);

    if ( rndp > 0.96 ) { // do Compton scattering
      process = 13;
    } else if (rndp < 5.53/pow(momentum,1.184) ) {
      process = 12;
      attenuation = -0.378;
      x0Max = log(rndx)/attenuation *momentum ;
    } else {
      process = 14; 
      //x0Max *= 0.5;
    }
    */
   
    process = 14;

    return Trk::PathLimit(x0Max,process);
  }

  if ( particle == Trk::electron && charge>0. ) {   // positron

    double mass = m_particleMasses.mass[Trk::electron];
    double gamma = momentum/mass; 

    // annihilation
    // energy dependent factor 
    double fpa = log(2*gamma)/gamma; 
    // material dependent factor - use Al(Z=13) for the moment
    double fza = 13.;
    double attenuation = -fpa*fza; // adjust
    
    x0Max = log(rndx)/attenuation ;
    process = 5;

    return Trk::PathLimit(x0Max,process);

  }

  // presumably here we are left with hadrons only
  if (m_hadInt) {

    // sample free path in terms of nuclear interaction length
    double al = 1.;          // scaling here 
    
    /*
      
      if ( particle == Trk::pion || particle == Trk::kaon || particle == Trk::pi0 || particle == Trk::k0) {
      al *= 1./(1.+ exp(-0.5*(momentum-270.)*(momentum-270.)/60./60.));
      }
      if ( particle == Trk::proton || particle == Trk::neutron ) al *=0.7;
      if ( particle == Trk::pion || particle == Trk::pi0) al *=0.9;
    */
  
    x0Max = -log(rndx)*al ;
    
    process = 121;

    //std::cout <<"hadronic path limit:"<<momentum<<","<<al<<","<< x0Max << std::endl;
    return Trk::PathLimit(x0Max,process);
  }  


  return Trk::PathLimit(x0Max,process);
    
  // return Trk::PathLimit(x0Max,process);
}


ISF::ISFParticleVector  iFatras::ProcessSamplingTool::interact(const ISF::ISFParticle* parent,
							       Trk::ExCellCharged& eCell,
							       const Trk::Material* mat) const
{
  ISF::ISFParticleVector childVector(0);
  
  int process = eCell.materialProcess;
  if ( process==0 ) return childVector;

  Trk::ParticleHypothesis particle = eCell.pHypothesis;
  const Trk::TrackParameters* parm=eCell.leadParameters;
    
  double mass = m_particleMasses.mass[particle];
  double p = parm->momentum().mag();
  //double E = sqrt( p*p + mass*mass);
  Amg::Vector3D position=parm->position();
  Amg::Vector3D momentum=parm->momentum();
  
  if ( process == 5 ) {     // positron annihilation

    double fmin = mass/p; 
     
    double fr = 1.-pow(fmin,CLHEP::RandFlat::shoot(m_randomEngine));

    // double cTh = 1.-fmin/(1.-fr)/fr;

    // first implementation: ctH=1 

    ISF::ISFParticle* child1 = new ISF::ISFParticle( position,
                                        fr*momentum,
                                        0.,
                                        0.,
                                        22,
                                        eCell.time,
                                        *parent );

    ISF::ISFParticle* child2 = new ISF::ISFParticle( position,
                                        (1-fr)*momentum,
                                        0.,
                                        0.,
                                        22,
                                        eCell.time,
                                        *parent );

    childVector.push_back(child1);     
    childVector.push_back(child2);     

  } else if (process==121) {    // hadronic interaction

    //const Amg::Vector3D pDir = momentum.unit();

    // const Trk::CurvilinearParameters parm(position,momentum,parent->charge());

    childVector = m_hadIntProcessor->doHadIntOnLayer(parent, eCell.time, position, momentum, mat, particle);

  } else if ( process == 201 ) {    // decay  
    childVector = m_particleDecayer->decayParticle(*parent, position, momentum, eCell.time);
  }

  // validation mode 
  if (m_validationMode) {

    // add process info for children
    for (unsigned int i=0; i<childVector.size(); i++) {
      ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
      validInfo->setProcess(process);
      if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo->setGeneration(1);     // assume parent is a primary track
      childVector[i]->setUserInformation(validInfo);
    }
    // save interaction info 
    if ( m_validationTool ) {
      Amg::Vector3D* nMom = 0;
      m_validationTool->saveISFVertexInfo(process, position,*parent,momentum,nMom,childVector);
      delete nMom;
    }
  }
  
  // register TruthIncident
  ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
			       childVector,
			       process,
			       parent->nextGeoID(),  // inherits from the parent
			       ISF::fKillsPrimary );
  m_truthRecordSvc->registerTruthIncident( truth);

  //  
  return childVector;
}

ISF::ISFParticleVector  iFatras::ProcessSamplingTool::interact(const ISF::ISFParticle* parent,
							       Trk::ExCellNeutral& eCell,
							       const Trk::Material* mat) const
{
  ISF::ISFParticleVector childVector(0);
  
  int process = eCell.materialProcess;
  Trk::ParticleHypothesis particle = eCell.pHypothesis;
  const Trk::NeutralParameters* parm=eCell.leadParameters;
  
  if ( process==0 ) return childVector;
  //double mass = m_particleMasses.mass[particle];
  //double p = parm.momentum().mag();
  /*double E = sqrt( p*p + mass*mass);*/
  Amg::Vector3D position=parm->position();
  Amg::Vector3D momentum=parm->momentum();

  if (process==14) {  // photon conversion   

    childVector = m_conversionTool->doConversionOnLayer(parent, eCell.time, *parm); 

  } else if (process==121) {    // hadronic interaction

    //const Amg::Vector3D pDir = momentum.unit();

    //const Trk::CurvilinearParameters parm(position,momentum,parent->charge());

    childVector =  m_hadIntProcessor->doHadIntOnLayer(parent, eCell.time, position, momentum, mat, particle);

  } else  if ( process == 201 ) {    // decay  

    childVector = m_particleDecayer->decayParticle(*parent, position, momentum, eCell.time);
  }

  // validation mode 
  if (m_validationMode) {

    // add process info for children
    for (unsigned int i=0; i<childVector.size(); i++) {
      ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
      validInfo->setProcess(process);
      if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo->setGeneration(1);     // assume parent is a primary track
      childVector[i]->setUserInformation(validInfo);
    }
    // save interaction info 
    if ( m_validationTool ) {
      Amg::Vector3D* nMom = 0;
      m_validationTool->saveISFVertexInfo(process, position,*parent,momentum,nMom,childVector);
      delete nMom;
    }
  }
  
  // register TruthIncident
  ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
			       childVector,
			       process,
			       parent->nextGeoID(),  // inherits from the parent
			       ISF::fKillsPrimary );
  m_truthRecordSvc->registerTruthIncident( truth);

  //
  return childVector; 
  
}


