/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaLayerRecalibTool/corr_HV_EMBPS.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <TProfile2D.h>
#include <TFile.h>
#include <TSystem.h>

#include "PathResolver/PathResolver.h"


corr_HV_EMBPS::corr_HV_EMBPS(){
  std::string filename = PathResolverFindCalibFile("egammaLayerRecalibTool/v1/HVmaps_embps_2012.root");
  m_file = TFile::Open(filename.c_str());
  if (not m_file or m_file->IsZombie()) {
    std::cerr << "FATAL: cannot open " << filename << std::endl;
  }
  else {
    for (int iperiod=0;iperiod<6;iperiod++) {
      for (int igap=0;igap<2;igap++) {
	char name[12];
	snprintf(name,sizeof(name),"h%d%d",iperiod+1,igap);
	m_hHV[iperiod][igap]=(TProfile2D*) (m_file->Get(name));
	if (not m_hHV[iperiod][igap]) {
	  std::cerr << "FATAL: cannot find " << name << std::endl;
	}
      }
    }
  }
}
//===============================================================================
corr_HV_EMBPS::~corr_HV_EMBPS()
{
    m_file->Close();
}

//===============================================================================
float corr_HV_EMBPS::getCorr(int run, float eta,float phi) const
{
   if (std::fabs(eta) > 1.5) return 1.;
   if (run<200804) return 1.;
   int iperiod;
   if (run<204932) iperiod=0;
   else if (run<205112) iperiod=1;
   else if (run<211670) iperiod=2;
   else if (run<212619) iperiod=3;
   else if (run<212809) iperiod=4;
   else iperiod=5;
   //std::cout << "period " << iperiod << std::endl;

   int  ieta=(int)((eta+1.6)/0.4);
   if (ieta<0 || ieta>7) return 1.;
   if (phi<0.) phi=phi+2*M_PI;
   int iphi=(int)(phi*(32./(2*M_PI)));
   if (iphi<0) iphi=0;
   if (iphi>31) iphi=31;
   //std::cout << " ieta,iphi " << ieta << " " << iphi << std::endl;

   float hv1 = m_hHV[iperiod][0]->GetBinContent(ieta+1,iphi+1);
   float hv2 = m_hHV[iperiod][1]->GetBinContent(ieta+1,iphi+1);

   float c1= getRecoCorrection(hv1,eta);
   float c2= getRecoCorrection(hv2,eta);
   float delta1 = getDataCorrection(hv1,eta);
   float delta2 = getDataCorrection(hv2,eta);

   float extra1=1.;
   if (run<211670 && hv1<1300.) extra1 = getExtraScaleFactor();
   float extra2=1.;
   if (run<211670 && hv2<1300.) extra2 = getExtraScaleFactor();

   if ((c1+c2)<0.01) return 1.;
   float oldScale = (c1+c2);
   float newScale = (c1*delta1*extra1 + c2*delta2*extra2);

   float newCorr = oldScale/newScale;

   //std::cout << " run,eta,phi " << run << " " << eta << " " << phi << " " << " period "<< iperiod << std::endl;
   //std::cout << " HV values   " << hv1 << " " << hv2 << std::endl;
   //std::cout << " reco scales " << c1 << " " << c2 << std::endl;
   //std::cout << " delta scale " << delta1 << " " << delta2 << std::endl;
   //std::cout << " extra       " << extra1 << " " << extra2 << std::endl;
   //std::cout << " old/new scales " << oldScale << " " << newScale << std::endl;
   //std::cout << " newCorr " << newCorr << std::endl;

   return newCorr;

}

//===============================================================================
// return scale factor of response vs HV used in reconstruction
float corr_HV_EMBPS::getRecoCorrection(float hv,float eta) const
{
      float nominal = 2000.;
      float T = 88.37;

      int ieta=(std::fabs(eta)/0.025);
      float d;
      if (ieta>=0 && ieta<16)        d = 0.196; //cm
      else if (ieta>=16 && ieta<32)  d = 0.193; //cm
      else if (ieta>=32 && ieta<48)  d = 0.2; //cm
      else  d = 0.190; //cm

      if (hv>(nominal-2.)) return 1.;
      double efield=std::fabs(hv)/(d*1e+3);
      double enominal=nominal/(d*1e+3);
      double scale=Respo(efield, enominal,T);

      return scale;
}

//===============================================================================
float corr_HV_EMBPS::Respo(float e, float e_nominal,float tempe)  const
{
  if (e < -999.) return 1.;
  if (e < 0.01) return 0;
  if ( e > e_nominal ) return 1;
  float resp = (InvCharge(e_nominal)*vdrift(e,tempe))/(InvCharge(e)*vdrift(e_nominal,tempe));
  return resp;
}

//===============================================================================
float corr_HV_EMBPS::InvCharge(float e) const
{
  float q = 1.;
  if ( e > 2.) q=(1.+0.36/e);
  else if (e>1e-6) q =(1.04+0.25/e);
  return q;
}

//===============================================================================
float corr_HV_EMBPS::vdrift(float e, float tempe) const
{
  const float T = tempe;
  static const float P[6] = {-0.01481,-0.0075,0.141,12.4,1.627,0.317};
  if ( e < -999.) return 0.;
  float vd = (P[0]*T+1)*( P[2]*e*std::log(1+ (P[3]/e)) + P[4]*std::pow(e,P[5])) + P[1]*T; // vdrift formula walcowialk mm/micro_s
  return vd;
}


//===============================================================================
float corr_HV_EMBPS::getDataCorrection(float hv,float eta) const
//
// values are measured 1 +   <Eraw0_end of run>-<Eraw0 beginning of run> / <Eraw0>  (i.e <Eraw end>/<Eraw begin>)
//  for the sectors with both sides going from 1600->1200V from beginning to end of run
//  as a function of eta (possibly)
{
  if (hv>1300.) return 1.;
  if (hv>1000.) {
    if (eta<-1.2) return 0.9925;
    if (eta<-0.8) return 0.9918;
    if (eta<-0.4) return 0.9889;
    if (eta<0.) return 0.9935;
    if (eta<0.4) return 0.9908;
    if (eta<0.8) return 0.99197;
    if (eta<1.2) return 0.9974;
    return 0.9971;
  }
  return 1.015;
}

//===============================================================================
float corr_HV_EMBPS::getExtraScaleFactor() const
{
   return 1.013;
}
