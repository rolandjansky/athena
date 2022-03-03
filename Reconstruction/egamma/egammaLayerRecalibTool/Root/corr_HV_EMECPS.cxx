#include "egammaLayerRecalibTool/corr_HV_EMECPS.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <string>
#include <utility>

#include <TH2D.h>
#include <TFile.h>
#include <TSystem.h>

#include "PathResolver/PathResolver.h"


corr_HV_EMECPS::corr_HV_EMECPS(){
  std::string filename = PathResolverFindCalibFile("egammaLayerRecalibTool/v2/emecps_hv_corr_2015-2016_207X.root");
  
  m_file = TFile::Open(filename.c_str());
  if (not m_file or m_file->IsZombie()) {
    std::cerr << "FATAL: cannot open " << filename << std::endl;
  }
  else {
	m_HV[0]=(TH2D*) (m_file->Get("hc"));
        if (!m_HV[0]) {
            std::cerr << " cannot read histogram hc " << std::endl;
        }
	m_HV[1]=(TH2D*) (m_file->Get("ha"));
        if (!m_HV[1]) {
            std::cerr << " cannot read histogram hc " << std::endl;
        }
  }
}
//===============================================================================
corr_HV_EMECPS::~corr_HV_EMECPS()
{
    m_file->Close();
}

//===============================================================================
float corr_HV_EMECPS::getCorr(int run, float eta,float phi) const
{
   float newCorr=1.;
   if (eta>-1.5 && eta<1.5) return newCorr;
   if (eta<-1.85 || eta > 1.85) return newCorr;

   int iside=0;
   if (eta>0.) iside=1;

   if (!m_HV[iside]) {
      std::cerr << " cannot find histogram to apply correction " << std::endl;
   } else {
     int ibin = std::as_const(*m_HV[iside]).GetXaxis()->FindFixBin(((Double_t)(run+0.1)));
      if (ibin<1 || ibin>7) return newCorr;
      if (phi>3.14159) phi=phi-6.283185;
      int jbin = std::as_const(*m_HV[iside]).GetYaxis()->FindFixBin(phi);
      if (jbin<1 || jbin>2048) return newCorr;
      newCorr =  m_HV[iside]->GetBinContent(ibin,jbin); 
   }
   //std::cout << " in  corr_HV_EMECPS run,eta,phi,corr " << run << " " << eta << " " << phi << " " << newCorr << std::endl;
   return newCorr;

}
