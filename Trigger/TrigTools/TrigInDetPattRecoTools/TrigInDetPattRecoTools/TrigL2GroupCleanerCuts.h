/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_GROUP_CLEANER_CUTS_H__
#define __TRIG_L2_GROUP_CLEANER_CUTS_H__

////////////////////////////////////////////////////////////////////////////////
//
// original filename: GC_Constants.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// Description: parameters for group cleaner 
// 
// date: 29/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


struct TrigL2GroupCleanerCutsBase {

  double dPhidRCut, deltadZdRCut;
  double etaCutInner, etaCutOuter;
  double d0Cut;
  double phi0Offset;
  double invdPhidRBinSize, invPhi0BinSize;
	
  TrigL2GroupCleanerCutsBase( double ddZdR, 
			      double etaI, 
			      double etaO,  
			      double d0, 
			      double dPhidRBinSize, 
			      long phi0Bins, 
			      double delPhidelRCut=0.0003 ) :

    //		dPhidRCut          ( 0.0003 ), /* was 0.003 in cm  */
    dPhidRCut          ( delPhidelRCut ), /* was 0.003 in cm  */
    deltadZdRCut       ( ddZdR ),
    etaCutInner        ( etaI ),
    etaCutOuter        ( etaO ),
    d0Cut              ( d0 ),
    phi0Offset         ( 0.155/0.0003 ),
    //	invdPhidRBinSize   ( dPhidRBins/(2*dPhidRCut) ),
    invdPhidRBinSize   ( 1.0/dPhidRBinSize ),
    invPhi0BinSize     ( phi0Bins/(2*M_PI) ) {
  }
};

struct TrigL2GroupCleanerCutsLoose : public TrigL2GroupCleanerCutsBase {

  TrigL2GroupCleanerCutsLoose(double deltadzdrCut, 
			      double etaCutInner,
			      double etaCutOuter,
			      double d0Cut,
			      double dPhidRBinSize,
			      long   phi0Bins,
			      double delPhidelRCut ) :
    TrigL2GroupCleanerCutsBase(deltadzdrCut,etaCutInner,etaCutOuter,d0Cut,dPhidRBinSize,phi0Bins,delPhidelRCut) {
  }

  TrigL2GroupCleanerCutsLoose& operator=(const TrigL2GroupCleanerCutsLoose& tc) {

    dPhidRCut = tc.dPhidRCut;
    deltadZdRCut = tc.deltadZdRCut;
    etaCutInner = tc.etaCutInner;
    etaCutOuter = tc.etaCutOuter;
    d0Cut = tc.d0Cut;
    phi0Offset = tc.phi0Offset;
    invdPhidRBinSize = tc.invdPhidRBinSize;
    invPhi0BinSize = tc.invPhi0BinSize;
    return *this;
  }

};

struct TrigL2GroupCleanerCutsTight : public TrigL2GroupCleanerCutsBase {
  TrigL2GroupCleanerCutsTight(double deltadzdrCut,
			      double etaCutInner,
			      double etaCutOuter,
			      double d0Cut,
			      double dPhidRBinSize,
			      long   phi0Bins,
			      double delPhidelRCut ) :
    TrigL2GroupCleanerCutsBase(deltadzdrCut,etaCutInner,etaCutOuter,d0Cut,dPhidRBinSize,phi0Bins,delPhidelRCut){
  }

  TrigL2GroupCleanerCutsTight& operator=(const TrigL2GroupCleanerCutsTight& tc) {
    dPhidRCut = tc.dPhidRCut;
    deltadZdRCut = tc.deltadZdRCut;
    etaCutInner = tc.etaCutInner;
    etaCutOuter = tc.etaCutOuter;
    d0Cut = tc.d0Cut;
    phi0Offset = tc.phi0Offset;
    invdPhidRBinSize = tc.invdPhidRBinSize;
    invPhi0BinSize = tc.invPhi0BinSize;
    return *this;
  }
};


#endif
