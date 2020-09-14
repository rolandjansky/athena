/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  ICompetingStrip_ClustersOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface class for CompetingStrip_ClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef ICOMPETINGSTRIP_CLUSTERSONTRACKCREATOR_H
#define ICOMPETINGSTRIP_CLUSTERSONTRACKCREATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"      // typedef
// needed for typedef of AnnealingFactor:
#include "TrkToolInterfaces/IWeightCalculator.h"

#include "TrkToolInterfaces/ICompetingRIOsOnTrackTool.h"
#include "InDetCompetingRIOsOnTrack/CompetingStrip_ClustersOnTrack.h" // needed due to inheritance of the return pointer
// STL
#include <list>

/** @class ICompetingStrip_ClustersOnTrackCreator
    This class provides the interface for tools that create
    InDet::CompetingStrip_ClustersOnTrack objects using a given
    track hypothesis and a list of InDet::Strip_Cluster
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    @author Sebastian.Fleischmann@cern.ch
*/

namespace Trk {
class PrepRawData;
//class CompetingRIOsOnTrack;
}

namespace InDet {
//class CompetingStrip_ClustersOnTrack;

static const InterfaceID IID_ICompetingStrip_ClustersOnTrackCreator("ICompetingStrip_ClustersOnTrackCreator",1,0);


//class ICompetingStrip_ClustersOnTrackCreator : virtual public IAlgTool {
class ICompetingStrip_ClustersOnTrackCreator : virtual public Trk::ICompetingRIOsOnTrackTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:

    static const InterfaceID& interfaceID() {
        return IID_ICompetingStrip_ClustersOnTrackCreator;
    }    //!< The AlgTool InterfaceID
    virtual StatusCode initialize()=0;     //!< standard AlgTool method
    virtual StatusCode finalize()=0;       //!< standard AlgTool method

    /** abstract base method for the creation of a single InDet::CompetingStrip_ClustersOnTrack,
        it takes a list of RIOs (PrepRawData) and the given Track Parameter, cutValue defines
        the cut used for calculating the initial assignment probabilities (see Trk::IWeightCalculator).
     */
    virtual const InDet::CompetingStrip_ClustersOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData * > &,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const = 0;   // pure virtual

    /** abstract base method for the update of the assignment probabilities and effective measurements
        of an InDet::CompetingStrip_ClustersOnTrack using a new track prediction.
        -  Trk::CompetingRIOsOnTrack is used because of interface reasons
        (common interface Trk::ICompetingRIOsOnTrackTool)*/
    virtual void updateCompetingROT(
        //const InDet::CompetingStrip_ClustersOnTrack&,
        const Trk::CompetingRIOsOnTrack&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor
        //                const bool recreateROTs=false
    ) const = 0;

    /** @brief update of the assignment probabilities
    and effective measurements of a Trk::CompetingRIOsOnTrack using
    a given set of assignment probabilities */
    virtual StatusCode updateCompetingROTprobs(
        const Trk::CompetingRIOsOnTrack&,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&
    ) const = 0;

};
} // end of namespace InDet
#endif // ICOMPETINGSTRIP_CLUSTERSONTRACKCREATOR_H
