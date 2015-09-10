/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONHITTIMINGTOOL_H
#define MUON_MUONHITTIMINGTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"

/** @class MuonHitTimingTool
    
    MuonHitTimingTool calculates the time shift for a set of cluster hits WRT the current bunch
    Internally handles different technolgies

    @author MCP projects
*/


namespace Muon{

  class MuonClusterOnTrack;
  class MuonIdHelperTool;
  
  class MuonHitTimingTool : virtual public Muon::IMuonHitTimingTool, public AthAlgTool{
  public:
    MuonHitTimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor **/
    virtual ~MuonHitTimingTool();

    /** standard initialization method **/
    virtual StatusCode initialize();
    
    /** standard finalization method **/
    virtual StatusCode finalize();

    /** Calculate the time offset of a given set of hits wrt to the current bunch */
    TimingResult calculateTimingResult( const std::vector<const MuonClusterOnTrack*>& hits ) const;

    /** return a set of technologies accepted by the tool */
    std::set<MuonStationIndex::TechnologyIndex> acceptedTechnologies() const { return m_acceptedTechnologies; }

  private:
    ToolHandle<MuonIdHelperTool>                 m_idHelper;
    std::vector<ToolHandle<IMuonHitTimingTool> > m_hitTimingTools;
    std::set<MuonStationIndex::TechnologyIndex>  m_acceptedTechnologies;
  };
}

#endif
