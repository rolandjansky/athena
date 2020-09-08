/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "FlowAfterburner/AddFlowByShifting.h"
#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "StoreGate/StoreGateSvc.h"
#include "GeneratorObjects/HijingEventParams.h"

#include "TGraph.h"

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

  declareProperty("FlowFunctionName"  , m_flow_function_name ="jjia_minbias_new");
  declareProperty("FlowImplementation", m_flow_implementation="exact"           );
  declareProperty("FlowFluctuations"  , m_flow_fluctuations  =false             );

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

  m_flow_function= NULL;
  m_flow_implementation_type = 0;
  m_particles_processed = 0;

  m_graph_fluc=NULL;
  for(int ihar = 0; ihar< 6; ihar++){
    m_psi_n[ihar] =0.0;
    m_v_n  [ihar] =0.0;
    m_EbE_Multiplier_vn[ihar]=1.0;
  }
}




StatusCode AddFlowByShifting::initialize(){
  ATH_MSG_INFO(">>> AddFlowByShifting from Initialize <<<");

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
  ATH_MSG_INFO("FlowFluctuations   : " << m_flow_fluctuations  );

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


  // Select the flow-implementing function based of the function-choice variable
  m_flow_function=&AddFlowByShifting::jjia_minbias_new;
  if     (m_flow_function_name=="jjia_minbias_new"       ) m_flow_function=&AddFlowByShifting::jjia_minbias_new;
  else if(m_flow_function_name=="jjia_minbias_new_v2only") m_flow_function=&AddFlowByShifting::jjia_minbias_new_v2only;
  else if(m_flow_function_name=="fixed_vn"               ) m_flow_function=&AddFlowByShifting::fixed_vn;
  else if(m_flow_function_name=="fixed_v2"               ) m_flow_function=&AddFlowByShifting::fixed_v2;
  else if(m_flow_function_name=="jjia_minbias_old"       ) m_flow_function=&AddFlowByShifting::jjia_minbias_old;
  else if(m_flow_function_name=="ao_test"                ) m_flow_function=&AddFlowByShifting::ao_test;
  else if(m_flow_function_name=="custom"                 ) m_flow_function=&AddFlowByShifting::custom_vn;
  else if(m_flow_function_name=="p_Pb_cent_eta_indep"    ) m_flow_function=&AddFlowByShifting::p_Pb_cent_eta_indep;

  m_flow_implementation_type=1;
  if(m_flow_implementation=="approximate") m_flow_implementation_type=0;
  if(m_flow_implementation=="exact"      ) m_flow_implementation_type=1;



  //TGraph storing the v2_RP/delta Vs b_imp values to be used in implementing the EbyE fluctuations
  //the values below are b_imp-low,b_imp-high, delta/v2_RP for different centralities
  //underflow and overflow bins are added for smooth extrapolation
  //The delta/v2_RP values are taken from Fig15 of EbE vn paper (arXiv:1305.2942)
  //                 <0  ,  0-1 ,  1-2 , 2-3  ,  3-4 ,  4-5 , 5-10 , 10-15, 15-20, 20-25, 25-30,
  float b_lo[21]={  -1.00, 0.000, 1.483, 2.098, 2.569, 2.966, 3.317, 4.687, 5.739, 6.627, 7.409, 
                    8.117, 8.767, 9.373, 9.943,10.479,10.991,11.479,11.947,15.00 ,100.0};
  //                30-35, 35-40, 40-45, 45-50, 50-55, 55-60, 60-65, 65-70, 70-
  float b_hi[21]={ -1.00 , 1.483, 2.098, 2.569, 2.966, 3.317, 4.687, 5.739, 6.627, 7.409, 8.117,//bimp_high
                  8.767, 9.373, 9.943,10.479,10.991,11.479,11.947,12.399,15.00 ,100.0};
  float val [21]={  5.600, 5.600, 5.600,1.175 ,0.8253,0.7209,0.5324,0.4431,0.3984,0.3844,0.3847,
                 0.3935,0.4106,0.4310,0.4574,0.4674,0.4873,0.4796,0.4856,0.5130,0.5130};
  float bimp_vals[21];
  for(int i=0;i<21;i++) bimp_vals[i]=(b_lo[i]+b_hi[i])/2.0;
  if(m_flow_fluctuations) m_graph_fluc=new TGraph(21,bimp_vals,val);


  // Initialization terminated
  return StatusCode::SUCCESS;
}









