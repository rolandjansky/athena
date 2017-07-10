/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONPERFORMANCEFUNCTIONS_CXX
#define ELECTRONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setElectronWorkingPoint(ElectronCutLevel cutLevel) {
  m_eleCutLevel = cutLevel;
}

void UpgradePerformanceFunctions::setElectronRandomSeed(unsigned seed) {
  m_electronRandom.SetSeed(seed);
}

float UpgradePerformanceFunctions::getElectronSmearedEnergy(float eMeV, float eta) {
  float deltaE = m_electronRandom.Gaus(0., getElectronEnergyResolution(eMeV, eta));
  return (eMeV+deltaE);
}


//*********************************************************************
float UpgradePerformanceFunctions::getElectronEfficiency(float ptMeV, float eta) 
//*********************************************************************
//---- Get the electron ID efficiency
{
  
  //---- old code 
  //   if (m_eleCutLevel==looseElectron) {
  //     return 0.97 - 0.103*exp(1.-ptGeV/15.0);
  //   } else if (m_eleCutLevel==tightElectron) {
  //     return 0.85 - 0.191*exp(1.-ptGeV/20.0);
  //   }
  //   std::cout << "getElectronEfficiency: Invalid working point." << std::endl;
  //   return 0.0;

  float ptGeV = ptMeV / 1000.;

  //---- define all the different efficiency maps
  const int nEtBins=6;
  const int netaBins=9;
 
  //---- Loose efficiencies
  const float ElEff_Step1p6_mu200_Loose[nEtBins][netaBins] = {
    {0.811966, 0.157915, 0.774224, 0.255651, 0.683, 0.673, 0.496, 0.549, 0.593 }, // Et[  7 -  15]
    {0.742899, 0.156085, 0.750086, 0.251285, 0.660, 0.736, 0.447, 0.622, 0.650 }, // Et[ 15 -  20]
    {0.880800, 0.196373, 0.857302, 0.276088, 0.676, 0.783, 0.475, 0.704, 0.714 }, // Et[ 20 -  30]
    {0.911897, 0.203653, 0.886948, 0.294688, 0.624, 0.831, 0.482, 0.766, 0.766 }, // Et[ 30 -  50]
    {0.929496, 0.222480, 0.907982, 0.294609, 0.705, 0.851, 0.537, 0.808, 0.812 }, // Et[ 50 -  80]
    {0.942924, 0.240126, 0.924407, 0.273356, 0.649, 0.855, 0.702, 0.890, 0.800 }, // Et[ 80 - 120]
  };
  //---- Medium efficiencies
  const float ElEff_Step1p6_mu200_Medium[nEtBins][netaBins] = { // Uses Tight Gold mu=200 for forward electrons
    {0.711072, 0.121269, 0.686265, 0.222915, 0.390, 0.494, 0.311, 0.343, 0.394 }, // Et[  7 -  15]
    {0.665538, 0.129042, 0.662286, 0.217019, 0.476, 0.538, 0.298, 0.395, 0.470 }, // Et[ 15 -  20]
    {0.806082, 0.152595, 0.778103, 0.253627, 0.497, 0.598, 0.301, 0.467, 0.495 }, // Et[ 20 -  30]
    {0.837797, 0.174947, 0.817937, 0.281170, 0.478, 0.652, 0.311, 0.538, 0.556 }, // Et[ 30 -  50]
    {0.904484, 0.202508, 0.876919, 0.288759, 0.527, 0.689, 0.340, 0.594, 0.595 }, // Et[ 50 -  80]
    {0.932139, 0.224329, 0.913220, 0.273356, 0.514, 0.671, 0.421, 0.589, 0.600 }, // Et[ 80 - 120]
  };
  //---- Tight efficiencies
  const float ElEff_Step1p6_mu200_Tight[nEtBins][netaBins] = {
    {0.502564, 0.0865130, 0.511916, 0.179267, 0.390, 0.494, 0.311, 0.343, 0.394 }, // Et[  7 -  15]
    {0.477559, 0.0881834, 0.479972, 0.151342, 0.476, 0.538, 0.298, 0.395, 0.470 }, // Et[ 15 -  20]
    {0.610466, 0.0885553, 0.575190, 0.182265, 0.497, 0.598, 0.301, 0.467, 0.495 }, // Et[ 20 -  30]
    {0.622935, 0.1266680, 0.625772, 0.221215, 0.478, 0.652, 0.311, 0.538, 0.556 }, // Et[ 30 -  50]
    {0.780432, 0.1690660, 0.752847, 0.250731, 0.527, 0.689, 0.340, 0.594, 0.595 }, // Et[ 50 -  80]
    {0.883690, 0.1974720, 0.850847, 0.242215, 0.514, 0.671, 0.421, 0.589, 0.600 }, // Et[ 80 - 120]
  };


  float *mapToUse = (float *) ElEff_Step1p6_mu200_Loose;

  if(m_layout == UpgradePerformanceFunctions::Step1p6){
    if(m_avgMu == 200){
      if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Step1p6_mu200_Loose;
      else if(m_eleCutLevel==UpgradePerformanceFunctions::mediumElectron) mapToUse = (float *) ElEff_Step1p6_mu200_Medium;
      else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Step1p6_mu200_Tight;
      else std::cout << "getElectronEfficiency: working point." << std::endl;
    } else {
      std::cout << "getElectronEfficiency: Invalid <mu> value." << std::endl;
    }
  } else{
    std::cout << "getElectronEfficiency: Invalid layout." << std::endl;
  }


  float efficiency = 1.;
  efficiency = interpolateElectronIDMap(mapToUse, ptGeV, eta);

  // HGTD hack to improve Electron ID/ISO efficiency
  if (m_bUseHGTD0 || m_bUseHGTD1){
    if (fabs(eta) > 2.4 && fabs(eta) < 3.6)
      efficiency = 1.2*efficiency;
  }



  if(efficiency>1.0) efficiency = 1.0;

  return efficiency;

}


