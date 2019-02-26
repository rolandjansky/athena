/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  CompetingSCT_ClustersOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingSCT_ClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGSCT_CLUSTERSONTRACKTOOL_H
#define COMPETINGSCT_CLUSTERSONTRACKTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// InDet
#include "InDetCompetingRIOsOnTrackTool/ICompetingSCT_ClustersOnTrackCreator.h"
//#include "InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack.h"
// #include "SiClusterOnTrackTool/SCT_ClusterOnTrackTool.h"

//#include "StoreGate/StoreGateSvc.h"
// Trk
//#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
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
    class CompetingSCT_ClustersOnTrack;

/** @class CompetingSCT_ClustersOnTrackTool
    This tool creates
    InDet::CompetingSCT_ClustersOnTrack objects using a given
    track hypothesis and a list of InDet::SCT_Cluster
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    It uses Trk::IRIO_OnTrackCreator (jobOption: ToolForSCT_ClusterOnTrackCreation)
    to create the SCT_ClusterOnTrack and
    Trk::IWeightCalculator (jobOption: ToolForWeightCalculation) to calculate the assignment probabilities of the
    competing measurements.
    
    From each SCT_Cluster (RIO) a SCT_ClusterOnTrack (ROTs) is created.
    
    The annealing factor used in the weight calculation can be given in the function call
    and the used cut values for Barrel or End-Cap SCT_ detector, resp. are given by jobOptions (WeightCutValueBarrel
    resp. WeightCutValueEndCap).
    
    This tool can also be used to update the assignment probabilities and the effective measurements
    after an annealing iteration step.
    
    @author Sebastian.Fleischmann@cern.ch
*/


class CompetingSCT_ClustersOnTrackTool : public AthAlgTool,
    virtual public InDet::ICompetingSCT_ClustersOnTrackCreator {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    CompetingSCT_ClustersOnTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~CompetingSCT_ClustersOnTrackTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** creation of an InDet::CompetingSCT_ClustersOnTrack from a list of Trk::PrepRawData
        (which should be InDet::SCT_Cluster, otherwise they will be ignored).
        - TrackParameters should be given in the frame of one of the SCT_Cluster 
        - the AnnealingFactor is given to the IWeightCalculator */
    virtual const InDet::CompetingSCT_ClustersOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData* >&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const;

    /** update of the assignment probabilities and effective measurements
        of an InDet::CompetingSCT_ClustersOnTrack using a new track prediction.
        - Trk::IWeightCalculator is called first to recalculate the assignment probabilities
        - the new effective measurements are calculated afterwards (this cannot be done by the
            Trk::IWeightCalculator, because it has no detector specific knowledge) 
        - the update can be performed because CompetingSCT_ClustersOnTrackTool is a
            friend of CompetingSCT_ClustersOnTrack 
        - if recreateROTs==true the SCT_ClusterOnTrack (ROTs) contained in the
            CompetingSCT_ClustersOnTrack are recreated using IRIO_OnTrackCreator
            to do detector specific corrections of the measurements again (might
            be needed when the predicted TrackParameters have changed rapidly) */
    virtual void updateCompetingROT(
        //const InDet::CompetingSCT_ClustersOnTrack&,
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

    ToolHandle< Trk::IRIO_OnTrackCreator >  m_SCT__ROTCreator;  //!< Det-specific ROT creation for SCT
    // TODO: change SCT_ClusterOnTrackTool to interface class (eg. ISCT_ClusterOnTrackTool)
    //       once it exists some time

    ToolHandle< Trk::IWeightCalculator >    m_WeightCalculator; //!< Weight calculator tool for assignment probabilities

    double m_jo_EndCapCutValue;                                  //!< job option set cutValue of assignment probabilities for end-cap
    double m_jo_BarrelCutValue;                                  //!< job option set cutValue of assignment probabilities for barrel SCT_

    ToolHandle< Trk::IExtrapolator >        m_extrapolator;     //!<  the Extrapolator tool

    ///////////////////////////////////////////////////////////////////
    // Private functions:
    ///////////////////////////////////////////////////////////////////
    void testCompetingROT(const InDet::CompetingSCT_ClustersOnTrack&)const;

};
} // end of namespace InDet
#endif // COMPETINGSCT_CLUSTERSONTRACKTOOL_H
