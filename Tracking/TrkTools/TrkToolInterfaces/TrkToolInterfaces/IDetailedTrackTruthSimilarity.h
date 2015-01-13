/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Interface for track-truth match quality estimator tools.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

#ifndef IDETAILEDTRACKTRUTHSIMILARITY_H
#define IDETAILEDTRACKTRUTHSIMILARITY_H

#include "GaudiKernel/IAlgTool.h"

// Forard declarations of types.
class DetailedTrackTruth;

namespace Trk {
  
  static const InterfaceID IID_IDetailedTrackTruthSimilarity("Trk::IDetailedTrackTruthSimilarity",1,0);

  /** @class IDetailedTrackTruthSimilarity
      @brief Interface for track-truth match quality estimator tools
      @author Andrei Gaponenko
  */

  class IDetailedTrackTruthSimilarity : virtual public IAlgTool {

  public:

    static const InterfaceID& interfaceID() { return IID_IDetailedTrackTruthSimilarity; }

    /** @brief This method returns a number representing the quality of match
     * between the track and the TruthTrajectory in the DetailedTrackTruth argument.
     *
     * The range of return values is [0., 1.], with larger values corresponding
     * to better matches.
     */
    virtual double trackTruthSimilarity(const DetailedTrackTruth& dtt) = 0;

  };
  
} // namespace Trk

#endif/*IDETAILEDTRACKTRUTHSIMILARITY_H*/
