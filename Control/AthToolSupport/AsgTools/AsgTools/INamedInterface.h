/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__I_NAMED_INTERFACE_H
#define ASG_TOOLS__I_NAMED_INTERFACE_H

#ifndef XAOD_STANDALONE

#include "GaudiKernel/INamedInterface.h"

#else

#include <string>

class INamedInterface
{
  /// virtual destructor for base class
public:
  virtual ~INamedInterface () noexcept = default;

  /// Return the name of the tool
public:
  virtual const std::string& name() const = 0;
};

#endif


#endif
