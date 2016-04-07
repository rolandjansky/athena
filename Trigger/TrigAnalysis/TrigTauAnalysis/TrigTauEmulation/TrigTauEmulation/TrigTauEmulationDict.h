/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUEMULATION_TRIGTAUEMULATIONDICT_H
#define TRIGTAUEMULATION_TRIGTAUEMULATIONDICT_H

#include "TrigTauEmulation/ILevel1EmulationTool.h"

#include "TrigTauEmulation/IJetRoISelectionTool.h"
#include "TrigTauEmulation/JetRoISelectionTool.h"

#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/HltEmulationTool.h"
#include "TrigTauEmulation/ToolsRegistry.h"
#include "TrigTauEmulation/EmTauSelectionTool.h"
#include "TrigTauEmulation/FastTrackSelectionTool.h"
#include "TrigTauEmulation/Level1TopoSelectionTool.h"
#include "TrigTauEmulation/TrigTauORLTool.h"

#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"

#ifdef ASGTOOL_STANDALONE

// Helper macro for declaring the setProperty functions to the dictionary:
#define SETPROPERTY_INSTAN( TYPE )                                            \
   template StatusCode asg::AsgTool::setProperty< TYPE >( const std::string&, \
                                                          const TYPE& )

// Declare all possible setProperty template instantiations to Reflex:
SETPROPERTY_INSTAN( ToolHandle<TrigTauEmul::ILevel1EmulationTool> );
SETPROPERTY_INSTAN( ToolHandleArray<IJetRoISelectionTool> );

// Make the compiler forget about this macro now...
#undef SETPROPERTY_INSTAN
#endif


#endif
