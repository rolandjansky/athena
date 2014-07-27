/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File:  Generators/FlowAfterburnber/AddFlowByShifting.cxx
// Description:
//    This code is used to introduce particle flow
//    to particles from generated events
//
// AuthorList:
// Andrzej Olszewski: Initial Code February 2006
// 11.10.2006: Add predefined flow function by name

#include <set>
#include <cmath>

// For the Athena-based random numbers
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "FlowAfterburner/AddFlowByShifting.h"
#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "StoreGate/StoreGateSvc.h"
#include "GeneratorObjects/HijingEventParams.h"


double AddFlowByShifting::vn_func(double x, void *params)
{
   float *par_float = (float*) params;
   float phi_0  = par_float[0];
    float *vn    = par_float+1;
   float *psi_n = vn+6;
   double val=x   +2*(   vn[0]*sin(1*(x-psi_n[0]))/1.0 + vn[1]*sin(2*(x-psi_n[1]))/2.0 +
                         vn[2]*sin(3*(x-psi_n[2]))/3.0 + vn[3]*sin(4*(x-psi_n[3]))/4.0 +
                         vn[4]*sin(5*(x-psi_n[4]))/5.0 + vn[5]*sin(6*(x-psi_n[5]))/6.0 );
    return val-phi_0;
}

double AddFlowByShifting::vn_func_derivative(double x, void *params)
{
   float *par_float = (float*) params;
   //float phi_0  = par_float[0];
   float *vn    = par_float+1;
   float *psi_n = vn+6;
   double val=1   +2*(   vn[0]*cos(1*(x-psi_n[0]))/1.0 + vn[1]*cos(2*(x-psi_n[1]))/2.0 +
                         vn[2]*cos(3*(x-psi_n[2]))/3.0 + vn[3]*cos(4*(x-psi_n[3]))/4.0 +
                         vn[4]*cos(5*(x-psi_n[4]))/5.0 + vn[5]*cos(6*(x-psi_n[5]))/6.0 );
   return val;
}



AddFlowByShifting::AddFlowByShifting(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  // Set users' request
  declareProperty("McTruthKey",     m_inkey="GEN_EVENT");
  declareProperty("McFlowKey",      m_outkey="FLOW_EVENT");

  declareProperty("FlowFunctionName"  , m_flow_function_name="jjia_minbias_new");
  declareProperty("FlowImplementation", m_flow_implementation="exact"          );

  declareProperty("RandomizePhi"      , m_ranphi_sw  =0                        );

  declareProperty("FlowEtaSwitch"     , m_floweta_sw =0                        );
  declareProperty("FlowMinEtaCut"     , m_flow_mineta=0                        );
  declareProperty("FlowMaxEtaCut"     , m_flow_maxeta=10.0                     );

  declareProperty("FlowPtSwitch"      , m_flowpt_sw  =0                        );
  declareProperty("FlowMinPtCut"      , m_flow_minpt =0                        );
  declareProperty("FlowMaxPtCut"      , m_flow_maxpt =1000000                  );

  declareProperty("custom_v1"         , m_custom_v1 =0                         );
  declareProperty("custom_v2"         , m_custom_v2 =0                         );
  declareProperty("custom_v3"         , m_custom_v3 =0                         );
  declareProperty("custom_v4"         , m_custom_v4 =0                         );
  declareProperty("custom_v5"         , m_custom_v5 =0                         );
  declareProperty("custom_v6"         , m_custom_v6 =0                         );
  declareProperty("FlowBSwitch"       , m_flowb_sw  =0                         );//currently not used

  m_sgSvc = 0;
  p_AtRndmGenSvc = 0;
  p_engine = 0;

  m_flow_function_index = 0;
  m_flow_implementation_type = 0;
  m_particles_processed = 0;

  for(int i = 0; i< 6; i++)
    psi_n[i] = 0.f;

  v1 = 0.f;
  v2 = 0.f;
  v3 = 0.f;
  v4 = 0.f;
  v5 = 0.f;
  v6 = 0.f;
}




StatusCode AddFlowByShifting::initialize(){
  ATH_MSG_INFO(">>> AddFlowByShifting from Initialize <<<");

  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find StoreGateSvc");
    return sc;
  }

  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc", p_AtRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == p_AtRndmGenSvc){
      ATH_MSG_ERROR("Could not initialize Random Number Service");
      return RndmStatus;
  }

  // getting our random numbers stream
  p_engine  =  p_AtRndmGenSvc->GetEngine("FLOW");

  ATH_MSG_INFO("**********Settings for Afterburner************");
  ATH_MSG_INFO("McTruthKey         : " << m_inkey              );
  ATH_MSG_INFO("McFlowKey          : " << m_outkey             );

  ATH_MSG_INFO("FlowFunctionName   : " << m_flow_function_name );
  ATH_MSG_INFO("FlowInplementation : " << m_flow_implementation);

  ATH_MSG_INFO("RandomizePhi       : " << m_ranphi_sw          );

  ATH_MSG_INFO("FlowEtaSwitch      : " << m_floweta_sw         );
  ATH_MSG_INFO("FlowMinEtaCut      : " << m_flow_mineta        );
  ATH_MSG_INFO("FlowMaxEtaCut      : " << m_flow_maxeta        );

  ATH_MSG_INFO("FlowPtSwitch       : " << m_flowpt_sw          );
  ATH_MSG_INFO("FlowMinPtCut       : " << m_flow_minpt         );
  ATH_MSG_INFO("FlowMaxPtCut       : " << m_flow_maxpt         );

  ATH_MSG_INFO("FlowV1             : " << m_custom_v1          );
  ATH_MSG_INFO("FlowV2             : " << m_custom_v2          );
  ATH_MSG_INFO("FlowV3             : " << m_custom_v3          );
  ATH_MSG_INFO("FlowV4             : " << m_custom_v4          );
  ATH_MSG_INFO("FlowV5             : " << m_custom_v5          );
  ATH_MSG_INFO("FlowV6             : " << m_custom_v6          );
  ATH_MSG_INFO("FlowBSwitch        : " << m_flowb_sw           );
  ATH_MSG_INFO("********************************r*************");


  // The "int" version of the function-choice variable
  // int comparisons are quicker than string comparisons, and will speed the program
  m_flow_function_index=0;
  if     (m_flow_function_name=="jjia_minbias_new"       ) m_flow_function_index=0;
  else if(m_flow_function_name=="jjia_minbias_new_v2only") m_flow_function_index=1;
  else if(m_flow_function_name=="fixed_vn"               ) m_flow_function_index=2;
  else if(m_flow_function_name=="fixed_v2"               ) m_flow_function_index=3;
  else if(m_flow_function_name=="jjia_minbias_old"       ) m_flow_function_index=4;
  else if(m_flow_function_name=="ao_test"                ) m_flow_function_index=5;
  else if(m_flow_function_name=="custom"                 ) m_flow_function_index=6;
  else if(m_flow_function_name=="p_Pb_cent_eta_indep"    ) m_flow_function_index=7;

  m_flow_implementation_type=1;
  if(m_flow_implementation=="approximate") m_flow_implementation_type=0;
  if(m_flow_implementation=="exact"      ) m_flow_implementation_type=1;
  // Initialization terminated
  return StatusCode::SUCCESS;

}









StatusCode AddFlowByShifting::execute() {
  ATH_MSG_INFO(">>> AddFlowByShifting from execute");

 // Get hijing event parameters
  const HijingEventParams *hijing_pars;
  if( m_sgSvc->retrieve(hijing_pars, "Hijing_event_params").isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve Hijing_event_params");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Event parameters: B = " << hijing_pars->get_b()<<
                "  BPhi = " << hijing_pars->get_bphi() );


  // Read Data from Transient Store
  const McEventCollection* mcCollptr;
  if ( m_sgSvc->retrieve(mcCollptr, m_inkey).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve truth McEventCollection");
    return StatusCode::FAILURE;
  }


  // Loop over all events in original McEventCollection and
  // Copy to a new (modifiable) collection
  McEventCollection::const_iterator citr;
  McEventCollection* mcFlowCollptr = new  McEventCollection();
  for (citr = mcCollptr->begin(); citr!=mcCollptr->end(); ++citr) {
    mcFlowCollptr->push_back(new HepMC::GenEvent(*(*citr)));
  }


  //Geneate the reaction plane angles (some of them may or may not be used later on)
  //Store the angles into the hijing event parameters
  for(int ihar=0;ihar<6;ihar++){
    psi_n[ihar] =(CLHEP::RandFlat::shoot(p_engine)-0.5)*2*M_PI / (ihar+1);   //Principal value must be within -PI/n to PI/n
    (*hijing_pars).set_psi(ihar+1,psi_n[ihar]);
  }
  psi_n[1]=hijing_pars->get_bphi()                   ;//the psi2 plane is aligned with the impact parameter
  psi_n[1]=atan2(sin(2*psi_n[1]),cos(2*psi_n[1]))/2.0;//ensure that Psi2 is within [-PI/2,PI/2]
  (*hijing_pars).set_psi(2,psi_n[1]);
  ATH_MSG_DEBUG(" Psi2 for event : "<<(*hijing_pars).get_psi(2));


  // Add flow by phi angle shifting
  McEventCollection::iterator itr;
  for (itr = mcFlowCollptr->begin(); itr!=mcFlowCollptr->end(); ++itr) {
    ATH_MSG_DEBUG("Next event in the bag ...");

    //int g_id = (*itr)->signal_process_id();
    //GeneratorName_print(g_id);
    //std::cout << std::endl;

    HepMC::GenVertex* mainvtx =(*itr)->barcode_to_vertex(-1);
    int particles_in_event = (*itr)->particles_size();
    m_particles_processed = 0;
    for ( HepMC::GenVertex::particle_iterator partit =
	    (*mainvtx).particles_begin(HepMC::children);
	  partit != (*mainvtx).particles_end(HepMC::children); partit++ ) {

      // Process particles from main vertex
      HepMC::GenParticle* parent = (*partit);
      CLHEP::HepLorentzVector momentum(parent->momentum().px(),
				parent->momentum().py(),
				parent->momentum().pz(),
				parent->momentum().e());
      ATH_MSG_DEBUG("Parent particle: " << parent->barcode()        <<
                    " PID = "           << parent->pdg_id()         <<
                    " Status = "        << parent->status()         <<
                    " Eta = "           << momentum.pseudoRapidity()<<
                    " Phi = "           << momentum.phi()            );

      //skip particle if eta is outside implementation range
      if(m_floweta_sw){
        float eta=fabs(momentum.pseudoRapidity());
        if (eta<m_flow_mineta || eta> m_flow_maxeta) continue;
      }

      //skip particle if pT is outside implementation range
      if(m_flowpt_sw){
        float pT=momentum.perp();
        if (pT<m_flow_minpt || pT> m_flow_maxpt) continue;
      }

      // Randomize phi if explicitely requested
      if(m_ranphi_sw) {
	double phishift = SetParentToRanPhi(parent);
	MoveDescendantsToParent(parent, phishift)  ;// adjust decsandants to parent position
      }

      // Add flow to particles from main vertex
      double phishift = AddFlowToParent(parent, hijing_pars);
      MoveDescendantsToParent(parent, phishift);// adjust decsandants to parent position
    }

    // correct for double counting
    if(m_ranphi_sw) m_particles_processed /= 2;
    // correct for incoming particles
    ATH_MSG_INFO( " Particles in event: " << particles_in_event <<
                  " Processed for flow: " << m_particles_processed+2);
  }

  if(m_sgSvc->record(mcFlowCollptr, m_outkey).isFailure()){
    ATH_MSG_ERROR("Could not record flow McEventCollection");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}



StatusCode AddFlowByShifting::finalize() {
  ATH_MSG_INFO(">>> AddFlowByShifting from finalize <<<");
  // End of finalization step
  return StatusCode::SUCCESS;
}





double AddFlowByShifting::SetParentToRanPhi(HepMC::GenParticle* parent)
{
  // Set particle to random phi
  // Return phi shift
  m_particles_processed++;

  double phi, phishift;
  CLHEP::HepLorentzVector momentum(parent->momentum().px(),
			    parent->momentum().py(),
			    parent->momentum().pz(),
			    parent->momentum().e());
  phi = momentum.phi();

  double rannum = CLHEP::RandFlat::shoot(p_engine);
  double ranphi = (rannum-0.5)*2*M_PI;
  phishift = ranphi - phi;

  momentum.setPhi(ranphi*Gaudi::Units::rad);
  parent->set_momentum( momentum );

  ATH_MSG_INFO("Parent phi randomized = " << momentum.phi());

  return phishift;
}





void AddFlowByShifting::MoveDescendantsToParent
                          (HepMC::GenParticle* parent, double phishift)
{
  // Move the branch of descendant vertices and particles
  // by phishift to parent particle position
  HepMC::GenVertex *endvtx = parent->end_vertex();
  if ( endvtx ) {
    ATH_MSG_DEBUG("Processing branch of parent particle "<< parent->barcode());

    // now rotate descendant vertices
    for ( HepMC::GenVertex::vertex_iterator
	    descvtxit = endvtx->vertices_begin(HepMC::descendants);
	  descvtxit != endvtx->vertices_end(HepMC::descendants);
	  ++descvtxit) {
      HepMC::GenVertex *descvtx = (*descvtxit);
      ATH_MSG_DEBUG("Processing vertex " << descvtx->barcode());

      // rotate vertex
      if(fabs(phishift) > 1e-7) {
	CLHEP::HepLorentzVector position(descvtx->position().x(),
				  descvtx->position().y(),
				  descvtx->position().z(),
				  descvtx->position().t());
	position.rotateZ(phishift*Gaudi::Units::rad);
	descvtx->set_position( position );
      }

      // now rotate their associated particles
      for ( HepMC::GenVertex::particle_iterator descpartit
	    = descvtx->particles_begin(HepMC::children);
	  descpartit != descvtx->particles_end(HepMC::children);
	  ++descpartit ) {
	HepMC::GenParticle* descpart = (*descpartit);
        CLHEP::HepLorentzVector momentum(descpart->momentum().px(),
				  descpart->momentum().py(),
				  descpart->momentum().pz(),
				  descpart->momentum().e());
	ATH_MSG_DEBUG("Descendant particle: " << descpart->barcode()<<
                      " PID = "               << descpart->pdg_id() <<
                      " Status = "            << descpart->status() <<
                      " Eta = "               << momentum.pseudoRapidity() <<
                      " Phi = "               << momentum.phi() );

	m_particles_processed++;
	// rotate particle
	if(fabs(phishift) > 1e-7) {
	  momentum.rotateZ(phishift*Gaudi::Units::rad);
	  descpart->set_momentum( momentum );
   	  ATH_MSG_DEBUG(" Phi shift =   " << phishift<<
                        " Phi shifted = " << momentum.phi());
	}
      }
    }
  }

  return;
}












double AddFlowByShifting::AddFlowToParent (HepMC::GenParticle* parent, const HijingEventParams *hijing_pars)
{
  m_particles_processed++;

  CLHEP::HepLorentzVector momentum(parent->momentum().px(),
			    parent->momentum().py(),
			    parent->momentum().pz(),
			    parent->momentum().e());
  double pt    = momentum.perp();
  double eta   = momentum.pseudoRapidity();
  double phi_0 = momentum.phi();

  int error_=0;
  if(pt    !=pt)    {ATH_MSG_ERROR("ERROR pt  of track  is not defined");error_=1;} //true if pt==nan
  if(eta   !=eta)   {ATH_MSG_ERROR("ERROR eta of track  is not defined");error_=1;}
  if(phi_0 !=phi_0) {ATH_MSG_ERROR("ERROR phi of track  is not defined");error_=1;}
  if(error_==1){
    ATH_MSG_ERROR("Original Particle Momentum(px,py,pz,e,m)="<<parent->momentum().px()<<"  "
                                                             <<parent->momentum().py()<<"  "
                                                             <<parent->momentum().pz()<<"  "
                                                             <<parent->momentum().e() <<"  "
                                                             <<parent->momentum().m() <<"  ");
  }



  double b = hijing_pars->get_b();
  //double bphi = hijing_pars->get_bphi();


  v1=0,v2=0,v3=0,v4=0,v5=0,v6=0;


  //Call the appropriate function to set the vn values
  if      (m_flow_function_index==0)  jjia_minbias_new        (b,eta,pt);
  else if (m_flow_function_index==1)  jjia_minbias_new_v2only (b,eta,pt);
  else if (m_flow_function_index==2)  fixed_vn                (b,eta,pt);
  else if (m_flow_function_index==3)  fixed_v2                (b,eta,pt);
  else if (m_flow_function_index==4)  jjia_minbias_old        (b,eta,pt);
  else if (m_flow_function_index==5)  ao_test                 (b,eta,pt);
  else if (m_flow_function_index==6)  custom_vn               (b,eta,pt);
  else if (m_flow_function_index==7)  p_Pb_cent_eta_indep     (b,eta,pt);


  double phishift=0;

  // Old fashioned rotation(approximate)
  if (m_flow_implementation_type==0){
     float phi=phi_0;
     phishift=  -2*( v1*sin(1*(phi-psi_n[0]))/1.0 + v2*sin(2*(phi-psi_n[1]))/2.0 +
                     v3*sin(3*(phi-psi_n[2]))/3.0 + v4*sin(4*(phi-psi_n[3]))/4.0 +
                     v5*sin(5*(phi-psi_n[4]))/5.0 + v6*sin(6*(phi-psi_n[5]))/6.0 );

  }

  // New fashioned rotation(exact)
  else if (m_flow_implementation_type==1){
    const gsl_root_fsolver_type *T = gsl_root_fsolver_brent;
    gsl_root_fsolver *s = gsl_root_fsolver_alloc (T);
    double x_lo=-2*M_PI,x_hi=2*M_PI;
    float params[13];
    for(int ipar=0;ipar<13;ipar++) {params[ipar]=0;}
    gsl_function F;
    F.function = &(AddFlowByShifting::vn_func);
    F.params =&params;
    gsl_root_fsolver_set (s, &F, x_lo, x_hi);
    int iter=0;
    params[0]=phi_0;
    params[1]=v1; params[2]=v2; params[3]=v3; params[4]=v4; params[5]=v5;  params[6]=v6;
    params[7]=psi_n[0]; params[8]=psi_n[1]; params[9]=psi_n[2]; params[10]=psi_n[3]; params[11]=psi_n[4];  params[12]=psi_n[5];
    int status;
    double phi=phi_0;
    do
    {
       iter++;
       status = gsl_root_fsolver_iterate (s);
       phi = gsl_root_fsolver_root (s);
       x_lo = gsl_root_fsolver_x_lower (s);
       x_hi = gsl_root_fsolver_x_upper (s);
       status = gsl_root_test_interval (x_lo, x_hi,0, 0.00001);
    }
    while (status == GSL_CONTINUE && iter < 1000);
    gsl_root_fsolver_free (s);

    if (iter>=1000) return 0;

 /*
    float val=phi   +2*( v1*sin(1*(phi-psi_n[0]))/1.0 + v2*sin(2*(phi-psi_n[1]))/2.0 +
                         v3*sin(3*(phi-psi_n[2]))/3.0 + v4*sin(4*(phi-psi_n[3]))/4.0 +
                         v5*sin(5*(phi-psi_n[4]))/5.0 + v6*sin(6*(phi-psi_n[5]))/6.0 );
    std::cout<<phi_0<<"  "<<val<<"  "<<phi_0-val<<std::endl;  // */
    phishift = phi-phi_0;
  }

  if(fabs(phishift) > 1e-7) {
    momentum.rotateZ(phishift*Gaudi::Units::rad);
    parent->set_momentum( momentum );
  }
  ATH_MSG_DEBUG( "Parent particle: V1 = " << v1 <<
                                 " V2 = " << v2 <<
                                 " V3 = " << v3 <<
                                 " V4 = " << v4 <<
                                 " V5 = " << v5 <<
                                 " V6 = " << v6 <<
                          " Phi shift = " << phishift <<
                        " Phi shifted = " << momentum.phi() );

  return phishift;
}



// New parameterization for vn
void AddFlowByShifting::jjia_minbias_new(double b, double eta, double pt)
{
  pt=pt/1000.0; //convert to GeV

  float a1,a2,a3,a4;
  a1=0.4397*exp(-(b-4.526)*(b-4.526)/72.0) + 0.636;
  a2=1.916/(b+2) +0.1;
  a3=4.79*0.0001*(b-0.621)*(b-10.172)*(b-23)+1.2;   // this is >0 for b>0
  a4=0.135*exp(-0.5*(b-10.855)*(b-10.855)/4.607/4.607) +0.0120;

  float temp1 = pow(pt    , a1) / (1+exp( (pt-3.0)/a3));
  float temp2 = pow(pt+0.1,-a2) / (1+exp(-(pt-4.5)/a3));
  float temp3 =  0.01           / (1+exp(-(pt-4.5)/a3));

  v2 = ( a4*(temp1+temp2) + temp3 )* exp(-0.5* eta*eta /6.27/6.27) ;

  float fb=0.97 +1.06*exp(-0.5*b*b/3.2/3.2);
  v3=pow(fb*sqrt(v2),3);

  float gb= 1.096 +1.36 *exp(-0.5*b*b/3.0/3.0);
  gb=gb*sqrt(v2);
  v4=pow(gb,4);
  v5=pow(gb,5);
  v6=pow(gb,6);
  v1=0;
}


// New parameterization for vn
void AddFlowByShifting::jjia_minbias_new_v2only(double b, double eta, double pt)
{
  pt=pt/1000.0; //convert to GeV

  float a1,a2,a3,a4;
  a1=0.4397*exp(-(b-4.526)*(b-4.526)/72.0) + 0.636;
  a2=1.916/(b+2) +0.1;
  a3=4.79*0.0001*(b-0.621)*(b-10.172)*(b-23)+1.2;   // this is >0 for b>0
  a4=0.135*exp(-0.5*(b-10.855)*(b-10.855)/4.607/4.607) +0.0120;

  float temp1 = pow(pt    , a1) / (1+exp( (pt-3.0)/a3));
  float temp2 = pow(pt+0.1,-a2) / (1+exp(-(pt-4.5)/a3));
  float temp3 =  0.01           / (1+exp(-(pt-4.5)/a3));

  v2 = ( a4*(temp1+temp2) + temp3 )* exp(-0.5* eta*eta /6.27/6.27) ;

  v1=0;
  v3=0;
  v4=0;
  v5=0;
  v6=0;
}


// Fixed vn
void AddFlowByShifting::fixed_vn(double b, double eta, double pt)
{
  b=b;eta=eta;pt=pt   ;// to avoid compiler warnings
  v1=0.0000; v2=0.0500;
  v3=0.0280; v4=0.0130;
  v5=0.0045; v6=0.0015;
}


// Fixed 5% v2 (other vn=0)
void AddFlowByShifting::fixed_v2(double b, double eta, double pt)
{
  b=b;eta=eta;pt=pt   ; // to avoid compiler warnings
  v1=0.0000; v2=0.0500;
  v3=0.0000; v4=0.0000;
  v5=0.0000; v6=0.0000;
}

// Old parameterization for v2
void AddFlowByShifting::jjia_minbias_old(double b, double eta, double pt)
{
  v1 = 0;
  v2 = 0.03968 * b
               * (1 - 2.1/(1 + exp(1.357*(pt/1000))))
               * exp(-(eta*eta)/(2*6.37*6.37));
}


void AddFlowByShifting::ao_test (double b, double eta, double pt)
{
  pt /= 1000;
  if( pt > 2 ) pt = 2; // flat max at pt > 2
  eta = eta;  // to avoid compiler warnings
  v2 = 0.02 * b * pt;
  v1 = 0;
}


void AddFlowByShifting::custom_vn (double b, double eta, double pt)
{
  b=b;eta=eta;pt=pt   ; // to avoid compiler warnings
  v1=m_custom_v1;
  v2=m_custom_v2;
  v3=m_custom_v3;
  v4=m_custom_v4;
  v5=m_custom_v5;
  v6=m_custom_v6;
}

// p_Pb vn
void AddFlowByShifting::p_Pb_cent_eta_indep(double b, double eta, double pt)
{
  //static int counts=0;
  //if(counts<100){
  //  std::cout<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA  "<<counts<<std::endl;
  //  counts++;
  //}
  b=b;eta=eta;  //to avoid compiler warnings
  pt=pt/1000.0; //convert to GeV

  float an_val[4][3];

  an_val[0][0] = 0.1149;
  an_val[0][1] = 1.181;
  an_val[0][2] = 0.3767;

  an_val[1][0] = 0.0498;
  an_val[1][1] = 1.688;
  an_val[1][2] = 0.5046;

  an_val[2][0] = 0.02095;
  an_val[2][1] = 2.196;
  an_val[2][2] = 0.6259;

  an_val[3][0] = 0.00682*0.5;//added in 0.5 factor by hand
  an_val[3][1] = 4.938;
  an_val[3][2] = 1.237;

  v1=0;
  v2=an_val[0][0]*pow(pt,an_val[0][1])*exp(-an_val[0][2]*pt);
  v3=an_val[1][0]*pow(pt,an_val[1][1])*exp(-an_val[1][2]*pt);
  v4=an_val[2][0]*pow(pt,an_val[2][1])*exp(-an_val[2][2]*pt);
  v5=an_val[3][0]*pow(pt,an_val[3][1])*exp(-an_val[3][2]*pt);
  v6=0;
}
