/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonFourMomentumCorrection/get_MaterialResolutionEffect.h"

#include "PathResolver/PathResolver.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TArrayD.h"

get_MaterialResolutionEffect::get_MaterialResolutionEffect()
  : asg::AsgMessaging("get_MaterialResolutionEffect")
{
  //std::cout << " Initialize get_MaterialResolutionEffect " << std::endl;

  const std::string filename = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/histos-systematics-material.root");
  
  m_file0.reset(TFile::Open( filename.c_str(), "READ" ));

  for (Int_t isys=0;isys<4;isys++) {
    for (Int_t ieta=0;ieta<8;ieta++) {
      for (Int_t iconv=0;iconv<3;iconv++) {   //0=electron, 1=unconverted, 2=converted
         char name[31];
         if (isys==0 && iconv==0) sprintf(name,"systElec_A_etaBin_%d",ieta);
         if (isys==1 && iconv==0) sprintf(name,"systElec_CD_etaBin_%d",ieta);
         if (isys==2 && iconv==0) sprintf(name,"systElec_EL_etaBin_%d",ieta);
         if (isys==3 && iconv==0) sprintf(name,"systElec_FMX_etaBin_%d",ieta);

         if (isys==0 && iconv==1) sprintf(name,"systUnconv_A_etaBin_%d",ieta);
         if (isys==1 && iconv==1) sprintf(name,"systUnconv_CD_etaBin_%d",ieta);
         if (isys==2 && iconv==1) sprintf(name,"systUnconv_EL_etaBin_%d",ieta);
         if (isys==3 && iconv==1) sprintf(name,"systUnconv_FMX_etaBin_%d",ieta);
         if (isys==0 && iconv==2) sprintf(name,"systConv_A_etaBin_%d",ieta);
         if (isys==1 && iconv==2) sprintf(name,"systConv_CD_etaBin_%d",ieta);
         if (isys==2 && iconv==2) sprintf(name,"systConv_EL_etaBin_%d",ieta);
         if (isys==3 && iconv==2) sprintf(name,"systConv_FMX_etaBin_%d",ieta);
         
         m_hSystPeak.at(isys).at(ieta).at(iconv).reset( dynamic_cast<TH1*>(m_file0->Get(name)));
         m_hSystPeak.at(isys).at(ieta).at(iconv)->SetDirectory( nullptr );
         if (!m_hSystPeak.at(isys).at(ieta).at(iconv).get()) ATH_MSG_FATAL("cannot find histogram " << name << " in file '" << filename << "'");


         if (isys==0 && iconv==0) sprintf(name,"systElec_sigmaG_A_etaBin_%d",ieta);
         if (isys==1 && iconv==0) sprintf(name,"systElec_sigmaG_CD_etaBin_%d",ieta);
         if (isys==2 && iconv==0) sprintf(name,"systElec_sigmaG_EL_etaBin_%d",ieta);
         if (isys==3 && iconv==0) sprintf(name,"systElec_sigmaG_FMX_etaBin_%d",ieta);

         if (isys==0 && iconv==1) sprintf(name,"systUnconv_sigmaG_A_etaBin_%d",ieta);
         if (isys==1 && iconv==1) sprintf(name,"systUnconv_sigmaG_CD_etaBin_%d",ieta);
         if (isys==2 && iconv==1) sprintf(name,"systUnconv_sigmaG_EL_etaBin_%d",ieta);
         if (isys==3 && iconv==1) sprintf(name,"systUnconv_sigmaG_FMX_etaBin_%d",ieta);
         if (isys==0 && iconv==2) sprintf(name,"systConv_sigmaG_A_etaBin_%d",ieta);
         if (isys==1 && iconv==2) sprintf(name,"systConv_sigmaG_CD_etaBin_%d",ieta);
         if (isys==2 && iconv==2) sprintf(name,"systConv_sigmaG_EL_etaBin_%d",ieta);
         if (isys==3 && iconv==2) sprintf(name,"systConv_sigmaG_FMX_etaBin_%d",ieta);

         m_hSystResol.at(isys).at(ieta).at(iconv).reset( dynamic_cast<TH1*>(m_file0->Get(name)));
         m_hSystResol.at(isys).at(ieta).at(iconv)->SetDirectory( nullptr );
         if (!(m_hSystResol.at(isys).at(ieta).at(iconv).get())) ATH_MSG_FATAL("cannot find histogram " << name << " in file '" << filename << "'");
      }      
    }
  }

   // IBL+PP0 material systematics stored in 2D file
  m_hsyst_IBL_PP0.at(0).reset(dynamic_cast<TH2*>(m_file0->Get("systElec_IBLPP0")));
  m_hsyst_IBL_PP0.at(0)->SetDirectory( nullptr );
  if (!(m_hsyst_IBL_PP0.at(0).get())) ATH_MSG_FATAL("cannot find histogram systElec_IBLPP0 in file '" << filename << "'");
  m_hsyst_IBL_PP0.at(1).reset(dynamic_cast<TH2*>(m_file0->Get("systUnconv_IBLPP0")));
  m_hsyst_IBL_PP0.at(1)->SetDirectory( nullptr );

  if (!(m_hsyst_IBL_PP0.at(1).get())) ATH_MSG_FATAL("cannot find histogram systUnconv_IBLPP0 in file '" << filename << "'");
  m_hsyst_IBL_PP0.at(2).reset(dynamic_cast<TH2*>(m_file0->Get("systConv_IBLPP0")));
  m_hsyst_IBL_PP0.at(2)->SetDirectory( nullptr );
  if (!(m_hsyst_IBL_PP0.at(2).get())) ATH_MSG_FATAL("cannot find histogram systConv_IBLPP0 in file '" << filename << "'");
   

  m_etBins=m_hSystResol.at(0).at(0).at(1)->GetXaxis()->GetXbins();
}

