/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitChain.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "TMath.h"
#include <TClass.h>
#ifdef USE_GPU
#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include "ISF_FastCaloSimEvent/TFCSCenterPositionCalculation.h"
#include "ISF_FastCaloSimEvent/TFCSHistoLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSHistoLateralShapeGausLogWeight.h"
#include "ISF_FastCaloSimEvent/TFCSHitCellMappingWiggle.h"
#include "ISF_FastCaloSimEvent/TFCSHitCellMapping.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimEvent/ICaloGeometry.h"
#include "ISF_FastCaloGpu/CaloGpuGeneral.h"
#include "ISF_FastCaloGpu/GeoLoadGpu.h"
#include "ISF_FastCaloGpu/Args.h"
#include "HepPDT/ParticleData.hh"
//#include "HepPDT/ParticleDataTable.hh"
#endif


//=============================================
//======= TFCSLateralShapeParametrizationHitChain =========
//=============================================

#ifdef USE_GPU
TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(const char* name, const char* title, ICaloGeometry* geo):TFCSLateralShapeParametrization(name,title),m_geo(geo),m_number_of_hits_simul(nullptr)
{
}

//override the function for FCS-GPU
void TFCSLateralShapeParametrizationHitChain::set_geometry(ICaloGeometry* geo)
{
  TFCSLateralShapeParametrization::set_geometry(geo);
  if(m_number_of_hits_simul) m_number_of_hits_simul->set_geometry(geo);
  m_geo=geo;
}

#else
TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(const char* name, const char* title):TFCSLateralShapeParametrization(name,title),m_number_of_hits_simul(nullptr)
{
}
#endif

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(TFCSLateralShapeParametrizationHitBase* hitsim):TFCSLateralShapeParametrization(TString("hit_chain_")+hitsim->GetName(),TString("hit chain for ")+hitsim->GetTitle()),m_number_of_hits_simul(nullptr)
{
  set_pdgid_Ekin_eta_Ekin_bin_calosample(*hitsim);
  
  m_chain.push_back(hitsim);
}

unsigned int TFCSLateralShapeParametrizationHitChain::size() const 
{
  if(m_number_of_hits_simul) return m_chain.size()+1;
   else return m_chain.size();
}

void TFCSLateralShapeParametrizationHitChain::push_back_init( const Chain_t::value_type& value ) 
{
  if(m_ninit==size()) {
    chain().push_back(value);
  } else {  
    Chain_t::iterator it(&chain()[m_ninit]);
    chain().insert(it,value);
  }  
  ++m_ninit;
}

const TFCSParametrizationBase* TFCSLateralShapeParametrizationHitChain::operator[](unsigned int ind) const 
{
  if(m_number_of_hits_simul) {
    if(ind==0) return m_number_of_hits_simul;
    return m_chain[ind-1];
  } else {
    return m_chain[ind];
  }  
}

TFCSParametrizationBase* TFCSLateralShapeParametrizationHitChain::operator[](unsigned int ind) {
  if(m_number_of_hits_simul) {
    if(ind==0) return m_number_of_hits_simul;
    return m_chain[ind-1];
  } else {
    return m_chain[ind];
  }  
}

void TFCSLateralShapeParametrizationHitChain::set_daughter(unsigned int ind,TFCSParametrizationBase* param) 
{
  TFCSLateralShapeParametrizationHitBase* param_typed=nullptr;
  if(param!=nullptr) {
    if(!param->InheritsFrom(TFCSLateralShapeParametrizationHitBase::Class())) {
      ATH_MSG_ERROR("Wrong class type "<<param->IsA()->GetName());
      return;
    }
    param_typed=static_cast<TFCSLateralShapeParametrizationHitBase*>(param);
  }  
  if(m_number_of_hits_simul) {
    if(ind==0) m_number_of_hits_simul=param_typed;
     else m_chain.at(ind-1)=param_typed;
  } else {
    m_chain.at(ind)=param_typed;
  }  
}

int TFCSLateralShapeParametrizationHitChain::get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  // TODO: should we still do it?
  if(m_number_of_hits_simul) {
    int n=m_number_of_hits_simul->get_number_of_hits(simulstate,truth,extrapol);
    if(n<1) n=1;
    return n;
  }
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    int n=hitsim->get_number_of_hits(simulstate,truth,extrapol);
    if(n>0) return n;
  } 
  return 1;
}

