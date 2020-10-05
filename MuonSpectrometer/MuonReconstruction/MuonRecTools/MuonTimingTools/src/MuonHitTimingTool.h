/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONHITTIMINGTOOL_H
#define MUON_MUONHITTIMINGTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/** @class MuonHitTimingTool
    
    MuonHitTimingTool calculates the time shift for a set of cluster hits WRT the current bunch
    Internally handles different technolgies

    @author MCP projects
*/


namespace Muon{

  class MuonClusterOnTrack;
  
  class MuonHitTimingTool : virtual public Muon::IMuonHitTimingTool, public AthAlgTool{
  public:
    MuonHitTimingTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor **/
    virtual ~MuonHitTimingTool()=default;

    /** standard initialization method **/
    virtual StatusCode initialize();

    /** Calculate the time offset of a given set of hits wrt to the current bunch */
    TimingResult calculateTimingResult( const std::vector<const MuonClusterOnTrack*>& hits ) const;

    /** return a set of technologies accepted by the tool */
    std::set<MuonStationIndex::TechnologyIndex> acceptedTechnologies() const { return m_acceptedTechnologies; }

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this,"MuonIdHelperSvc","Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandleArray<IMuonHitTimingTool> m_hitTimingTools;
    std::set<MuonStationIndex::TechnologyIndex>  m_acceptedTechnologies;
  };
}

#endif
