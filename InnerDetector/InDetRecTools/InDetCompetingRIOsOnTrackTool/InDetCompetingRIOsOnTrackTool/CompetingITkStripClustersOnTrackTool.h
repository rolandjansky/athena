/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  CompetingITkStripClustersOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingITkStripClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// James Smith jl.smith@cern.ch
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGITKSTRIPCLUSTERSONTRACKTOOL_H
#define COMPETINGITKSTRIPCLUSTERSONTRACKTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// InDet
#include "InDetCompetingRIOsOnTrackTool/ICompetingITkStripClustersOnTrackCreator.h"
//#include "InDetCompetingRIOsOnTrack/CompetingITkStripClustersOnTrack.h"
// #include "SiClusterOnTrackTool/ITk_Strip_ClusterOnTrackTool.h"

//#include "StoreGate/StoreGateSvc.h"
// Trk
//#include "InDetRIO_OnTrack/ITk_Strip_ClusterOnTrack.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkToolInterfaces/IWeightCalculator.h"
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
    class CompetingITkStripClustersOnTrack;

/** @class CompetingITkStripClustersOnTrackTool
    This tool creates
    InDet::CompetingITkStripClustersOnTrack objects using a given
    track hypothesis and a list of InDet::ITk_Strip_Cluster
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    It uses Trk::IRIO_OnTrackCreator (jobOption: ToolForITk_Strip_ClusterOnTrackCreation)
    to create the ITk_Strip_ClusterOnTrack and
    Trk::IWeightCalculator (jobOption: ToolForWeightCalculation) to calculate the assignment probabilities of the
    competing measurements.
    
    From each ITk_Strip_Cluster (RIO) a ITk_Strip_ClusterOnTrack (ROTs) is created.
    
    The annealing factor used in the weight calculation can be given in the function call
    and the used cut values for Barrel or End-Cap ITk_Strip_ detector, resp. are given by jobOptions (WeightCutValueBarrel
    resp. WeightCutValueEndCap).
    
    This tool can also be used to update the assignment probabilities and the effective measurements
    after an annealing iteration step.
    
    @author Sebastian.Fleischmann@cern.ch
*/


class CompetingITkStripClustersOnTrackTool : public AthAlgTool,
    virtual public InDet::ICompetingITkStripClustersOnTrackCreator {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    CompetingITkStripClustersOnTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~CompetingITkStripClustersOnTrackTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** creation of an InDet::CompetingITkStripClustersOnTrack from a list of Trk::PrepRawData
        (which should be InDet::ITk_Strip_Cluster, otherwise they will be ignored).
        - TrackParameters should be given in the frame of one of the ITk_Strip_Cluster 
        - the AnnealingFactor is given to the IWeightCalculator */
    virtual const InDet::CompetingITkStripClustersOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData* >&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const;

    /** update of the assignment probabilities and effective measurements
        of an InDet::CompetingITkStripClustersOnTrack using a new track prediction.
        - Trk::IWeightCalculator is called first to recalculate the assignment probabilities
        - the new effective measurements are calculated afterwards (this cannot be done by the
            Trk::IWeightCalculator, because it has no detector specific knowledge) 
        - the update can be performed because CompetingITkStripClustersOnTrackTool is a
            friend of CompetingITkStripClustersOnTrack 
        - if recreateROTs==true the ITk_Strip_ClusterOnTrack (ROTs) contained in the
            CompetingITkStripClustersOnTrack are recreated using IRIO_OnTrackCreator
            to do detector specific corrections of the measurements again (might
            be needed when the predicted TrackParameters have changed rapidly) */
    virtual void updateCompetingROT(
        //const InDet::CompetingITkStripClustersOnTrack&,
        const Trk::CompetingRIOsOnTrack&,
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
        const Trk::CompetingRIOsOnTrack&,
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&
    ) const;


private:

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////

    ToolHandle< Trk::IRIO_OnTrackCreator >  m_ITk_Strip__ROTCreator;  //!< Det-specific ROT creation for Strip
    // TODO: change ITk_Strip_ClusterOnTrackTool to interface class (eg. IITk_Strip_ClusterOnTrackTool)
    //       once it exists some time

    ToolHandle< Trk::IWeightCalculator >    m_WeightCalculator; //!< Weight calculator tool for assignment probabilities

    double mjo_EndCapCutValue;                                  //!< job option set cutValue of assignment probabilities for end-cap
    double mjo_BarrelCutValue;                                  //!< job option set cutValue of assignment probabilities for barrel ITk_Strip_

    ToolHandle< Trk::IExtrapolator >        m_extrapolator;     //!<  the Extrapolator tool

    ///////////////////////////////////////////////////////////////////
    // Private functions:
    ///////////////////////////////////////////////////////////////////
    void testCompetingROT(const InDet::CompetingITkStripClustersOnTrack&)const;

};
} // end of namespace InDet
#endif // COMPETINGITKSTRIPCLUSTERSONTRACKTOOL_H
