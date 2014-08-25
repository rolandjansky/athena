/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ImpactInCaloAlg.h
//   Header file for class ImpactInCaloAlg
///////////////////////////////////////////////////////////////////
// Top algorithm class for extrapolating tracks to Calo
///////////////////////////////////////////////////////////////////


#ifndef IMPACTINCALOALG_H
#define IMPACTINCALOALG_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "StoreGate/DataHandle.h"

//#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"   // typedef

// #include "Particle/TrackParticleContainer.h"
// #include "Particle/TrackParticle.h"

//#include "TrackToCalo/ImpactInCalo.h"

//#include "CaloDetDescr/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"


class StoreGateSvc;
class IExtrapolateToCaloTool;
class ICaloCoordinateTool;
class CaloDetDescrManager;
class CaloCellList;
class CaloDepthTool;
class ImpactInCalo;

namespace Trk {
    class Track;
    class ITrackSelectorTool;
}
namespace Rec {
    class TrackParticle;
    class TrackParticleContainer;
}

/**
@class ImpactInCaloAlg
This Algorithm is meant to be an example of use of the TrackToCalo tools.
It is based on the TrackToCaloAlg, but uses the IExtrapolateToCaloTool:
 
 - It loops on a given Track collection ( choosen by jobOption ) and extrapolates all tracks
   to the layers of the calorimeter using the IExtrapolateToCaloTool.
 
 - Impacts are stored in an ImpactInCaloCollection, which can then be used to fill CBNT
 
 - An example of primitive Track to Cluster matching is provided, as well as an example of how
   one can retreive all the CaloCell's around a track extrapolation, using the CaloCellList
   class ( these 2 are off by default )
 
    @author Sebastian.Fleischmann@cern.ch
*/

class ImpactInCaloAlg : public AthAlgorithm  {

public:

    // Constructor with parameters:
    ImpactInCaloAlg(const std::string &name,ISvcLocator *pSvcLocator);

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    // Basic algorithm methods:
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

    /** Loop on Trk::Tracks and create ImpactInCaloCollection */
    StatusCode CreateTrkImpactInCalo();

    /** Retreive ImpactInCaloCollection and compare to CaloClusters */
    StatusCode CompareImpactWithCluster();
    /** Debug output of impacts */
    StatusCode PrintImpact();

    /** List of cells crossed by Trk::Tracks
     ( neta and nphi are the total window width, e.g .5x5 ) */
    CaloCellList* CellsCrossedByTrack(const Trk::Track& trk ,
                                      const CaloCell_ID::CaloSample sam,
                                      int neta, int nphi);
    /** Debug output of cells crossed*/
    StatusCode PrintCellsCrossed();

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
//private:
//     ImpactInCaloAlg();
//     ImpactInCaloAlg(const ImpactInCaloAlg&);
//     ImpactInCaloAlg &operator=(const ImpactInCaloAlg&);

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    std::string m_TrackName;                    //!< jobOption: Track collection name
    const TrackCollection *m_tracks;            //!< pointer to TrackCollection
    std::string m_TrackParticleName;            //!< jobOption: TrackParticle collection name
    const Rec::TrackParticleContainer *m_particle;
    std::string m_trkinput;                     //!< jobOption: which input type to use

    std::string m_cluster_container;            //!< jobOption: Cluster container name
    std::string m_cell_container;               //!< jobOption: Cell container name
    const CaloCell_ID* m_calo_id;
    const CaloDetDescrManager* m_calo_dd;

    std::string m_ImpactInCalosOutputName;          //!< jobOption: Output name of ImpactInCaloCollection
    ToolHandle < IExtrapolateToCaloTool > m_toCalo; //!< the ExtrapolateToCaloTool
    ToolHandle < Trk::ITrackSelectorTool> m_trackSelectorTool;//!< handle to ITrackSelectorTool
    //CaloDepthTool* m_calodepth;
    //CaloPhiRange m_phiRange;

    bool m_extrapolateInBothDirections; //!< jobOption: extrapolate each track in both directions (along and opposite momentum)?
    bool m_compareImpWithCluster;       //!< jobOption: compare impacts with clusters?
    bool m_printCellsCrossed;           //!< jobOption: print cells crossed by track?

};

#endif //IMPACTINCALOALG_H

