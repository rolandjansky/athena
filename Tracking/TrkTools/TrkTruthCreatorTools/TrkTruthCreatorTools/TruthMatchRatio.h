/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * A track-truth match quality estimator tool.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

#ifndef TRUTHMATCHRATIO_H
#define TRUTHMATCHRATIO_H

#include "TrkToolInterfaces/IDetailedTrackTruthSimilarity.h"
#include "TrkTruthCreatorTools/CommonTruthMatchWeights.h"

namespace Trk {
  
  class TruthMatchRatio: virtual public IDetailedTrackTruthSimilarity,
			 public CommonTruthMatchWeights 
  {
  public:
    TruthMatchRatio(const std::string& type, const std::string& name, const IInterface* parent);

    /** The match quality returned by this tool is a (weighted) ratio
     * of the number of hits common to the track and the
     * TruthTrajectory to the number of hits on track.
     *
     * See also IDetailedTrackTruthSimilarity::trackTruthSimilarity() 
     */
    virtual double trackTruthSimilarity(const DetailedTrackTruth& dtt);
  };
  
} // end namespace Trk

#endif/*TRUTHMATCHRATIO_H*/
