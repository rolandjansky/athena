/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
using std::fabs;

#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"
        
     
fineEtaPhiBinning::fineEtaPhiBinning(void){
    absetabins = std::vector<double>(0);
    absetabins.push_back(0.);
    absetabins.push_back(0.21);
    absetabins.push_back(0.42); 
    absetabins.push_back(0.63);  
    absetabins.push_back(0.84); 
    absetabins.push_back(1.05); 
    absetabins.push_back(1.37);   
    absetabins.push_back(1.52);   
    absetabins.push_back(1.74);  
    absetabins.push_back(1.95); 
    absetabins.push_back(2.18);  
    absetabins.push_back(2.75); // extended the last bin to cover forward region
//     absetabins.push_back(2.5); 
    
}
        
int fineEtaPhiBinning::getSector(double phi) const {

  int sector = -9;
  if (phi>2.905 || phi<=-2.905) sector = 9;
  else if (phi>2.59 && phi<=2.905) sector = 8;
  else if (phi>2.12 && phi<=2.59) sector = 7;
  else if (phi>1.805 && phi<=2.12) sector = 6;
  else if (phi>1.335 && phi<=1.805) sector = 5;
  else if (phi>1.02 && phi<=1.335) sector = 4;
  else if (phi>0.55 && phi<=1.02) sector = 3;
  else if (phi>0.235 && phi<=0.55) sector = 2;
  else if (phi>-0.235 && phi<=0.235) sector = 1;
  else if (phi>-0.55 && phi<=-0.235) sector = 16;
  else if (phi>-1.02 && phi<=-0.55) sector = 15;
  else if (phi>-1.335 && phi<=-1.02) sector = 14;
  else if (phi>-1.805 && phi<=-1.335) sector = 13;
  else if (phi>-2.12 && phi<=-1.805) sector = 12;
  else if (phi>-2.59 && phi<=-2.12) sector = 11;
  else if (phi>-2.905 && phi<=-2.59) sector = 10;
  return sector;

}

int fineEtaPhiBinning::getECSector(double phi) const {

  int sector = -9;
  if (phi>3.011 || phi<=-3.011) sector = 9;
  else if (phi>2.487 && phi<=3.011) sector = 8;
  else if (phi>2.225 && phi<=2.487) sector = 7;
  else if (phi>1.702 && phi<=2.225) sector = 6;
  else if (phi>1.440 && phi<=1.702) sector = 5;
  else if (phi>0.916 && phi<=1.440) sector = 4;
  else if (phi>0.655 && phi<=0.916) sector = 3;
  else if (phi>0.131 && phi<=0.655) sector = 2;
  else if (phi>-0.131 && phi<=0.131) sector = 1;
  else if (phi>-0.655 && phi<=-0.131) sector = 16;
  else if (phi>-0.916 && phi<=-0.655) sector = 15;
  else if (phi>-1.440 && phi<=-0.916) sector = 14;
  else if (phi>-1.702 && phi<=-1.440) sector = 13;
  else if (phi>-2.225 && phi<=-1.702) sector = 12;
  else if (phi>-2.487 && phi<=-2.225) sector = 11;
  else if (phi>-3.011 && phi<=-2.487) sector = 10;
  return sector;

}


int fineEtaPhiBinning::getPhiSector(const TLorentzVector & m) const {
    
    return (fabs(m.Eta()) < 1.19 ? getSector(m.Phi()) : getECSector(m.Phi()));

}

int fineEtaPhiBinning::getEtaBin(const TLorentzVector & mst) const
{
    double eta = mst.Eta();
    double signeta = eta >= 0 ? 1 : -1;
    int addeta = signeta < 0 ? 0 : -1;
    for (unsigned int k = 1; k < absetabins.size(); k++){
        if (absetabins[k-1] < fabs(eta) && fabs(eta) < absetabins[k]){
            return signeta * (k)+addeta; 
        }
    }
    return signeta * 99;
}

int fineEtaPhiBinning::bin(const TLorentzVector & m) const{
    
    int etabin = getEtaBin(m);
    int phibin = getPhiSector(m);
    return phibin + 16 * etabin;
    
}
