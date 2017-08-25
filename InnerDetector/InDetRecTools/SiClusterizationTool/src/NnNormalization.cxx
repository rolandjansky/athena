/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SiClusterizationTool/NnNormalization.h"
#include "SiClusterizationToolNormalizationException.h"
#include <cmath>
#include <limits>

inline bool 
roughlyEqual(const double d, const double target, const double tolerance=0.001){
	return (fabs(d-target)<tolerance);
}

double norm_rawToT(const double input){
  return (input-21.)/12.;
}

double back_rawToT(const double input){
  return input*12.+21.;
}

double norm_ToT(const double input){
  return (input-15000.)/15000.;
}

double back_ToT(const double input){
  return input*15000.+15000.;
}

double norm_pitch(const double input,const bool addIBL){
  if (!addIBL){
    if (roughlyEqual(input,0.4)) return -0.5;
    if (roughlyEqual(input,0.6)) return +0.5;
    //sroe, changed in response to ATLASRECTS-1827
    return std::numeric_limits<double>::quiet_NaN();
    //throw SiClusterizationToolNormalizationException();
    //return -1; //never reached!
  } else {
    return (input-0.37)*10.;
  }
}

double back_pitch(const double input,const bool addIBL){
  if (!addIBL){
    if (roughlyEqual(input,-0.5)) return 0.4;
    if (roughlyEqual(input,0.5))  return 0.6;
    return std::numeric_limits<double>::quiet_NaN();
    //throw SiClusterizationToolNormalizationException();
    //return 0; //never reached!
  } else {
    return input*0.10+0.37;
  }
}

double norm_layerNumber(const double input){
  return (input-0.95)/0.8;
}

double back_layerNumber(const double input){
  return input*0.8+0.95;
}
double norm_layerType(const double input){
  return input/0.65;
}
double back_layerType(const double input){
  return input*0.65;
}

double norm_phi(const double input){
  return (input-0.1)*10.;
}

double back_phi(const double input){
  return input*0.1+0.1;
}

double norm_theta(const double input){
  return (input)/0.8;
}

double back_theta(const double input){
  return input*0.8;
}
double norm_phiBS(const double input){
  return (input-0.1)*10.;
}

double back_phiBS(const double input){
  return input*0.1+0.1;
}

double norm_thetaBS(const double input){
  return (input)/0.8;
}

double back_thetaBS(const double input){
  return input*0.8;
}
double norm_etaModule(const double input){
  return input*0.5;
}
double back_etaModule(const double input){
  return input*2.;
}

double norm_posX(const double input,const bool recenter){
  if (recenter) return (input)/0.41;
  return (input-(-0.58))/0.41;
}

double back_posX(const double input,const bool recenter){
  if (recenter) return input*0.41;
  return input*0.41+(-0.58);
}

double norm_posY(const double input){
  return input/0.34;
}

double back_posY(const double input){
  return input*0.34+0.;
}
double norm_errorX(const double input){
  return (input-0.0005)*1000.;
}
double back_errorX(const double input){
  return input*0.001+0.0005;
}

double norm_errorY(const double input){
  return (input-0.018)/0.048;
}

double back_errorY(const double input){
  return input*0.048+0.018;
}

double errorHalfIntervalX(const int nParticles){
	return (nParticles==1) ? 0.03:0.05;
}

double errorHalfIntervalY(const int nParticles){
	return (nParticles==1) ? 0.3:0.4;
}


