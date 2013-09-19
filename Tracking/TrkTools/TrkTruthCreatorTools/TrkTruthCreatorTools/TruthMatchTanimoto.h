/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * A track-truth match quality estimator tool.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

#ifndef TRUTHMATCHTANIMOTO_H
#define TRUTHMATCHTANIMOTO_H

#include "TrkToolInterfaces/IDetailedTrackTruthSimilarity.h"
#include "TrkTruthCreatorTools/CommonTruthMatchWeights.h"

namespace Trk {
  
  class TruthMatchTanimoto: virtual public IDetailedTrackTruthSimilarity,
			    public CommonTruthMatchWeights
  {
  public:
    TruthMatchTanimoto(const std::string& type, const std::string& name, const IInterface* parent);

    /** Tanimoto distance between sets A and B is
     * 
     * D(A,B) = (N_a + N_b - 2*N_ab) / (N_a + N_b - N_ab)
     *
     * where N_a and N_b are the number of elements in each set,
     * and N_ab is the number of elements in the intersection.
     * 
     * This tool considers sets of PRDs on the track and TruthTrajectory
     * and returns (1-D).
     * 
     * One can set different weights for hits in different
     * subdetectors. (The computation uses a "sum of weights" instead
     * of the plain "number of elements".)
     * 
     * See also IDetailedTrackTruthSimilarity::trackTruthSimilarity() 
     */
    virtual double trackTruthSimilarity(const DetailedTrackTruth& dtt);
  };
  
} // end namespace Trk

#endif/*TRUTHMATCHTANIMOTO_H*/
