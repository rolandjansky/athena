/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelExamples/ToyDetectorManager.h"
#include "ForwardScrutinizer.h"
#include "CentralScrutinizer.h"

ToyDetectorManager::ToyDetectorManager()
{
  setName("Toy");
}


ToyDetectorManager::~ToyDetectorManager()
{
  // Clean up the central scrutinizers:  
  for (size_t ic=0;ic<m_centralScrutinizer.size();ic++) {  
    delete m_centralScrutinizer[ic];  
  }  
  // Clean up the forward scrutinizers:  
  for (size_t ix=0;ix<m_forwardScrutinizer.size();ix++) {  
    delete m_forwardScrutinizer[ix];  
  }  
}


unsigned int ToyDetectorManager::getNumTreeTops() const
{
  return m_volume.size(); 
}

PVConstLink ToyDetectorManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

const ForwardScrutinizer * ToyDetectorManager::getForwardScrutinizer(unsigned int i) const
{
  return m_forwardScrutinizer[i];
}

const CentralScrutinizer * ToyDetectorManager::getCentralScrutinizer(unsigned int i) const
{
  return m_centralScrutinizer[i];
}

unsigned int ToyDetectorManager::getNumScrutinizers(Type type) const
{
  return (type==CENTRAL) ? m_centralScrutinizer.size() : m_forwardScrutinizer.size();  
}

void  ToyDetectorManager::addTreeTop(PVLink vol){
  m_volume.push_back(vol);
}


void  ToyDetectorManager::addCentralScrutinizer(const CentralScrutinizer *scrut){
  m_centralScrutinizer.push_back(scrut);
}


void  ToyDetectorManager::addForwardScrutinizer(const ForwardScrutinizer *scrut){
  m_forwardScrutinizer.push_back(scrut);
}
