#include <iostream>

#include "Ntuple.h"
#include "RoI.h"
#include "RoIBase.h"


RoI::RoI(double phi, double eta): RoIBase(phi,eta) { lverb=false; }


void RoI::SetWeights(Ntuple* nt) {
  
  //* L2 *//
  int ind=-1;
  double drmax=1000;
  for (unsigned int ib=0;ib<nt->T2BjN;ib++){

    double dr = sqrt(pow((*nt->T2BjEta)[ib]-this->Eta(),2) + pow(phiCorr(phiCorr((*nt->T2BjPhi0)[ib]) - phiCorr(this->Phi())),2));

    if (dr<drmax){ drmax = dr; ind = ib; }
  }

  if (ind!=-1){

    if (drmax<0.05){

      l2Tag.xIP1D((*nt->T2BjXIP1D)[ind]); 
      l2Tag.xIP2D((*nt->T2BjXIP2D)[ind]); 
      l2Tag.xIP3D((*nt->T2BjXIP3D)[ind]); 
      l2Tag.xCHI2((*nt->T2BjXChi2)[ind]); 
      l2Tag.xSVtx((*nt->T2BjXSVtx)[ind]); 
      l2Tag.xMVtx((*nt->T2BjXMVtx)[ind]);
      l2Tag.xEVtx((*nt->T2BjXEVtx)[ind]);
      l2Tag.xNVtx((*nt->T2BjXNVtx)[ind]);

    } else
      if (lverb)
	std::cout << "RoI::SetTag Association with dr>0.05 " << drmax << std::endl;
  } else
    if (lverb)
      std::cout << "RoI::SetTag RoI not associated " << std::endl;
  
  //* EF *//
  ind=-1;
  drmax=1000;
  for (unsigned int ib=0;ib<nt->EFBjN;ib++){
    double dr = sqrt(pow((*nt->EFBjEta)[ib]  - this->Eta(),2)+
		     pow(phiCorr(phiCorr((*nt->EFBjPhi0)[ib]) - phiCorr(this->Phi())),2));
    if (dr<drmax){ drmax = dr; ind = ib; }
  }

  if (ind!=-1){
    if (drmax<0.05){

      efTag.xIP1D((*nt->EFBjXIP1D)[ind]); 
      efTag.xIP2D((*nt->EFBjXIP2D)[ind]); 
      efTag.xIP3D((*nt->EFBjXIP3D)[ind]); 
      efTag.xCHI2((*nt->EFBjXChi2)[ind]); 
      efTag.xSVtx((*nt->EFBjXSVtx)[ind]); 
      efTag.xMVtx((*nt->EFBjXMVtx)[ind]);
      efTag.xEVtx((*nt->EFBjXEVtx)[ind]);
      efTag.xNVtx((*nt->EFBjXNVtx)[ind]);

    } else
      if (lverb)
	std::cout << "Association with dr>0.05 " << drmax << std::endl;
  } else
    if (lverb)
      std::cout << "RoI not associated " << std::endl;

}
