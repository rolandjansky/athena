/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HadIntProcessorParametric.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/HadIntProcessorParametric.h"

// Gaudi Kernel
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// ISF 
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFTruthIncident.h"
#include "ISF_Event/ParticleClipboard.h"
// Fatras
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
// STD
#include <math.h>

// Tracking
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/MaterialProperties.h"

// statics doubles 
Trk::ParticleMasses iFatras::HadIntProcessorParametric::s_particleMasses;

// constructor
iFatras::HadIntProcessorParametric::HadIntProcessorParametric(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_minimumHadOutEnergy(200.*CLHEP::MeV),
  m_childMomParam(5.),
  m_cutChain(false),
  m_hadIntFromX0(false),
  m_hadIntProbScale(1.0),
  m_minimumHadInitialEnergy(1000.*CLHEP::MeV),
  m_particleBroker("ISF_ParticleBroker", n),
  m_truthRecordSvc("ISF_TruthRecordSvc", n),
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_processCode(121),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_validationMode(false),
  m_validationTool(""),
  m_hadIntValidation(false),
  m_hadIntValidationTreeName("FatrasHadronicInteractions"),
  m_hadIntValidationTreeDescription("Validation output from the HadIntProcessorParametric"),
  m_hadIntValidationTreeFolder("/val/FatrasHadronicInteractions"),
  m_hadIntValidationTree(0),
  m_hadIntPointX(0.),
  m_hadIntPointY(0.),
  m_hadIntPointR(0.),
  m_hadIntPointZ(0.),
  m_hadIntMotherPdg(0),
  m_hadIntMotherP(0.),
  m_hadIntMotherPhi(0.),
  m_hadIntMotherEta(0.),
  m_hadIntChildren(0)
{
      // property setting
      declareProperty("MinimumHadronicOutEnergy"            , m_minimumHadOutEnergy);
      declareProperty("HadronicChildMomParam"               , m_childMomParam);
      declareProperty("HadronicInteractionFromX0"           , m_hadIntFromX0);
      declareProperty("HadronicInteractionScaleFactor"      , m_hadIntProbScale); 
      declareProperty("MinimumHadronicInitialEnergy"        , m_minimumHadInitialEnergy);
      // the steering --------------------------------------------------------------
      declareProperty("ShortenHadIntChain"                  , m_cutChain);
      declareProperty("ValidationMode"                      , m_validationMode);
      declareProperty("PhysicsValidationTool"               , m_validationTool);
      declareProperty("HadronicInteractionValidation"       , m_hadIntValidation);
      // validation mode
      declareProperty("RandomNumberService"                 , m_rndGenSvc          , "Random number generator");
      declareProperty("RandomStreamName"                    , m_randomEngineName   , "Name of the random number stream");
      // MC Truth Properties
      declareProperty("PhysicsProcessCode"                  , m_processCode        , "MCTruth Physics Process Code" );
      // ISF Services and Tools
      declareProperty("ParticleBroker"                      , m_particleBroker     , "ISF ParticleBroker Svc"       );
      declareProperty("TruthRecordSvc"                      , m_truthRecordSvc     , "ISF Particle Truth Svc"       );
}

// destructor
iFatras::HadIntProcessorParametric::~HadIntProcessorParametric()
{}

// Athena standard methods
// initialize
StatusCode iFatras::HadIntProcessorParametric::initialize()
{

    ATH_MSG_INFO( "initialize()" );

    // get the random generator serice
     if (m_rndGenSvc.retrieve().isFailure()){
          ATH_MSG_FATAL( "Could not retrieve " << m_rndGenSvc );
          return StatusCode::FAILURE;
     } else
          ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndGenSvc );
    
    //Get own engine with own seeds:
    m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
    if (!m_randomEngine) {
        ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
        return StatusCode::FAILURE;
    }    

    // ISF Services
    if (m_particleBroker.retrieve().isFailure()){
        ATH_MSG_FATAL( "Could not retrieve " << m_particleBroker );
        return StatusCode::FAILURE;
    }
    if (m_truthRecordSvc.retrieve().isFailure()){
        ATH_MSG_FATAL( "Could not retrieve " << m_truthRecordSvc );
        return StatusCode::FAILURE;
    }

    // the validation setup -------------------------------- PART 3: Hadronic Interaction ---------------------
    if (m_validationMode){

      // retrieve the physics validation tool
      if (m_validationTool.retrieve().isFailure()){
	ATH_MSG_FATAL( "Could not retrieve " << m_validationTool );
	return StatusCode::FAILURE;
      } else
	ATH_MSG_VERBOSE( "Successfully retrieved " << m_validationTool );
    }
      
    if (m_hadIntValidation){

      ITHistSvc* tHistSvc = 0;
      // now register the Tree
      if (service("THistSvc",tHistSvc).isFailure()) 
           ATH_MSG_ERROR( "initialize() Could not find Hist Service -> Switching ValidationMode Off !" ); 
      else {
    
        ATH_MSG_VERBOSE( "Booking hadronic interaction validation TTree ... " );

        // create the new Tree
        m_hadIntValidationTree = new TTree(m_hadIntValidationTreeName.c_str(), m_hadIntValidationTreeDescription.c_str());

           // counter for boundary surfaces
           m_hadIntValidationTree->Branch("HadIntPointX"    ,  &m_hadIntPointX, 			"hintX/F");
           m_hadIntValidationTree->Branch("HadIntPointY"    ,  &m_hadIntPointY, 			"hintY/F");
           m_hadIntValidationTree->Branch("HadIntPointR"    ,  &m_hadIntPointR, 			"hintR/F");
           m_hadIntValidationTree->Branch("HadIntPointZ"    ,  &m_hadIntPointZ, 			"hintZ/F");
           m_hadIntValidationTree->Branch("HadIntMotherPdg"    ,  &m_hadIntMotherPdg,       "hintMotherPdg/I");
           m_hadIntValidationTree->Branch("HadIntMotherBarcode"    ,  &m_hadIntMotherBarcode, "hintMotherBarcode/I");
           m_hadIntValidationTree->Branch("HadIntMotherP" ,  &m_hadIntMotherP,    "hintMotherP/F");
           m_hadIntValidationTree->Branch("HadIntMotherPt" ,  &m_hadIntMotherPt,    "hintMotherPt/F");
           m_hadIntValidationTree->Branch("HadIntMotherPhi"    ,  &m_hadIntMotherPhi,       "hintMotherPhi/F");
           m_hadIntValidationTree->Branch("HadIntMotherEta"    ,  &m_hadIntMotherEta,       "hintMohterEta/F");
    
           m_hadIntValidationTree->Branch("HadIntChildren"     ,  &m_hadIntChildren,        "hintcs/I");
           m_hadIntValidationTree->Branch("HadIntChildE  "     ,  &m_hadIntChildE,          "hintce/F");
           m_hadIntValidationTree->Branch("HadIntChildPdg"     ,  m_hadIntChildPdg,         "hintChildPdg[hintcs]/I");
           m_hadIntValidationTree->Branch("HadIntChildP"  ,  m_hadIntChildP,      "hintChildP[hintcs]/F");
           m_hadIntValidationTree->Branch("HadIntChildPcms"  ,  m_hadIntChildPcms,      "hintChildPcms[hintcs]/F");
           m_hadIntValidationTree->Branch("HadIntChildTh"  ,  m_hadIntChildTh,      "hintChildTh[hintcs]/F");
           m_hadIntValidationTree->Branch("HadIntChildThc"  ,  m_hadIntChildThc,      "hintChildThc[hintcs]/F");
           m_hadIntValidationTree->Branch("HadIntChildPhi"     ,  m_hadIntChildPhi,         "hintChildPhi[hintcs]/F");
           m_hadIntValidationTree->Branch("HadIntChildEta"     ,  m_hadIntChildEta,         "hintChildEta[hintcs]/F");
           m_hadIntValidationTree->Branch("HadIntChildDeltaPhi",  m_hadIntChildDeltaPhi,    "hintChildPhi[hintcs]/F");
           m_hadIntValidationTree->Branch("HadIntChildDeltaEta",  m_hadIntChildDeltaEta,    "hintChildEta[hintcs]/F");


        if ((tHistSvc->regTree(m_hadIntValidationTreeFolder, m_hadIntValidationTree)).isFailure()) {
           ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
           delete m_hadIntValidationTree; m_hadIntValidationTree = 0;
        } else
           ATH_MSG_INFO( "TTree for Hadronic Interactions validation booked." );
      }
    } // ------------- end of validation mode -----------------------------------------------------------------

    return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::HadIntProcessorParametric::finalize()
{
    ATH_MSG_INFO( "finalize() successful" );
    return StatusCode::SUCCESS;
}


bool iFatras::HadIntProcessorParametric::hadronicInteraction(const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
							     double p, double /*E*/, double charge, 
							     const Trk::MaterialProperties& mprop, double pathCorrection,
							     Trk::ParticleHypothesis particle) const 
{
  const Trk::MaterialProperties* extMprop = dynamic_cast<const Trk::MaterialProperties*>(&mprop);
  double prob = 0.;

  // m_hadIntProbScale is used later, not here
  if (extMprop && !m_hadIntFromX0) {

    double al = absorptionLength(extMprop, p, charge, particle);  // in mm

    if (al>0.) prob = exp( (-1.)*pathCorrection*extMprop->thickness()/al );
    else       prob = exp( (-1.)*pathCorrection*extMprop->thicknessInL0()); 

  } else {
    ATH_MSG_VERBOSE( "  [ had ] Nuclear Interaction length not available, using appox. from X0" );
    // using approximation lambda = 0.37 * Z * X0 instead -- giving a warning message
    prob = exp( (-1.)*pathCorrection*mprop.thicknessInX0() / ( 0.37 * mprop.averageZ())  );
  }

  if (msgLvl(MSG::VERBOSE)) 
    ATH_MSG_VERBOSE( "  [ had ] Probability of hadronic interaction = " 
    << (1. - prob) * 100. * m_hadIntProbScale << " %." );
  
  // apply a global scalor of the probability
  // (1. - prob) is generally O(0.01), so this is the right way to scale it
  // TODO fix time info (if needed)
  if (CLHEP::RandFlat::shoot(m_randomEngine) < (1. - prob) * m_hadIntProbScale ) return recordHadState(0.,p, 
												       position,
												       momentum.unit(),
												       particle);
  
  // no hadronic interactions were computed
  return false;
}


/** absorption length */
double iFatras::HadIntProcessorParametric::absorptionLength(const Trk::MaterialProperties* mat,
							    double p, double, Trk::ParticleHypothesis particle) const {
  double al = mat->l0();

  /* // these parametrization comes from comparison with G4 sampler, but give too many interactions 
     // not understood yet
     if ( particle == Trk::pion ) al = ( 0.53+0.2*exp(-p/1000.) ) * mat->l0();
     else if ( particle == Trk::kaon ) al = ( 0.1+0.65*exp(-p/5000.) ) * mat->l0();
     else if ( particle == Trk::proton ) al = 0.57 * mat->l0();
     else al =  mat->l0(); 
  */ 

  if ( particle == Trk::pion || particle == Trk::kaon || particle == Trk::pi0 || particle == Trk::k0) {
    al *= 1./(1.+ exp(-0.5*(p-270.)*(p-270.)/60./60.));
  }
  if ( particle == Trk::proton || particle == Trk::neutron ) al *=0.7;
  if ( particle == Trk::pion || particle == Trk::pi0) al *=0.9;

  return al;
}


ISF::ISFParticleVector iFatras::HadIntProcessorParametric::getHadState(const ISF::ISFParticle* parent,double time,double p,
								       const Amg::Vector3D& vertex,
								       const Amg::Vector3D& particleDir,
								       Trk::ParticleHypothesis particle  
								       ) const
{  
  ISF::ISFParticleVector chDef(0); 

  // sampling of hadronic interaction
  double m = s_particleMasses.mass[particle];
  double E = sqrt(p*p + m*m);

  // get the maximum multiplicity    
  double multiplicity_max = 0.25 * E/1000. + 18.;
  
  // multiplicity distribution
  double randx , randy, arg = 0.;
  
  double p1 = 0.;
  double p2 = 0.;
  
  if (E > 15000.) {
    p1 = 8.69;
    p2 = 2.34;
  } else {
    p1 = 6.77;
    p2 = 2.02;
  }
  
  for (;;) {
    randx = 30. * CLHEP::RandFlat::shoot(m_randomEngine);
    randy = 1.  * CLHEP::RandFlat::shoot(m_randomEngine);
    arg = exp(-0.5*( (randx-p1)/p2 + exp(-(randx-p1)/p2) ) );
    if (randy < arg && randx>3 && randx<multiplicity_max) break;
  }
  
  randx *= (1.2-0.4*exp(-0.001*p));     // trying to adjust
  
  int Npart = (int)randx;
  
  // protection against Npart < 3
  if (Npart < 3) {
    ATH_MSG_VERBOSE( "[ had ] Number of particles smaller than 3, parameterisation not valid." 
		     << " Doing Nothing");
    return chDef;
  }

  ATH_MSG_VERBOSE( "[ had ] interaction of " << parent->barcode() 
		   << " with " << Npart << " outgoing particles " );
  
  // record the interaction
  
  // ------ now create the new hadrons ------
  ATH_MSG_DEBUG(  "[ had ] create hadronic shower for particle with PDG ID "
		  << parent->pdgCode() << " and barcode "
		  << parent->barcode() );
  
  /* TODO: update to ISF TruthService
  // create GenVertex at interaction point, add the pion as incoming and add vertex to the event
  HepMC::GenVertex* decayvertex = new HepMC::GenVertex(CLHEP::HepLorentzVector(vertex, 0.));
  decayvertex->add_particle_in(m_particleBroker->current());
  HepMC::GenEvent* parentEvent = s_currentGenParticle->parent_event();
  if( !parentEvent) {
  ATH_MSG_WARNING( "[ had ] particle with PDG ID "
  << m_particleBroker->current()->pdgCode() << " and barcode "
  << m_particleBroker->current()->barcode() << " does not have a parent!"
  );
  } else {
  parentEvent->add_vertex(decayvertex);
  }
  */
  
  // create the genParticles
  
  // validation if needed 
  if (m_hadIntValidationTree){
    m_hadIntPointX        = vertex.x();
    m_hadIntPointY        = vertex.y();
    m_hadIntPointZ        = vertex.z();
    m_hadIntPointR        = vertex.perp();
    m_hadIntMotherPdg     = parent->pdgCode();
    m_hadIntMotherBarcode = parent->barcode();
    m_hadIntMotherP       = p;
    m_hadIntMotherPt      = p*particleDir.perp();
    m_hadIntMotherPhi     = particleDir.phi();
    m_hadIntMotherEta     = particleDir.eta();	
    // reset the children 
    m_hadIntChildren      = 0;
  }
  
  ATH_MSG_VERBOSE( "[ had ] incoming particle energy | mass | momentum " 
		   << E << " | " << m << " | " << p << " | " );
  
  /* TODO: this will not work with the new barcode style */
  if (m_cutChain && ( parent->barcode()>100000 || parent->barcode()==0 ) ) {
    if (m_hadIntValidationTree) m_hadIntValidationTree->Fill();
    ATH_MSG_VERBOSE( "[ had ] interaction initiated by a secondary particle, no children saved " ); 
    return chDef;
  }
  
  int gen_part = 0;
    
  // new sampling: sample particle type and energy in the CMS frame of outgoing particles
  // creation of shower particles
  double chargedist = 0.;
  std::vector<double> charge(Npart);
  std::vector<Trk::ParticleHypothesis> childType(Npart);
  std::vector<double> newm(Npart);
  std::vector<int> pdgid(Npart);    
  
  // children type sampling  : simplified
  //double pif = 0.19;
  //double nef = 0.20;
  //double prf = 0.20;

  // sample heavy particles (alpha) but don't save  
  double pif = 0.10; 
  double nef = 0.30;
  double prf = 0.30;
  
  if ( particle == Trk::pion || particle == Trk::kaon || particle == Trk::pi0 || particle == Trk::k0 ) {
      pif = 0.15;
      nef = 0.25;
      prf = 0.25;
    }
  if ( particle == Trk::proton ) {
    pif = 0.06;
    nef = 0.25;
    prf = 0.35;
  }
  if ( particle == Trk::neutron ) {
    pif = 0.03;
    nef = 0.35;
    prf = 0.17;
  }
  
  for (int i=0; i<Npart; i++) {
    chargedist  = CLHEP::RandFlat::shoot(m_randomEngine);
    if (chargedist<pif) {
      charge[i]=0.;
      childType[i]=Trk::pi0;
      newm[i]=s_particleMasses.mass[Trk::pi0]; // MeV
      pdgid[i]=111;
      continue;
    }
    if ( chargedist<2*pif) {
      charge[i]=1.;
      childType[i]=Trk::pion;
      newm[i]=s_particleMasses.mass[Trk::pion]; // MeV
      pdgid[i]=211;
      continue;
    }
    if (chargedist<3*pif) {
      charge[i]=-1.;
      childType[i]=Trk::pion;
      newm[i]=s_particleMasses.mass[Trk::pion]; // MeV
      pdgid[i]=-211;
      continue;
    }
    if (chargedist<3*pif+nef) {
      charge[i]=0.;
      childType[i]=Trk::neutron;
      newm[i]=939.565; // MeV
      pdgid[i]=2112; // neutron
      continue;
    }
    if (chargedist<3*pif+nef+prf) {
      charge[i]=1.;
      childType[i]=Trk::proton;
      newm[i]=s_particleMasses.mass[Trk::proton]; // MeV
      pdgid[i]=2212;
      continue;
    }
    charge[i]=2.;
    childType[i]=Trk::proton;
    newm[i]=4000.;
    pdgid[i]=20000;
  }

  // move the incoming particle type forward
  if ( childType[0] != particle ) {
    for (int i=1; i<Npart; i++) {
      if (childType[i]==particle) {
        childType[i]=childType[0];
        childType[0]=particle;
        double cho = charge[i];
        charge[i]=charge[0];
        charge[0]=parent ? parent->charge() : cho;
	newm[i]=s_particleMasses.mass[childType[i]]; // MeV
	newm[0]=s_particleMasses.mass[childType[0]]; // MeV
        break;
      }
    }
  }

  /*
  // sample momentum of daughters in CMS frame of outgoing particles  [ exp(-par/p) ]
  std::vector<double> mom;
  mom.clear();mom.reserve(Npart);
  double mom_n = 0.;  
  for (int _npart=0; _npart<Npart; _npart++) {
    rand1  = CLHEP::RandFlat::shoot(m_randomEngine);
    mom_n = -log(rand1)/m_childMomParam * p;
    int ipos = _npart;
    while ( ipos>0 && mom_n>mom[ipos-1]) ipos--;
    mom.insert(mom.begin()+ipos,mom_n);
  }  
  
  // check if configuration acceptable - if not, resample hardest mom
  double momR = 0.;
  for (int i=1; i<Npart; i++) momR += mom[i];
  if (momR < mom[0]) mom[0] = mom[1]+rand1*(momR-mom[1]);
  */

  std::vector<double> mom(Npart);
  std::vector<double> th(Npart);
  std::vector<double> ph(Npart);

  // sample first particle energy fraction and random momentum direction
  double eps = 2./Npart;
  double rnd  = CLHEP::RandFlat::shoot(m_randomEngine);
  mom[0] = 0.5*pow(eps,rnd);          
  th[0]  = acos( 2*CLHEP::RandFlat::shoot(m_randomEngine)-1.);
  ph[0]  = 2*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
  
  // toss particles around in a way which preserves the total momentum (0.,0.,0.) at this point
  // TODO shoot first particle along the impact direction preferentially

  Amg::Vector3D ptemp(mom[0]*sin(th[0])*cos(ph[0]),mom[0]*sin(th[0])*sin(ph[0]),mom[0]*cos(th[0]));
  double ptot = mom[0];
  
  double theta = 0.; double phi = 0.; 
  for (int i=1; i<Npart-2; i++) {
    eps = 1./(Npart-i); 
    //mom[i] = pow(eps,CLHEP::RandFlat::shoot(m_randomEngine))*(1-ptot);
    mom[i] = ( eps + CLHEP::RandFlat::shoot(m_randomEngine)*(1-eps))*(1-ptot); 
    if (ptemp.mag()<1-ptot) {
      while ( fabs(ptemp.mag()-mom[i])>1-ptot-mom[i] ){
	mom[i] =  ( eps + CLHEP::RandFlat::shoot(m_randomEngine)*(1-eps))*(1-ptot);      
      }
    }
    // max p remaining
    double p_rem=1-ptot-mom[i];
    double cthmax = fmin(1.,(-ptemp.mag()*ptemp.mag()-mom[i]*mom[i]+p_rem*p_rem)/2/ptemp.mag()/mom[i]);
    //if (cthmax<-1.) std::cout <<"problem in theta sampling:p_rem:ptot:pcurr:"<<p_rem<<","<<ptemp.mag()<<","<<mom[i]<< std::endl;
    double rnd  = CLHEP::RandFlat::shoot(m_randomEngine);
    theta = acos( (cthmax+1.)*rnd-1.);          
    phi = 2*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
    HepGeom::Vector3D<double> test(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
    HepGeom::Vector3D<double> dnewHep = HepGeom::RotateZ3D(ptemp.phi())*HepGeom::RotateY3D(ptemp.theta())*test;
    Amg::Vector3D dnew( dnewHep.x(), dnewHep.y(), dnewHep.z() );
    th[i]=dnew.theta();    
    ph[i]=dnew.phi();          
    ptemp += mom[i]*dnew;
    ptot += mom[i];
  }
  
  eps = 0.5; 
  mom[Npart-2] = pow(eps,CLHEP::RandFlat::shoot(m_randomEngine))*(1-ptot);
  mom[Npart-1] = 1-ptot-mom[Npart-2];
  
  if (ptemp.mag()<1-ptot) {
    while (mom[Npart-1]+mom[Npart-2]<ptemp.mag()) { 
      mom[Npart-2] = pow(eps,CLHEP::RandFlat::shoot(m_randomEngine))*(1-ptot);
      mom[Npart-1] = 1-ptot-mom[Npart-2];
    }
  }
  if (ptemp.mag()<fabs(mom[Npart-1]-mom[Npart-2]) ) {
    double diff = ptemp.mag()*CLHEP::RandFlat::shoot(m_randomEngine);
    double sum = mom[Npart-1]-mom[Npart-2];
    mom[Npart-2]=0.5*(sum+diff);  
    mom[Npart-1]=0.5*(sum-diff);  
  }
  double cth =(-ptemp.mag()*ptemp.mag()-mom[Npart-2]*mom[Npart-2]+mom[Npart-1]*mom[Npart-1])/2/ptemp.mag()/mom[Npart-2];
  if (fabs(cth)>1.) cth = (cth>0.) ? 1. : -1.;
  
  theta = acos(cth);
  phi = 2*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
  HepGeom::Vector3D<double> test(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
  HepGeom::Vector3D<double> dnewHep = HepGeom::RotateZ3D(ptemp.phi())*HepGeom::RotateY3D(ptemp.theta())*test;
  Amg::Vector3D dnew( dnewHep.x(), dnewHep.y(), dnewHep.z() );
  
  th[Npart-2]=dnew.theta();    
  ph[Npart-2]=dnew.phi();    
  ptemp += mom[Npart-2]*dnew;
  Amg::Vector3D dlast = -ptemp;
  th[Npart-1]=dlast.theta(); 
  ph[Npart-1]=dlast.phi();    
  
  // particle sampled, rotate, boost and save final state
  //CLHEP::HepLorentzVector bv(p*particleDir.unit().x(),p*particleDir.unit().y(),p*particleDir.unit().z(),s_currentGenParticle->momentum().e()+mtot);  
  double etot = 0.;
  for (int i=0;i<Npart; i++) etot += sqrt(mom[i]*mom[i]+newm[i]*newm[i]);
  double summ = 0.;
  for (int i=0;i<Npart; i++) summ += newm[i];

  // std::cout <<"hadronic interaction: current energy, expected :"<< etot <<","<< sqrt(summ*summ+2*summ*p+m*m)<< std::endl;
  // rescale (roughly) to the expected energy
  float scale = sqrt(summ*summ+2*summ*p+m*m)/etot;
  etot = 0.;
  for (int i=0;i<Npart; i++) {
    mom[i] *= scale;
    etot += sqrt(mom[i]*mom[i]+newm[i]*newm[i]);
  }

  
  CLHEP::HepLorentzVector bv(p*particleDir.unit().x(),p*particleDir.unit().y(),p*particleDir.unit().z(),sqrt(etot*etot+p*p));  
  std::vector<CLHEP::HepLorentzVector> childBoost(Npart);
  
  //std::cout <<"boost vector:"<<p<<","<<bv.e()<<","<<bv.m()<<std::endl;
  //std::cout <<"etot, mother E,m:"<<etot<<","<<E<<","<<m<<std::endl;
  
  Amg::Vector3D in(0.,0.,0.); 
  Amg::Vector3D fin(0.,0.,0.); 
  
  for (int i=0; i<Npart; i++) {
    Amg::Vector3D dirCms(sin(th[i])*cos(ph[i]),sin(th[i])*sin(ph[i]),cos(th[i])); 
    //Amg::Vector3D rotDirCms=HepGeom::RotateZ3D(particleDir.phi())*HepGeom::RotateY3D(particleDir.theta())*dirCms; 
    Amg::Vector3D childP = mom[i]*dirCms;
    in += childP;
    CLHEP::HepLorentzVector newp(childP.x(),childP.y(),childP.z(),sqrt(mom[i]*mom[i]+newm[i]*newm[i]));
    CLHEP::HepLorentzVector childPB = newp.boost(bv.boostVector());
    childBoost[i]=childPB;
    fin += Amg::Vector3D(childPB.x(),childPB.y(),childPB.z());
  } 
  
  double eout = 0.;
  
  // child particle vector for TruthIncident
  //  Reserve space for as many paricles as created due to hadr. int.
  //  However, the number of child particles for TruthIncident might be
  //  smaller due to (momentum) cuts
  ISF::ISFParticleVector           children(Npart);
  ISF::ISFParticleVector::iterator childrenIt = children.begin();
  unsigned short                numChildren = 0;
  
  for (int i=0; i<Npart; i++) {
    if (pdgid[i]<10000) {
      Amg::Vector3D childP = Amg::Vector3D(childBoost[i].x(),childBoost[i].y(),childBoost[i].z());
      Amg::Vector3D chP = Amg::Vector3D(sin(th[i])*cos(ph[i]),sin(th[i])*sin(ph[i]),cos(th[i]));
      
      eout += childBoost[i].e();     
      
      // validation if needed
      if (m_hadIntValidationTree && m_hadIntChildren < MAXHADINTCHILDREN){
	m_hadIntChildPdg[m_hadIntChildren]      = pdgid[i];   
	m_hadIntChildP[m_hadIntChildren]        = childP.mag();
	m_hadIntChildPcms[m_hadIntChildren]     = mom[i];
	m_hadIntChildTh[m_hadIntChildren]        = childP.unit().dot(particleDir);
	m_hadIntChildThc[m_hadIntChildren]       =chP.dot(particleDir);
	m_hadIntChildPhi[m_hadIntChildren]      = childP.phi();
	m_hadIntChildEta[m_hadIntChildren]      = childP.eta();
	double deltaPhi = m_hadIntMotherPhi - m_hadIntChildPhi[m_hadIntChildren];
	// rescale the deltaPhi
	deltaPhi -= deltaPhi > M_PI ? M_PI : 0.;
	deltaPhi += deltaPhi < -M_PI ? M_PI : 0.;		 
	m_hadIntChildDeltaPhi[m_hadIntChildren] = deltaPhi;
	m_hadIntChildDeltaEta[m_hadIntChildren] = m_hadIntMotherEta - m_hadIntChildEta[m_hadIntChildren];
	++m_hadIntChildren;
      }      
      
      if (childP.mag()> m_minimumHadOutEnergy) {
	// get the new particle    
	double mass = s_particleMasses.mass[ childType[i] ];
	
	// create the particle
	ISF::ISFParticle *child = new ISF::ISFParticle ( vertex,
							 childP,
							 mass,
							 charge[i],
							 pdgid[i],
							 time, 
							 *parent );
	// in the validation mode, add process info
	if (m_validationMode) {
	  ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
	  validInfo->setProcess(m_processCode);
	  if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
	  else validInfo->setGeneration(1);     // assume parent is a primary track
	  child->setUserInformation(validInfo);
	}
	// record child for TruthIncident
	*childrenIt = child;
	++childrenIt; numChildren++;
      }
      
      gen_part++;
    }
  } // particle loop
  
  children.resize(numChildren);
  ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
			       children,
			       m_processCode,
			       parent->nextGeoID(),
			       ISF::fKillsPrimary );
  m_truthRecordSvc->registerTruthIncident( truth);

  // save info for validation
  if (m_validationMode && m_validationTool) {
    Amg::Vector3D* nMom = 0;
    m_validationTool->saveISFVertexInfo(m_processCode,vertex,*parent,p*particleDir,nMom,children);
    delete nMom;
  }

  
  m_hadIntChildE = eout;
  
  if (m_hadIntValidationTree) m_hadIntValidationTree->Fill();
  
  ATH_MSG_VERBOSE( "[ had ] it was kinematically possible to create " << gen_part << " shower particles " ); 
  
  return children;

} 

bool iFatras::HadIntProcessorParametric::doHadronicInteraction(double time, const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
							       const Trk::Material* /*ematprop*/,
							       Trk::ParticleHypothesis particle, bool processSecondaries) const {
  // get parent particle
  const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  assert(parent);

  ISF::ISFParticleVector ispVec=getHadState(parent, time, momentum.mag(), position, momentum.unit(), particle);

  // having no secondaries does not necessarily mean the interaction did not take place  : TODO : add flag into ::getHadState
  //  if (!ispVec.size()) return false;

  // push onto ParticleStack

  if (processSecondaries && ispVec.size() ) {
    for (unsigned int ic=0; ic<ispVec.size(); ic++) m_particleBroker->push(ispVec[ic], parent);
  }

  return true;

}

ISF::ISFParticleVector iFatras::HadIntProcessorParametric::doHadIntOnLayer(const ISF::ISFParticle* parent, double time,
									   const Amg::Vector3D& position, const Amg::Vector3D& momentum,
									   const Trk::Material* /*emat*/,
									   Trk::ParticleHypothesis particle) const {

  return getHadState(parent, time, momentum.mag(), position, momentum.unit(), particle);
  
}

/** interface for processing of the presampled nuclear interaction */                           
bool iFatras::HadIntProcessorParametric::recordHadState(double time, double p,
							const Amg::Vector3D& vertex,
							const Amg::Vector3D& particleDir,
							Trk::ParticleHypothesis particle ) const {
  // get parent particle
  const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  assert(parent);
  
  ISF::ISFParticleVector ispVec=getHadState(parent, time, p, vertex, particleDir, particle);
  
  // having no secondaries does not necessarily mean the interaction did not take place : TODO : add flag into ::getHadState
  //  if (!ispVec.size()) return false;
  
  // push onto ParticleStack
  if (ispVec.size() ) {
    for (unsigned int ic=0; ic<ispVec.size(); ic++) m_particleBroker->push(ispVec[ic], parent);
  }
  
  return true;
}

