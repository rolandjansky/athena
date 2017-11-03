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
  for (size_t ic=0;ic<centralScrutinizer.size();ic++) {  
    delete centralScrutinizer[ic];  
  }  
  // Clean up the forward scrutinizers:  
  for (size_t ix=0;ix<forwardScrutinizer.size();ix++) {  
    delete forwardScrutinizer[ix];  
  }  
}


unsigned int ToyDetectorManager::getNumTreeTops() const
{
  return volume.size(); 
}

PVConstLink ToyDetectorManager::getTreeTop(unsigned int i) const
{
  return volume[i];
}

const ForwardScrutinizer * ToyDetectorManager::getForwardScrutinizer(unsigned int i) const
{
  return forwardScrutinizer[i];
}

const CentralScrutinizer * ToyDetectorManager::getCentralScrutinizer(unsigned int i) const
{
  return centralScrutinizer[i];
}

unsigned int ToyDetectorManager::getNumScrutinizers(Type type) const
{
  return (type==CENTRAL) ? centralScrutinizer.size() : forwardScrutinizer.size();  
}

void  ToyDetectorManager::addTreeTop(PVLink vol){
  volume.push_back(vol);
}


void  ToyDetectorManager::addCentralScrutinizer(const CentralScrutinizer *scrut){
  centralScrutinizer.push_back(scrut);
}


void  ToyDetectorManager::addForwardScrutinizer(const ForwardScrutinizer *scrut){
  forwardScrutinizer.push_back(scrut);
}
