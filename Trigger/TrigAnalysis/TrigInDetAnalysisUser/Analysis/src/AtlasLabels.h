/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    AtlasLabels.h         
//   
//   @author M.Sutton
// 
//
//   $Id: AtlasLabels.h, v0.0   Thu 25 Mar 2010 10:34:20 CET $


#ifndef __ATLASLABELS_H
#define __ATLASLABELS_H

#include "Rtypes.h"

void ATLASLabel( Double_t x, Double_t y, const std::string&  text="" , Color_t color=1, int nx=1, int ny=1 ); 
void      Label( Double_t x, Double_t y, const std::string&  text="" , Color_t color=1, int nx=1, int ny=1 ); 


void ATLASLabelOld(Double_t x,Double_t y,bool Preliminary=false,Color_t color=1); 

void ATLASVersion( const std::string& version="", Double_t x=0.88, Double_t y=0.975, Color_t color=1 ); 

#endif // __ATLASLABELS_H
