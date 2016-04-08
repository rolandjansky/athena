/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

double norm_nVTX(int nVTX)
{
  return (((double)nVTX)-0.3)/0.5;
}

int back_nVTX(double nVTX)
{
  return (int)std::floor(nVTX*0.5+0.3+0.5);
}

double norm_nTracksAtVtx(int nTracksAtVtx)
{
  return ((double)nTracksAtVtx-1.)/1.6;
}

int back_nTracksAtVtx(double nTracksAtVtx)
{
  return (int)std::floor(nTracksAtVtx*1.6+1.+0.5);
}

double norm_nSingleTracks(int nSingleTracks)
{
  return ((double)nSingleTracks-0.2)/0.5;
}

int back_nSingleTracks(double nSingleTracks)
{
  return (int)std::floor(nSingleTracks*0.5+0.2+0.5);
}


double norm_energyFraction(double energyFraction)
{
  return (energyFraction-0.23)/0.33;
}

double back_energyFraction(double energyFraction)
{
  return (energyFraction*0.33)+0.23;
}

double norm_mass(double mass)
{
  return (mass-974.)/1600.;
}

double back_mass(double mass)
{
  return (mass*1600.)+974.;
}

double norm_significance3d(double s3d)
{
  return (s3d-7.)/14.;
}

double back_significance3d(double s3d)
{
  return (s3d*14.)+7.;
}

double norm_IP3D(double ip3d)
{
  return (ip3d-6.3)/6.;
}

double back_IP3D(double ip3d)
{
  return (ip3d*6.)+6.3;
}


double norm_cat_pT(int cat_pT)
{
  return ((double)cat_pT-3.)/3.;
}

int back_cat_pT(double cat_pT)
{
  return (int)std::floor(cat_pT*3.+3.+0.5);
}

double norm_cat_eta(double cat_eta)
{
  return (double)(cat_eta-1.);
}

int back_cat_eta(int cat_eta)
{
  return (int)std::floor(cat_eta+1.+0.5);
}




