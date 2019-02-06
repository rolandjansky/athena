/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef PATINTERFACES_GLOBAL_H
#define PATINTERFACES_GLOBAL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.

namespace CP
{
  class CorrectionCode;
  template<class T> class ICorrectionTool;
  class MakeSystematicsVector;
  class ISystematicsTool;
  class SystematicCode;
  class SystematicVariation;
  class SystematicSet;
  class SystematicsTool;
}

bool operator < (const CP::SystematicVariation& a, const CP::SystematicVariation& b);
bool operator == (const CP::SystematicVariation& a, const CP::SystematicVariation& b);

bool operator < (const CP::SystematicSet& a, const CP::SystematicSet& b);
bool operator == (const CP::SystematicSet& a, const CP::SystematicSet& b);

#endif
