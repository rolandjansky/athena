/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_DetManager.h"
#include "AthenaKernel/getMessageSvc.h"

ZDC_DetManager::ZDC_DetManager()
  : AthMessaging (Athena::getMessageSvc(), "ZDC_DetManager")
{
  setName("ZDC");
  
  ATH_MSG_DEBUG( " CONSTRUCTOR OF DETMANAGER :: getNumTreeTops() = "
                 << ZDC_DetManager::getNumTreeTops() );
}

ZDC_DetManager::~ZDC_DetManager()
{  
  ATH_MSG_DEBUG( " DESTRUCTOR OF DETMANAGER start:: getNumTreeTops() = " 
                 << ZDC_DetManager::getNumTreeTops() );
  
  for (unsigned int i=0; i<m_volume.size(); i++) m_volume[i]->unref();

  ATH_MSG_DEBUG( " DESTRUCTOR OF DETMANAGER end:: getNumTreeTops() = " 
                 << ZDC_DetManager::getNumTreeTops() );
}

unsigned int ZDC_DetManager::getNumTreeTops() const
{
   return m_volume.size(); 
}

PVConstLink ZDC_DetManager::getTreeTop(unsigned int i) const
{
   return m_volume[i];
}

void  ZDC_DetManager::addTreeTop(PVLink vol)
{
  ATH_MSG_DEBUG( " ADDTREETOP OF DETMANAGER start:: getNumTreeTops() = " 
                 << getNumTreeTops() );
 
  vol->ref();
  
  m_volume.push_back(vol);
  
  ATH_MSG_DEBUG( " ADDTREETOP OF DETMANAGER end:: getNumTreeTops() = " 
                 << getNumTreeTops() );
}

