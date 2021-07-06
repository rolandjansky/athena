/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "IdDict/IdDictDefs.h"
#include "Identifier/IdentifierHash.h"
#include "ZdcIdentifier/ZdcHardwareID.h"
#include "GaudiKernel/MsgStream.h"


ZdcHardwareID::ZdcHardwareID( void ) 
{
}

ZdcHardwareID::~ZdcHardwareID(void) 
{
}

int ZdcHardwareID::ppm(const HWIdentifier& /*hwid*/) 
{
  return 0;
}

int ZdcHardwareID::channel(const HWIdentifier& /*hwid*/) 
{
  return 0;
}

HWIdentifier ZdcHardwareID::ppm_id(int /*ppm*/) 
{
  return HWIdentifier(0);
}

HWIdentifier ZdcHardwareID::channel_id(int /*ppm*/, int /*channel*/) 
{
  return HWIdentifier(0);
}
