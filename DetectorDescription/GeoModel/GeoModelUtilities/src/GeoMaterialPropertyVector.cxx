/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoMaterialPropertyVector.h"
#include <stdexcept>
#include <iostream>

bool GeoMaterialPropertyVector::operator ++()
{
  m_CurrentEntry++;
  if(m_CurrentEntry < m_NumEntries) 
    return true;
  else
    return false; 
}

GeoMaterialPropertyVector& GeoMaterialPropertyVector::operator =(const GeoMaterialPropertyVector& right)
{
  if (this == &right) return *this;
	
  for(unsigned i=0; i<m_MPV.size(); ++i)
    delete m_MPV[i];
  m_MPV.clear();

  m_NumEntries = 0;
  m_CurrentEntry = -1;
  
  for(int i = 0 ; i < right.m_NumEntries; i++) 
  {
    GeoMPVEntry *newElement = new GeoMPVEntry(right.GetEntry(i));
    m_MPV.push_back(newElement);
    m_NumEntries++;
  }

  return *this;
}

GeoMaterialPropertyVector::GeoMaterialPropertyVector(double *PhotonMomenta,
						     double *PropertyValues,
						     int     NumElements)
{
  m_NumEntries = 0;
  m_CurrentEntry = -1;

  for(int i = 0; i < NumElements; i++) 
    AddElement(PhotonMomenta[i], PropertyValues[i]);
}

GeoMaterialPropertyVector::GeoMaterialPropertyVector(const GeoMaterialPropertyVector &right)
{
  m_NumEntries = 0;
  m_CurrentEntry = -1;
  
  for(int i = 0 ; i < right.m_NumEntries; i++) 
  {
    GeoMPVEntry *newElement = new GeoMPVEntry(right.GetEntry(i));
    m_MPV.push_back(newElement);
    m_NumEntries++;
  }
}

GeoMaterialPropertyVector::~GeoMaterialPropertyVector()
{
  for(unsigned i=0; i<m_MPV.size(); ++i)
    delete m_MPV[i];
  m_MPV.clear();
}

void GeoMaterialPropertyVector::ResetIterator()
{
  m_CurrentEntry = -1;
}

void GeoMaterialPropertyVector::AddElement(double aPhotonMomentum,
					   double aPropertyValue) 
{
  GeoMPVEntry *newElement;

  newElement = new GeoMPVEntry(aPhotonMomentum, aPropertyValue);
  m_MPV.push_back(newElement);
  m_NumEntries++; 
}

double GeoMaterialPropertyVector::GetProperty() const
{
  if(m_CurrentEntry == -1 || m_CurrentEntry >= m_NumEntries) 
    throw std::runtime_error("GeoMaterialPropertyVector::GetProperty ==>Iterator attempted to Retrieve Property out of range");
  else
    return m_MPV[m_CurrentEntry]->GetProperty();
}

double GeoMaterialPropertyVector::GetPhotonMomentum() const
{
  if(m_CurrentEntry == -1 || m_CurrentEntry >= m_NumEntries) 
    throw std::runtime_error("GeoMaterialPropertyVector::GetPhotonMomentum ==>Iterator attempted to Retrieve Photon Momentum out of range");
  else 
    return m_MPV[m_CurrentEntry]->GetPhotonMomentum();
}

void GeoMaterialPropertyVector::DumpVector()
{
  if (m_MPV.empty())  
  {
    std::cerr << "nothing to dump\n";
    throw std::runtime_error("GeoMaterialPropertyVector::DumpVector ==>Nothing to dump!  Vector is empty");
  }

  for (int i = 0; i < m_NumEntries; i++) 
  {
    std::cout << "m_MPV["<< i << "]: ";
    m_MPV[i]->DumpEntry();
  }
  std::cout << " Done DumpVector of " << m_NumEntries << " entries\n";

} 

GeoMPVEntry GeoMaterialPropertyVector::GetEntry(int i) const
{
  return *m_MPV[i];
}
