/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  ICompetingPixelClustersOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface class for CompetingPixelClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef ICOMPETINGPIXELCLUSTERSONTRACKCREATOR_H
#define ICOMPETINGPIXELCLUSTERSONTRACKCREATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"  //typedef
// needed for typedef of AnnealingFactor:
#include "TrkToolInterfaces/IWeightCalculator.h"

#include "TrkToolInterfaces/ICompetingRIOsOnTrackTool.h"
#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h" // needed due to inheritance of the return pointer
// STL
#include <list>

/** @class ICompetingPixelClustersOnTrackCreator
    This class provides the interface for tools that create
    InDet::CompetingPixelClustersOnTrack objects using a given
    track hypothesis and a list of InDet::PixelCluster
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    @author Sebastian.Fleischmann@cern.ch
*/

namespace Trk {
  class PrepRawData;
  class CompetingRIOsOnTrack;
}

namespace InDet {
//class CompetingPixelClustersOnTrack;

static const InterfaceID IID_ICompetingPixelClustersOnTrackCreator("ICompetingPixelClustersOnTrackCreator",1,0);


//class ICompetingPixelClustersOnTrackCreator : virtual public IAlgTool {
class ICompetingPixelClustersOnTrackCreator : virtual public Trk::ICompetingRIOsOnTrackTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:

    static const InterfaceID& interfaceID() {
        return IID_ICompetingPixelClustersOnTrackCreator;
    }    //!< The AlgTool InterfaceID
    virtual StatusCode initialize()=0;     //!< standard AlgTool method
    virtual StatusCode finalize()=0;       //!< standard AlgTool method

    /** abstract base method for the creation of a single InDet::CompetingPixelClustersOnTrack,
        it takes a list of RIOs (PrepRawData) and the given Track Parameter, cutValue defines
        the cut used for calculating the initial assignment probabilities (see Trk::IWeightCalculator).
     */
    virtual const InDet::CompetingPixelClustersOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData * > &,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const = 0;   // pure virtual

    /** abstract base method for the update of the assignment probabilities and effective measurements
        of an InDet::CompetingPixelClustersOnTrack using a new track prediction.
        -  Trk::CompetingRIOsOnTrack is used because of interface reasons
        (common interface Trk::ICompetingRIOsOnTrackTool)*/
    virtual void updateCompetingROT(
        Trk::CompetingRIOsOnTrack&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor
        //                const bool recreateROTs=false
    ) const = 0;
    
    /** @brief update of the assignment probabilities
    and effective measurements of a Trk::CompetingRIOsOnTrack using
    a given set of assignment probabilities */
    virtual StatusCode updateCompetingROTprobs(
        Trk::CompetingRIOsOnTrack&,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&
    ) const = 0;

};
} // end of namespace InDet
#endif // ICOMPETINGPIXELCLUSTERSONTRACKCREATOR_H
