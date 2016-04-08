/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_BeamTrack_FPCONFIG_H
#define ALFA_BeamTrack_FPCONFIG_H
// configuration structure for FPTracker
#include <string>
 
     struct FPConfig {
     
	  bool UseALFA;
	  std::string ConfDir;
	  
	  
	  //configuration parameters for FPTracker
	  int    IP;
	//  float  xinner0;
	  bool   useaper; //user appature true false
	  double apermb; // MB aperture constant in mm (0 = nominal 22.02 mm)
	  double xcol1;//colimator settings
	  double xcol2;
	  double pbeam0; //beam energy
		
	  float RPDistance;
	  
	  //float  endMarker; //RP Position
	  float  absZMagMax; //dont read magnets after this value
	  
	  
     
	  //Other Configuration Variables to add for FPTracker
     };
#endif
