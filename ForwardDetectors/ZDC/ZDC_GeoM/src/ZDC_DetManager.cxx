/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_GeoM/ZDC_DetManager.h"

ZDC_DetManager::ZDC_DetManager()
{
  setName("ZDC");
  
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " CONSTRUCTOR OF DETMANAGER :: getNumTreeTops() = "
					    << getNumTreeTops() << endreq;
}

ZDC_DetManager::~ZDC_DetManager()
{  
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " DESTRUCTOR OF DETMANAGER start:: getNumTreeTops() = " 
					    << getNumTreeTops() << endreq;
  
  for (unsigned int i=0; i<volume.size(); i++) volume[i]->unref();

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " DESTRUCTOR OF DETMANAGER end:: getNumTreeTops() = " 
					    << getNumTreeTops() <<endreq;
}

unsigned int ZDC_DetManager::getNumTreeTops() const
{
   return volume.size(); 
}

PVConstLink ZDC_DetManager::getTreeTop(unsigned int i) const
{
   return volume[i];
}

void  ZDC_DetManager::addTreeTop(PVLink vol)
{
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " ADDTREETOP OF DETMANAGER start:: getNumTreeTops() = " 
					    << getNumTreeTops() << endreq;
 
  vol->ref();
  
  volume.push_back(vol);
  
  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " ADDTREETOP OF DETMANAGER end:: getNumTreeTops() = " 
					    << getNumTreeTops() << endreq;
}

