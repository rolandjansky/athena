/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  ICompetingTRT_DriftCirclesOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface class for CompetingTRT_DriftCirclesOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef ICOMPETINGTRT_DRIFTCIRCLESONTRACKCREATOR_H
#define ICOMPETINGTRT_DRIFTCIRCLESONTRACKCREATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
// needed for typedef of AnnealingFactor:
#include "TrkToolInterfaces/IWeightCalculator.h"

#include "TrkToolInterfaces/ICompetingRIOsOnTrackTool.h"
#include "InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack.h" // needed due to inheritance of the return pointer
// STL
#include <list>

/** @class ICompetingTRT_DriftCirclesOnTrackCreator
    This class provides the interface for tools that create
    InDet::CompetingTRT_DriftCirclesOnTrack objects using a given
    track hypothesis and a list of InDet::TRT_DriftCircle
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
*/

namespace Trk {
class PrepRawData;
//class CompetingRIOsOnTrack;
}

namespace InDet {
//class CompetingTRT_DriftCirclesOnTrack;
static const InterfaceID IID_ICompetingTRT_DriftCirclesOnTrackCreator("ICompetingTRT_DriftCirclesOnTrackCreator",1,0);


//class ICompetingTRT_DriftCirclesOnTrackCreator : virtual public IAlgTool {
class ICompetingTRT_DriftCirclesOnTrackCreator : virtual public Trk::ICompetingRIOsOnTrackTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:

    static const InterfaceID& interfaceID() {
        return IID_ICompetingTRT_DriftCirclesOnTrackCreator;
    }    //!< The AlgTool InterfaceID
    virtual StatusCode initialize()=0;     //!< standard AlgTool method
    virtual StatusCode finalize()=0;       //!< standard AlgTool method

    /** abstract base method for the creation of a single InDet::CompetingTRT_DriftCirclesOnTrack,
        it takes a list of RIOs (PrepRawData) and the given Track Parameter, cutValue defines
        the cut used for calculating the initial assignment probabilities (see Trk::IWeightCalculator).
     */
    virtual const InDet::CompetingTRT_DriftCirclesOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData * > &,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const = 0;   // pure virtual

    /** abstract base method for the update of the assignment probabilities and effective measurements
        of an InDet::CompetingTRT_DriftCirclesOnTrack using a new track prediction.
        -  Trk::CompetingRIOsOnTrack is used because of interface reasons
        (common interface Trk::ICompetingRIOsOnTrackTool)*/
    virtual void updateCompetingROT(
        //const InDet::CompetingTRT_DriftCirclesOnTrack&,
        Trk::CompetingRIOsOnTrack&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor
        //                const bool recreateROTs=false
    ) const = 0;
};
} // end of namespace InDet
#endif // ICOMPETINGTRT_DRIFTCIRCLESONTRACKCREATOR_H
