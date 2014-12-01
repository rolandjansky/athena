/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetAmbiTrackSelectionTool.h, (c) ATLAS Detector software
// Markus Elsing
///////////////////////////////////////////////////////////////////

#ifndef INDETINDETAMBITRACKSELECTIONTOOL_H
#define INDETINDETAMBITRACKSELECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include <map>
#include <vector>

class SiliconID;
class Identifier;
class IBLParameterSvc;
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
  class ITrtDriftCircleCutTool;

  class InDetAmbiTrackSelectionTool : virtual public Trk::IAmbiTrackSelectionTool, public AthAlgTool
    {
    public:
      InDetAmbiTrackSelectionTool(const std::string&,const std::string&,const IInterface*);
      
      /** default destructor */
      virtual ~InDetAmbiTrackSelectionTool ();
      
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      virtual const Trk::Track* getCleanedOutTrack(const Trk::Track*, const Trk::TrackScore score) ;
      virtual StatusCode registerPRDs(const Trk::Track* ptrTrack);
      virtual void reset();
      virtual std::vector<const Trk::PrepRawData*> getPrdsOnTrack(const Trk::Track* ptrTrack);
      
      
    private:
      
      /** method to create a new track from a vector of TSOS's */
      const Trk::Track* createSubTrack( const std::vector<const Trk::TrackStateOnSurface*>& tsos, const Trk::Track* track ) const ;
      
      /**Association tool - used to work out which (if any) PRDs are shared between 
       tracks*/
      ToolHandle<Trk::IPRD_AssociationTool> m_assoTool;
      /** TRT minimum number of drift circles tool- returns allowed minimum number of TRT drift circles */
      ToolHandle<ITrtDriftCircleCutTool>    m_selectortool;
      ServiceHandle<IBLParameterSvc>                        m_IBLParameterSvc; 
      
      /**atlas id helper*/
      const SiliconID* m_detID;
      
      /** some cut values */
      int m_minHits;
      int m_minTRT_Hits;
      int m_maxShared;
      int m_maxSharedModules;
      int m_maxTracksPerPRD;
      int m_minNotShared;
      float m_minScoreShareTracks;
      bool m_cosmics;      
      bool m_parameterization; // Use table of min number DCs
      bool m_doPixelClusterSplitting;
      float m_sharedProbCut;
      int   m_maxSplitSize; //!< A.S.: to be removed once EDM is updated
    }; 
} // end of namespace

inline StatusCode InDet::InDetAmbiTrackSelectionTool::registerPRDs(const Trk::Track* ptrTrack)
{
  return m_assoTool->addPRDs(*ptrTrack);
}

inline void InDet::InDetAmbiTrackSelectionTool::reset()
{
  m_assoTool->reset();
}
inline std::vector<const Trk::PrepRawData*> InDet::InDetAmbiTrackSelectionTool::getPrdsOnTrack(const Trk::Track* ptrTrack)
{
  return m_assoTool->getPrdsOnTrack(*ptrTrack);
}


#endif 