//*********************************************************************
float UpgradePerformanceFunctions::interpolateElectronIDMap(float *ptEtaMap,float pT,float eta)
//*********************************************************************
{
  //---- get efficiency value from the 2D maps: linear interpolation in pT and no interpolation in eta

  const int netaBins=9+1;
  const float etaBins[netaBins]={0.00,1.37,1.52,2.37,2.47,2.50,3.16,3.35,4.00,4.90};
  const int nEtBins=6+1;
  const float EtBins[nEtBins]={7.0,15.0,20.0,30.0,50.0,80.0,120.00};

  //---- if pt is out of range, use values estimated at the boundaries
  if(pT < 7) pT = 7.1;
  if(pT >=120) pT =119.; 
  if( fabs(eta)>4.9 ) return 0.;	//---- no electrons after |eta|=4.9


  //--- find associated bin eta
  int etaBin =0;
  for( int ietaBin = 0 ; ietaBin < netaBins ; ietaBin++ ){
    if( fabs(eta) < etaBins[ietaBin] ) {etaBin = ietaBin-1; break;}
  }


  //--- linear interpolation in pT
  int EtBin=0.;
  for(int ipT=0;ipT<nEtBins-1;ipT++){
    float binCenter=(EtBins[ipT+1]-EtBins[ipT])/2.+EtBins[ipT];
    //	printf("bin center =%f \n",binCenter);
    if(pT<binCenter) {EtBin=ipT; break;}
  }
  if(EtBin==0) EtBin=1;
  if(pT>(EtBins[nEtBins-1]-EtBins[nEtBins -2])/2.+EtBins[nEtBins -2]) EtBin = nEtBins -2;

  float y0=ptEtaMap[etaBin+(EtBin-1)*(netaBins-1)];
  float y1=ptEtaMap[etaBin+EtBin*(netaBins-1)];
  float x0=(EtBins[EtBin]-EtBins[EtBin-1])/2.+EtBins[EtBin-1];
  float x1=(EtBins[EtBin+1]-EtBins[EtBin])/2.+EtBins[EtBin];
  
  float value = y0 +(y1-y0)/(x1-x0) * (pT-x0);
  //   printf("ID Interpolation: pt=%f eta=%f ptBin=%d etaBin=%d y0=%f y1=%f x0=%f x1=%f eff_int=%f \n",
  //   		pT, eta, EtBin,etaBin,y0,y1,x0,x1,value);

  return value;
}


//*********************************************************************
float UpgradePerformanceFunctions::getElectronFakeRate(float ptMeV, float eta) 
//*********************************************************************
{
  //---- returns the probability to find a fake electron in a true jet
  //----  pt and eta passed as arguments are the one of the true jet.


  //----- old code
  //   if (m_eleCutLevel==looseElectron) {
  //     return 0.11*exp(-0.033*ptGeV);
  //   } else if (m_eleCutLevel==tightElectron) {
  //     return 0.0048*exp(-0.035*ptGeV);
  //   }
  //   std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
  //   return 0.0;

  float ptGeV = ptMeV / 1000.;

  //---- define all the different rejection maps
  const int nEtBins=6;
  const int netaBins=9;

  //---- Loose rejections
  const float ElRej_Step1p6_mu200_Loose[nEtBins][netaBins] = { 
    {0.00195988, 0.00216694, 0.00192640, 0.001870430, 0.03387, 0.21455, 0.03159, 0.05765, 0.10045 }, // Et[  7 -  15]
    {0.00151187, 0.00166038, 0.00111048, 0.000465166, 0.03352, 0.07307, 0.00990, 0.02623, 0.05692 }, // Et[ 15 -  20]
    {0.01049130, 0.01070800, 0.01025600, 0.004284760, 0.01399, 0.07198, 0.01366, 0.03694, 0.05564 }, // Et[ 20 -  30]
    {0.02967210, 0.02160850, 0.01595040, 0.009497750, 0.01000, 0.07853, 0.00737, 0.04065, 0.08466 }, // Et[ 30 -  50]
    {0.02899700, 0.01360980, 0.01074280, 0.009661120, 0.00200, 0.06102, 0.00377, 0.02259, 0.08377 }, // Et[ 50 -  80]
    {0.01316430, 0.00536150, 0.00440815, 0.004994890, 0.00200, 0.02148, 0.00400, 0.01124, 0.08000 }, // Et[ 80 - 120]
  };
  //---- Medium rejections
  const float ElRej_Step1p6_mu200_Medium[nEtBins][netaBins] = { // Uses Tight for forward
    {0.000543203, 0.00101639, 0.001031270, 0.000934764, 0.00178, 0.04728, 0.00776, 0.01083, 0.05098 }, // Et[  7 -  15]
    {0.000545234, 0.00077875, 0.000603672, 0.000155274, 0.00400, 0.01879, 0.00275, 0.00351, 0.02338 }, // Et[ 15 -  20]
    {0.003723280, 0.00600581, 0.005556440, 0.002600780, 0.00200, 0.02239, 0.00216, 0.00693, 0.02708 }, // Et[ 20 -  30]
    {0.008094550, 0.00918458, 0.007661610, 0.006000600, 0.00200, 0.02764, 0.00246, 0.00976, 0.03101 }, // Et[ 30 -  50]
    {0.005466250, 0.00500072, 0.004162790, 0.002824940, 0.00200, 0.01569, 0.00300, 0.00821, 0.03074 }, // Et[ 50 -  80]
    {0.001913070, 0.00125468, 0.001579960, 0.001103200, 0.00200, 0.00983, 0.00400, 0.00900, 0.03000 }, // Et[ 80 - 120]
  }; 
  //---- Tight rejections
  const float ElRej_Step1p6_mu200_Tight[nEtBins][netaBins] = { 
    {0.000254955, 0.000338584, 0.000272351, 8.71700e-07, 0.00178, 0.04728, 0.00776, 0.01083, 0.05098 }, // Et[  7 -  15]
    {0.000200977, 0.000104911, 0.000151485, 0.000154067, 0.00400, 0.01879, 0.00275, 0.00351, 0.02338 }, // Et[ 15 -  20]
    {0.001551070, 0.002399770, 0.001684880, 0.000680364, 0.00200, 0.02239, 0.00216, 0.00693, 0.02708 }, // Et[ 20 -  30]
    {0.003018910, 0.002903950, 0.002375240, 0.001279900, 0.00200, 0.02764, 0.00246, 0.00976, 0.03101 }, // Et[ 30 -  50]
    {0.001606890, 0.001138090, 0.000816277, 0.001022920, 0.00200, 0.01569, 0.00300, 0.00821, 0.03074 }, // Et[ 50 -  80]
    {0.000484336, 0.000187961, 0.000255402, 0.000796447, 0.00200, 0.00983, 0.00400, 0.00900, 0.03000 }, // Et[ 80 - 120]
  }; 


  float *mapToUse = (float *) ElRej_Step1p6_mu200_Loose;

  if(m_layout == UpgradePerformanceFunctions::Step1p6){
    if(m_avgMu == 200){
      if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Step1p6_mu200_Loose;
      else if(m_eleCutLevel==UpgradePerformanceFunctions::mediumElectron) mapToUse = (float *) ElRej_Step1p6_mu200_Medium;
      else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Step1p6_mu200_Tight;
      else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
    } else {
      std::cout << "getElectronFakeRate: Invalid <mu> value." << std::endl;
    }
  } else{
    std::cout << "getElectronFakeRate: Invalid layout." << std::endl;
  }



  float efficiency = 1.;
  efficiency = interpolateElectronIDMap(mapToUse, ptGeV, eta);

  if(efficiency>1.0) efficiency = 1.0;
  if(efficiency<=0.0) efficiency = 0.0001;

  return efficiency;

}


//*********************************************************************
float UpgradePerformanceFunctions::getElectronEnergyResolution(float eMeV, float eta) 
//*********************************************************************
{
  //---- Returns the absolute energy resolution in MeV

  //---- old code 
  //   const double ET_pu=0.6; //0.6 GeV in ET for mu=80,in a trigger tower, after pu suppression; supposed identical for a cluster; taken from ATLAS-TDR-022-2013 page 25
  //   float S=0;
  //   float C=0;
  //   if (fabs(eta)<1.4) {
  //     S = 0.10;
  //     C = 0.010;
  //   }
  //   else {
  //     S = 0.15;
  //     C = 0.015;
  //   }
  //   
  //   double mu = m_avgMu;
  //   double sigma2 = 0.3*0.3 + ET_pu*ET_pu*cosh(eta)*cosh(eta)*mu/80. + S*S*eGeV + C*C*eGeV*eGeV;
  //   return sqrt(sigma2);


  float eGeV = eMeV / 1000.;


  //---- define all the different resolution maps
  const int nEtBins=6;
  const int netaBins=13;
  
  const float ElERes_Step1p6_mu200[nEtBins][netaBins] = {
    {0.1198100*100., 0.1802750*100., 0.1152670*100., 0.0983971*100., 23.463, 31.330, 30.128, 30.949, 31.443, 31.591, 66.794, 50.884, 41.477 }, // Et[  7 -  15]
    {0.0818551*100., 0.1259540*100., 0.0787411*100., 0.0671563*100., 20.472, 22.108, 26.614, 34.214, 21.778, 36.023, 41.312, 26.732, 38.689 }, // Et[ 15 -  20]
    {0.0584391*100., 0.1038040*100., 0.0565978*100., 0.0525409*100.,  4.708, 15.902, 18.687, 25.653, 18.251, 21.885, 29.429, 29.209, 27.694 }, // Et[ 20 -  30]
    {0.0392075*100., 0.0779442*100., 0.0393392*100., 0.0407998*100.,  4.059, 10.854, 12.880, 22.009, 13.843, 17.826, 21.827, 18.746, 23.942 }, // Et[ 30 -  50]
    {0.0284292*100., 0.0615827*100., 0.0300611*100., 0.0351892*100.,  2.645,  7.612,  9.939, 21.029, 10.993, 12.557, 19.503, 11.262, 19.107 }, // Et[ 50 -  80]
    {0.0206782*100., 0.0481260*100., 0.0226532*100., 0.0312146*100.,  3.379,  7.322, 12.745, 25.013, 12.438, 11.777, 13.288, 15.000, 18.000 }, // Et[ 80 - 120]
  };

  float *mapToUse = (float *) ElERes_Step1p6_mu200;

  if(m_layout == UpgradePerformanceFunctions::Step1p6){
    if(m_avgMu == 200){
      mapToUse = (float *) ElERes_Step1p6_mu200;
    } else {
      std::cout << "getElectronEnergyResolution: Invalid <mu> value." << std::endl;
    }
  } else{
    std::cout << "getElectronEnergyResolution: Invalid layout." << std::endl;
  }



  float resolution = 1.;
  float Et = eGeV/cosh(eta);
  resolution = interpolateElectronResolutionMap(mapToUse, Et, eta);
  resolution = resolution/100. * eMeV; 

  return resolution;
  
}


