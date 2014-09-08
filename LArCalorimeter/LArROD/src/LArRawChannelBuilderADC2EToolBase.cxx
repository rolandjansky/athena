/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderADC2EToolBase.h"
#include "GaudiKernel/MsgStream.h"

LArRawChannelBuilderADC2EToolBase::LArRawChannelBuilderADC2EToolBase(const std::string& type,
								     const std::string& name,
								     const IInterface* parent):
  LArRawChannelBuilderToolBaseClass(type,name,parent)
{
  declareInterface<ILArRawChannelBuilderADC2EToolBase>( this );
}
