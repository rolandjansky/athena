/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRUTHSUMMARYTOOL_H
#define MUON_MUONTRUTHSUMMARYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include <string>
#include <set>
#include <map>

namespace Trk {
  class Track;
  class MeasurementBase;
}

class IIncidentSvc;

namespace Muon {
  class MuonSegment;

  /** Interface for tools calculating hit count summaries for track */
  class MuonTruthSummaryTool : public AthAlgTool, virtual public IMuonTruthSummaryTool, virtual public IIncidentListener {      
  public:
    //** Constructor with parameters */
    MuonTruthSummaryTool( const std::string& t, const std::string& n, const IInterface* p );
 
    // Athena Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /** clear tool */
    void clear();

    /** init truth */
    void init();

    /** add identifier */
    void add( const Identifier& id, int level );

    /** add segment */
    void add( const MuonSegment& seg, int level );

    /** add track */
    void add( const Trk::Track& track, int level );

    /** print summary */
    std::string printSummary() const;

    void handle(const Incident& inc);

  private:

    /** add measurements */
    void add( const std::vector<const Trk::MeasurementBase*>& measurements, int level );
    void getTruth(std::string name );
    std::string printSummary( const std::set<Identifier>& truth, const std::set<Identifier>& found ) const;

    ToolHandle<MuonIdHelperTool>                m_idHelper;
    ToolHandle<MuonEDMHelperTool>               m_helper;
    ToolHandle<MuonEDMPrinterTool>              m_printer;
    ServiceHandle< IIncidentSvc >               m_incidentSvc;
    bool m_wasInit;

    std::string m_CSC_TruthName;
    std::string m_RPC_TruthName;
    std::string m_TGC_TruthName;
    std::string m_MDT_TruthName;
    std::string m_MM_TruthName;
    std::string m_STGC_TruthName;

    std::set<Identifier> m_truthHits;
    std::map<int,std::set<Identifier> > m_truthDataPerLevel;
  };
  
} // end of name space

#endif

