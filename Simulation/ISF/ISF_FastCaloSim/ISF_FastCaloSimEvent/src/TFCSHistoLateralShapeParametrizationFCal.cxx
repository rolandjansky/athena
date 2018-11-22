/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoisson.h"

#include "ISF_FastCaloSimEvent/TFCSHistoLateralShapeParametrizationFCal.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

#include "TFile.h"
#include "TMath.h"
#include "TH2.h"


//=============================================
//======= TFCSHistoLateralShapeParametrizationFCal =========
//=============================================

TFCSHistoLateralShapeParametrizationFCal::TFCSHistoLateralShapeParametrizationFCal(const char* name, const char* title) :
  TFCSLateralShapeParametrizationHitBase(name,title),m_nhits(0)
{
  reset_phi_symmetric();
}

TFCSHistoLateralShapeParametrizationFCal::~TFCSHistoLateralShapeParametrizationFCal()
{
}

int TFCSHistoLateralShapeParametrizationFCal::get_number_of_hits(TFCSSimulationState &simulstate, const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/) const
{
  if (!simulstate.randomEngine()) {
    return -1;
  }

  return CLHEP::RandPoisson::shoot(simulstate.randomEngine(), m_nhits);
}

void TFCSHistoLateralShapeParametrizationFCal::set_number_of_hits(float nhits)
{
  m_nhits=nhits;
}

FCSReturnCode TFCSHistoLateralShapeParametrizationFCal::simulate_hit(Hit &hit, TFCSSimulationState &simulstate, const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  if (!simulstate.randomEngine()) {
    return FCSFatal;
  }

  const int cs=calosample();
  const double center_eta=0.5*( extrapol->eta(cs, CaloSubPos::SUBPOS_ENT) + extrapol->eta(cs, CaloSubPos::SUBPOS_EXT) );
  const double center_phi=0.5*( extrapol->phi(cs, CaloSubPos::SUBPOS_ENT) + extrapol->phi(cs, CaloSubPos::SUBPOS_EXT) );
  const double center_r=0.5*( extrapol->r(cs, CaloSubPos::SUBPOS_ENT) + extrapol->r(cs, CaloSubPos::SUBPOS_EXT) );
  const double center_z=0.5*( extrapol->z(cs, CaloSubPos::SUBPOS_ENT) + extrapol->z(cs, CaloSubPos::SUBPOS_EXT) );
  
  const double center_x=center_r*cos(center_phi);
  const double center_y=center_r*sin(center_phi);

  float alpha, r, rnd1, rnd2;
  rnd1 = CLHEP::RandFlat::shoot(simulstate.randomEngine());
  rnd2 = CLHEP::RandFlat::shoot(simulstate.randomEngine());
  if(is_phi_symmetric()) {
    if(rnd2>=0.5) { //Fill negative phi half of shape
      rnd2-=0.5;
      rnd2*=2;
      m_hist.rnd_to_fct(alpha,r,rnd1,rnd2);
      alpha=-alpha;
    } else { //Fill positive phi half of shape
      rnd2*=2;
      m_hist.rnd_to_fct(alpha,r,rnd1,rnd2);
    }
  } else {
    m_hist.rnd_to_fct(alpha,r,rnd1,rnd2);
  }
  if(TMath::IsNaN(alpha) || TMath::IsNaN(r)) {
    ATH_MSG_ERROR("  Histogram: "<<m_hist.get_HistoBordersx().size()-1<<"*"<<m_hist.get_HistoBordersy().size()-1<<" bins, #hits="<<m_nhits<<" alpha="<<alpha<<" r="<<r<<" rnd1="<<rnd1<<" rnd2="<<rnd2);
    alpha=0;
    r=0.001;

    ATH_MSG_ERROR("  This error could probably be retried");
    return FCSFatal;
  }
  
  const float delta_x = r * cos(alpha);
  const float delta_y = r * sin(alpha);

  hit.setXYZE(center_x+delta_x,center_y+delta_y,center_z,hit.E());

  ATH_MSG_DEBUG("HIT: E="<<hit.E()<<" cs="<<cs<<" x="<<hit.x()<<" y="<<hit.y()<<" z="<<hit.z()<<" r=" << r <<" alpha="<<alpha);

  return FCSSuccess;
}


bool TFCSHistoLateralShapeParametrizationFCal::Initialize(TH2* hist)
{
  if(!hist) return false;
	m_hist.Initialize(hist);
	if(m_hist.get_HistoContents().size()==0) return false;
	
	set_number_of_hits(hist->Integral());

  return true;
}

bool TFCSHistoLateralShapeParametrizationFCal::Initialize(const char* filepath, const char* histname)
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

void TFCSHistoLateralShapeParametrizationFCal::Print(Option_t *option) const
{
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");
  TFCSLateralShapeParametrizationHitBase::Print(option);

  if(longprint) {
    if(is_phi_symmetric()) {
      ATH_MSG_INFO(optprint <<"  Histo: "<<m_hist.get_HistoBordersx().size()-1<<"*"<<m_hist.get_HistoBordersy().size()-1<<" bins, #hits="<<m_nhits<<" (phi symmetric)");
    } else {
      ATH_MSG_INFO(optprint <<"  Histo: "<<m_hist.get_HistoBordersx().size()-1<<"*"<<m_hist.get_HistoBordersy().size()-1<<" bins, #hits="<<m_nhits<<" (not phi symmetric)");
    }
  }  
}