//*********************************************************************
float UpgradePerformanceFunctions::interpolateElectronResolutionMap(float *ptEtaMap,float pT,float eta)
//*********************************************************************
{
  //---- get resolution value from the 2D maps: linear interpolation in pT and no interpolation in eta


  const int nEtBins=6+1;
  const float EtBins[nEtBins]={7.0,15.0,20.0,30.0,50.0,80.0,120.00};
  const int netaBins=13+1;
  const float etaBins[netaBins]={0.00,1.37,1.52,2.37,2.47,2.50,2.80,3.16,3.35,3.70,4.00,4.30,4.60,4.90};



  //---- if pt or eta is out of range, use values estimated at the boundaries
  if(pT < 7) pT = 7.1;
  if(pT >=120) pT =119.; 
  if( fabs(eta)>4.9 ) eta = 4.8;	


  //--- find associated bin in eta
  int etaBin =0;
  for( int ietaBin = 0 ; ietaBin < netaBins ; ietaBin++ ){
    if( fabs(eta) < etaBins[ietaBin] ) {etaBin = ietaBin-1; break;}
  }


  //--- linear interpolation in pT
  int EtBin=0.;
  for(int ipT=0;ipT<nEtBins-1;ipT++){
    float binCenter=(EtBins[ipT+1]-EtBins[ipT])/2.+EtBins[ipT];
    //	printf("bin center =%f \n",binCenter);
    if(pT<binCenter) {EtBin=ipT; break;}
  }
  if(EtBin==0) {EtBin=1;}
  if(pT>(EtBins[nEtBins-1]-EtBins[nEtBins -2])/2.+EtBins[nEtBins -2]) {EtBin = nEtBins -2;}
  
  float y0=ptEtaMap[etaBin+(EtBin-1)*(netaBins-1)];
  float y1=ptEtaMap[etaBin+EtBin*(netaBins-1)];
  float x0=(EtBins[EtBin]-EtBins[EtBin-1])/2.+EtBins[EtBin-1];
  float x1=(EtBins[EtBin+1]-EtBins[EtBin])/2.+EtBins[EtBin];
  
  float value = y0 +(y1-y0)/(x1-x0) * (pT-x0);
 
  //  printf("Resol Interpolation: pt=%f eta=%f ptBin=%d etaBin=%d y0=%f y1=%f x0=%f x1=%f eff_int=%f \n",
  //   		pT, eta, EtBin,etaBin,y0,y1,x0,x1,value);

  return value;
}

