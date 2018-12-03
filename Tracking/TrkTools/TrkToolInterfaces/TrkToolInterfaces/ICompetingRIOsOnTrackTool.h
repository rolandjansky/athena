/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  ICompetingRIOsOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface class for CompetingRIOsOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_ICOMPETINGRIOSONTRACKTOOL_H
#define TRK_ICOMPETINGRIOSONTRACKTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" // typedef
// needed for typedef of AnnealingFactor:
#include "TrkToolInterfaces/IWeightCalculator.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
// STL
#include <list>

/** @class ICompetingRIOsOnTrackTool
    @brief This class provides the interface for tools that create
    Trk::CompetingRIOsOnTrack objects using a given
    track hypothesis and a list of Trk::PrepRawData.

    @author Sebastian Fleischmann
*/

namespace Trk {

class PrepRawData;
class CompetingRIOsOnTrack;
class RIO_OnTrack;

static const InterfaceID IID_ICompetingRIOsOnTrackTool("ICompetingRIOsOnTrackTool",1,0);


class ICompetingRIOsOnTrackTool : virtual public IAlgTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:

    static const InterfaceID& interfaceID() {
        return IID_ICompetingRIOsOnTrackTool;
    }    //!< The AlgTool InterfaceID

    /** @brief abstract base method for making a Trk::CompetingRIOsOnTrack object.

        It takes a list of RIOs (PrepRawData) and the given Track Parameter, cutValue
        defines the cut used for calculating the initial assignment probabilities
        (see Trk::IWeightCalculator).
     */
    virtual const Trk::CompetingRIOsOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData * > &,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const = 0;   // pure virtual

    /** @brief abstract base method for the update of the assignment probabilities
        and effective measurements of a Trk::CompetingRIOsOnTrack using a new track
        prediction. */
    virtual void updateCompetingROT(
        const Trk::CompetingRIOsOnTrack&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor
    ) const = 0;

    /** @brief update of the assignment probabilities
        and effective measurements of a Trk::CompetingRIOsOnTrack using
        a given set of assignment probabilities */
    virtual StatusCode updateCompetingROTprobs(
        const Trk::CompetingRIOsOnTrack&,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&
    ) const;

    /** making of simple competing ROT (i.e. which competes against outlier hypothesis) */
    virtual const Trk::CompetingRIOsOnTrack* createSimpleCompetingROT(
        const Trk::PrepRawData&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor,
        const Trk::TrackState::MeasurementType=Trk::TrackState::unidentified) const;

    /** write documentation! */
    virtual void reequipCompetingROT(Trk::CompetingRIOsOnTrack*&,
                                     const Trk::RIO_OnTrack*,
                                     const Trk::TrackParameters&,
                                     const Trk::IWeightCalculator::AnnealingFactor,
                                     const Trk::TrackState::MeasurementType
                                     = Trk::TrackState::unidentified) const;

};
} // end of namespace Trk

inline StatusCode Trk::ICompetingRIOsOnTrackTool::updateCompetingROTprobs(
        const Trk::CompetingRIOsOnTrack&,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&
    ) const {
    std::cout << "CONFIGURATION WARNING: dummy version of CompetingRIOsOnTrack update called." << std::endl;
    return StatusCode::FAILURE;
}


inline const Trk::CompetingRIOsOnTrack* Trk::ICompetingRIOsOnTrackTool::createSimpleCompetingROT
( const Trk::PrepRawData&, const Trk::TrackParameters&,
  const Trk::IWeightCalculator::AnnealingFactor,
  const Trk::TrackState::MeasurementType) const
{
  std::cout << "CONFIGURATION WARNING: dummy version of single-hit CompetingRIOsOnTrack maker called." << std::endl;
  return 0;
}

inline void Trk::ICompetingRIOsOnTrackTool::reequipCompetingROT
( Trk::CompetingRIOsOnTrack*&,
  const Trk::RIO_OnTrack*,
  const Trk::TrackParameters&,
  const Trk::IWeightCalculator::AnnealingFactor,
  const Trk::TrackState::MeasurementType) const
{
  std::cout << "CONFIGURATION WARNING: dummy version of single-hit CompROT re-equipping called." << std::endl;
  return;
}




#endif // TRK_ICOMPETINGRIOSONTRACKTOOL_H
