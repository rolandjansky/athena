#include "egammaLayerRecalibTool/corr_pileupShift.h"
#include <iostream>
#include <string>

#include "PathResolver/PathResolver.h"


corr_pileupShift::corr_pileupShift(){

  std::string filename = PathResolverFindCalibFile("egammaLayerRecalibTool/v2/layer_average.root");
    
  m_file = TFile::Open(filename.c_str());
  if (not m_file or m_file->IsZombie()) {
    std::cerr << "FATAL: cannot open " << filename << std::endl;
  }
  else {
    for (int layer=0;layer<4;layer++) {
      char name[12];
      snprintf(name,sizeof(name),"average_%d",layer);
      m_haverage[layer]=(TH1D*) (m_file->Get(name));
      if (not m_haverage[layer]) {
	  std::cerr << "FATAL: cannot find " << name << std::endl;
	}
    }
  }
  m_runMin=296939;
  m_runMax=311563;
}
//===============================================================================
corr_pileupShift::~corr_pileupShift()
{
    m_file->Close();
}

//===============================================================================
float corr_pileupShift::getCorr(int layer, int run, float mu, float eta) const
{
   
   if (layer<0 || layer>3) return 0.;
   if (run<m_runMin || run>m_runMax) return 0.;
   double aeta=std::fabs(eta);
   if (aeta> 2.5) return 0.;
   if (aeta>2.37) aeta=2.37;

   int ibin = m_haverage[layer]->FindBin(aeta);
   float corr = mu * m_haverage[layer]->GetBinContent(ibin);

   return corr;

}
