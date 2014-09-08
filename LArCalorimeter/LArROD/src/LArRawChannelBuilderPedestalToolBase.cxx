/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderPedestalToolBase.h"
#include "GaudiKernel/MsgStream.h"

LArRawChannelBuilderPedestalToolBase::LArRawChannelBuilderPedestalToolBase(const std::string& type,
								     const std::string& name,
								     const IInterface* parent):
  LArRawChannelBuilderToolBaseClass(type,name,parent)
{
  declareInterface<ILArRawChannelBuilderPedestalToolBase>( this );
}
