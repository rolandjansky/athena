/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PRD_PlanarTruthTrajectoryBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASRECOTOOLS_PRD_PLANARTRUTHTRAJECTORYBUILDER_H
#define ISF_FATRASRECOTOOLS_PRD_PLANARTRUTHTRAJECTORYBUILDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryBuilder.h"
#include "TrkTruthTrackInterfaces/PRD_TruthTrajectory.h"

class AtlasDetectorID;
class PRD_MultiTruthCollection;  

namespace HepMC {
  class GenParticle;
}
  
namespace Trk {
  class IPRD_Provider;
  class IPRD_TruthTrajectoryManipulator;
  class PrepRawData;
}
 
namespace iFatras {   
  /**
     @class IPRD_TruthTrajectoryBuilder

     @brief The truth PRD trajectory builder, works with a cache, calling the ID sub-helpers

     It is based on the Trk::PRD_TruthTrajectoryBuilder and is made for working with iFatras::PlanarCluster 
     to avoid ISF dependencies in the Tracking package
       
     The truth PRD trajectory builder, works with a cache, and calls the ID sub-helpers. This is done
     because it should also run for upgrade layouts and thus not know about dedicated detector technologies.
     The cache is filled and created in the BeginEvent incident, the accessible PRDs are steered by the PRD multi truth collections
     and handed forward from the IPRD_Provider (which should use a hash-lookup)
   
     A dedicated PRD_TruthTrajectoryManipulator can be used to shape the TruthTrajectories after beeing created

     @author Noemi.Calace -at- cern.ch
   */
     
  class PRD_PlanarTruthTrajectoryBuilder : public AthAlgTool, virtual public Trk::IPRD_TruthTrajectoryBuilder {

     public:     
        //** Constructor with parameters */
       PRD_PlanarTruthTrajectoryBuilder( const std::string& t, const std::string& n, const IInterface* p );
 
       // Athena algtool's Hooks
       StatusCode  initialize();
       StatusCode  finalize();

       /** return a vector of PrepRawData trajectories - uses internal cache**/
       const std::map< const HepMC::GenParticle*, Trk::PRD_TruthTrajectory >& truthTrajectories() const;

       /** Event refresh - can't be an IIncident, because it has to run after PRD creation and PRD truth creation */
       StatusCode refreshEvent();

     private:
       //! Helper to detect type of sub-detector from PRD->identify().
       const AtlasDetectorID*                                 m_idHelper;
       
       //!< Identifier to PRD relation in the Inner Detector for Planar Clusters
       ToolHandle<Trk::IPRD_Provider>                         m_idPrdPlanarClusterProvider;  
       
       //!< PRD truth tracjectory manipulators
       ToolHandleArray<Trk::IPRD_TruthTrajectoryManipulator>  m_prdTruthTrajectoryManipulators;   
       
       //!< PRD multi truth collection names this builder is working on
       std::vector<std::string>                               m_prdPlanarMultiTruthCollectionNames;     

       //!< the retrieved PRD muli truth collections
       std::vector<const PRD_MultiTruthCollection*>           m_prdPlanarMultiTruthCollections;         
        
       //!< minimum pT to be even considered
       double                                                 m_minPt;
       //!< Track geantinos or not        
       bool                                                   m_geantinos;
       //!< the cache for the return (cleared by Incident)
       mutable std::map< const HepMC::GenParticle*, Trk::PRD_TruthTrajectory > m_gpPrdTruthTrajectories; 
  };

} // end of namespace

#endif // ISF_FATRASRECOTOOLS_PRD_PLANARTRUTHTRAJECTORYBUILDER_H
