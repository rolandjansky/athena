/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PRD_TruthTrajectoryBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRUTHTRACKTOOLS_PRD_TRUTHTRAJECTORYBUILDER_H
#define TRK_TRUTHTRACKTOOLS_PRD_TRUTHTRAJECTORYBUILDER_H 1

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryBuilder.h"
#include "TrkTruthTrackInterfaces/PRD_TruthTrajectory.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryManipulator.h"
#include "TrkToolInterfaces/IPRD_Provider.h"

class AtlasDetectorID;

#include "AtlasHepMC/GenParticle_fwd.h"
  
namespace Trk {
    
  class PrepRawData;
    
  /**
   @class IPRD_TruthTrajectoryBuilder

   @brief The truth PRD trajectory builder, works with a cache, it distinguishes between ID and MS and calls the sub-helpers

   The truth PRD trajectory builder, works with a cache, it distinguishes between ID and MS and calls the sub-helpers. This is done
   because it should also run for upgrade layouts and thus not know about dedicated detector technologies.
   The cache is filled and created in the BeginEvent incident, the accessible PRDs are steered by the PRD multi truth collections
   and handed forward from the IPRD_Provider (which should use a hash-lookup)
   
   A dedicated PRD_TruthTrajectoryManipulator can be used to shape the TruthTrajectories after beeing created
       
   @author Andreas.Salzburger -at- cern.ch, Thijs.Cornelissen -at- cern.ch
   */
     
  class PRD_TruthTrajectoryBuilder : public AthAlgTool, virtual public IPRD_TruthTrajectoryBuilder {

     public:     
        //** Constructor with parameters */
       PRD_TruthTrajectoryBuilder( const std::string& t, const std::string& n, const IInterface* p );
 
       // Athena algtool's Hooks
       StatusCode  initialize();
       StatusCode  finalize();

       /** return a vector of PrepRawData trajectories - uses internal cache**/
       const std::map< const HepMC::GenParticle*, PRD_TruthTrajectory >& truthTrajectories() const;

       /** Event refresh - can't be an IIncident, because it has to run after PRD creation and PRD truth creation */
       StatusCode refreshEvent();

     private:
       const AtlasDetectorID*                               m_idHelper;                         //! Helper to detect type of sub-detector from PRD->identify().
                                                                                                
        ToolHandle<IPRD_Provider>                           m_idPrdProvider;                    //!< Identifier to PRD relation in the Inner Detector
        ToolHandle<IPRD_Provider>                           m_msPrdProvider;                    //!< Identifier to PRD relation in the Muons System
        
        ToolHandleArray<IPRD_TruthTrajectoryManipulator>    m_prdTruthTrajectoryManipulators;   //!< PRD truth tracjectory manipulators
        
	SG::ReadHandleKeyArray<PRD_MultiTruthCollection> m_prdMultiTruthCollectionNames {this,"PRD_MultiTruthCollections",{"PixelClusterTruth","SCT_ClusterTruth","TRT_DriftCircleTruth"}, "PRD multi truth collection names this builder is working on"};
	std::vector<const PRD_MultiTruthCollection*> m_prdMultiTruthCollections;         //!< the retrieved PRD muli truth collections
        
	Gaudi::Property<double>                             m_minPt {this,"MinimumPt",400.,"minimum pT to be even considered"};
	Gaudi::Property<bool>                               m_geantinos {this,"Geantinos",false,"Track geantinos or not"};
        mutable std::map< const HepMC::GenParticle*, PRD_TruthTrajectory > m_gpPrdTruthTrajectories; //!< the cache for the return (cleared by Incident)
        
  };

} // end of namespace

#endif // TRK_TRUTHTRACKTOOLS_PRD_TRUTHTRAJECTORYBUILDER_H
