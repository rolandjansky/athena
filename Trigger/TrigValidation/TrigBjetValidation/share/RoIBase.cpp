#include "Ntuple.h"
#include "RoIBase.h"


int RoIBase::checkQuark(Ntuple *nt, double deltaRCut) {

  deltaRCut=0.3;

  bool foundB=false, foundC=false, foundtau=false;
  double drMin=1000;

  for (unsigned int q=0; q<nt->NPar; q++) {

    if ((*nt->GenStat)[q]!=3 && (*nt->PtGen)[q]>5000) {

      double detaq  = m_eta-(*nt->EtaGen)[q];
      double dphiq  = phiCorr(phiCorr(m_phi)-phiCorr((*nt->PhiGen)[q]));
      double drq    = sqrt(pow(detaq,2)+pow(dphiq,2));

      if (abs((*nt->Type)[q]) == 5)
        if (drq<drMin) drMin=drq;
     
      if (drq < deltaRCut) {

        if (abs((*nt->Type)[q]) == 5)  foundB  =true;
        if (abs((*nt->Type)[q]) == 4)  foundC  =true;
        if (abs((*nt->Type)[q]) == 15) foundtau=true;
      }
    } 
  }

  if (foundB) 
    return 5;
  else if (foundC) 
    return 4;
  else if (foundtau) 
    return 15;
  else 
    return 0;

}


double RoIBase::phiCorr(double phi){
  if (phi < -M_PI) phi += 2*M_PI;
  if (phi >  M_PI) phi -= 2*M_PI;
  return phi;
}
