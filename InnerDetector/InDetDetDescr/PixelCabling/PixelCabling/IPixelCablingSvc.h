/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IPIXELCABLINGSVC_H
#define IPIXELCABLINGSVC_H

#include "GaudiKernel/IInterface.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

class IPixelCablingSvc: virtual public IInterface {

 public:
  virtual ~IPixelCablingSvc(){};
  static const InterfaceID& interfaceID( ) ;

  virtual Identifier getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column) = 0;
  virtual Identifier getPixelId(Identifier offlineId, uint32_t FE, uint32_t row, uint32_t column) = 0;

  virtual uint32_t getFE(Identifier * pixelId, Identifier offlineId) = 0;
  virtual uint32_t getColumn(Identifier * pixelId, Identifier offlineId) = 0;
  virtual uint32_t getRow(Identifier * pixelId, Identifier offlineId) = 0;

  virtual unsigned int getLocalFEI4(const uint32_t fe, const uint64_t onlineId) = 0;

  enum moduletype {DBM, IBL_PLANAR, IBL_3D, PIX_BARREL, PIX_ENDCAP, NONE};
  virtual moduletype getModuleType(const Identifier& id) = 0;

  enum pixeltype {NORMAL, LONG, GANGED};
  virtual pixeltype getPixelType(const Identifier& id) = 0;

};

inline const InterfaceID& IPixelCablingSvc::interfaceID(){
  static const InterfaceID IID("IPixelCablingSvc",1,0);
  return IID;
}

#endif
