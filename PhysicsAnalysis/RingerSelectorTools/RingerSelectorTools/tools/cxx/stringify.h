/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_STRINGIZE_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_STRINGIZE_H

// Simple stringify macro
#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)
#define __PASTE(x,y) x##y
#define PASTE(x) __PASTE(x,y)

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_STRINGIZE_H
