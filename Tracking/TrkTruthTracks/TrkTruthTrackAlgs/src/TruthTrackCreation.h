/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthTrackCreation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKTRUTHTRACKALGS_TRUTHTRACKCREATION_H
#define TRKTRUTHTRACKALGS_TRUTHTRACKCREATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"

// GaudiHandle
#include "GaudiKernel/ToolHandle.h"
#include <string>

class AtlasDetectorID;

namespace Trk 
{

  class IPRD_TruthTrajectoryBuilder;
  class IPRD_TruthTrajectorySelector;  
  class ITruthTrackBuilder;
  class ITrackSelectorTool;
  class IPRD_AssociationTool;
  class ITrackSummaryTool;
    
  /** @class TruthTrackCreation
      
      @brief Algorithm to convert PRD multi truth maps into a track collection 
      
      Algorithm to convert PRD multi truth maps into a track collection,
      basic cuts on the prdTruthTrajectory can be applied. Additionally a track selector
      can be used a posteriory to mimic some efficiency effects. 
      
      @author  Andreas Salzburger <Andreas.Salzburger@cern.ch>
  */  

  class TruthTrackCreation : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       TruthTrackCreation(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~TruthTrackCreation();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:

        std::string                                         m_outputTrackCollectionName;      //!< output collection for truth tracks
        std::string                                         m_skippedTrackCollectionName;     //!< output collection for skipped tracks
        std::string                                         m_outputSegmentCollectionName;    //!< output collection for truths segments
                                                            
        ToolHandle<Trk::IPRD_TruthTrajectoryBuilder>        m_prdTruthTrajectoryBuilder;      //!< truth tools
        ToolHandle<Trk::ITruthTrackBuilder>                 m_truthTrackBuilder;              //!< truth tools

        ToolHandleArray<Trk::IPRD_TruthTrajectorySelector>  m_prdTruthTrajectorySelectors;    //!< PRD truth trajectory selectors
        ToolHandleArray<Trk::ITrackSelectorTool>            m_trackSelectors;                 //!< track selectors for a posteriory track selection
        ToolHandle<Trk::IPRD_AssociationTool>               m_assoTool;                       //!< association tool for PRDs
        ToolHandle<Trk::ITrackSummaryTool>                  m_trackSummaryTool;               //!< summary tool for completing the track

      
    }; 
} // end of namespace

#endif 
