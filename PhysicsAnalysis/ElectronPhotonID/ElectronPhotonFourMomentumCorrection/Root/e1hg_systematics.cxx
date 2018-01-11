/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonFourMomentumCorrection/e1hg_systematics.h"
#include "PathResolver/PathResolver.h"
#include <stdlib.h>
#include <math.h>

#include "TAxis.h"
#include "Riostream.h"

#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif

e1hg_systematics::e1hg_systematics()
{

  m_file0 = TFile::Open( PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/e1hg_systematics_histos.root").c_str() );


  for (Int_t ieta=0;ieta<8;ieta++) {
     char name[60];
     sprintf(name,"elec_%d",ieta);
     m_helec[ieta] = (TH1D*) m_file0->Get(name);
     sprintf(name,"unconv_%d",ieta);
     m_hphot[ieta] = (TH1D*) m_file0->Get(name);
     sprintf(name,"conv_%d",ieta);
     m_hphot2[ieta] = (TH1D*) m_file0->Get(name);

  }

  TAxis* aa=m_helec[0]->GetXaxis();
  m_etBins = aa->GetXbins();

}

//=========================================================================
e1hg_systematics::~e1hg_systematics()
{
  m_file0->Close();
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

   int ibinEt=m_etBins->GetSize()-2;
   for (int i=1;i<m_etBins->GetSize();i++) {
      if (et<m_etBins->GetAt(i)) {
         ibinEt=i-1;
         break;
      }
   }

//   cout << " energy, et , ibinEt " << energyGeV << " " << et << " " << ibinEt << endl;

   Double_t scale=0.;
   //HACK: some ES model dependency needs to be introduced
   /*Default up to es2017_summer
     if (aeta<1.80) scale=0.;
     else if (aeta<2.3) scale = 0.050*(aeta-1.8)/0.4 /0.05;
     else scale=0.025/0.05;
   */

   if (aeta<1.80) scale=0.;
   else if (aeta<2.3) scale = (0.050*(aeta-1.8)/0.4+0.025)/0.05;
   else scale = 0.05/0.05;
   
   if( !interpolate ) {
     if (particle_type==0) return scale*m_helec[ieta]->GetBinContent(ibinEt+1);
     else if (particle_type==1) return scale*m_hphot[ieta]->GetBinContent(ibinEt+1);
     else  return scale*m_hphot2[ieta]->GetBinContent(ibinEt+1); //This is 2, since if particle_type is not 0,1,2 we have returned 0 above
   } else {
     if (particle_type==0) return scale*m_helec[ieta]->Interpolate(et);
     else if (particle_type==1) return scale*m_hphot[ieta]->Interpolate(et);
     else  return scale*m_hphot2[ieta]->Interpolate(et); //This is 2, since if particle_type is not 0,1,2 we have returned 0
   }

}
