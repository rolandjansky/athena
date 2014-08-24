//
//   @file    TrigSuperRoi.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
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


