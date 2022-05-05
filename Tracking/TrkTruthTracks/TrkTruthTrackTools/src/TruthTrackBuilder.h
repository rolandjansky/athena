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
#include "GaudiKernel/IPartPropSvc.h"
// Trk includes
#include "TrkTruthTrackInterfaces/ITruthTrackBuilder.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

class AtlasDetectorID;

namespace HepPDT{
    class ParticleDataTable;
}

namespace Trk {
 
  class Track;
   
  /**
   @class ITruthTrackBuilder

   @brief The truth track builder from a PRD trajectory
       
   @author Andreas.Salzburger -at- cern.ch, Thijs.Cornelissen -at- cern.ch
   */
     
  class TruthTrackBuilder : public AthAlgTool, virtual public ITruthTrackBuilder {

     public:     
        //** Constructor with parameters */
       TruthTrackBuilder( const std::string& t, const std::string& n, const IInterface* p );
 
       // Athena algtool's Hooks
       StatusCode  initialize();
       StatusCode  finalize();

       /** return a map of GenParticles to PRDs for further processing **/
       Track* createTrack(const PRD_TruthTrajectory& prdTraj, SegmentCollection* segs = 0 ) const;

     private:
        ToolHandle<ITrackFitter>                    m_trackFitter{this, "TrackFitter", ""};               //!< fits the PRDs
        ToolHandle<IExtrapolator>                   m_extrapolator{this, "ExtrapolationTool", ""};              //!< extrapolator
        ToolHandle< IRIO_OnTrackCreator >           m_rotcreator{this, "RotCreatorTool", ""};
        ToolHandle< IRIO_OnTrackCreator >           m_rotcreatorbroad{this, "BroadRotCreatorTool", ""};
        
        const AtlasDetectorID                       *m_DetID;
        ServiceHandle<IPartPropSvc>                 m_particlePropSvc;           //!< Pointer to the particle properties svc */        
        const HepPDT::ParticleDataTable*            m_particleDataTable;         //!< ParticleDataTable needed to get connection pdg_code <-> charge */
        
        Gaudi::Property<size_t>                     m_minNdof{this, "MinDegreesOfFreedom", 6};                   //!< checks min degrees of freedom if bigger -1
        Gaudi::Property<bool>                       m_onlyPrimaries{this, "OnlyPrimaries", false};             //!< restrict track creation to primaries
        Gaudi::Property<int>                        m_primaryBarcodeCutOff{this, "PrimaryBarcodeCutOff", 100000};      //!< primary barcode cut off
        Gaudi::Property<unsigned int>               m_minSiHits{this, "MinSiHits", 7};                 //!< min number of Si hits for refit
        Gaudi::Property<unsigned int>               m_minSiHitsForward{this, "MinSiHitsForward", 7};          //!< min number of Si hits for refit in forward region (ITk specific)
        Gaudi::Property<float>                      m_forwardBoundary{this, "ForwardBoundary", 2.5};           //!< Boundary eta value defining the forward region
        
        Gaudi::Property<int> m_matEffects {this, "MatEffects", 3,
                "Type of material interaction in extrapolation (Default Pion)"}; 
  };

} // end of namespace

#endif // TRK_TRUTHTRACKTOOLS_TRUTHTRACKBUILDER_H