float UpgradePerformanceFunctions::getElectronFakeRescaledEnergy(float eMeV, float eta) 
{
  // 
  //   if (m_eleCutLevel == looseElectron) {
  //     float binning[20] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 200};
  //     float scales[20] = {-0.60, -0.636549, -0.677891, -0.701189, -0.707942, -0.703792, -0.701427, -0.700264, -0.70114, -0.701068, -0.698009, -0.692705, -0.681838, -0.678387, -0.676639, -0.668481, -0.663736, -0.668471, -0.666521, -0.665515};
  //     float sigmas[20] = {0.13, 0.132724, 0.143584, 0.145539, 0.149064, 0.153829, 0.156095, 0.157976, 0.160515, 0.162841, 0.169783, 0.174657, 0.183426, 0.185493, 0.189531, 0.194585, 0.201598, 0.195728, 0.202998, 0.206324};
  //     
  //     //Checking which bin we are in
  //     int i=0;
  //     while (i<19 && eMeV/1000.>binning[i+1]) i++;
  //     
  //     return eMeV * (1 + m_electronRandom.Gaus(scales[i],sigmas[i]) );
  //   } else if (m_eleCutLevel == tightElectron) {
  //     float binning[20] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 200};
  //     float scales[20] = {-0.5, -0.541424, -0.601461, -0.621471, -0.621278, -0.608145, -0.622162, -0.624346, -0.622605, -0.637172, -0.620544, -0.637961, -0.637561, -0.610907, -0.630854, -0.60472, -0.622769, -0.64527, -0.610933, -0.612421};
  //     float sigmas[20] = {0.17, 0.174439, 0.183794, 0.191502, 0.196909, 0.195764, 0.185789, 0.182931, 0.184728, 0.185637, 0.201431, 0.198198, 0.204317, 0.215402, 0.220042, 0.228437, 0.224068, 0.196602, 0.215806, 0.22049};
  //     
  //     //Checking which bin we are in
  //     int i=0;
  //     while (i<19 && eMeV/1000.>binning[i+1]) i++;
  //     
  //     return eMeV * (1 + m_electronRandom.Gaus(scales[i],sigmas[i]) );
  //   }
  //   std::cout << "getElectronFakeRescaledEnergy: Invalid working point." << std::endl;
  //   return 0.;

  float eGeV = eMeV / 1000.;

  //---- define all the different resolution maps
  const int nEtBins=6;
  const int netaBins=13;
  
  const float FakeElERes_Step1p6_mu200[nEtBins][netaBins] = { 
    {0.215850*100., 0.2509200*100., 0.226899*100., 0.2177890*100., 26.483, 29.914, 25.624, 19.891, 26.264, 40.642, 37.070, 25.230, 27.114 }, // Et[  7 -  15]
    {0.127096*100., 0.0954196*100., 0.137824*100., 0.1581890*100., 19.351, 19.388, 21.214, 21.687, 22.560, 24.960, 28.403, 19.745, 18.065 }, // Et[ 15 -  20]
    {0.108781*100., 0.1208100*100., 0.123372*100., 0.0899702*100., 17.189, 26.337, 26.394, 27.880, 28.668, 28.830, 19.176, 20.914, 21.044 }, // Et[ 20 -  30]
    {0.114487*100., 0.1151550*100., 0.123138*100., 0.1039210*100., 28.864, 26.548, 28.442, 21.397, 31.808, 33.471, 32.299, 34.021, 13.374 }, // Et[ 30 -  50]
    {0.132072*100., 0.1280030*100., 0.146805*100., 0.1293320*100., 17.310, 25.298, 30.297, 22.810, 24.702, 40.313, 25.989, 13.648, 13.000 }, // Et[ 50 -  80]
    {0.149085*100., 0.1444860*100., 0.163099*100., 0.1728600*100., 20.000, 29.422, 26.174, 18.309, 27.016, 30.000, 25.000, 13.000, 13.000 }, // Et[ 80 - 120]
  }; 

  const float FakeElEMean_Step1p6_mu200[nEtBins][netaBins] = { 
    {-0.129779*100., -0.0694363*100., -0.120145*100., -0.192361*100., -41.114, -37.021, -35.317, -42.786, -38.173, -27.559, -21.585, -35.194, -35.063 }, // Et[  7 -  15]
    {-0.469581*100., -0.4570630*100., -0.450520*100., -0.458052*100., -59.498, -55.379, -51.320, -54.775, -50.341, -44.135, -41.046, -45.722, -52.332 }, // Et[ 15 -  20]
    {-0.608841*100., -0.6018530*100., -0.594601*100., -0.642869*100., -62.387, -57.335, -56.578, -57.478, -54.888, -47.901, -53.553, -53.412, -49.805 }, // Et[ 20 -  30]
    {-0.694832*100., -0.7042020*100., -0.681389*100., -0.717625*100., -56.299, -64.534, -63.959, -70.303, -52.127, -42.937, -42.434, -43.946, -61.434 }, // Et[ 30 -  50]
    {-0.740818*100., -0.7518000*100., -0.720020*100., -0.760622*100., -88.036, -68.803, -62.959, -70.482, -44.580, -64.827, -47.059, -70.419, -66.659 }, // Et[ 50 -  80]
    {-0.737606*100., -0.7561220*100., -0.711455*100., -0.777686*100., -83.808, -68.931, -67.335, -83.441, -73.737, -42.267, -44.499, -30.000, -50.000 }, // Et[ 80 - 120]
  };

  float resolution = 1.;
  float mean = 1.;
  float Et = eGeV/cosh(eta);
  resolution = interpolateElectronResolutionMap((float *) FakeElERes_Step1p6_mu200, Et, eta);
  resolution = resolution/100.; 

  mean = interpolateElectronResolutionMap((float *) FakeElEMean_Step1p6_mu200, Et, eta);
  mean = mean/100.; 

  float fakeElEnergy = eMeV * (1 + m_electronRandom.Gaus(mean,resolution) );

  //printf("Fake el. energy = %f [ mean=%f res=%f ] Et=%f eta=%f\n",fakeElEnergy/1000.,mean/1000.,resolution/1000., Et, eta); 

  return fakeElEnergy;

}

