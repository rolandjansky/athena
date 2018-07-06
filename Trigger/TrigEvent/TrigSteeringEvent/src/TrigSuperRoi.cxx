/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrigSuperRoi.cxx         
//   
//
//   @author M.Sutton
// 
//
//   $Id: TrigSuperRoi.cxx, v0.0   Thu  5 Dec 2013 16:13:43 CET sutt $


#include "TrigSteeringEvent/TrigSuperRoi.h"


TrigSuperRoi::TrigSuperRoi(bool fullscan) : 
  TrigRoiDescriptor(fullscan)
{
  setComposite(true);
} 

TrigSuperRoi::TrigSuperRoi(double eta, double etaMinus, double etaPlus, 
			   double phi, double phiMinus, double phiPlus, 
			   double zed, double zedMinus, double zedPlus) : 
  TrigRoiDescriptor( eta, etaMinus, etaPlus,
		     phi, phiMinus, phiPlus,
		     zed, zedMinus, zedPlus ) 
{  
  setComposite(true);
} 


// Destructor
TrigSuperRoi::~TrigSuperRoi() { }


