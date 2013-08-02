/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTOOLS_ILARMCSYMTOOL_H
#define LARTOOLS_ILARMCSYMTOOL_H

#include "GaudiKernel/IAlgTool.h"

class Identifier;
class HWIdentifier;

static const InterfaceID IID_ILArMCSymTool("ILArMCSymTool", 1 , 0);

/** @brief interface Tool for converting LAr readout identifier to a subset of the identifiers.
Used in MC when symmetry can be assumed. 

*/

class ILArMCSymTool: virtual public IAlgTool
{
 public:
    /// interface ID
  static const InterfaceID& interfaceID() { return IID_ILArMCSymTool; }

  // destructor 
  virtual ~ILArMCSymTool() {}
  
  // retrieve methods 
  virtual HWIdentifier symOnline(const HWIdentifier & id)=0;
  virtual HWIdentifier symOnline(const Identifier & id)=0;

};




#endif
