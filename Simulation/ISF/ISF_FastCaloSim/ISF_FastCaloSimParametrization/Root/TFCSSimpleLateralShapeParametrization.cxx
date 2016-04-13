/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSSimpleLateralShapeParametrization.h"
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"

//=============================================
//======= TFCSLateralShapeParametrization =========
//=============================================

TFCSSimpleLateralShapeParametrization::TFCSSimpleLateralShapeParametrization(const char* name, const char* title):TFCSLateralShapeParametrization(name,title)
{

    sigma_x = 0;
    sigma_y = 0;

    //sigma2_x = 0;
    //sigma2_y = 0;

}

void TFCSSimpleLateralShapeParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* extrapol)
{
  int cs=calosample();
  double hit_eta=0.5*( extrapol->eta(cs,TFCSExtrapolationState::SUBPOS_ENT) + extrapol->eta(cs,TFCSExtrapolationState::SUBPOS_EXT) );
  double hit_phi=0.5*( extrapol->phi(cs,TFCSExtrapolationState::SUBPOS_ENT) + extrapol->phi(cs,TFCSExtrapolationState::SUBPOS_EXT) );
  double hit_weight=1;

  double x, y;
  getHitXY(x, y);

  // delta_eta and delta_phi;
  double delta_eta = x;
  double delta_phi = y;

  hit_eta += delta_eta;
  hit_phi += delta_phi;
  simulstate.deposit_HIT(cs,hit_eta,hit_phi,hit_weight);
}



bool TFCSSimpleLateralShapeParametrization::Initialize(const char* filepath, const char* histname)
{
    // Setup random numbers
    rnd = new TRandom3();
    rnd->SetSeed(0);


    // input file with histogram to fit
    TFile *f = new TFile(filepath);
    if (f == NULL) return false;

    // histogram with hit pattern
    TH2D *inputShape = (TH2D*)f->Get(histname);
    if (inputShape == NULL) return false;

    // Function to fit with
    double hiEdge  = inputShape->GetYaxis()->GetBinLowEdge( inputShape->GetNbinsY() );
    //TF1 *x_func = new TF1("dlbgx","gaus(0)+gaus(3)",-hiEdge,hiEdge);
    //TF1 *y_func = new TF1("dlbgy","gaus(0)+gaus(3)",-hiEdge,hiEdge);
    TF1 *x_func = new TF1("fx","gaus",-hiEdge,hiEdge);
    TF1 *y_func = new TF1("fy","gaus",-hiEdge,hiEdge);

    //test
    //TFile *out = new TFile("out.root","recreate");

    // Project into x and y histograms
    TH1F *h_xrms = new TH1F("h_xrms","h_xrms",100,-hiEdge,hiEdge);
    TH1F *h_yrms = new TH1F("h_yrms","h_yrms",100,-hiEdge,hiEdge);

    double val = 0; // bin content
    double r   = 0; // radius
    double a   = 0; // angle
    double ypos= 0;
    double xpos= 0;

    // Loop over to project along axes, takes bin center as position
    for (int xbin = 1; xbin < inputShape->GetNbinsX() + 1; xbin++)
    {
        a = inputShape->GetXaxis()->GetBinCenter(xbin);

        for (int ybin = 1; ybin < inputShape->GetNbinsY() + 1; ybin++)
        {
            val = inputShape->GetBinContent(xbin,ybin);

            r = inputShape->GetYaxis()->GetBinCenter(ybin);

            ypos = r * TMath::Sin( a );
            xpos = r * TMath::Cos( a );

            h_xrms->Fill( xpos, val);
            h_yrms->Fill( ypos, val);

        }
    }

    h_xrms->Fit(x_func,"0");
    TF1 *fitx = h_xrms->GetFunction("fx");
    // posibly center

    h_yrms->Fit(y_func,"0");
    TF1 *fity = h_yrms->GetFunction("fy");
    // posibly center

    //test
    //out->Write();
    //out->Close();

    // Finally set sigma
    sigma_x = fitx->GetParameter(2);
    sigma_y = fity->GetParameter(2);


    // clean up
    delete x_func;
    delete y_func;

    delete h_xrms;
    delete h_yrms;
    f->Close();

    return true;
}

void TFCSSimpleLateralShapeParametrization::getHitXY(double &x, double &y)
{


    x = rnd->Gaus(0, sigma_x);
    y = rnd->Gaus(0, sigma_y);

}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSSimpleLateralShapeParametrization)
