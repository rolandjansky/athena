/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: GC_Constants.h
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


#if ! defined( IDSCAN_GCCONSTANTS_H )
#define IDSCAN_GCCONSTANTS_H

// to convert GeV to MeV and cm to mm
//#include "CLHEP/Units/PhysicalConstants.h"
#include <cmath>

struct groupCleanerParams
{
	const double dPhidRCut, deltadZdRCut;
	const double etaCutInner, etaCutOuter;
	const double d0Cut;
	const double phi0Offset;
	const double invdPhidRBinSize, invPhi0BinSize;
	
	groupCleanerParams( const double ddZdR, const double etaI, 
			    const double etaO,  const double d0, 
			    const double dPhidRBinSize, const long phi0Bins, const double delPhidelRCut=0.0003 ) :

	  //		dPhidRCut          ( 0.0003 ), /* was 0.003 in cm  */
		dPhidRCut          ( delPhidelRCut ), /* was 0.003 in cm  */
		deltadZdRCut       ( ddZdR ),
		etaCutInner        ( etaI ),
		etaCutOuter        ( etaO ),
		d0Cut              ( d0 ),
		phi0Offset         ( 0.155/0.0003 ),
       //	invdPhidRBinSize   ( dPhidRBins/(2*dPhidRCut) ),
                invdPhidRBinSize   ( 1.0/dPhidRBinSize ),
		invPhi0BinSize     ( phi0Bins/(2.0*M_PI) )
		{};
};

struct GCP_loose : public groupCleanerParams
{
	GCP_loose(double deltadzdrCut, 
		  double etaCutInner,
		  double etaCutOuter,
		  double d0Cut,
		  double dPhidRBinSize,
		  long   phi0Bins,
		  double delPhidelRCut ) :
		groupCleanerParams(
				   deltadzdrCut,
				   etaCutInner,
				   etaCutOuter,
				   d0Cut,
				   dPhidRBinSize,
				   phi0Bins,
				   delPhidelRCut
		)   {};
};

struct GCP_tight : public groupCleanerParams
{
        GCP_tight(double deltadzdrCut,
		  double etaCutInner,
		  double etaCutOuter,
		  double d0Cut,
		  double dPhidRBinSize,
		  long   phi0Bins,
		  double delPhidelRCut ) :
		groupCleanerParams(
				   deltadzdrCut,
				   etaCutInner,
				   etaCutOuter,
				   d0Cut,
				   dPhidRBinSize,
				   phi0Bins,
				   delPhidelRCut
		)   {};
};


#endif
