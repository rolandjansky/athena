/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONSTANTS_H 
#define CONSTANTS_H

const double EPSILON     =      1.0E-6;
const double PROTONMASS  =    938.272;    //MeV
const double EMASS       =      0.511003;//MeV
const double MUMASS      =    105.65837; //MeV
const double PIMASS      =    139.57;    //MeV
const double JPSIMASS    =   3096.916;   //MeV
const double KMASS       =    493.677;   //MeV
const double PHIMASS     =   1020.;      //MeV
const double DSMASS      =   1968.2;     //MeV
const double KPLUSMASS   =    493.67700; //MeV
const double BPLUSMASS   =   5279.13;    //MeV
const double BSMASS      =   5369.3;     //MeV
const double KSTARMASS   =    891.66;    //MeV
const double BDMASS      =   5279.5;     //MeV
const double LAMBDAMASS  =   1115.683;   //MeV
const double LAMBDABMASS =   5620.2;     //MeV
const double BCMASS      =   6277.0;     //MeV

/// list of decay constants BMuMuX
const int di_to_muons = 0;      
const int b_to_K = 1;           // to recognize type of decay B+/- ->K+/-      mu+ mu-
const int bD_to_Kstar = 2; 		// to recognize type of decay Bd   ->K*(K+Pi-) mu+ mu-
const int bS_to_Phi = 3; 		// to recognize type of decay Bs   ->Phi(K+K-) mu+ mu-
const int lB_to_L = 4; 		    // to recognize type of decay Lb   ->L(PPi-)   mu+ mu-
const int bC_to_PiPi = 5; 	// to recognize type of decay Bc   ->D+(K-pi+pi+) mu+ mu-
//const int bP_to_Kstar = 5;      // to recognize type of decay B+   ->K*+       mu+ mu-
const int di_to_electrons = 6;      

#endif
