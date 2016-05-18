/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderToolBase.h"

LArRawChannelBuilderToolBase::LArRawChannelBuilderToolBase(const std::string& type,
							   const std::string& name,
							   const IInterface* parent):
  LArRawChannelBuilderToolBaseClass(type,name,parent),
  m_buildIt(nullptr),
  m_larRawChannelContainer(nullptr),
  m_energy(0),
  m_time(0),
  m_gain()
{
  declareInterface<ILArRawChannelBuilderToolBase>( this );
}
