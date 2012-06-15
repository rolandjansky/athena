/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoMaterialPropertyVector.h"
#include <stdexcept>
#include <iostream>

bool GeoMaterialPropertyVector::operator ++()
{
  CurrentEntry++;
  if(CurrentEntry < NumEntries) 
    return true;
  else
    return false; 
}

GeoMaterialPropertyVector& GeoMaterialPropertyVector::operator =(const GeoMaterialPropertyVector& right)
{
  if (this == &right) return *this;
	
  for(unsigned i=0; i<MPV.size(); ++i)
    delete MPV[i];
  MPV.clear();

  NumEntries = 0;
  CurrentEntry = -1;
  
  for(int i = 0 ; i < right.NumEntries; i++) 
  {
    GeoMPVEntry *newElement = new GeoMPVEntry(right.GetEntry(i));
    MPV.push_back(newElement);
    NumEntries++;
  }

  return *this;
}

GeoMaterialPropertyVector::GeoMaterialPropertyVector(double *PhotonMomenta,
						     double *PropertyValues,
						     int     NumElements)
{
  NumEntries = 0;
  CurrentEntry = -1;

  for(int i = 0; i < NumElements; i++) 
    AddElement(PhotonMomenta[i], PropertyValues[i]);
}

GeoMaterialPropertyVector::GeoMaterialPropertyVector(const GeoMaterialPropertyVector &right)
{
  NumEntries = 0;
  CurrentEntry = -1;
  
  for(int i = 0 ; i < right.NumEntries; i++) 
  {
    GeoMPVEntry *newElement = new GeoMPVEntry(right.GetEntry(i));
    MPV.push_back(newElement);
    NumEntries++;
  }
}

GeoMaterialPropertyVector::~GeoMaterialPropertyVector()
{
  for(unsigned i=0; i<MPV.size(); ++i)
    delete MPV[i];
  MPV.clear();
}

void GeoMaterialPropertyVector::ResetIterator()
{
  CurrentEntry = -1;
}

void GeoMaterialPropertyVector::AddElement(double aPhotonMomentum,
					   double aPropertyValue) 
{
  GeoMPVEntry *newElement;

  newElement = new GeoMPVEntry(aPhotonMomentum, aPropertyValue);
  MPV.push_back(newElement);
  NumEntries++; 
}

double GeoMaterialPropertyVector::GetProperty() const
{
  if(CurrentEntry == -1 || CurrentEntry >= NumEntries) 
    throw std::runtime_error("GeoMaterialPropertyVector::GetProperty ==>Iterator attempted to Retrieve Property out of range");
  else
    return MPV[CurrentEntry]->GetProperty();
}

double GeoMaterialPropertyVector::GetPhotonMomentum() const
{
  if(CurrentEntry == -1 || CurrentEntry >= NumEntries) 
    throw std::runtime_error("GeoMaterialPropertyVector::GetPhotonMomentum ==>Iterator attempted to Retrieve Photon Momentum out of range");
  else 
    return MPV[CurrentEntry]->GetPhotonMomentum();
}

void GeoMaterialPropertyVector::DumpVector()
{
  if (MPV.empty())  
  {
    std::cerr << "nothing to dump\n";
    throw std::runtime_error("GeoMaterialPropertyVector::DumpVector ==>Nothing to dump!  Vector is empty");
  }

  for (int i = 0; i < NumEntries; i++) 
  {
    std::cout << "MPV["<< i << "]: ";
    MPV[i]->DumpEntry();
  }
  std::cout << " Done DumpVector of " << NumEntries << " entries\n";

} 

GeoMPVEntry GeoMaterialPropertyVector::GetEntry(int i) const
{
  return *MPV[i];
}