float TFCSLateralShapeParametrizationHitChain::getMinWeight() const
{
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    float weight=hitsim->getMinWeight();
    if(weight>0.) return weight;
  } 
  return -1.;
}
  
float TFCSLateralShapeParametrizationHitChain::getMaxWeight() const
{
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    float weight=hitsim->getMaxWeight();
    if(weight>0.) return weight;
  }
  return -1.;
}


float TFCSLateralShapeParametrizationHitChain::get_E_hit(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{  
  const int nhits = get_number_of_hits(simulstate,truth,extrapol);
  const int sample = calosample();
  if(nhits<=0 || sample<0) return simulstate.E(sample);
  const float maxWeight = getMaxWeight();// maxWeight = -1 if  shapeWeight class is not in m_chain  
  
  if(maxWeight>0) return simulstate.E(sample)/(maxWeight*nhits); // maxWeight is used only if shapeWeight class is in m_chain
  else return simulstate.E(sample)/nhits; // Otherwise, old definition of E_hit is used
}

float TFCSLateralShapeParametrizationHitChain::get_sigma2_fluctuation(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  if(m_number_of_hits_simul) {
    double sigma2=m_number_of_hits_simul->get_sigma2_fluctuation(simulstate,truth,extrapol);
    if(sigma2>0) return sigma2;
  }
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    double sigma2=hitsim->get_sigma2_fluctuation(simulstate,truth,extrapol);
    if(sigma2>0) return sigma2;
  } 
  //Limit to factor s_max_sigma2_fluctuation fluctuations
  return s_max_sigma2_fluctuation;
}

void TFCSLateralShapeParametrizationHitChain::PropagateMSGLevel(MSG::Level level) const
{
  for(TFCSLateralShapeParametrizationHitBase* reset : m_chain) reset->setLevel(level);
}  

FCSReturnCode TFCSLateralShapeParametrizationHitChain::init_hit(TFCSLateralShapeParametrizationHitBase::Hit& hit,TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  hit.reset_center();
  if(get_nr_of_init()>0) {
    ATH_MSG_DEBUG("E("<<calosample()<<")="<<simulstate.E(calosample())<<" before init");

    auto initloopend=m_chain.begin()+get_nr_of_init();
    for(auto hititr=m_chain.begin(); hititr!=initloopend; ++hititr) {
      TFCSLateralShapeParametrizationHitBase* hitsim=*hititr;

      FCSReturnCode status = hitsim->simulate_hit(hit, simulstate, truth, extrapol);

      if (status != FCSSuccess) {
        ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitChain::simulate(): simulate_hit init call failed");
        return FCSFatal;
      }
    }
  }
  return FCSSuccess;
}

FCSReturnCode TFCSLateralShapeParametrizationHitChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  MSG::Level old_level=level();
  const bool debug = msgLvl(MSG::DEBUG);
  const bool verbose = msgLvl(MSG::VERBOSE);

  //Execute the first get_nr_of_init() simulate calls only once. Used for example to initialize the center position
  TFCSLateralShapeParametrizationHitBase::Hit hit;
  if(init_hit(hit,simulstate,truth,extrapol)!=FCSSuccess) {
    ATH_MSG_ERROR("init_hit() failed");
    return FCSFatal;
  }

  int cs = calosample();
  //Initialize hit energy only now, as init loop above might change the layer energy
  const float Elayer=simulstate.E(cs);
  if (Elayer == 0) {
    ATH_MSG_VERBOSE("Elayer=0, nothing to do");
    return FCSSuccess;
  }
  const float Ehit=get_E_hit(simulstate,truth,extrapol);
  if (Ehit * Elayer <= 0) {
    ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitChain::simulate(): Ehit and Elayer have different sign! Ehit="<<Ehit<<" Elayer="<<Elayer);
    return FCSFatal;
  }

  // Call get_number_of_hits() only once inside get_E_hit(...), 
  // as it could contain a random number
  int nhit = TMath::Nint(Elayer/Ehit);
  if(nhit<1) nhit=1;

  float sumEhit=0;

  if (debug) {
    PropagateMSGLevel(old_level);
    ATH_MSG_DEBUG("E("<<cs<<")="<<simulstate.E(cs)<<" #hits~"<<nhit);
  }

  //
  //simulate the hits in GPU
  //
#ifdef USE_GPU
  int ichn       = 0;
  bool our_chainA = false;
  if ( cs > 0 && cs < 8 && cs != 4 )
    our_chainA = true;    
  if ( nhit > MIN_GPU_HITS && our_chainA && simulstate.get_geold()!=nullptr) {
    ATH_MSG_DEBUG("running FastCaloSim in GPU, nhits = "<<nhit<<" ; Ehit = "<<Ehit );
    GeoLoadGpu* gld = (GeoLoadGpu*)simulstate.get_geold() ;

    Chain0_Args args;
    args.debug            = debug;
    args.cs               = cs;
    args.extrapol_eta_ent = extrapol->eta( cs, SUBPOS_ENT );
    args.extrapol_eta_ext = extrapol->eta( cs, SUBPOS_EXT );
    args.extrapol_phi_ent = extrapol->phi( cs, SUBPOS_ENT );
    args.extrapol_phi_ext = extrapol->phi( cs, SUBPOS_EXT );
    args.extrapol_r_ent   = extrapol->r( cs, SUBPOS_ENT ) ;
    args.extrapol_r_ext   = extrapol->r( cs, SUBPOS_EXT ) ;
    args.extrapol_z_ent   = extrapol->z( cs, SUBPOS_ENT ) ;
    args.extrapol_z_ext   = extrapol->z( cs, SUBPOS_EXT ) ;
    args.pdgId  = truth->pdgid();
    args.charge = HepPDT::ParticleID( args.pdgId ).charge();
    args.nhits  = nhit;
    args.rand   = 0;
    args.geo    = gld->get_geoPtr();
    args.rd4h   = simulstate.get_gpu_rand();
    args.ncells = gld->get_ncells();
    ichn = 0;
    //reweight the cell energies for pion
    bool reweight = false ;
    for ( auto hitsim : m_chain ) {
      if ( ichn == 0 ) {
	args.extrapWeight = ( (TFCSCenterPositionCalculation*)hitsim)->getExtrapWeight();
      }

      if ( ichn == 1 ) {

	args.is_phi_symmetric = ( (TFCSHistoLateralShapeParametrization*)hitsim )->is_phi_symmetric();
	args.fh2d = ( (TFCSHistoLateralShapeParametrization*)hitsim)->LdFH()->hf2d_d();
	args.fh2d_h = *( ( (TFCSHistoLateralShapeParametrization*)hitsim )->LdFH()->hf2d_h() );
      }
      if ( ichn == 2 && m_chain.size()==4 ) {
	args.fh1d = ( (TFCSHistoLateralShapeGausLogWeight*)hitsim )->LdFH()->hf1d_d();
	args.fh1d_h = *(( (TFCSHistoLateralShapeGausLogWeight*)hitsim )->LdFH()->hf1d_h() );
	reweight = true;
      }
      if ( ( ichn == 2 && m_chain.size()==3 ) || ( ichn == 3 && m_chain.size()==4 ) ) {
	//      ATH_MSG_DEBUG("---NumberOfBins :" << ( (TFCSHitCellMappingWi    ggle*)hitsim )->get_number_of_bins() );
	args.fhs = ( (TFCSHitCellMappingWiggle*)hitsim )->LdFH()->hf_d();
	args.fhs_h = *(( (TFCSHitCellMappingWiggle*)hitsim )->LdFH()->hf_h() );
      }

      ichn++;
    }
    if(reweight)
      CaloGpuGeneral::simulate_hits( Ehit, 1.2*nhit, args ,reweight);
    else
      CaloGpuGeneral::simulate_hits( Ehit, nhit, args ,reweight);

    for ( unsigned int ii = 0; ii < args.ct; ++ii ) {

      const CaloDetDescrElement_Gpu* cellele = gld->index2cell( args.hitcells_E_h[ii].cellid );
      const CaloDetDescrElement* cell_h = m_geo->getDDE(cs,cellele->eta(),cellele->phi());
      sumEhit+=args.hitcells_E_h[ii].energy;
      if(reweight){
	if(std::abs(sumEhit)>std::abs(Elayer))
	  simulstate.deposit( cell_h, args.hitcells_E_h[ii].energy );
      }
    }
  }else
