/* emacs: this is -*- c++ -*- */
/**
 **     @file    AtlasStyle.h
 **
 **     @brief   ATLAS Style, based on a style file from BaBar
 **
 **     @author  mark sutton
 **     @date    Thu 25 Mar 2010 10:34:20 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  ANALYSIS_ATLASSTYLE_H
#define ANALYSIS_ATLASSTYLE_H

#include "TStyle.h"

void SetAtlasStyle();

TStyle* AtlasStyle(); 

#endif // ANALYSIS_ATLASSTYLE_H
