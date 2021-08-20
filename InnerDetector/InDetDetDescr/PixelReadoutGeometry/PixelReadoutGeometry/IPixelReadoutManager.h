/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPIXELREADOUTMANAGER_H
#define IPIXELREADOUTMANAGER_H

#include <cstdint>

#include <GaudiKernel/IInterface.h>
#include <PixelReadoutDefinitions/PixelReadoutDefinitions.h>

class Identifier;
class IdentifierHash;


namespace InDetDD
{

class IPixelReadoutManager : virtual public IInterface
{
public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IPixelReadoutManager, 1, 0);
  virtual ~IPixelReadoutManager() = default;

  virtual PixelModuleType getModuleType(Identifier id) const = 0;
  virtual PixelDiodeType getDiodeType(Identifier id) const = 0;

  virtual Identifier getPixelIdfromHash(IdentifierHash offlineIdHash,
                                        uint32_t FE,
                                        uint32_t row,
                                        uint32_t column) const = 0;
  virtual Identifier getPixelId(Identifier offlineId,
                                uint32_t FE,
                                uint32_t row,
                                uint32_t column) const = 0;

  virtual uint32_t getFE(Identifier diodeId,
                         Identifier offlineId) const = 0;
  virtual uint32_t getColumn(Identifier diodeId,
                             Identifier offlineId) const = 0;
  virtual uint32_t getRow(Identifier diodeId,
                          Identifier offlineId) const = 0;
};

}

#endif
