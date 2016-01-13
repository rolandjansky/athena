/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonAmbiTrackSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUON_MUONAMBITRACKSELECTIONTOOL_H
#define MUON_MUONAMBITRACKSELECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IAmbiTrackSelectionTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include <map>
#include <vector>

class Identifier;

namespace Trk{
class Track;
}


namespace Muon 
{

  class MuonEDMPrinterTool;
  class MuonIdHelperTool;

  /** @class MuonAmbiTrackSelectionTool 
      This tool cross checks the hits on a track with the hits already stored in 
      a PRD_AssociationTool. Shared hits are allowed to some extent und under certain conditions
      (e.g. the track score is high enough), additional shared hits are removed if necessary.
      This tool does not delete any tracks. If the client is going to discard tracks entered into
      this tool, the tracks have to be deleted by this client.
      
      @author  Niels van Eldik <niels.van.eldik@cern.ch>
  */  

  class MuonAmbiTrackSelectionTool : virtual public Trk::IAmbiTrackSelectionTool, public AthAlgTool {
  public:
    MuonAmbiTrackSelectionTool(const std::string&,const std::string&,const IInterface*);
      
    /** default destructor */
    virtual ~MuonAmbiTrackSelectionTool ();
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
      
    virtual const Trk::Track* getCleanedOutTrack(const Trk::Track*, const Trk::TrackScore score) ;
    virtual StatusCode registerPRDs(const Trk::Track* ptrTrack);
    virtual void reset();
    virtual std::vector<const Trk::PrepRawData*> getPrdsOnTrack(const Trk::Track* ptrTrack);
      
      
  private:
      
    /**Association tool - used to work out which (if any) PRDs are shared between 
       tracks*/
    ToolHandle<Trk::IPRD_AssociationTool> m_assoTool;
    ToolHandle<Muon::MuonEDMPrinterTool>  m_printer;
    ToolHandle<Muon::MuonIdHelperTool>    m_idHelperTool;

    /** maximum hit overlap fraction between two track, if higher track will be rejected*/
    double m_maxOverlapFraction;
    
    /** flag to keep partial overlaps */
    bool m_keepPartial;

    /** flag to keep overlaps which share more than one presicion layer */
    bool m_keepMoreThanOne;
  }; 
} // end of namespace

inline StatusCode Muon::MuonAmbiTrackSelectionTool::registerPRDs(const Trk::Track* ptrTrack)
{
  return m_assoTool->addPRDs(*ptrTrack);
}

inline void Muon::MuonAmbiTrackSelectionTool::reset()
{
  m_assoTool->reset();
}
inline std::vector<const Trk::PrepRawData*> Muon::MuonAmbiTrackSelectionTool::getPrdsOnTrack(const Trk::Track* ptrTrack)
{
  return m_assoTool->getPrdsOnTrack(*ptrTrack);
}


#endif 
