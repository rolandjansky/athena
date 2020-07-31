/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// SUMMARY: This code implements a "particle decayer" to allow us to augment the standard 
// "particle gun" with correlated particles, such as those from a decay. We assume that 
// some previous code has generated a scalar or a boson according to some preferred angular and momentum 
// distributions using a "geantino" (PDG ID 999). ParticleDecayer will then find this input 
// geantino assigns a given mass and (using the user defined parameters provided) select a decay vertex location where 
// the "geantino" is forced to decay into a particle/antiparticle pair.
// The processes that the user can generate are 
// 
// s -> gd + gd
//      gd -> l + l
//
// or
//
// gd -> l + l 
//
//
// See the jobOption in share folder for more information.
//
 

#include "ParticleDecayer/ParticleDecayer.h"
#include "GeneratorObjects/McEventCollection.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//function to generate a lifetime (actually decay-length) according to the proper lifetime of the particle particle 
double ParticleDecayer::rnd_ExpLifetime(double ct) {
  //Get random number seeds from Atlas RNG service
  CLHEP::HepRandomEngine* engine = atRndmGenSvc().GetEngine("ParticleDecayer");
  double r = engine->flat(); //< Return random num in [0,1]
  return ((-ct)*(double)log((double)1.-r));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//function to generate a uniform distribution in (a,b)
double ParticleDecayer::rnd_DoubleRange(double a, double b) {
  //Get random number seeds from Atlas RNG service
  CLHEP::HepRandomEngine* engine = atRndmGenSvc().GetEngine("ParticleDecayer");
  //const long* seeds = engine->getSeeds();
  double r = engine->flat(); //< Return random num in [0,1]
  return a + r*(b-a);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//function to generate a cos theta valua according to angular distribution
double ParticleDecayer::cosgen(int itype){
  double x,fx,hit;
  x=0;	

  if(itype==0){  // flat
    x=rnd_DoubleRange(-1.,1.);
    return x;
  }
	
  if(itype==1){  // f(cost)=1.5*cost^2
    hit=1.5;
    fx=0;
     while(hit>fx){
       x=rnd_DoubleRange(-1.,1.);
       hit=rnd_DoubleRange(0.,1.5);
       fx=1.5*x*x;
     }
    return x;
  }

  if(itype==2){    // f(cost)=0.375(1+cost^2)
    hit=0.75;
    fx=0;
    while(hit>fx){
      x=rnd_DoubleRange(-1.,1.);
      hit=rnd_DoubleRange(0.,0.75);
      fx=0.375*(1+x*x);
    }
    return x;
  }

  if(itype==3){      // f(cost)=0.75(1-cost^2)
    hit=1.5;
    fx=0;
    while(hit>fx){
      x=rnd_DoubleRange(-1.,1.);
      hit=rnd_DoubleRange(0.,1.5);
      fx=0.75*(1-x*x);
    }
    return x;
  }

  return x;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode ParticleDecayer::changeMass( HepMC::GenParticlePtr genpart, double newMass )
{
   double e     = genpart->momentum().e(); 
   double theta = genpart->momentum().theta(); 
   double phi   = genpart->momentum().phi(); 
   //Sanity check 
   double p2 = e*e - newMass*newMass;
   if ( p2 < 0 ) {
      ATH_MSG_FATAL("ParticleDecayer::fillEvt:   -- you have generated a tachyon!");
      return StatusCode::FAILURE;
   }
   //At this point, we have e, theta, and phi.  Put them together to get the four-momentum.
   double p  = std::sqrt(p2);
   double px = p*std::sin(theta)*std::cos(phi);
   double py = p*std::sin(theta)*std::sin(phi);
   double pz = p*std::cos(theta);
   //Fill the four-momentum
   const CLHEP::HepLorentzVector updatedLV(px,py,pz,e);
   genpart->set_momentum(HepMC::FourVector(updatedLV.x(),updatedLV.y(),updatedLV.z(),updatedLV.e()));
   genpart->set_generated_mass(newMass);
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode ParticleDecayer::setDecayPosition( HepMC::GenParticlePtr genpart, HepMC::GenEvent* event, bool doScalarDecay )
{
   HepMC::GenVertexPtr vtxp = genpart->production_vertex();
   if(!vtxp) {
      ATH_MSG_FATAL("ParticleDecayer::fillEvt: -- no production vertex position found!");
      return StatusCode::FAILURE;
   }
   if ( doScalarDecay ) // scalar decay is prompt. decay vtx == scalar production vertex
      {
         HepMC::GenVertexPtr end_vtx = HepMC::newGenVertexPtr();
         end_vtx->set_position(vtxp->position());
         end_vtx->add_particle_in(genpart);
         event->add_vertex(end_vtx);
         return StatusCode::SUCCESS;
      }

   //lifetime handling
   double gamma = genpart->momentum().e()/genpart->momentum().m();
   double ctau  = 0.;
   double theta = genpart->momentum().theta();

   // Make sure theta is between 0 and pi
   while( theta > CLHEP::pi )
      {                                                                                                                
         theta -= CLHEP::pi;                                                                                           
      }                                                                                                                
   while( theta < 0 )                                                                                                  
      {                                                                                                                
         theta += CLHEP::pi;
      }

   if (m_doExponentialDecay)
      {
         if(m_expDecayDoVariableLifetime) // Variable proper lifetime distribution, such that fixed fraction of events decays within detector, independent of boost
            {
               double distanceToEdge = -999.;
               if ( theta < m_thetaEndCapBarrel || theta > ( CLHEP::pi - m_thetaEndCapBarrel) ) // Particle escapes through endcap
                  {
                     distanceToEdge = std::abs(m_endCapDistance/cos(theta));
                  }
               else // Particle escapes through barrel
                  {
                     distanceToEdge = m_barrelRadius/sin(theta);
                  }
               if ( gamma < 1. )
                  {
                     ATH_MSG_FATAL("Gamma factor cannot be smaller than 1" );
                     return StatusCode::FAILURE;
                  }
               double Limit  = distanceToEdge / gamma; // ctau is enhanced by factor gamma in lab frame
               double lambda = -1.*Limit/log(1. - m_expDecayFractionToKeep);
               ctau = rnd_ExpLifetime(lambda);
               if (m_expDecayDoTruncateLongDecays)
                  {
                     while ( ctau > Limit )  // If decay is outside detector, let's sample again (Produces a truncated exponential)
                        {
                           ctau = rnd_ExpLifetime(lambda);
                        }
                  }
            }
         else if( m_particleLifeTime > 0 ) // Fixed proper lifetime distribution. Note that if m_particleLifeTime <= 0. ctau remains 0
            {
               ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- set lifetime of the dark photon according to ctau = " << m_particleLifeTime);
               ctau = rnd_ExpLifetime(m_particleLifeTime);
            }
      }else if (m_doUniformDecay)
      {         
         double decayRadius = -999.;
         if ( theta < m_thetaEndCapBarrel || theta > ( CLHEP::pi - m_thetaEndCapBarrel) ) // Particle escapes through endcap
            {
               double outerLength = std::abs(m_endCapDistance/cos(theta));
               double outerRadius = outerLength*sin(theta);
               decayRadius        = rnd_DoubleRange(0., std::min(outerRadius, std::abs(m_barrelRadius)) );
            }else // Particle escapes through barrel
            {
               decayRadius = rnd_DoubleRange(0., std::abs(m_barrelRadius));
            }

         double decayLength = decayRadius/sin(theta);
         ctau = decayLength/gamma;
      }else
      {
         ATH_MSG_FATAL("have to pick uniform or exponential decay distance");
         return StatusCode::FAILURE;
      }
   
   double ctg = gamma * ctau;
   double px = genpart->momentum().px();
   double py = genpart->momentum().py();
   double pz = genpart->momentum().pz();
   double p  = sqrt(px*px + py*py + pz*pz);

   const CLHEP::HepLorentzVector posLV(((ctg*px/p)+(vtxp->position().x())), ((ctg*py/p)+(vtxp->position().y())), ((ctg*pz/p)+(vtxp->position().z())), ((ctg)+(vtxp->position().t())));

   //set the decay vertex position of the particle
   //Create a HepMC vertex at the decay position of the particle 
   ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- set the decay vertex");
   HepMC::GenVertexPtr end_vtx = HepMC::newGenVertexPtr();
   end_vtx->set_position(HepMC::FourVector(posLV.x(),posLV.y(),posLV.z(),posLV.t()));
   end_vtx->add_particle_in(genpart);
   event->add_vertex(end_vtx);
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ParticleDecayer::ParticleDecayer(const std::string& name, ISvcLocator* pSvcLocator) : 
   GenModule(name, pSvcLocator),
   m_truthParticleContainerName("GEN_EVENT"),
   m_eventCounter(0)
{
  declareProperty("McEventCollection",     m_truthParticleContainerName);  
  declareProperty("LJType",                m_LJType = 1); //1 = one dark photon per LeptonJet, 2 = two dark photons per LeptonJet
  declareProperty("ScalarPDGID",           m_scalarPDGID); //new PDG ID of the scalar 
  declareProperty("ScalarMass",            m_scalarMass); //new mass of the scalar in MeV
  declareProperty("ParticleID",            m_particleID = 999); //PDG ID of the geantino
  declareProperty("ParticleMass",          m_particleMass = 400); //new mass of the dark photon in MeV (default 400 MeV)
  declareProperty("ParticleLifeTime",      m_particleLifeTime = 0); //ctau of the dark photon in mm (default prompt)
  declareProperty("ParticlePolarization",  m_particlePolarization = 0); //polarization of the dark photon (default 0 : isotropic decay, -1 : transverse, 1 : longitudinal)
  declareProperty("OppositePolarization",  m_oppositePolarization = false);//In case of LJType == 2 and opposite polarization for the two dark photons in the event
  declareProperty("ParticlePDGID",         m_particlePDGID = 700022); //new PDG ID of the dark photon (default 700022)
  declareProperty("DecayBRElectrons",      m_BRElectron = 1.); //BR of dark photon decay to electrons  
  declareProperty("DecayBRMuons",          m_BRMuon = 0.); //BR of dark photon decay to muons 
  declareProperty("DecayBRPions",          m_BRPion = 0.); //BR of dark photon decay to pions 

  //Choose between the different types of displaced decays
  declareProperty("DoUniformDecay",               m_doUniformDecay = false);
  declareProperty("DoExponentialDecay",           m_doExponentialDecay = true );
  declareProperty("ExpDecayDoVariableLifetime",   m_expDecayDoVariableLifetime   = false );
  declareProperty("ExpDecayPercentageToKeep",     m_expDecayFractionToKeep       = 0.8   );
  declareProperty("ExpDecayDoTruncateLongDecays", m_expDecayDoTruncateLongDecays = false );

  //Dimensions within which to decay in case of non-prompt events
  declareProperty("BarrelRadius",                 m_barrelRadius         = 10.e3 );      // outer limit for decay radius
  declareProperty("EndCapDistance",               m_endCapDistance       = 15.e3 );      // outer limit along z for decays in endcap
  declareProperty("ThetaEndCapBarrel",            m_thetaEndCapBarrel    = 0.439067982); // theta if where to switch between barrel and endcap (default: eta = 1.5)
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StatusCode ParticleDecayer::genInitialize()
{
   // Check here if the tool has been configured correctly
   if(m_doUniformDecay && m_doExponentialDecay)
      {
         ATH_MSG_FATAL("Cannot configure exponential and uniform decay at the same time.");
         return StatusCode::FAILURE;
      }

   double TOTBR = m_BRElectron + m_BRMuon + m_BRPion;
   if (TOTBR>1.0000001) {
      ATH_MSG_FATAL("ParticleDecayer::genInitialize:   -- Branching Ratio sum is larger than 1!! Please check the values in your jobOption.");
      ATH_MSG_FATAL("ParticleDecayer::genInitialize:   -- Branching Ratio Electrons " << m_BRElectron);
      ATH_MSG_FATAL("ParticleDecayer::genInitialize:   -- Branching Ratio Muons     " << m_BRMuon);
      ATH_MSG_FATAL("ParticleDecayer::genInitialize:   -- Branching Ratio Pions     " << m_BRPion);
      ATH_MSG_FATAL("ParticleDecayer::genInitialize:   -- Total Branching Ratio     " << TOTBR);
      return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- daughters randomly produced: " << m_BRElectron << "% e, " << m_BRMuon << "% mu, " << m_BRPion << "% pi");

   if( m_LJType != 1 && m_LJType != 2 )
      {
         ATH_MSG_FATAL("LJType is set to " << m_LJType);
         ATH_MSG_FATAL("LJType has to be set to either 1 or 2");
         return StatusCode::FAILURE;
      }
   

   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ParticleDecayer::~ParticleDecayer() {}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode ParticleDecayer::fillEvt(HepMC::GenEvent* event) {

  ATH_MSG_DEBUG("ParticleDecayer::fillEvt: -- EventCounter: " << m_eventCounter);

  StatusCode status = StatusCode::SUCCESS;

  // Extract the event from the TES
  McEventCollection* mcEvtColl;
  if (evtStore()->contains<McEventCollection>(m_truthParticleContainerName) && evtStore()->retrieve(mcEvtColl, m_truthParticleContainerName).isSuccess() ) {
    ATH_MSG_DEBUG("ParticleDecayer::fillEvt: -- Found an McEventCollection for ParticleDecayer");
  } else {
    ATH_MSG_FATAL("TES is empty!!!!");
    status = StatusCode::FAILURE;  
    return status;  
  }

  event = mcEvtColl->back();

  if (event == NULL) {
    ATH_MSG_FATAL("ParticleDecayer::fillEvt: -- McEvent was not successfully created");
    status = StatusCode::FAILURE;  
    return status;  
  }
  
  for ( auto  genpart : *event) { 

     //////////////////////////////////////////////
     //    only one dark photon per LeptonJet    //
     //////////////////////////////////////////////
     if (m_LJType == 1) {
        if (genpart->pdg_id()==m_particleID) { //if geantino assign the new mass and cross-check
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt: -- only one dark photon per LeptonJet");
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- found MC particle with PDG ID = " << genpart->pdg_id());
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- assign the new mass of the dark photon, m = " << m_particleMass);

           //Change the mass of the parent particle ( set by user input + command )
           //Changes the magnitude of the spatial part of the 4-momentum such that the new 4-momentum has the desired inv mass
           CHECK( changeMass( genpart, m_particleMass ) ); 

           //Add decay position to the event
           CHECK( setDecayPosition( genpart, event ) );

           //assign the new PDG_ID of the particle
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- set the new PDG ID =  " << m_particlePDGID);
           genpart->set_pdg_id(m_particlePDGID);  
           
           //set the new status (decayed) of the particle
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- set the new status = 2");
           genpart->set_status(2); 
           
           //set the new momentum of the particle
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- set the new momentum");
           
           ///*** Now allow the two-body decay of the particle
           CHECK( DFTwoBodyDecay( genpart, m_particlePolarization ) );

        }
     }else if (m_LJType == 2) {
        /////////////////////////////////////////////
        //      two dark photons per LeptonJet     //
        /////////////////////////////////////////////
        if (genpart->pdg_id()==m_particleID) {

           ATH_MSG_DEBUG("ParticleDecayer::fillEvt: -- two dark photons per LeptonJet");
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- found MC particle with PDG ID = " << genpart->pdg_id());
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- assign the new mass of the dark scalar, m = " << m_scalarMass);

           //Get the mass of the parent particle ( set by user input + command )
           //Change the mass of the parent particle ( set by user input + command )
           //Changes the magnitude of the spatial part of the 4-momentum such that the new 4-momentum has the desired inv mass
           CHECK( changeMass( genpart, m_scalarMass ) ); 

           //set the new PDG_ID of the scalar
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- set the new PDG ID =  " << m_scalarPDGID);
           genpart->set_pdg_id(m_scalarPDGID);  

           //set the new status (decayed) of the scalar
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- set the new status = 2");
           genpart->set_status(2); 

           //set the decay vertex position of the scalar
           //Create a HepMC vertex at the decay position of the scalar 
           CHECK( setDecayPosition( genpart, event, true ) ); 
           
           ///*** Now allow the two-body decay of the scalar
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- allow the two-body decay of the dark scalar to dark photons...");
           ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- dark photon has PDG ID = " << m_particlePDGID);

           std::vector<CLHEP::HepLorentzVector> darkPhotonLVs; 
           CHECK( getDecayProducts( CLHEP::HepLorentzVector( genpart->momentum().px(), genpart->momentum().py(), genpart->momentum().pz(), genpart->momentum().e() ),
                                    m_particleMass,
                                    darkPhotonLVs) );

           //add dark photon 1
           auto v0=darkPhotonLVs.at(0).vect();
           addParticle( genpart->end_vertex(), m_particlePDGID, HepMC::FourVector(v0.x(),v0.y(),v0.z(),0.0) , 2);
           //add dark photon 2
           auto v1=darkPhotonLVs.at(1).vect();
           addParticle( genpart->end_vertex(), m_particlePDGID, HepMC::FourVector(v1.x(),v1.y(),v1.z(),0.0), 2);
           
           //lifetime handling of the dark photons
           int polarizationSwitch = 1;
#ifdef HEPMC3
           auto pItBegin = genpart->end_vertex()->particles_out().end();
           auto pItEnd = genpart->end_vertex()->particles_out().end();
#else
           HepMC::GenVertex::particles_out_const_iterator pItBegin    = genpart->end_vertex()->particles_out_const_begin();
           HepMC::GenVertex::particles_out_const_iterator pItEnd = genpart->end_vertex()->particles_out_const_end();
#endif
           for ( auto pIt=pItBegin ; pIt != pItEnd; ++pIt )
              {
                 //Add decay position to the event
                 CHECK( setDecayPosition( *pIt, event ) );
                 //And perform two-body decay
                 ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- Now allow the two-body decay of the dark photons");
                 CHECK( DFTwoBodyDecay( *pIt, polarizationSwitch*m_particlePolarization ) );
                 if(m_oppositePolarization)
                    {
                       polarizationSwitch = -polarizationSwitch;
                    }
              }

        }
     }else
        {
           ATH_MSG_FATAL("LJType set to " << m_LJType );
           ATH_MSG_FATAL("LJType must be 1 or 2" );
           return StatusCode::FAILURE;
        }
  }

  //set the event number
  event->set_event_number(m_eventCounter);
  
  //Add a unit entry to the event weight vector if it's currently empty
  if (event->weights().empty()) {
     event->weights().push_back(1.); 
  }
  
  //increment the event counter
  m_eventCounter++;
  
  return status;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleDecayer::addParticle(HepMC::GenVertexPtr prod_vtx, int pdg, HepMC::FourVector momentum, int statusCode) {

  double mass = 0.;
  if( pdg == m_particlePDGID)
     {
        mass = m_particleMass;
     }else
     {
        mass = getParticleMass(pdg);
     }
  double energy=std::sqrt(std::pow(momentum.x(),2)+std::pow(momentum.y(),2)+std::pow(momentum.z(),2)+mass*mass);   
HepMC::GenParticlePtr aParticle = HepMC::newGenParticlePtr (HepMC::FourVector(momentum.x(), momentum.y(), momentum.z(), energy), 
		                           pdg, statusCode);

  prod_vtx->add_particle_out(aParticle);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double ParticleDecayer::getParticleMass(int pid) {
  IPartPropSvc* partPropSvc = 0;
  StatusCode PartPropStatus = Gaudi::svcLocator()->service("PartPropSvc", partPropSvc);
  if (!PartPropStatus.isSuccess() || 0 == partPropSvc) throw GaudiException("PartPropSvc error", "I_ParticleDecayer", StatusCode::FAILURE);      
  m_particleTable = partPropSvc->PDT();

  const HepPDT::ParticleData* particle = m_particleTable->particle(HepPDT::ParticleID(abs(pid)));
  return particle->mass().value();
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode ParticleDecayer::DFTwoBodyDecay( HepMC::GenParticlePtr genpart, int Polarization ) {


   ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- allow the two-body decay of the dark photon...");

   //create heplorentzvector from genpart's fourMomentum, as it is more useful
   CLHEP::HepLorentzVector boostDF( genpart->momentum().px(), genpart->momentum().py(), genpart->momentum().pz(), genpart->momentum().e() );

   //Given branching fractions, pick decay mode
   int ModeOfDecay;
   double unif = rnd_DoubleRange(0.,1.);
   if (unif<=m_BRElectron) {
      ModeOfDecay = 11;
   } else if (unif<=(m_BRElectron+m_BRMuon) && unif>m_BRElectron) {
      ModeOfDecay = 13;
   } else {
      ModeOfDecay = 211;
   }
   ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- decayMode = " << ModeOfDecay);
   
   //Now that we have a decay mode. get the associated particle mass
   double decayPartMass = getParticleMass(ModeOfDecay);

   //Choose how to decay
   //angular distribution handling, see pag.6 of http://arxiv.org/pdf/hep-ph/0605296v2.pdf
   int type = 0; //isotropic by default
   if(Polarization==0) {
      type = 0; //isotropic
   } else if(Polarization==-1 && (ModeOfDecay==11 || ModeOfDecay==13)) {
      type = 2; //transverse polarization of the dark photon + fermions
   } else if(Polarization==1 && (ModeOfDecay==11 || ModeOfDecay==13)) {
      type = 3;  //longitudinal polarization of the dark photon + fermions
   } else if(Polarization==-1 && ModeOfDecay==211) {
      type = 3;  //transverse polarization of the dark photon + scalars
   } else if(Polarization==1 && ModeOfDecay==211) {
      type = 1;  //longitudinal polarization of the dark photon + scalars
   } else {
      ATH_MSG_FATAL("ParticleDecayer::fillEvt:   -- wrong polarization value... please check!");
      return StatusCode::FAILURE;
   }

   //Now we are ready to decay the dark photon
   std::vector<CLHEP::HepLorentzVector> daughterLVs; 
   CHECK( getDecayProducts( boostDF, decayPartMass, daughterLVs, type) );
   
   //Add the daughters to the pool file
   ATH_MSG_DEBUG("ParticleDecayer::fillEvt:   -- Add the daughters to the pool file");
   auto end_vtx = genpart->end_vertex();
   auto v0=daughterLVs.at(0).vect();
   addParticle(end_vtx,  ModeOfDecay, HepMC::FourVector(v0.x(),v0.y(),v0.z(),0.0),    1);
   auto v1=daughterLVs.at(1).vect();
   addParticle(end_vtx, -ModeOfDecay, HepMC::FourVector(v1.x(),v1.y(),v1.z(),0.0),    1);

   return StatusCode::SUCCESS;
}


StatusCode ParticleDecayer::getDecayProducts( CLHEP::HepLorentzVector parentLV,
                                              double decayPartMass,
                                              std::vector<CLHEP::HepLorentzVector>& daughterLVs,
                                              int decayType)
{
   double parentMass          = parentLV.m();
   CLHEP::Hep3Vector boostVec = parentLV.boostVector();

   if( decayPartMass > parentMass/2.)
      {
         ATH_MSG_FATAL("Decay particle has more than half the mass of parent.");
         return StatusCode::FAILURE;
      }
   //Get the angles in the rest frame
   double phi_rf   = rnd_DoubleRange(-CLHEP::pi, CLHEP::pi); 
   double ct_rf    = cosgen(decayType);
   double theta_rf = std::acos(ct_rf);

   //construct p1 particle momentum in rest-frame (_rf)
   double p1_rf = sqrt(parentMass*parentMass/4. - decayPartMass*decayPartMass);
   double px_rf = p1_rf*std::cos(phi_rf)*std::sin(theta_rf);
   double py_rf = p1_rf*std::sin(phi_rf)*std::sin(theta_rf);
   double pz_rf = p1_rf*ct_rf;
   CLHEP::HepLorentzVector hlv1(  px_rf,  py_rf,  pz_rf, parentMass/2.);
   CLHEP::HepLorentzVector hlv2( -px_rf, -py_rf, -pz_rf, parentMass/2.);

   //Transform from the rotated frame to the (x,y,z) frame.
   hlv1.rotateUz((parentLV.vect()).unit());
   hlv2.rotateUz((parentLV.vect()).unit());

   //Now boost back to the lab-frame
   hlv1.boost(boostVec);
   hlv2.boost(boostVec);

   daughterLVs.push_back(hlv1);
   daughterLVs.push_back(hlv2);

   return StatusCode::SUCCESS;
}