//=========================================================================
get_MaterialResolutionEffect::~get_MaterialResolutionEffect(){
  ///m_file0->Close();
}

//============================================================================
// inputs are particle_type (0=elec, 1=reco unconv photon, 2=reco conv photon, 3=true unconv photon)
//            energy in MeV
//            eta
//            response_type (0=gaussian core fit, 1=sigmaeff 80%, 2=sigma eff 90%)
//
// returned value is sigmaE/E change in quadrature to resolution
//
double get_MaterialResolutionEffect::getDelta(int particle_type, double energy, double eta,int response_type,int isyst) const
{

   //cout << " in getDelta " << endl;
   if (particle_type<0 || particle_type>2) return -999;
   if (response_type<0 || response_type>1) return -999;

   float aeta=fabs(eta);
   double energyGeV = energy*0.001;
   double et = energyGeV/cosh(eta);

// IBL+PP0
   if (isyst==5) {
      double et2=et;
      if (et<5.) et2=5.1;
      if (et>2000) et2=1999.;
      if (particle_type==3) particle_type=2;
      return 0.01*m_hsyst_IBL_PP0.at(particle_type)->GetBinContent(m_hsyst_IBL_PP0.at(particle_type)->GetXaxis()->FindBin(aeta),m_hsyst_IBL_PP0.at(particle_type)->GetYaxis()->FindBin(et2));
   }


   int ieta=0;
   if (aeta<0.4) ieta=0;
   else if (aeta<0.8) ieta=1;
   else if (aeta<1.1) ieta=2;
   else if (aeta<1.37) ieta=3;
   else if (aeta<1.52) ieta=4;
   else if (aeta<1.80) ieta=5;
   else if (aeta<2.10) ieta=6;
   else ieta=7;


   int ibinEt=m_etBins->GetSize()-2;
   for (int i=1;i<m_etBins->GetSize();i++) {
      if (et<m_etBins->GetAt(i)) {
         ibinEt=i-1;
         break;
      }
   }

   //cout << " in getDelta  isyst: " << isyst << " ieta " << ieta << " " << " particle_type " << particle_type << endl;
   //cout << " m_hSystResol " << m_hSystResol[isyst][ieta][particle_type] << endl;

   if (response_type==0) {
      return 0.01*m_hSystPeak.at(isyst).at(ieta).at(particle_type)->GetBinContent(ibinEt+1);
   }
   else {
      return 0.01*m_hSystResol.at(isyst).at(ieta).at(particle_type)->GetBinContent(ibinEt+1);
   }

}