StatusCode AddFlowByShifting::execute() {
  ATH_MSG_INFO(">>> AddFlowByShifting from execute");

 // Get hijing event parameters
  const HijingEventParams *hijing_pars;
  if( evtStore()->retrieve(hijing_pars, "Hijing_event_params").isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve Hijing_event_params");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Event parameters: B = " << hijing_pars->get_b()<<
                            "  BPhi = " << hijing_pars->get_bphi());


  // Read Data from Transient Store
  const McEventCollection* mcCollptr;
  if ( evtStore()->retrieve(mcCollptr, m_inkey).isFailure() ) {
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


  //Geneate the event-plane angles (some of them may or may not be used later on)
  //Store the angles into the hijing event parameters
  for(int ihar=0;ihar<6;ihar++){
    m_psi_n[ihar] =(CLHEP::RandFlat::shoot(p_engine)-0.5)*2*M_PI / (ihar+1);   //Principal value must be within -PI/n to PI/n
    (*hijing_pars).set_psi(ihar+1,m_psi_n[ihar]);
  }
  m_psi_n[1]=hijing_pars->get_bphi()                   ;//the psi2 plane is aligned with the impact parameter
  m_psi_n[1]=std::atan2(std::sin(2*m_psi_n[1]),std::cos(2*m_psi_n[1]))/2.0;//ensure that Psi2 is within [-PI/2,PI/2]
  (*hijing_pars).set_psi(2,m_psi_n[1]);
  ATH_MSG_DEBUG(" Psi2 for event : "<<(*hijing_pars).get_psi(2));


  // Add flow by phi angle shifting
  McEventCollection::iterator itr;
  for (itr = mcFlowCollptr->begin(); itr!=mcFlowCollptr->end(); ++itr) {
    ATH_MSG_DEBUG("Next event in the bag ...");



    auto mainvtx=HepMC::barcode_to_vertex(*itr,-1);
    if(m_flow_fluctuations) Set_EbE_Fluctuation_Multipliers(mainvtx,hijing_pars->get_b());

#ifdef HEPMC3
    int particles_in_event = (*itr)->particles().size();
    m_particles_processed = 0;
    for ( auto parent: mainvtx->particles_out()) {
#else
    int particles_in_event = (*itr)->particles_size();
    m_particles_processed = 0;
    for ( auto parent: *mainvtx) {
#endif

      // Process particles from main vertex
      CLHEP::HepLorentzVector momentum(parent->momentum().px(),
				parent->momentum().py(),
				parent->momentum().pz(),
				parent->momentum().e());
      ATH_MSG_DEBUG("Parent particle: " << HepMC::barcode(parent)        <<
                    " PID = "           << parent->pdg_id()         <<
                    " Status = "        << parent->status()         <<
                    " Eta = "           << momentum.pseudoRapidity()<<
                    " Phi = "           << momentum.phi()            );

      //skip particle if eta is outside implementation range
      if(m_floweta_sw){
        float eta=std::abs(momentum.pseudoRapidity());
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

  if(evtStore()->record(mcFlowCollptr, m_outkey).isFailure()){
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





double AddFlowByShifting::SetParentToRanPhi(HepMC::GenParticlePtr parent)
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
  parent->set_momentum(  HepMC::FourVector(momentum.px(),momentum.py(),momentum.pz(),momentum.e()) );

  ATH_MSG_INFO("Parent phi randomized = " << momentum.phi());

  return phishift;
}





void AddFlowByShifting::MoveDescendantsToParent
                          (HepMC::GenParticlePtr parent, double phishift)
{
  // Move the branch of descendant vertices and particles
  // by phishift to parent particle position
  auto endvtx = parent->end_vertex();
  if ( endvtx ) {
    ATH_MSG_DEBUG("Processing branch of parent particle "<< HepMC::barcode(parent));

    // now rotate descendant vertices
#ifdef HEPMC3
    for (HepMC::GenVertexPtr descvtx:  HepMC::descendant_vertices(endvtx)) {
#else
    for ( HepMC::GenVertex::vertex_iterator
	    descvtxit = endvtx->vertices_begin(HepMC::descendants);
	  descvtxit != endvtx->vertices_end(HepMC::descendants);
	  ++descvtxit) {
      auto descvtx = (*descvtxit);
#endif
      ATH_MSG_DEBUG("Processing vertex " << HepMC::barcode(descvtx));

      // rotate vertex
      if(std::abs(phishift) > 1e-7) {
	CLHEP::HepLorentzVector position(descvtx->position().x(),
				  descvtx->position().y(),
				  descvtx->position().z(),
				  descvtx->position().t());
	position.rotateZ(phishift*Gaudi::Units::rad);
	descvtx->set_position(HepMC::FourVector( position.x(),position.y(),position.z(),position.t()) );
      }

      // now rotate their associated particles
#ifdef HEPMC3
      for (auto descpart: descvtx->particles_out()){
#else
      for (auto descpart: *descvtx){
#endif
        CLHEP::HepLorentzVector momentum(descpart->momentum().px(),
				  descpart->momentum().py(),
				  descpart->momentum().pz(),
				  descpart->momentum().e());
	ATH_MSG_DEBUG("Descendant particle: " << HepMC::barcode(descpart)<<
                      " PID = "               << descpart->pdg_id() <<
                      " Status = "            << descpart->status() <<
                      " Eta = "               << momentum.pseudoRapidity() <<
                      " Phi = "               << momentum.phi() );

	m_particles_processed++;
	// rotate particle
	if(std::abs(phishift) > 1e-7) {
	  momentum.rotateZ(phishift*Gaudi::Units::rad);
	  descpart->set_momentum( HepMC::FourVector(momentum.px(),momentum.py(),momentum.pz(),momentum.e()) );
   	  ATH_MSG_DEBUG(" Phi shift =   " << phishift<<
                        " Phi shifted = " << momentum.phi());
	}
      }
    }
  }

  return;
}












double AddFlowByShifting::AddFlowToParent (HepMC::GenParticlePtr parent, const HijingEventParams *hijing_pars)
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




  //Call the appropriate function to set the vn values
  for(int ihar = 0; ihar< 6; ihar++){m_v_n  [ihar]=0.0;} //reset the vn for this particle
  double b = hijing_pars->get_b();
  (*this.*m_flow_function)(b,eta,pt);//get the vn for this particle
  if(m_flow_fluctuations){//add EbE fluctuations
    for(int ihar = 0; ihar< 6; ihar++){
      m_v_n[ihar] *= m_EbE_Multiplier_vn[ihar];
      if(m_v_n[ihar]>0.5) {ATH_MSG_WARNING(" Vn Too large  "<<ihar+1<<"  "<<m_EbE_Multiplier_vn[ihar]<<"  "<<m_v_n[ihar]);m_v_n[ihar]=0.5;}
    }
  }

  double phishift=0;

  // Old fashioned rotation(approximate)
  if (m_flow_implementation_type==0){
     float phi=phi_0;
     phishift=  -2*( m_v_n[0]*sin(1*(phi-m_psi_n[0]))/1.0 + m_v_n[1]*sin(2*(phi-m_psi_n[1]))/2.0 +
                     m_v_n[2]*sin(3*(phi-m_psi_n[2]))/3.0 + m_v_n[3]*sin(4*(phi-m_psi_n[3]))/4.0 +
                     m_v_n[4]*sin(5*(phi-m_psi_n[4]))/5.0 + m_v_n[5]*sin(6*(phi-m_psi_n[5]))/6.0 );

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
    params[1]=m_v_n  [0]; params[2]=m_v_n  [1]; params[3]=m_v_n  [2]; params[4 ]=m_v_n  [3]; params[5 ]=m_v_n  [4];  params[6 ]=m_v_n  [5];
    params[7]=m_psi_n[0]; params[8]=m_psi_n[1]; params[9]=m_psi_n[2]; params[10]=m_psi_n[3]; params[11]=m_psi_n[4];  params[12]=m_psi_n[5];
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

    phishift = phi-phi_0;
  }

  if(std::abs(phishift) > 1e-7) {
    momentum.rotateZ(phishift*Gaudi::Units::rad);
    parent->set_momentum( HepMC::FourVector(momentum.px(),momentum.py(),momentum.pz(),momentum.e()) );
  }
  ATH_MSG_DEBUG( "Parent particle: V1 = " << m_v_n[0] <<
                                 " V2 = " << m_v_n[1] <<
                                 " V3 = " << m_v_n[2] <<
                                 " V4 = " << m_v_n[3] <<
                                 " V5 = " << m_v_n[4] <<
                                 " V6 = " << m_v_n[5] <<
                          " Phi shift = " << phishift <<
                        " Phi shifted = " << momentum.phi() );

  return phishift;
}



// New parameterization for vn
void AddFlowByShifting::jjia_minbias_new(double b, double eta, double pt)
{
  pt=pt/1000.0; //convert to GeV

  float a1,a2,a3,a4;
  a1=0.4397*std::exp(-(b-4.526)*(b-4.526)/72.0) + 0.636;
  a2=1.916/(b+2) +0.1;
  a3=4.79*0.0001*(b-0.621)*(b-10.172)*(b-23)+1.2;   // this is >0 for b>0
  a4=0.135*std::exp(-0.5*(b-10.855)*(b-10.855)/4.607/4.607) +0.0120;

  float temp1 = std::pow(pt    , a1) / (1+std::exp( (pt-3.0)/a3));
  float temp2 = std::pow(pt+0.1,-a2) / (1+std::exp(-(pt-4.5)/a3));
  float temp3 =  0.01           / (1+std::exp(-(pt-4.5)/a3));

  m_v_n[1] = ( a4*(temp1+temp2) + temp3 )* std::exp(-0.5* eta*eta /6.27/6.27) ;

  float fb=0.97 +1.06*std::exp(-0.5*b*b/3.2/3.2);
  m_v_n[2]=std::pow(fb*std::sqrt(m_v_n[1]),3);

  float gb= 1.096 +1.36 *std::exp(-0.5*b*b/3.0/3.0);
  gb=gb*sqrt(m_v_n[1]);
  m_v_n[3]=pow(gb,4);
  m_v_n[4]=pow(gb,5);
  m_v_n[5]=pow(gb,6);
  m_v_n[0]=0;
}


// New parameterization for vn
void AddFlowByShifting::jjia_minbias_new_v2only(double b, double eta, double pt)
{
  pt=pt/1000.0; //convert to GeV

  float a1,a2,a3,a4;
  a1=0.4397*std::exp(-(b-4.526)*(b-4.526)/72.0) + 0.636;
  a2=1.916/(b+2) +0.1;
  a3=4.79*0.0001*(b-0.621)*(b-10.172)*(b-23)+1.2;   // this is >0 for b>0
  a4=0.135*std::exp(-0.5*(b-10.855)*(b-10.855)/4.607/4.607) +0.0120;

  float temp1 = std::pow(pt    , a1) / (1+std::exp( (pt-3.0)/a3));
  float temp2 = std::pow(pt+0.1,-a2) / (1+std::exp(-(pt-4.5)/a3));
  float temp3 =  0.01           / (1+std::exp(-(pt-4.5)/a3));

  m_v_n[1] = ( a4*(temp1+temp2) + temp3 )* std::exp(-0.5* eta*eta /6.27/6.27) ;

  m_v_n[0]=0;
  m_v_n[2]=0;
  m_v_n[3]=0;
  m_v_n[4]=0;
  m_v_n[5]=0;
}


// Fixed vn
void AddFlowByShifting::fixed_vn(double /*b*/, double /*eta*/, double /*pt*/)
{
  m_v_n[0]=0.0000; m_v_n[1]=0.0500;
  m_v_n[2]=0.0280; m_v_n[3]=0.0130;
  m_v_n[4]=0.0045; m_v_n[5]=0.0015;
}


// Fixed 5% v2 (other vn=0)
void AddFlowByShifting::fixed_v2(double /*b*/, double /*eta*/, double /*pt*/)
{
  m_v_n[0]=0.0000; m_v_n[1]=0.0500;
  m_v_n[2]=0.0000; m_v_n[3]=0.0000;
  m_v_n[4]=0.0000; m_v_n[5]=0.0000;
}

// Old parameterization for v2
void AddFlowByShifting::jjia_minbias_old(double b, double eta, double pt)
{
  m_v_n[0] = 0;
  m_v_n[1] = 0.03968 * b
               * (1 - 2.1/(1 + std::exp(1.357*(pt/1000))))
               * std::exp(-(eta*eta)/(2*6.37*6.37));
  m_v_n[2]=0.0000; m_v_n[3]=0.0000;
  m_v_n[4]=0.0000; m_v_n[5]=0.0000;
}


void AddFlowByShifting::ao_test (double b, double /*eta*/, double pt)
{
  pt/=1000;
  if(pt>2) pt = 2; // flat max at pt > 2
  m_v_n[1] = 0.02 * b * pt;
  m_v_n[0] = 0;
}


void AddFlowByShifting::custom_vn (double /*b*/, double /*eta*/, double /*pt*/)
{
  m_v_n[0]=m_custom_v1;
  m_v_n[1]=m_custom_v2;
  m_v_n[2]=m_custom_v3;
  m_v_n[3]=m_custom_v4;
  m_v_n[4]=m_custom_v5;
  m_v_n[5]=m_custom_v6;
}

// p_Pb vn
void AddFlowByShifting::p_Pb_cent_eta_indep(double /*b*/, double /*eta*/, double pt)
{
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

  m_v_n[0]=0;
  m_v_n[1]=an_val[0][0]*std::pow(pt,an_val[0][1])*std::exp(-an_val[0][2]*pt);
  m_v_n[2]=an_val[1][0]*std::pow(pt,an_val[1][1])*std::exp(-an_val[1][2]*pt);
  m_v_n[3]=an_val[2][0]*std::pow(pt,an_val[2][1])*std::exp(-an_val[2][2]*pt);
  m_v_n[4]=an_val[3][0]*std::pow(pt,an_val[3][1])*std::exp(-an_val[3][2]*pt);
  m_v_n[5]=0;
}



void AddFlowByShifting::Set_EbE_Fluctuation_Multipliers(HepMC::GenVertexPtr mainvtx,float b){
    int Total_Multiplicity=0;
    double EbE_Vn[6];
    for(int ihar=0;ihar<6;ihar++){m_EbE_Multiplier_vn[ihar]=1.0;EbE_Vn[ihar]=0.0;}
  
    for(auto  parent: *mainvtx) {
       float eta= parent->momentum().pseudoRapidity();
       float pT = parent->momentum().perp();

       for(int ihar = 0; ihar< 6; ihar++){m_v_n  [ihar]=0.0;}
       (*this.*m_flow_function)(b,eta,pT);
       for(int ihar = 0; ihar< 6; ihar++){EbE_Vn[ihar] += m_v_n  [ihar];}
       Total_Multiplicity++;
    }


    for(int ihar = 0; ihar< 6; ihar++){m_v_n  [ihar]=0.0;}//keep vn as zero before we return from function
    if(Total_Multiplicity<=0) return; 


    for(int ihar=0;ihar<6;ihar++){
      EbE_Vn[ihar]/=Total_Multiplicity;
      float vn_rp=0,delta=0;//BG parameterizations

      //in the following we assume that the vn in the event is sqrt(<vn^2>)
      //This is because the vn(pT) were tuned to 2PC/EP measurements
      //then : vn_evt=vn_rp^2 + 2*(delta^2)
      //which is used together with the "alpha" to get the "vn_rp" and "delta"
      if(ihar==0) continue; //No EbE fluctuation for v1
      else if(ihar==1) {    //v2
        float alpha=1.0/m_graph_fluc->Eval(b);// ratio of V2_RP over delta (from Fig 15 of EbE vn paper)
        delta=EbE_Vn[ihar]/sqrt(2.0+alpha*alpha);
        vn_rp=alpha*delta;
      }
      else if(ihar>=2) { //v3-v6
        vn_rp =0;
        delta=EbE_Vn[ihar]/sqrt(2.0);
      }
      if(EbE_Vn[ihar]==0) {ATH_MSG_WARNING("Zero EbeV"<<ihar+1); continue;}
      float X=CLHEP::RandGaussQ::shoot(p_engine,vn_rp,delta);
      float Y=CLHEP::RandGaussQ::shoot(p_engine,0.0  ,delta);
      m_EbE_Multiplier_vn[ihar]=sqrt(X*X+ Y*Y)/EbE_Vn[ihar]; 
      ATH_MSG_INFO("EbE_Multiplier_v"<<ihar+1<<"="<<m_EbE_Multiplier_vn[ihar]);
    }
}

