/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRUTHSUMMARYTOOL_H
#define MUON_MUONTRUTHSUMMARYTOOL_H

#include <atomic>
#include <functional>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

namespace Trk {
class Track;
class MeasurementBase;
}  // namespace Trk

class IIncidentSvc;
class ITHistSvc;
class TTree;

namespace Muon {
class MuonSegment;

/** Interface for tools calculating hit count summaries for track */
class MuonTruthSummaryTool : public AthAlgTool, virtual public IMuonTruthSummaryTool, virtual public IIncidentListener {
  public:
    //** Constructor with parameters */
    MuonTruthSummaryTool(const std::string& t, const std::string& n, const IInterface* p);

    // Athena Hooks
    StatusCode initialize();
    StatusCode finalize();

    /** clear tool */
    void clear();

    /** init truth */
    void init() const;

    /** get the associated barcode for the identifier, return -1 if the channel was not hit by a muon */
    int getBarcode(const Identifier& id) const;

    /** get the associated pdgId for a given barcode */
    int getPdgId(int barcode) const;

    /** add identifier */
    void add(const Identifier& id, int level) const;

    /** add segment */
    void add(const MuonSegment& seg, int level) const;

    /** add track */
    void add(const Trk::Track& track, int level) const;


    /** print summary */
    std::string printSummary();

    void handle(const Incident& inc);

    struct IdentifierHash {
        size_t operator()(const Identifier& id) const
        {
            return std::hash<Identifier::value_type>{}(id.get_compact());
        }
    };

  private:
    /** add measurements */
    void        add(const std::vector<const Trk::MeasurementBase*>& measurements, int level) const;
    std::string printSummary(const std::unordered_set<Identifier, IdentifierHash>& truth,
                             const std::unordered_set<Identifier, IdentifierHash>& found);

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<IMuonEDMHelperSvc>      m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                    "Handle to the service providing the IMuonEDMHelperSvc interface"};
    ToolHandle<MuonEDMPrinterTool>        m_printer;
    ServiceHandle<IIncidentSvc>           m_incidentSvc;
    mutable std::atomic<bool>             m_wasInit;

    SG::ReadHandleKeyArray<PRD_MultiTruthCollection> m_TruthNames{
        this, "TruthNames", {"RPC_TruthMap", "TGC_TruthMap", "MDT_TruthMap"}, "truth names"};

    mutable std::unordered_map<int, int> m_pdgIdLookupFromBarcode ATLAS_THREAD_SAFE;  // protected by mutex
    mutable std::unordered_map<Identifier, int, IdentifierHash> m_truthHits
        ATLAS_THREAD_SAFE;  // protected by mutex. map containing truth hits associated with muons, stores barcode as
                            // second element
    mutable std::unordered_map<int, std::unordered_set<Identifier, IdentifierHash>> m_truthDataPerLevel
                                          ATLAS_THREAD_SAFE;  // protected by mutex
    std::unordered_map<int, unsigned int> m_lossesPerLevel;
    mutable unsigned int m_truthHitsTotal ATLAS_THREAD_SAFE;  // protected by mutex

    mutable std::recursive_mutex m_mutex;

    TTree*       m_tree;
    ITHistSvc*   m_thistSvc;
    bool         m_writeTree;  // Set to true in order to write out ntuple
    std::string  m_treeName;
    std::string  m_histStream;
    unsigned int m_level;
    int          m_selectedPdgId;

    std::vector<unsigned int>          m_numChambers;
    std::vector<std::vector<uint8_t>*> m_numMissedHits;
    std::vector<std::vector<uint8_t>*>
                                   m_missedHitTechnologyIndex;  // [0..5] maps to "MDT", "CSC", "RPC", "TGC", "STGC", "MM"
    std::vector<std::vector<int>*> m_missedHitStationPhi;       // increases with eta (0 at eta=0)
    std::vector<std::vector<int>*> m_missedHitStationSector;    // increases with eta (0 at eta=0)
    std::vector<std::vector<int>*> m_missedHitStationEta;       // increases with phi
    // std::vector< std::vector<float>    *  >     m_missedHitR; // Can't do these without re-writing code a bit to have
    // link to Meas. Not sure it's worth it. std::vector< std::vector<float>    *  >     m_missedHitZ; std::vector<
    // std::vector<std::string>* >         m_missedHitStationName;     // "BIL", "EMS", "T1F", etc.
    std::vector<std::vector<int>*> m_missedHitStationNameIndex;  // BI=0 , BM=1, BO=2, BE=3,

    void fillChamberVariables(const Identifier& chamberId, const unsigned int numMissedHits);
    void clearChamberVariables(const unsigned int level);
    void initChamberVariables(const unsigned int levels);
};

}  // namespace Muon

#endif