//*********************************************************************
float UpgradePerformanceFunctions::getElectronChargeFlipProb(float /*eMeV*/, float eta)
//*********************************************************************
{
  if(m_layout == UpgradePerformanceFunctions::Step1p6){
    if(m_avgMu == 200){
      if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron)
	{
	  if(eta<=1.37) return 0.000574553;
	  else if(eta<=1.52) return 0.00112754;
	  else if(eta<=2.37) return 0.0028518;
	  else if(eta<=2.47) return 0.006875;
	  else
	    {
	      std::cout<<"WARNING from ElectronPerformanceFunctions: No track for electrons with eta>2.47! Setting ElectronChargeFlipProb to -1"<<std::endl;
	      return -1;
	    }
	  //return 0.00150572; //Inclusive number
	}
      else if(m_eleCutLevel==UpgradePerformanceFunctions::mediumElectron)
	{
	  if(eta<=1.37) return 0.000517473;
	  else if(eta<=1.52) return 0.00118747;
	  else if(eta<=2.37) return 0.0029035;
	  else if(eta<=2.47) return 0.00714048;
	  else
	    {
	      std::cout<<"WARNING from ElectronPerformanceFunctions: No track for electrons with eta>2.47! Setting ElectronChargeFlipProb to -1"<<std::endl;
	      return -1;
	    }
	  //return 0.00143039; //Inclusive number
	}
      else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron)
	{
	  if(eta<=1.37) return 0.000393824;
	  else if(eta<=1.52) return 0.00166078;
	  else if(eta<=2.37) return 0.00292826;
	  else if(eta<=2.47) return 0.00722175;
	  else
	    {
	      std::cout<<"WARNING from ElectronPerformanceFunctions: No track for electrons with eta>2.47! Setting ElectronChargeFlipProb to -1"<<std::endl;
	      return -1;
	    }
	  //return 0.00133191; //Inclusive number
	}
      else std::cout << "getElectronEfficiency: working point." << std::endl;
    } else {
      std::cout << "getElectronEfficiency: Invalid <mu> value." << std::endl;
    }
  } else{
    std::cout << "getElectronEfficiency: Invalid layout." << std::endl;
  }

  // Return error value if there is a problem
  return -1;

}

#endif
