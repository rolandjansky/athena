/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexAnalyticSeedFinder.h, (c) ATLAS Detector software 2019
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXANALYTICSEEDFINDER_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXANALYTICSEEDFINDER_H

#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

/**
 * @class Trk::IVertexAnalyticSeedFinder
 *
 * An abstract base class for implementation of
 * Linearization point finders. Calculates the 
 * the starting point of the vertex fit given
 * with a set of tracks. Differs from IVertexSeedFinder
 * in that points are calculated using analytic functions
 * and can therefore return more information.
 *
 * The correct choice of a starting linearization point
 * does influence the precision of the adaptive fits and
 * may influence the convergence time of the Kalman filter.
 * See concrete implementations for more details.
 *
 *@author izaac.sanderswood@cern.ch
 *
 */

namespace Trk {
	class IVertexAnalyticSeedFinder : virtual public IVertexSeedFinder 
	{
	  public:
	    DeclareInterfaceID( IVertexAnalyticSeedFinder, 1, 0 );
	    /** 
	     * Finds a linearization point out of a vector of TrackParameters
	     * and returns it as an Amg::Vector3D object, along with a seed width
	     * which can be used in the vertex fit. If you want an 
	     * additional constraint can be taken into account.
	     * Must specify the primary vertex position.
	     */
    	virtual std::pair<Amg::Vector3D,Amg::MatrixX> findAnalyticSeed (const std::vector<const Trk::TrackParameters*>& perigeeList,
    														   const xAOD::Vertex * constraint=0) const = 0;
	}; // end of class Analytic Seed finder

}

#endif