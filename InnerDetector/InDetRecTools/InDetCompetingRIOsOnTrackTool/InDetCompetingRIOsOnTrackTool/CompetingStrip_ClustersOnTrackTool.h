/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  CompetingStrip_ClustersOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for CompetingStrip_ClustersOnTrack creation
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef COMPETINGSTRIP_CLUSTERSONTRACKTOOL_H
#define COMPETINGSTRIP_CLUSTERSONTRACKTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// InDet
#include "InDetCompetingRIOsOnTrackTool/ICompetingStrip_ClustersOnTrackCreator.h"
//#include "InDetCompetingRIOsOnTrack/CompetingStrip_ClustersOnTrack.h"
// #include "SiClusterOnTrackTool/Strip_ClusterOnTrackTool.h"

//#include "StoreGate/StoreGateSvc.h"
// Trk
//#include "InDetRIO_OnTrack/Strip_ClusterOnTrack.h"
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
    class CompetingStrip_ClustersOnTrack;

/** @class CompetingStrip_ClustersOnTrackTool
    This tool creates
    InDet::CompetingStrip_ClustersOnTrack objects using a given
    track hypothesis and a list of InDet::Strip_Cluster
    (given as Trk::PrepRawData to allow for a common interface in namespace Trk).
    It uses Trk::IRIO_OnTrackCreator (jobOption: ToolForStrip_ClusterOnTrackCreation)
    to create the Strip_ClusterOnTrack and
    Trk::IWeightCalculator (jobOption: ToolForWeightCalculation) to calculate the assignment probabilities of the
    competing measurements.
    
    From each Strip_Cluster (RIO) a Strip_ClusterOnTrack (ROTs) is created.
    
    The annealing factor used in the weight calculation can be given in the function call
    and the used cut values for Barrel or End-Cap Strip_ detector, resp. are given by jobOptions (WeightCutValueBarrel
    resp. WeightCutValueEndCap).
    
    This tool can also be used to update the assignment probabilities and the effective measurements
    after an annealing iteration step.
    
    @author Sebastian.Fleischmann@cern.ch
*/


class CompetingStrip_ClustersOnTrackTool : public AthAlgTool,
    virtual public InDet::ICompetingStrip_ClustersOnTrackCreator {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    CompetingStrip_ClustersOnTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~CompetingStrip_ClustersOnTrackTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** creation of an InDet::CompetingStrip_ClustersOnTrack from a list of Trk::PrepRawData
        (which should be InDet::Strip_Cluster, otherwise they will be ignored).
        - TrackParameters should be given in the frame of one of the Strip_Cluster 
        - the AnnealingFactor is given to the IWeightCalculator */
    virtual const InDet::CompetingStrip_ClustersOnTrack* createCompetingROT(
        const std::list< const Trk::PrepRawData* >&,
        const Trk::TrackParameters&,
        const Trk::IWeightCalculator::AnnealingFactor ) const;

    /** update of the assignment probabilities and effective measurements
        of an InDet::CompetingStrip_ClustersOnTrack using a new track prediction.
        - Trk::IWeightCalculator is called first to recalculate the assignment probabilities
        - the new effective measurements are calculated afterwards (this cannot be done by the
            Trk::IWeightCalculator, because it has no detector specific knowledge) 
        - the update can be performed because CompetingStrip_ClustersOnTrackTool is a
            friend of CompetingStrip_ClustersOnTrack 
        - if recreateROTs==true the Strip_ClusterOnTrack (ROTs) contained in the
            CompetingStrip_ClustersOnTrack are recreated using IRIO_OnTrackCreator
            to do detector specific corrections of the measurements again (might
            be needed when the predicted TrackParameters have changed rapidly) */
    virtual void updateCompetingROT(
        //const InDet::CompetingStrip_ClustersOnTrack&,
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

    ToolHandle< Trk::IRIO_OnTrackCreator >  m_Strip__ROTCreator;  //!< Det-specific ROT creation for Strip
    // TODO: change Strip_ClusterOnTrackTool to interface class (eg. IStrip_ClusterOnTrackTool)
    //       once it exists some time

    ToolHandle< Trk::IWeightCalculator >    m_WeightCalculator; //!< Weight calculator tool for assignment probabilities

    double mjo_EndCapCutValue;                                  //!< job option set cutValue of assignment probabilities for end-cap
    double mjo_BarrelCutValue;                                  //!< job option set cutValue of assignment probabilities for barrel Strip_

    ToolHandle< Trk::IExtrapolator >        m_extrapolator;     //!<  the Extrapolator tool

    ///////////////////////////////////////////////////////////////////
    // Private functions:
    ///////////////////////////////////////////////////////////////////
    void testCompetingROT(const InDet::CompetingStrip_ClustersOnTrack&)const;

};
} // end of namespace InDet
#endif // COMPETINGSTRIP_CLUSTERSONTRACKTOOL_H
