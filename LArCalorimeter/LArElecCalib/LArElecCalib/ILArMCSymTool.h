/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTOOLS_ILARMCSYMTOOL_H
#define LARTOOLS_ILARMCSYMTOOL_H

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/IAlgTool.h"

class Identifier;
class HWIdentifier;

static const InterfaceID IID_ILArMCSymTool("ILArMCSymTool", 1 , 0);

/** @brief interface Tool for converting LAr readout identifier to a subset of the identifiers.
Used in MC when symmetry can be assumed. 

*/

class ATLAS_NOT_THREAD_SAFE ILArMCSymTool: virtual public IAlgTool
{
 public:
    /// interface ID
  static const InterfaceID& interfaceID() { return IID_ILArMCSymTool; }

  // destructor 
  virtual ~ILArMCSymTool() {}
  
  // retrieve methods 
  virtual HWIdentifier symOnline(const HWIdentifier & id) const=0;
  virtual HWIdentifier symOnline(const Identifier & id) const=0;

};




#endif
