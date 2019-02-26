/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  CompetingRIOsOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingRIOsOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGRIOSONTRACKTOOL_H
#define COMPETINGRIOSONTRACKTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkToolInterfaces/ICompetingRIOsOnTrackTool.h"
#include "TrkParameters/TrackParameters.h"  // typedef

#include "TrkToolInterfaces/IWeightCalculator.h" // needed for IWeightCalculator::AnnealingFactor
// STL
#include <list>
//#include <vector>

class AtlasDetectorID;

namespace Trk {
class LocalParameters;
class ErrorMatrix;
class Surface;
class PrepRawData;
class MeasurementTypeID;
class CompetingRIOsOnTrack;

/** @class CompetingRIOsOnTrackTool
    @brief This tool creates Trk::CompetingRIOsOnTrack objects using a given
           track hypothesis and a list of Trk::PrepRawData.

    When making the objects, it internally creates Trk::RIO_OnTrack objects
    with the RIO_OnTrackTool. The creation is done by the underlying detector-specific
    classes (e.g. InDet::CompetingTRT_DriftCirclesOnTrackTool).
*/

class CompetingRIOsOnTrackTool : public AthAlgTool,
    virtual public Trk::ICompetingRIOsOnTrackTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    CompetingRIOsOnTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~CompetingRIOsOnTrackTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** creation of a Trk::CompetingRIOsOnTrack from a list of Trk::PrepRawData
        (which should be of the same type, otherwise they will be ignored).
        - TrackParameters should be given in the frame of one of the PrepRawData
        - the AnnealingFactor is given to the IWeightCalculator */
    virtual const Trk::CompetingRIOsOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData* >&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const;

    /** update of the assignment probabilities and effective measurements
        of a Trk::CompetingRIOsOnTrack using a new track prediction. All calcs are done
        by the detector-specific classes:
        - Trk::IWeightCalculator is called first to recalculate the assignment probabilities
        - the new effective measurements are calculated afterwards (this cannot be done by the
            Trk::IWeightCalculator, because it has no detector specific knowledge)   */
    virtual void updateCompetingROT(
        Trk::CompetingRIOsOnTrack&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor
        //                const bool recreateROTs=false
    ) const;
    
    /** update of the assignment probabilities
    and effective measurements of a Trk::CompetingRIOsOnTrack using
    a given set of assignment probabilities */
    virtual StatusCode updateCompetingROTprobs(
        Trk::CompetingRIOsOnTrack&,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&
    ) const;
    
    virtual const Trk::CompetingRIOsOnTrack* createSimpleCompetingROT(
        const Trk::PrepRawData&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor,
        const Trk::TrackState::MeasurementType=Trk::TrackState::unidentified) const;

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

    //! helper to find out the sub-det from PRD->identify()
    const AtlasDetectorID* m_idHelper; 
    MeasurementTypeID*     m_mbHelper; //!< wrapper around above helper

    // the sub-detector tools:
    
    // the job option control for the tools:
    
    //! specific competingROT creation for PixelClusters
    ToolHandle< ICompetingRIOsOnTrackTool > m_compPixelToolHandle;
    ICompetingRIOsOnTrackTool* m_compPixelTool;
    //! specific competingROT creation for SCT_Clusters
    ToolHandle< ICompetingRIOsOnTrackTool > m_compSCT_Tool;
    //! det-specific competingROT creation for TRT
    ToolHandle< ICompetingRIOsOnTrackTool > m_compTRT_Tool;
    //! Det-specific competingROT creation for MDT
    ToolHandle< ICompetingRIOsOnTrackTool > m_compMuonDriftCircleTool;
    //! Det-specific competingROT creation for TGC, RPC and CSC
    ToolHandle< ICompetingRIOsOnTrackTool > m_compMuonClusterTool;
    
};
} // end of namespace Trk
#endif // COMPETINGRIOSONTRACKTOOL_H
