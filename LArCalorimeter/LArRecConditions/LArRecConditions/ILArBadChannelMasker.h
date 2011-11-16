/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILArBadChannelMasker_H
#define ILArBadChannelMasker_H

#include "GaudiKernel/IAlgTool.h"
#include "CaloIdentifier/CaloGain.h"

class Identifier;
class HWIdentifier;
class LArBadChannel;

class ILArBadChannelMasker : public virtual IAlgTool
{
public:
   static const InterfaceID& interfaceID()
   { 
      static const InterfaceID IID_ILArBadChannelMasker("ILArBadChannelMasker", 1, 0);
      return IID_ILArBadChannelMasker; 
   }

   virtual ~ILArBadChannelMasker()
   {}

   virtual bool cellShouldBeMasked(const HWIdentifier& hardwareId, const int gain=CaloGain::UNKNOWNGAIN) const = 0;
   virtual bool cellShouldBeMasked(const Identifier& offlineId, const int gain=CaloGain::UNKNOWNGAIN) const = 0; 
   virtual bool cellShouldBeMaskedFEB(const HWIdentifier& febId, const int channelNumber, const int gain=CaloGain::UNKNOWNGAIN) const = 0;

   virtual bool isMaskingOn() const = 0;
};

#endif
