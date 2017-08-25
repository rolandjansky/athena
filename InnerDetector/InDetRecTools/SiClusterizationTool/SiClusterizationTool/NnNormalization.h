/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NnNormalization_H
#define NnNormalization_H

double norm_rawToT(const double input);

double back_rawToT(const double input);

double norm_ToT(const double input);

double back_ToT(const double input);

double norm_pitch(const double input,bool addIBL=false);

double back_pitch(const double input,const bool addIBL=false);

double norm_layerNumber(const double input);

double back_layerNumber(const double input);

double norm_layerType(const double input);

double back_layerType(const double input);

double norm_phi(const double input);

double back_phi(const double input);

double norm_theta(const double input);

double back_theta(const double input);

double norm_phiBS(const double input);

double back_phiBS(const double input);

double norm_thetaBS(const double input);

double back_thetaBS(const double input);

double norm_etaModule(const double input);

double back_etaModule(const double input);

double norm_posX(const double input,const bool recenter=false);

double back_posX(const double input,const bool recenter=false);

double norm_posY(const double input);

double back_posY(const double input);

double norm_errorX(const double input);

double back_errorX(const double input);

double norm_errorY(const double input);

double back_errorY(const double input);

double errorHalfIntervalX(const int nParticles);

double errorHalfIntervalY(const int nParticles);

#endif
