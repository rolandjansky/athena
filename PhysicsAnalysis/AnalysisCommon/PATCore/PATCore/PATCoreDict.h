/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __PATCOREDICT__
#define __PATCOREDICT__

/**
   @brief For dictionary generation.

   @author Karsten Koeneke (CERN)
   @date   October 2011

*/

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "PATCore/TAccept.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "PATCore/TSelectorToolBase.h"
#include "PATCore/TResult.h"
#include "PATCore/TCalculatorToolBase.h"
#include "PATCore/IAsgSelectionTool.h"
#include "PATCore/IAsgSelectionWithVertexTool.h"

#ifndef ROOTCORE
#include "PATCore/IAthSelectorTool.h"
#include "PATCore/IAthCalculatorTool.h"
#include "PATCore/IAthHistogramTool.h"
#include "PATCore/IUserDataCalcTool.h"
#endif

#endif
