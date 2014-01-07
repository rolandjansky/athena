/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

double norm_rawToT(double input)
{
  throw;
  return (input-21.)/12.;
}

double back_rawToT(double input)
{
  throw;
  return input*12.+21.;
}


double norm_ToT(double input)
{
  return (input-15000.)/15000.;
}

double back_ToT(double input)
{
  return input*15000.+15000.;
}

double norm_pitch(double input)
{
  if (fabs(input-0.4)<0.001)
  {
    return -0.5;
  }
  if (fabs(input-0.6)<0.001)
  {
    return +0.5;
  }
  std::cout << " WARNING inpus is: " << input << std::endl;
  throw;
  return -1;
}

double back_pitch(double input)
{
  if (fabs(input-(-0.5))<0.001)
  {
    return 0.4;
  }
  if (fabs(input-0.5)<0.001)
  {
    return 0.6;
  }
  std::cout << " WARNING inpus is: " << input << std::endl;
  throw;
  return 0;
}

double norm_layerNumber(double input)
{
  return (input-0.95)/0.8;
}

double back_layerNumber(double input)
{
  return input*0.8+0.95;
}
double norm_layerType(double input)
{
  return input/0.65;
}
double back_layerType(double input)
{
  return input*0.65;
}

double norm_phi(double input)
{
  return (input-0.1)/0.1;
}

double back_phi(double input)
{
  return input*0.1+0.1;
}

double norm_theta(double input)
{
  return (input)/0.8;
}

double back_theta(double input)
{
  return input*0.8;
}
double norm_phiBS(double input)
{
  return (input-0.1)/0.1;
}

double back_phiBS(double input)
{
  return input*0.1+0.1;
}

double norm_thetaBS(double input)
{
  return (input)/0.8;
}

double back_thetaBS(double input)
{
  return input*0.8;
}
double norm_etaModule(double input)
{
  return input/2.;
}
double back_etaModule(double input)
{
  return input*2.;
}

double norm_posX(double input)
{
  return (input)/0.41;//was -0.058
}
double back_posX(double input)
{
  return input*0.41;//was -0.058
}
double norm_posY(double input)
{
  return (input-0.)/0.34;
}
double back_posY(double input)
{
  return input*0.34+0.;
}
double norm_errorX(double input)
{
  return (input-0.0005)/0.001;
}
double back_errorX(double input)
{
  return input*0.001+0.0005;
}
double norm_errorY(double input)
{
  return (input-0.018)/0.048;
}
double back_errorY(double input)
{
  return input*0.048+0.018;
}
double errorHalfIntervalX(int nParticles)
{
  if (nParticles==1)
  {
    return 0.03;
  }
  else
  {
    return 0.05;
  }
}
double errorHalfIntervalY(int nParticles)
{
  if (nParticles==1)
  {
    return 0.3;
  }
  else
  {
    return 0.4;
  }
}
