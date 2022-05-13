/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// AthMessaging.cxx 
// Implementation file for class AthMessaging
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaKernel/getMessageSvc.h"

AthMessaging::AthMessaging (IMessageSvc* msgSvc,
                            const std::string& name) :
  m_imsg(msgSvc), m_nm(name)
{
  MsgStream ms (msgSvc, name);
  m_lvl = ms.level();
}

AthMessaging::AthMessaging (const std::string& name) :
  AthMessaging(Athena::getMessageSvc(), name)
{
}

AthMessaging::~AthMessaging()
{}

void AthMessaging::setLevel (MSG::Level lvl)
{
  m_lvl = lvl;
}
