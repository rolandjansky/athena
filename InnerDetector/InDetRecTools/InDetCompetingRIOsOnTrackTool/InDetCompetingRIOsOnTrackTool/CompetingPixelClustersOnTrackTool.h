/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  CompetingPixelClustersOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingPixelClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGPIXELCLUSTERSONTRACKTOOL_H
#define COMPETINGPIXELCLUSTERSONTRACKTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// InDet
#include "InDetCompetingRIOsOnTrackTool/ICompetingPixelClustersOnTrackCreator.h"
//#include "SiClusterOnTrackTool/PixelClusterOnTrackTool.h" // re-visit once this has an abstract interface

// Trk
#include "TrkParameters/TrackParameters.h"      // typedef
// STL
#include <list>
//#include <vector>
#include <string>

namespace Trk {
  class LocalParameters;
  class ErrorMatrix;
  class Surface;
  class PrepRawData;
  class IExtrapolator;
  class IRIO_OnTrackCreator;
  class IWeightCalculator;
}


namespace InDet {
    class CompetingPixelClustersOnTrack;

/** @class CompetingPixelClustersOnTrackTool
    This tool creates
    InDet::CompetingPixelClustersOnTrack objects using a given
    track hypothesis and a list of InDet::PixelCluster
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    It uses Trk::IRIO_OnTrackCreator (jobOption: ToolForPixelClusterOnTrackCreation)
    to create the PixelClusterOnTrack and
    Trk::IWeightCalculator (jobOption: ToolForWeightCalculation) to calculate the assignment probabilities of the
    competing measurements.
    
    From each PixelCluster (RIO) a PixelClusterOnTrack (ROTs) is created.
    
    The annealing factor used in the weight calculation can be given in the function call
    and the used cut values for Barrel or End-Cap Pixel detector, resp. are given by jobOptions (WeightCutValueBarrel
    resp. WeightCutValueEndCap).
    
    This tool can also be used to update the assignment probabilities and the effective measurements
    after an annealing iteration step.
    
    @author Sebastian.Fleischmann@cern.ch
*/


class CompetingPixelClustersOnTrackTool : public AthAlgTool,
    virtual public InDet::ICompetingPixelClustersOnTrackCreator {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    CompetingPixelClustersOnTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~CompetingPixelClustersOnTrackTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** creation of an InDet::CompetingPixelClustersOnTrack from a list of Trk::PrepRawData
        (which should be InDet::PixelCluster, otherwise they will be ignored).
        - TrackParameters should be given in the frame of one of the PixelCluster 
        - the AnnealingFactor is given to the IWeightCalculator */
    virtual const InDet::CompetingPixelClustersOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData* >&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const;

    /** update of the assignment probabilities and effective measurements
        of an InDet::CompetingPixelClustersOnTrack using a new track prediction.
        - Trk::IWeightCalculator is called first to recalculate the assignment probabilities
        - the new effective measurements are calculated afterwards (this cannot be done by the
            Trk::IWeightCalculator, because it has no detector specific knowledge) 
        - the update can be performed because CompetingPixelClustersOnTrackTool is a
            friend of CompetingPixelClustersOnTrack 
        - if recreateROTs==true the PixelClusterOnTrack (ROTs) contained in the
            CompetingPixelClustersOnTrack are recreated using IRIO_OnTrackCreator
            to do detector specific corrections of the measurements again (might
            be needed when the predicted TrackParameters have changed rapidly) */
    virtual void updateCompetingROT(
        //const InDet::CompetingPixelClustersOnTrack&,
        Trk::CompetingRIOsOnTrack&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor
        //                const bool recreateROTs=false
    ) const;

    virtual void reequipCompetingROT(Trk::CompetingRIOsOnTrack*&,
                                     const Trk::RIO_OnTrack*,
                                     const Trk::TrackParameters&,
                                     const Trk::IWeightCalculator::AnnealingFactor,
                                     const Trk::TrackState::MeasurementType
                                     = Trk::TrackState::unidentified) const;

    virtual StatusCode updateCompetingROTprobs(
        Trk::CompetingRIOsOnTrack&,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&
    ) const;

private:

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////

    ToolHandle< Trk::IRIO_OnTrackCreator >  m_Pixel_ROTCreator; //!< Det-specific ROT creation for Pixels
    // TODO: change PixelClusterOnTrackTool to interface class (eg. IPixelClusterOnTrackTool)
    //       once that one exists

    ToolHandle< Trk::IWeightCalculator >    m_WeightCalculator; //!< Weight calculator tool for assignment probabilities

    double                                  m_jo_EndCapCutValue; //!< job option set cutValue of assignment probabilities for Pixel end-cap
    double                                  m_jo_BarrelCutValue; //!< job option set cutValue of assignment probabilities for barrel Pixel

    ToolHandle< Trk::IExtrapolator >        m_extrapolator;     //!<  the Extrapolator tool

    ///////////////////////////////////////////////////////////////////
    // Private functions:
    ///////////////////////////////////////////////////////////////////
    void testCompetingROT(const InDet::CompetingPixelClustersOnTrack&)const;

};
} // end of namespace InDet
#endif // COMPETINGPIXELCLUSTERSONTRACKTOOL_H
