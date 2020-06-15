/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrizationChain.h"
#include "ISF_FastCaloSimEvent/TFCSParametrizationPlaceholder.h"
#include "ISF_FastCaloSimEvent/TFCSInvisibleParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include <algorithm>
#include <iterator>
#include "TBuffer.h"
#include "TDirectory.h"

//=============================================
//======= TFCSParametrizationChain =========
//=============================================

void TFCSParametrizationChain::recalc_pdgid_intersect()
{
  set_pdgid(m_chain[0]->pdgid());
  
  for(const auto& param: m_chain) {
    std::set< int > tmp;
 
    std::set_intersection(pdgid().begin(), pdgid().end(),
                          param->pdgid().begin(), param->pdgid().end(),
                          std::inserter(tmp,tmp.begin()));  
    set_pdgid(tmp);
  }
}

void TFCSParametrizationChain::recalc_pdgid_union()
{
  set_pdgid(chain()[0]->pdgid());
  
  for(const auto& param: chain()) {
    std::set< int > tmp;
 
    std::set_union(pdgid().begin(), pdgid().end(),
                   param->pdgid().begin(), param->pdgid().end(),
                   std::inserter(tmp,tmp.begin()));  
    set_pdgid(tmp);
  }
}

void TFCSParametrizationChain::recalc_Ekin_intersect()
{
  set_Ekin(*m_chain[0]);
  
  for(const auto& param: m_chain) {
    if(param->Ekin_min()>Ekin_min()) set_Ekin_min(param->Ekin_min());
    if(param->Ekin_max()<Ekin_max()) set_Ekin_max(param->Ekin_max());
    if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(param->Ekin_nominal());
  }

  if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(0.5*(Ekin_min()+Ekin_max()));
}

void TFCSParametrizationChain::recalc_eta_intersect()
{
  set_eta(*m_chain[0]);
  
  for(const auto& param: m_chain) {
    if(param->eta_min()>eta_min()) set_eta_min(param->eta_min());
    if(param->eta_max()<eta_max()) set_eta_max(param->eta_max());
    if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(param->eta_nominal());
  }

  if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(0.5*(eta_min()+eta_max()));
}

void TFCSParametrizationChain::recalc_Ekin_eta_intersect()
{
  recalc_Ekin_intersect();
  recalc_eta_intersect();
}

void TFCSParametrizationChain::recalc_Ekin_union()
{
  set_Ekin(*m_chain[0]);
  
  for(const auto& param: m_chain) {
    if(param->Ekin_min()<Ekin_min()) set_Ekin_min(param->Ekin_min());
    if(param->Ekin_max()>Ekin_max()) set_Ekin_max(param->Ekin_max());
    if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(param->Ekin_nominal());
  }

  if(Ekin_nominal()<Ekin_min() || Ekin_nominal()>Ekin_max()) set_Ekin_nominal(0.5*(Ekin_min()+Ekin_max()));
}

void TFCSParametrizationChain::recalc_eta_union()
{
  set_eta(*m_chain[0]);
  
  for(const auto& param: m_chain) {
    if(param->eta_min()<eta_min()) set_eta_min(param->eta_min());
    if(param->eta_max()>eta_max()) set_eta_max(param->eta_max());
    if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(param->eta_nominal());
  }

  if(eta_nominal()<eta_min() || eta_nominal()>eta_max()) set_eta_nominal(0.5*(eta_min()+eta_max()));
}

void TFCSParametrizationChain::recalc_Ekin_eta_union()
{
  recalc_Ekin_union();
  recalc_eta_union();
}

void TFCSParametrizationChain::recalc()
{
  clear();
  if(m_chain.size()==0) return;
  
  recalc_pdgid_intersect();
  recalc_Ekin_eta_intersect();
  
  m_chain.shrink_to_fit();
}

bool TFCSParametrizationChain::is_match_Ekin_bin(int Ekin_bin) const
{
  for(const auto& param : m_chain) if(!param->is_match_Ekin_bin(Ekin_bin)) return false;
  return true;
}

bool TFCSParametrizationChain::is_match_calosample(int calosample) const
{
  for(const auto& param : m_chain) if(!param->is_match_calosample(calosample)) return false;
  return true;
}

FCSReturnCode TFCSParametrizationChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  Int_t retry=0;
  Int_t retry_warning=1;

  FCSReturnCode status = FCSSuccess;
  for (int i = 0; i <= retry; i++) {
    if (i >= retry_warning) ATH_MSG_WARNING("TFCSParametrizationChain::simulate(): Retry simulate call " << i << "/" << retry);
    for(const auto& param: m_chain) {
      status = simulate_and_retry(param, simulstate, truth, extrapol);
      
      if (status >= FCSRetry) {
        retry=status-FCSRetry;
        retry_warning=retry>>1;
        if(retry_warning<1) retry_warning=1;
        break;
      }
      if (status == FCSFatal) return FCSFatal;
    }
    
    if(status==FCSSuccess) break;
  }  

  if(status != FCSSuccess) {
    ATH_MSG_FATAL("TFCSParametrizationChain::simulate(): Simulate call failed after " << retry << " retries");
    return FCSFatal;
  }  

  return FCSSuccess;
}

void TFCSParametrizationChain::Print(Option_t *option) const
{
  TFCSParametrization::Print(option);
  TString opt(option);
  //bool shortprint=opt.Index("short")>=0;
  //bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);

  char count='A';
  for(const auto& param: m_chain) {
    param->Print(opt+count+' ');
    count++;
  }
}

void TFCSParametrizationChain::Streamer(TBuffer &R__b)
{
   // Stream an object of class TFCSParametrizationChain.

   UInt_t R__s, R__c;
   TDirectory* dir=nullptr;
   
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
      if (R__v==1) {
        R__b.SetBufferOffset(R__s);
        R__b.ReadClassBuffer(TFCSParametrizationChain::Class(),this);
      } else {
        TFCSParametrization::Streamer(R__b);

        TObject* parent=R__b.GetParent();
        if(R__b.GetParent()) { 
          if(parent->InheritsFrom(TDirectory::Class())) {
            dir=(TDirectory*)parent;
          }
        }

        TFCSParametrizationChain::Chain_t &R__stl =  m_chain;
        R__stl.clear();
        TClass *R__tcl1 = TFCSParametrizationBase::Class();
        if (R__tcl1==0) {
          Error("m_chain streamer","Missing the TClass object for class TFCSParametrizationBase *!");
          return;
        }
        int R__i, R__n;
        R__b >> R__n;
        R__stl.reserve(R__n);
        for (R__i = 0; R__i < R__n; R__i++) {
          TFCSParametrizationBase* R__t;
          R__t = (TFCSParametrizationBase*)R__b.ReadObjectAny(R__tcl1);
          if(R__t!=nullptr) {
            if(R__t->InheritsFrom(TFCSParametrizationPlaceholder::Class())) {
              TFCSParametrizationBase* new_R__t=nullptr;

              if(dir) new_R__t=(TFCSParametrizationBase*)dir->Get(R__t->GetName());

              if(new_R__t) {
                delete R__t;
                R__t=new_R__t;
              } else {
                Error("TFCSParametrizationChain::Streamer","Found placeholder object in the parametrization chain, but could not read the real object from the file!");
              }
            }
          }
          R__stl.push_back(R__t);
        }

        R__b.CheckByteCount(R__s, R__c, TFCSParametrizationChain::IsA());
      }  
   } else {
      R__c = R__b.WriteVersion(TFCSParametrizationChain::IsA(), kTRUE);
      TFCSParametrization::Streamer(R__b);

      if(SplitChainObjects()) {
        TObject* parent=R__b.GetParent();
        if(R__b.GetParent()) { 
          if(parent->InheritsFrom(TDirectory::Class())) {
            dir=(TDirectory*)parent;
          }
        }
      }

      TFCSParametrizationChain::Chain_t &R__stl =  m_chain;
      int R__n=int(R__stl.size());
      R__b << R__n;
      if(R__n) {
        TFCSParametrizationChain::Chain_t::iterator R__k;
        int R__i=0;
        for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
          TFCSParametrizationBase* R__t = *R__k;
          TFCSParametrizationBase* new_R__t=nullptr;
          if(dir && R__t!=nullptr) {
            dir->WriteTObject(R__t);
            new_R__t=new TFCSParametrizationPlaceholder(R__t->GetName(),TString("Placeholder for: ")+R__t->GetTitle());
            R__t=new_R__t;
          }
          R__b << R__t;

          //delete new_R__t only after the end of read/write operations by calling TFCSParametrizationBase::DoCleanup();
          if(new_R__t) s_cleanup_list.push_back(new_R__t);

          ++R__i;
        }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

void TFCSParametrizationChain::unit_test(TFCSSimulationState* simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  if(!simulstate) simulstate=new TFCSSimulationState();
  if(!truth) truth=new TFCSTruthState();
  if(!extrapol) extrapol=new TFCSExtrapolationState();

  TFCSParametrizationChain chain("chain","chain");
  chain.setLevel(MSG::DEBUG);

  std::cout<<"====         Chain setup       ===="<<std::endl;
  chain.Print();
  std::cout<<"==== Simulate with empty chain ===="<<std::endl;
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==================================="<<std::endl<<std::endl;

  TFCSParametrizationBase* param;
  param=new TFCSInvisibleParametrization("A begin all","A begin all");
  param->setLevel(MSG::VERBOSE);
  chain.push_back(param);
  param=new TFCSParametrization("A end all","A end all");
  param->setLevel(MSG::DEBUG);
  chain.push_back(param);

  std::cout<<"====         Chain setup       ===="<<std::endl;
  chain.Print();
  std::cout<<"==== Simulate only begin/end all ===="<<std::endl;
  chain.simulate(*simulstate,truth,extrapol);
  std::cout<<"==== Simulate only begin/end all with chain retry===="<<std::endl;
  chain.set_RetryChainFromStart();
  chain.simulate(*simulstate,truth,extrapol);
  chain.reset_RetryChainFromStart();
  std::cout<<"==================================="<<std::endl<<std::endl;
}


