// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetAmbiTrackSelectionTool.h, (c) ATLAS Detector software
// Markus Elsing
///////////////////////////////////////////////////////////////////

#ifndef INDETINDETAMBITRACKSELECTIONTOOL_H
#define INDETINDETAMBITRACKSELECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"

#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "PixelGeoModel/IBLParameterSvc.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkTrack/TrackStateOnSurface.h"

#include "GaudiKernel/ToolHandle.h"

#include <map>
#include <vector>

class SiliconID;
class Identifier;
namespace Trk {
  class Track;
}

namespace InDet 
{

  /** @class InDetAmbiTrackSelectionTool 
      This tool cross checks the hits on a track with the hits already stored in 
      a PRD_AssociationTool. Shared hits are allowed to some extent und under certain conditions
      (e.g. the track score is high enough), additional shared hits are removed if necessary.
      This tool does not delete any tracks. If the client is going to discard tracks entered into
      this tool, the tracks have to be deleted by this client.
      
      @author  Martin Siebel <Martin.Siebel@CERN.ch>
  */  

  class InDetAmbiTrackSelectionTool : public extends<AthAlgTool, Trk::IAmbiTrackSelectionTool>
  {
  public:
    InDetAmbiTrackSelectionTool(const std::string&,const std::string&,const IInterface*);
      
    /** default destructor */
    virtual ~InDetAmbiTrackSelectionTool () = default;
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;

    virtual std::tuple<Trk::Track*,bool> getCleanedOutTrack(const Trk::Track *track,
                                                            const Trk::TrackScore score,
                                                            Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                            Trk::PRDtoTrackMap &prd_to_track_map) const override;

  private:
      
    /** method to create a new track from a vector of TSOS's */
    Trk::Track* createSubTrack( const std::vector<const Trk::TrackStateOnSurface*>& tsos, const Trk::Track* track ) const ;

    /** TRT minimum number of drift circles tool- returns allowed minimum number of TRT drift circles */
    PublicToolHandle<ITrtDriftCircleCutTool> m_selectortool{this, "DriftCircleCutTool", "InDet::InDetTrtDriftCircleCutTool"};
    ServiceHandle<IBLParameterSvc> m_IBLParameterSvc{this, "IBLParameterSvc", "IBLParameterSvc"};

    ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
         {this, "AssociationTool", "Trk::PRDtoTrackMapTool" };

    /**atlas id helper*/
    const SiliconID* m_detID{nullptr};
      
    /** some cut values */
    IntegerProperty m_minHits{this, "minHits", 5};
    IntegerProperty m_minTRT_Hits{this, "minTRTHits", 0};
    int m_maxShared{0};
    IntegerProperty m_maxSharedModules{this, "maxShared", 1};
    IntegerProperty m_maxTracksPerPRD{this, "maxTracksPerSharedPRD", 2};
    IntegerProperty m_minNotShared{this, "minNotShared", 6};
    FloatProperty m_minScoreShareTracks{this, "minScoreShareTracks", 0.0};
    BooleanProperty m_cosmics{this, "Cosmics", false};
    BooleanProperty m_parameterization{this, "UseParameterization", true}; // Use table of min number DCs
    BooleanProperty m_doPixelClusterSplitting{this, "doPixelSplitting", false};
    FloatProperty m_sharedProbCut{this, "sharedProbCut", 0.02};
    IntegerProperty m_maxSplitSize{this, "MaximalSplitSize", 49, "A.S.: remove that when solved properly by updating the SplitProb info with isExcluded. A.S.: to be removed once EDM is updated"};
  }; 
} // end of namespace


#endif 
