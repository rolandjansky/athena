/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthTrackBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRUTHTRACKTOOLS_TRUTHTRACKBUILDER_H
#define TRK_TRUTHTRACKTOOLS_TRUTHTRACKBUILDER_H 1

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
// Trk includes
#include "TrkTruthTrackInterfaces/ITruthTrackBuilder.h"

class IPartPropSvc;
class AtlasDetectorID;

namespace HepPDT{
    class ParticleDataTable;
}

namespace Trk {
 
  class Track;
  class ITrackFitter;
  class IExtrapolator;
  class IRIO_OnTrackCreator;
   
  /**
   @class ITruthTrackBuilder

   @brief The truth track builder from a PRD trajectory
       
   @author Andreas.Salzburger -at- cern.ch, Thijs.Cornelissen -at- cern.ch
   */
     
  class TruthTrackBuilder : public AthAlgTool, public IIncidentListener, virtual public ITruthTrackBuilder {

     public:     
        //** Constructor with parameters */
       TruthTrackBuilder( const std::string& t, const std::string& n, const IInterface* p );
 
       // Athena algtool's Hooks
       StatusCode  initialize();
       StatusCode  finalize();

       /** return a map of GenParticles to PRDs for further processing **/
       Track* createTrack(const PRD_TruthTrajectory& prdTraj, SegmentCollection* segs = 0 ) const;

       /** the incident listener for cache refresh */
       void handle(const Incident& inc) ;
    

     private:
        ServiceHandle<IIncidentSvc>                 m_incidentSvc;               //!< Incident Service for cache cleaning
        ToolHandle<ITrackFitter>                    m_trackFitter;               //!< fits the PRDs
        ToolHandle<IExtrapolator>                   m_extrapolator;              //!< extrapolator
        ToolHandle< IRIO_OnTrackCreator >           m_rotcreator;
        ToolHandle< IRIO_OnTrackCreator >           m_rotcreatorbroad;
        
        const AtlasDetectorID                       *m_DetID;
        ServiceHandle<IPartPropSvc>                 m_particlePropSvc;           //!< Pointer to the particle properties svc */        
        const HepPDT::ParticleDataTable*            m_particleDataTable;         //!< ParticleDataTable needed to get connection pdg_code <-> charge */
        
        size_t                                      m_minNdof;                   //!< checks min degrees of freedom if bigger -1
        bool                                        m_onlyPrimaries;             //!< restrict track creation to primaries
        int                                         m_primaryBarcodeCutOff;      //!< primary barcode cut off
        unsigned int                                m_minSiHits;                 //!< min number of Si hits for refit
        unsigned int                                m_minSiHitsForward;          //!< min number of Si hits for refit in forward region (ITk specific)
        float                                       m_forwardBoundary;           //!< Boundary eta value defining the forward region
        
        bool                                        m_materialInteractions;      //!< run with material interactions
  };

} // end of namespace

#endif // TRK_TRUTHTRACKTOOLS_TRUTHTRACKBUILDER_H
