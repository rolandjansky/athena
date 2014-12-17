/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonFourMomentumCorrection/e1hg_systematics.h"

#include <stdlib.h>
#include <math.h>

#include "TAxis.h"
#include "Riostream.h"

#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif

e1hg_systematics::e1hg_systematics()
{
  
#ifdef ROOTCORE
  file0 = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/e1hg_systematics_histos.root");
#else
  file0 = TFile::Open( PathResolver::find_file("ElectronPhotonFourMomentumCorrection/e1hg_systematics_histos.root", "XMLPATH").c_str() );
#endif

  for (Int_t ieta=0;ieta<8;ieta++) {
     char name[60];
     sprintf(name,"elec_%d",ieta);
     helec[ieta] = (TH1D*) file0->Get(name);
     sprintf(name,"unconv_%d",ieta);
     hphot[ieta] = (TH1D*) file0->Get(name);
     sprintf(name,"conv_%d",ieta);
     hphot2[ieta] = (TH1D*) file0->Get(name);

  }

  TAxis* aa=helec[0]->GetXaxis();
  etBins = aa->GetXbins();

}

//=========================================================================
e1hg_systematics::~e1hg_systematics()
{
  file0->Close();
}

//============================================================================
// inputs are particle_type (0=elec, 1=reco unconv photon, 2=reco conv photon)
//            energy in MeV
//            eta
//
// returned value is alpha = DeltaE / E
//
double e1hg_systematics::getAlpha(int particle_type, double energy, double eta, bool interpolate) const 
{

   //cout << " in getDelta " << endl;
   if (particle_type<0 || particle_type>2) return 0.;

   float aeta=fabs(eta);
   int ieta=0;
   if (aeta<0.4) ieta=0;
   else if (aeta<0.8) ieta=1;
   else if (aeta<1.1) ieta=2;
   else if (aeta<1.37) ieta=3;
   else if (aeta<1.52) ieta=4;
   else if (aeta<1.80) ieta=5;
   else if (aeta<2.10) ieta=6;
   else                ieta=7;


   double energyGeV = energy*0.001;
   double et = energyGeV/cosh(eta);

   int ibinEt=etBins->GetSize()-2;
   for (int i=1;i<etBins->GetSize();i++) {
      if (et<etBins->GetAt(i)) {
         ibinEt=i-1;
         break;
      }
   }

//   cout << " energy, et , ibinEt " << energyGeV << " " << et << " " << ibinEt << endl;

   Double_t scale=0.;
   if (aeta<1.80) scale=0.;
   else if (aeta<2.3) scale = 0.050*(aeta-1.8)/0.4 /0.05;
   else scale=0.025/0.05;
 
   if( !interpolate ) {
     if (particle_type==0) return scale*helec[ieta]->GetBinContent(ibinEt+1);
     else if (particle_type==1) return scale*hphot[ieta]->GetBinContent(ibinEt+1);
     else if (particle_type==2) return scale*hphot2[ieta]->GetBinContent(ibinEt+1);
     else return -1; // not possibile because of first return
   } else {
     if (particle_type==0) return scale*helec[ieta]->Interpolate(et);
     else if (particle_type==1) return scale*hphot[ieta]->Interpolate(et);
     else if (particle_type==2) return scale*hphot2[ieta]->Interpolate(et);
     else return -1; // not possibile because of first return
   }

}
