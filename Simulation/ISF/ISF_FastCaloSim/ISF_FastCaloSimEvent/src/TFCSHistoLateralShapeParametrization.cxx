/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSHistoLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

#include "TFile.h"
#include "TMath.h"

//=============================================
//======= TFCSHistoLateralShapeParametrization =========
//=============================================

TFCSHistoLateralShapeParametrization::TFCSHistoLateralShapeParametrization(const char* name, const char* title) :
  TFCSLateralShapeParametrizationHitBase(name,title),
  m_hist(0)
{
}

TFCSHistoLateralShapeParametrization::~TFCSHistoLateralShapeParametrization()
{
  if(m_hist) delete m_hist;
}

int TFCSHistoLateralShapeParametrization::get_number_of_hits(TFCSSimulationState& /*simulstate*/,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/) const
{
  return gRandom->Poisson(m_hist->Integral());
}

void TFCSHistoLateralShapeParametrization::set_number_of_hits(int nhits)
{
  if(m_hist) m_hist->Scale(nhits/m_hist->Integral());
}

void TFCSHistoLateralShapeParametrization::simulate_hit(Hit& hit,TFCSSimulationState& /*simulstate*/,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  const int cs=calosample();
  const double center_eta=0.5*( extrapol->eta(cs, CaloSubPos::SUBPOS_ENT) + extrapol->eta(cs, CaloSubPos::SUBPOS_EXT) );
  const double center_phi=0.5*( extrapol->phi(cs, CaloSubPos::SUBPOS_ENT) + extrapol->phi(cs, CaloSubPos::SUBPOS_EXT) );
  const double center_r=0.5*( extrapol->r(cs, CaloSubPos::SUBPOS_ENT) + extrapol->r(cs, CaloSubPos::SUBPOS_EXT) );
  const double center_z=0.5*( extrapol->z(cs, CaloSubPos::SUBPOS_ENT) + extrapol->z(cs, CaloSubPos::SUBPOS_EXT) );

  double alpha, r;
  
  m_hist->GetRandom2(alpha,r);
  const double delta_eta_mm = r * cos(alpha);
  const double delta_phi_mm = r * sin(alpha);

  const float dist000    = TMath::Sqrt(center_r * center_r + center_z * center_z);
  const float eta_jakobi = TMath::Abs(2.0 * TMath::Exp(-center_eta) / (1.0 + TMath::Exp(-2 * center_eta)));

  const double delta_eta = delta_eta_mm / eta_jakobi / dist000;
  const double delta_phi = delta_phi_mm / center_r;

  hit.eta() = center_eta + delta_eta;
  hit.phi() = center_phi + delta_phi;

  ATH_MSG_DEBUG("HIT: E="<<hit.E()<<" cs="<<cs<<" eta="<<hit.eta()<<" phi="<<hit.phi()<<" r="<<r<<" alpha="<<alpha);
}


bool TFCSHistoLateralShapeParametrization::Initialize(TH2* hist)
{
	m_hist=(TH2*)hist->Clone();
	m_hist->SetDirectory(0);
	for(int ix=1;ix<=m_hist->GetNbinsX();++ix) {
  	for(int iy=1;iy<=m_hist->GetNbinsY();++iy) {
  	  if(m_hist->GetBinContent(ix,iy)<0) {
        ATH_MSG_WARNING("Histo: "<<m_hist->GetName()<<" : "<<m_hist->GetTitle()<<" : bin("<<ix<<","<<iy<<")="<<m_hist->GetBinContent(ix,ix)<<" is negative. Fixing to 0!");
  	    m_hist->SetBinContent(ix,iy,0);
  	  }
  	}
	}

  return true;
}

bool TFCSHistoLateralShapeParametrization::Initialize(const char* filepath, const char* histname)
{
    // input file with histogram to fit
    std::unique_ptr<TFile> inputfile(TFile::Open( filepath, "READ" ));
    if (inputfile == NULL) return false;

    // histogram with hit pattern
    TH2 *inputShape = (TH2*)inputfile->Get(histname);
    if (inputShape == NULL) return false;

    bool OK=Initialize(inputShape);

    inputfile->Close();

    return OK;
}

void TFCSHistoLateralShapeParametrization::Print(Option_t *option) const
{
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");
  TFCSLateralShapeParametrizationHitBase::Print(option);

  if(longprint) ATH_MSG_INFO(optprint <<"  Histo: "<<m_hist->GetName()<<" : "<<m_hist->GetTitle()<<" integral="<<m_hist->Integral()<<" ptr="<<m_hist);
}
