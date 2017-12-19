/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/CandidateMatchHelpers.h"

double CandidateMatchHelpers::CorrectedEta(double clusterEta,double z_first,bool isEndCap){    
  if (clusterEta == -999){
    return clusterEta;
  }
  //===========================================================//
  //These are at the face of the calorimeter
  const  double Rcalo = 1500;
  const  double Zcalo = 3700;
  double Rclus=Rcalo;
  double Zclus=Zcalo;    
  if(clusterEta!=0){
    //eta !=0
    double tanthetaclus=(1/cosh(clusterEta))/tanh(clusterEta); 
    if (isEndCap) {
      Rclus=fabs(Zcalo*(tanthetaclus));
      //Negative Eta ---> negative Z
      if(clusterEta<0){
	Zclus = -Zclus;
      }
    } 	  
    else{
      if(tanthetaclus!=0){
	Zclus=Rcalo/(tanthetaclus);
      }
    }
  }
  else{
    //eta 0
    Zclus=0;
  }
  //===========================================================//    
  //correct for a possible shift in the track perigee wrt to (0,0)
  double thetaclus_corrected=atan2(Rclus,Zclus-z_first);
  double etaclus_corrected = 99999; //if theta =0 or M_PI the eta +-inf this happens when Rclus =0 
  if(Rclus!=0){
    etaclus_corrected = -log(tan(thetaclus_corrected/2));
  }
  //===========================================================//
  return etaclus_corrected;
}

double CandidateMatchHelpers::PhiROT(double Et,double Eta, int charge, double r_first ,bool isEndCap){
  //===========================================================//
  //Used in order to derive the formula below
  const  double Rcalo = 1200;
  //===========================================================//
  // correct phi for extrapolation to calo
  double phiRot = 0.0;
  double ecCorr = 1.0;
  if (isEndCap) {
    double ecFactor = 1.0/(0.8*atan(15.0/34.0));
    double sinTheta0 = 2.0*atan(exp(-fabs(Eta)));
    ecCorr = sinTheta0*sqrt(sinTheta0)*ecFactor;
  }
  ////
  double Rscaled =(Rcalo-r_first)*(1./Rcalo);
  phiRot = Rscaled*ecCorr*charge*430./(Et);    
  return  phiRot;
}

