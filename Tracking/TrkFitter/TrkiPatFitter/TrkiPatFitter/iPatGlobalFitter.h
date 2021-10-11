/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// iPatGlobalFitter.h
//   Header file for class iPatGlobalFitter
//
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKIPATFITTER_IPATGLOBALFITTER_H
#define TRKIPATFITTER_IPATGLOBALFITTER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "TrkFitterInterfaces/IGlobalTrackFitter.h"
#include "TrkiPatFitter/iPatFitter.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{

/**  GlobalTrackFitter tool providing methods used during alignment */

class iPatGlobalFitter: public iPatFitter,
			virtual public IGlobalTrackFitter
{
public:
    iPatGlobalFitter		(const std::string& type,
				 const std::string& name,
				 const IInterface* parent);
    ~iPatGlobalFitter		(void); 	// destructor

   Track* alignmentFit ( AlignmentCache&,
                const Track&,
                const RunOutlierRemoval  runOutlier=false,
                const ParticleHypothesis matEffects=Trk::nonInteracting) const;


private:
    /**  GlobalTrackFitter methods:
  	 access to the matrix of derivatives used during the latest track fit */
    std::unique_ptr<Amg::MatrixX>	derivMatrix(const FitState& fitState) const;

    /**  access to the global fitter's full covariance matrix */
    std::unique_ptr<Amg::MatrixX> fullCovarianceMatrix(const FitState& fitState) const;

    /**  access to the number of iterations taken by the latest track fit */
    static int				iterationsOfLastFit(const FitState& fitState) ;


    /**  set method for the minimum number of iterations for (alignment) friend */
    void			setMinIterations (int minIterations);

    // configurables (tools and options)
    bool			m_allParameters;	// all or 5 parameters for above matrix methods
};

} // end of namespace

#endif // TRKIPATFITTER_IPATGLOBALFITTER_H
