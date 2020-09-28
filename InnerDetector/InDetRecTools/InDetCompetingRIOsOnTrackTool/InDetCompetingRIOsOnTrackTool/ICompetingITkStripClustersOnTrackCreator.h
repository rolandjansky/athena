/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  ICompetingITkStripClustersOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface class for CompetingITkStripClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// James Smith jl.smith@cern.ch
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef ICOMPETINGITKSTRIPCLUSTERSONTRACKCREATOR_H
#define ICOMPETINGITKSTRIPCLUSTERSONTRACKCREATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"      // typedef
// needed for typedef of AnnealingFactor:
#include "TrkToolInterfaces/IWeightCalculator.h"

#include "TrkToolInterfaces/ICompetingRIOsOnTrackTool.h"
#include "InDetCompetingRIOsOnTrack/CompetingITkStripClustersOnTrack.h" // needed due to inheritance of the return pointer
// STL
#include <list>

/** @class ICompetingITkStripClustersOnTrackCreator
    This class provides the interface for tools that create
    InDet::CompetingITkStripClustersOnTrack objects using a given
    track hypothesis and a list of InDet::ITk_Strip_Cluster
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    @author Sebastian.Fleischmann@cern.ch
*/

namespace Trk {
class PrepRawData;
//class CompetingRIOsOnTrack;
}

namespace InDet {
//class CompetingITkStripClustersOnTrack;

static const InterfaceID IID_ICompetingITkStripClustersOnTrackCreator("ICompetingITkStripClustersOnTrackCreator",1,0);


//class ICompetingITkStripClustersOnTrackCreator : virtual public IAlgTool {
class ICompetingITkStripClustersOnTrackCreator : virtual public Trk::ICompetingRIOsOnTrackTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:

    static const InterfaceID& interfaceID() {
        return IID_ICompetingITkStripClustersOnTrackCreator;
    }    //!< The AlgTool InterfaceID
    virtual StatusCode initialize()=0;     //!< standard AlgTool method
    virtual StatusCode finalize()=0;       //!< standard AlgTool method

    /** abstract base method for the creation of a single InDet::CompetingITkStripClustersOnTrack,
        it takes a list of RIOs (PrepRawData) and the given Track Parameter, cutValue defines
        the cut used for calculating the initial assignment probabilities (see Trk::IWeightCalculator).
     */
    virtual const InDet::CompetingITkStripClustersOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData * > &,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const = 0;   // pure virtual

    /** abstract base method for the update of the assignment probabilities and effective measurements
        of an InDet::CompetingITkStripClustersOnTrack using a new track prediction.
        -  Trk::CompetingRIOsOnTrack is used because of interface reasons
        (common interface Trk::ICompetingRIOsOnTrackTool)*/
    virtual void updateCompetingROT(
        //const InDet::CompetingITkStripClustersOnTrack&,
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
#endif // ICOMPETINGITKSTRIPCLUSTERSONTRACKCREATOR_H
