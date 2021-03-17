/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  CompetingTRT_DriftCirclesOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingTRT_DriftCirclesOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGTRT_DRIFTCIRCLESONTRACKTOOL_H
#define COMPETINGTRT_DRIFTCIRCLESONTRACKTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// InDet
#include "InDetCompetingRIOsOnTrackTool/ICompetingTRT_DriftCirclesOnTrackCreator.h"

// Trk
#include "TrkParameters/TrackParameters.h"
// STL
#include <list>
#include <string>

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class LocalParameters;
  class Surface;
  class PrepRawData;
  class IRIO_OnTrackCreator;
  class IWeightCalculator;
  class IExtrapolator;
}


namespace InDet {
    class CompetingTRT_DriftCirclesOnTrack;

/** @class CompetingTRT_DriftCirclesOnTrackTool
    This tool creates
    InDet::CompetingTRT_DriftCirclesOnTrack objects using a given
    track hypothesis and a list of InDet::TRT_DriftCircle
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    It uses Trk::IRIO_OnTrackCreator (jobOption: ToolForTRT_DriftCircleOnTrackCreation)
    to create the TRT_DriftCircleOnTrack and
    Trk::IWeightCalculator (jobOption: ToolForWeightCalculation) to calculate the assignment probabilities of the
    competing measurements.
    
    From each TRT_DriftCircle (RIO) two TRT_DriftCircleOnTrack (ROTs) are created. This
    corresponds to the left and the right solution of the DriftCircle.
    
    The annealing factor used in the weight calculation can be given in the function call
    and the used cut values for Barrel TRT resp. End-Cap TRT are given by jobOptions (WeightCutValueBarrel
    resp. WeightCutValueEndCap).
    
    This tool can also be used to update the assignment probabilities and the effective measurements
    after an annealing iteration step.
*/


class CompetingTRT_DriftCirclesOnTrackTool : public AthAlgTool,
    virtual public InDet::ICompetingTRT_DriftCirclesOnTrackCreator {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    CompetingTRT_DriftCirclesOnTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~CompetingTRT_DriftCirclesOnTrackTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** creation of an InDet::CompetingTRT_DriftCirclesOnTrack from a list of Trk::PrepRawData
        (which should be TRT_DriftCircle, otherwise they will be ignored).
        - TrackParameters should be given in the frame of one of the TRT_DriftCircle 
        - the AnnealingFactor is given to the IWeightCalculator */
    virtual const InDet::CompetingTRT_DriftCirclesOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData* >&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const;

    /** update of the assignment probabilities and effective measurements
        of an InDet::CompetingTRT_DriftCirclesOnTrack using a new track prediction.
        - Trk::IWeightCalculator is called first to recalculate the assignment probabilities
        - the new effective measurements are calculated afterwards (this cannot be done by the
            Trk::IWeightCalculator, because it has no detector specific knowledge) 
        - the update can be performed because CompetingTRT_DriftCirclesOnTrackTool is a
            friend of CompetingTRT_DriftCirclesOnTrack 
        - if recreateROTs==true the TRT_DriftCircleOnTrack (ROTs) contained in the
            CompetingTRT_DriftCirclesOnTrack are recreated using IRIO_OnTrackCreator
            to do detector specific corrections of the measurements again (might
            be needed when the predicted TrackParameters have changed rapidly) */
    virtual void updateCompetingROT(
        //const InDet::CompetingTRT_DriftCirclesOnTrack&,
        Trk::CompetingRIOsOnTrack&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor
        //                const bool recreateROTs=false
    ) const;

    /** making of simple competing ROT (competes against outlier hypothesis) */
    virtual const Trk::CompetingRIOsOnTrack* createSimpleCompetingROT(
        const Trk::PrepRawData&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor,
        const Trk::TrackState::MeasurementType) const;

    virtual void reequipCompetingROT(Trk::CompetingRIOsOnTrack*&,
                                     const Trk::RIO_OnTrack*,
                                     const Trk::TrackParameters&,
                                     const Trk::IWeightCalculator::AnnealingFactor,
                                     const Trk::TrackState::MeasurementType
                                     = Trk::TrackState::unidentified) const;


private:

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////

    ToolHandle< Trk::IRIO_OnTrackCreator >  m_TRT_ROTCreator;   //!< Det-specific ROT creation for TRT

    ToolHandle< Trk::IWeightCalculator >    m_weightCalculator; //!< Weight calculator tool for assignment probabilities

    double m_jo_EndCapCutValue;                                  //!< job option set cutValue of assignment probabilities for TRT end-cap
    double m_jo_BarrelCutValue;                                  //!< job option set cutValue of assignment probabilities for barrel TRT

    ToolHandle< Trk::IExtrapolator >        m_extrapolator;     //!<  the Extrapolator tool

    ///////////////////////////////////////////////////////////////////
    // Private functions:
    ///////////////////////////////////////////////////////////////////

    //return the effective LocalParameters according to the weights (assignment probabilities)
    void calcEffectiveMeasurement(
        const Trk::LocalParameters* &effectiveLocalPar,
        const Amg::MatrixX* &effectiveErrMat,
        const std::vector< const InDet::TRT_DriftCircleOnTrack* >*,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >*,
        const Trk::TrackParameters*,
        const Trk::Surface* surf,
        bool nonVanishingROTsHaveCommonSurface
    )const;

    void calcEffectiveEndCapMeasurement(
        const Trk::LocalParameters* &effectiveLocalPar,
        const Amg::MatrixX* &effectiveErrMat,
        const std::vector< const InDet::TRT_DriftCircleOnTrack* >*,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >*,
        const Amg::Vector2D& localTrack,
        const Trk::Surface* surf
    )const;

    void testCompetingROT(const InDet::CompetingTRT_DriftCirclesOnTrack&)const;

};
} // end of namespace InDet
#endif // COMPETINGTRT_DRIFTCIRCLESONTRACKTOOL_H
