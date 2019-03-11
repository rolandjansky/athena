/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiPropertiesTool.h
 * @author Susumu.Oda@cern.ch
 **/
#ifndef ISiPropertiesTool_h
#define ISiPropertiesTool_h

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "SiPropertiesTool/SiliconProperties.h"
#include "Identifier/IdentifierHash.h"

/**
 * @class ISiPropertiesTool
 * Interface class for tool providing silicon properties (mobility, etc).
 * for each detector element. These depend on conditions such as temperature.
 **/

class ISiPropertiesTool: virtual public IAlgTool
{
 public:
  virtual ~ISiPropertiesTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISiPropertiesTool, 1, 0);

  /// Get Silicon properties for a given detector element.
  virtual const InDet::SiliconProperties& getSiProperties(const IdentifierHash& elementHash) const = 0;
};

#endif // ISiPropertiesTool_h
