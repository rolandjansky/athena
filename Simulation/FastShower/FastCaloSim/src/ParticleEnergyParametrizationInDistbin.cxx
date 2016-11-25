/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/ParticleEnergyParametrizationInDistbin.h"
#include "TBuffer.h"

using namespace std;

ParticleEnergyParametrizationInDistbin::ParticleEnergyParametrizationInDistbin():TNamed("","") 
{
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) m_ElayerProp[i]=0;
}

void ParticleEnergyParametrizationInDistbin::SetNoDirectoryHisto()
{
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) if(m_ElayerProp[i]) {
    m_ElayerProp[i]->SetDirectory(0);
  }
}

//______________________________________________________________________________
void ParticleEnergyParametrizationInDistbin::Streamer(TBuffer &R__b)
{
   // Stream an object of class ParticleEnergyParametrizationInDistbin.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TNamed::Streamer(R__b);
      int R__i;
      for (R__i = 0; R__i < 24; R__i++)
         R__b >> m_ElayerProp[R__i];
      m_mean.Streamer(R__b);
      m_RMS.Streamer(R__b);
      m_corr.Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, ParticleEnergyParametrizationInDistbin::IsA());

      // check for valid histograms and compute integral for random numbers
      for (R__i = 0; R__i < 24; R__i++) {
        TH1* h1=m_ElayerProp[R__i];
        if(!h1) continue;
        double integ=h1->Integral();
        if(integ<=0) {
          delete h1;
          m_ElayerProp[R__i]=0;
          continue;
        }
        double* fIntegral=h1->GetIntegral();
        if(!fIntegral) h1->ComputeIntegral();
        fIntegral=h1->GetIntegral();
        if(!fIntegral) {
          delete h1;
          m_ElayerProp[R__i]=0;
          continue;
        }
      }
   } else {
      R__c = R__b.WriteVersion(ParticleEnergyParametrizationInDistbin::IsA(), kTRUE);
      TNamed::Streamer(R__b);
      int R__i;
      for (R__i = 0; R__i < 24; R__i++)
         R__b << (TObject*)m_ElayerProp[R__i];
      m_mean.Streamer(R__b);
      m_RMS.Streamer(R__b);
      m_corr.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

ClassImp(ParticleEnergyParametrizationInDistbin)
