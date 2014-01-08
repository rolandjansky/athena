/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyPosmusc.h"

#include "MuonboyCore/MuonboyCofiti.h"

extern "C" void posmusc_();

MuonboyPosmusc::MuonboyPosmusc(){}
MuonboyPosmusc::~MuonboyPosmusc(){}

void MuonboyPosmusc::DoIt(
             double X0,
	     double Y0,
	     double Z0,
             double Ux,
	     double Uy,
	     double Uz,
             double P,
	     double Charge
){

  MuonboyCofiti cofiti;
  cofiti.SetVDEB(X0     ,0,0) ;
  cofiti.SetVDEB(Y0     ,0,1) ;
  cofiti.SetVDEB(Z0     ,0,2) ;
  cofiti.SetVDEB(Ux     ,0,3) ;
  cofiti.SetVDEB(Uy     ,0,4) ;
  cofiti.SetVDEB(Uz     ,0,5) ;
  cofiti.SetVDEB(P      ,0,6) ;
  cofiti.SetVDEB(Charge ,0,7) ;
  posmusc_();

}

