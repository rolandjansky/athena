/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSHistoLateralShapeParametrization.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

#include "TFile.h"

//=============================================
//======= TFCSHistoLateralShapeParametrization =========
//=============================================

TFCSHistoLateralShapeParametrization::TFCSHistoLateralShapeParametrization(const char* name, const char* title) :
  TFCSLateralShapeParametrization(name,title),
  m_hist(0),m_rnd(0)
{
}

void TFCSHistoLateralShapeParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  int cs=calosample();
  double center_eta=0.5*( extrapol->eta(cs, CaloSubPos::SUBPOS_ENT) + extrapol->eta(cs, CaloSubPos::SUBPOS_EXT) );
  double center_phi=0.5*( extrapol->phi(cs, CaloSubPos::SUBPOS_ENT) + extrapol->phi(cs, CaloSubPos::SUBPOS_EXT) );
  double center_r=0.5*( extrapol->r(cs, CaloSubPos::SUBPOS_ENT) + extrapol->r(cs, CaloSubPos::SUBPOS_EXT) );
  double center_z=0.5*( extrapol->z(cs, CaloSubPos::SUBPOS_ENT) + extrapol->z(cs, CaloSubPos::SUBPOS_EXT) );
  double hit_weight=1;

  double alpha, r;
  
  m_hist->GetRandom2(alpha, r);
  double delta_eta_mm = r * cos(alpha);
  double delta_phi_mm = r * sin(alpha);

  float dist000    = TMath::Sqrt(center_r * center_r + center_z * center_z);
  float eta_jakobi = TMath::Abs(2.0 * TMath::Exp(-center_eta) / (1.0 + TMath::Exp(-2 * center_eta)));
//	d_eta_mm = d_eta * eta_jakobi * dist000;
//	d_phi_mm = d_phi * cell_r * phi_dist2r;
  double delta_eta = delta_eta_mm / eta_jakobi / dist000;
  double delta_phi = delta_phi_mm / center_r;

  double hit_eta = center_eta + delta_eta;
  double hit_phi = center_phi + delta_phi;
  simulstate.deposit_HIT(cs,hit_eta,hit_phi,hit_weight);
}


bool TFCSHistoLateralShapeParametrization::Initialize(TH2* hist)
{
	m_hist=(TH2*)hist->Clone();
	m_hist->SetDirectory(0);

    return true;
}


bool TFCSHistoLateralShapeParametrization::Initialize(const char* filepath, const char* histname)
{
    // Setup random numbers
    m_rnd = new TRandom3();
    m_rnd->SetSeed(0);

    // input file with histogram to fit
    TFile *f = new TFile(filepath);
    if (f == NULL) return false;

    // histogram with hit pattern
    TH2 *inputShape = (TH2*)f->Get(histname);
    if (inputShape == NULL) return false;

    bool OK=Initialize(inputShape);

    f->Close();

    return OK;
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSHistoLateralShapeParametrization)
