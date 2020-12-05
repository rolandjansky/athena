/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

#include "TProfile.h"
#include "TCanvas.h"

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect12c;
  ect12c.setESModel(egEnergyCorr::es2012c);  
  ect12c.initialize();
  
  TH1D h2012cNom("h2012cNom","h2012cNom",500,-5,5);
  TH1D h2012cUp("h2012cUp","h2012cUp",500,-5,5);
  TH1D h2012cDown("h2012cDown","h2012cDown",500,-5,5);

  for(int i=1; i<=h2012cNom.GetNbinsX(); i++) {
    
    double eta = h2012cNom.GetBinCenter(i);
    double pIn = 40000.;

    // es2012c

    double pNom = ect12c.getCorrectedMomentum( PATCore::ParticleDataType::Full, 
					       PATCore::ParticleType::Electron,
					       pIn,
					       eta,
					       egEnergyCorr::Scale::Nominal);

    double pUp = ect12c.getCorrectedMomentum( PATCore::ParticleDataType::Full, 
					      PATCore::ParticleType::Electron,
					      pIn,
					      eta,
					      egEnergyCorr::Scale::MomentumUp);

    double pDown = ect12c.getCorrectedMomentum( PATCore::ParticleDataType::Full, 
						PATCore::ParticleType::Electron,
						pIn,
						eta,
						egEnergyCorr::Scale::MomentumDown);

    std::cout << eta << " " << (pNom-pIn)/pIn << " " << (pUp-pIn)/pIn << " " << (pDown-pIn)/pIn 
	      << " " << pIn << " " << pNom << " " << pUp << " " << pDown << std::endl;

  }
 
 return 0;
}