#endif
  {

    auto hitloopstart=m_chain.begin()+get_nr_of_init();
    int ihit=0;
    int ifail=0;
    int itotalfail=0;
    int retry_warning=1;
    int retry=0;
    do {
      hit.reset();
      hit.E()=Ehit;
      bool failed=false;
      if(debug) if(ihit==2) if(!verbose) {
	//Switch debug output back to INFO to avoid huge logs, but keep full log in verbose
	PropagateMSGLevel(MSG::INFO);
      }
      for(auto hititr=hitloopstart; hititr!=m_chain.end(); ++hititr) {
	TFCSLateralShapeParametrizationHitBase* hitsim=*hititr;

	FCSReturnCode status = hitsim->simulate_hit(hit, simulstate, truth, extrapol);

	if (status == FCSSuccess) continue;
	if (status == FCSFatal) {
	  if (debug) PropagateMSGLevel(old_level); 
	  return FCSFatal;
	}  
	failed=true;
	++ifail;
	++itotalfail;
	retry=status-FCSRetry;
	retry_warning=retry>>1;
	if(retry_warning<1) retry_warning=1;
	break;
      }
      if(!failed) {
	ifail=0;
	sumEhit+=hit.E();
	++ihit;

	if( ( (ihit==20*nhit) || (ihit==100*nhit) ) && ihit>=100 ) {
	  ATH_MSG_DEBUG("TFCSLateralShapeParametrizationHitChain::simulate(): Iterated " << ihit << " times, expected " << nhit <<" times. Deposited E("<<cs<<")="<<sumEhit<<" expected E="<<Elayer);
	}                                                                                                                         
	if(ihit>=1000*nhit && ihit>=1000) {
	  ATH_MSG_WARNING("TFCSLateralShapeParametrizationHitChain::simulate(): Aborting hit chain, iterated " << ihit << " times, expected " << nhit <<" times. Deposited E("<<cs<<")="<<sumEhit<<" expected E="<<Elayer<<", caused by:");
	  Print();
	  break;
	}  
      } else {
	if(ifail >= retry) {
	  ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitChain::simulate(): simulate_hit call failed after " << ifail << "/"<< retry <<"retries, total fails="<<itotalfail);
	  if (debug) PropagateMSGLevel(old_level); 
	  return FCSFatal;
	}
	if (ifail >= retry_warning) {
	  ATH_MSG_WARNING("TFCSLateralShapeParametrizationHitChain::simulate(): retry simulate_hit calls "<<ifail<<"/"<< retry<<", total fails="<<itotalfail);
	}
      }
    } while (std::abs(sumEhit)<std::abs(Elayer));
  }

  if (debug) PropagateMSGLevel(old_level);
  return FCSSuccess;
}

void TFCSLateralShapeParametrizationHitChain::Print(Option_t *option) const
{
  TFCSLateralShapeParametrization::Print(option);
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");

  if(m_number_of_hits_simul) {
    if(longprint) ATH_MSG_INFO(optprint <<"#:Number of hits simulation:");
    m_number_of_hits_simul->Print(opt+"#:");
  }
  if(longprint && get_nr_of_init()>0) ATH_MSG_INFO(optprint <<"> Simulation init chain:");
  auto hitloopstart=m_chain.begin()+get_nr_of_init();
  for(auto hititr=m_chain.begin(); hititr!=hitloopstart; ++hititr) {
    TFCSLateralShapeParametrizationHitBase* hitsim=*hititr;
    hitsim->Print(opt+"> ");
  } 
  if(longprint) ATH_MSG_INFO(optprint <<"- Simulation chain:");
  char count='A';
  for(auto hititr=hitloopstart; hititr!=m_chain.end(); ++hititr) {
    TFCSLateralShapeParametrizationHitBase* hitsim=*hititr;
    hitsim->Print(opt+count+" ");
    count++;
  } 
}
