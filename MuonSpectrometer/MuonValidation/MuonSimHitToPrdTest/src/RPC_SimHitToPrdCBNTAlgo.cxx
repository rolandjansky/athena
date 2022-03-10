/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_SimHitToPrdCBNTAlgo.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GeoAdaptors/GeoMuonHits.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcPad.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include "StoreGate/ReadHandle.h"

namespace {
    /// Maximal number of particles
    constexpr int MAX_PART = 100;
    constexpr int MAX_SIMHRPC = 10000;
    constexpr int MAX_DIGITRPC = 10000;
    constexpr int MAX_PADRPC = 1000;
    constexpr int MAX_RDOCHRPC = 10000;
    constexpr int MAX_PRDRPC = 10000;
}  // namespace
class RPC_SimHitToPrdCBNTAlgo::Clockwork {
public:
    // Constructor:
    Clockwork() = default;

    // Destructor:
    ~Clockwork() = default;

    // Point to NTuple:
    NTuple::Tuple* nt{nullptr};

    NTuple::Item<long> run;
    NTuple::Item<long> event;

    NTuple::Item<int> m_npartStored;
    NTuple::Array<float> m_partVertX;
    NTuple::Array<float> m_partVertY;
    NTuple::Array<float> m_partVertZ;
    NTuple::Array<float> m_partVertT;
    NTuple::Array<int> m_partPdgId;
    NTuple::Array<float> m_partMomX;
    NTuple::Array<float> m_partMomY;
    NTuple::Array<float> m_partMomZ;
    NTuple::Array<int> m_partStatus;
    NTuple::Array<int> m_partBarcode;
    NTuple::Array<float> m_partMass;

    NTuple::Item<int> m_SimHit_nRpc;  // number of SimHits in the collection

    NTuple::Array<int> m_SimHit_station;        // Return the station
    NTuple::Array<int> m_SimHit_eta;            // Return the station eta
    NTuple::Array<int> m_SimHit_phi;            // Return the station phi
    NTuple::Array<int> m_SimHit_doubletR;       // Return doubletR
    NTuple::Array<int> m_SimHit_doubletZ;       // Return doubletZ
    NTuple::Array<int> m_SimHit_doubletPhi;     // Return doubletPhi
    NTuple::Array<int> m_SimHit_gasGap;         // Return gasGap
    NTuple::Array<int> m_SimHit_measuresPhi;    // Return measuresPhi
    NTuple::Array<float> m_SimHit_time;         // Return the time
    NTuple::Array<float> m_SimHit_localx;       // Return the strip pos x  SD local frame
    NTuple::Array<float> m_SimHit_localy;       // Return the strip pos y
    NTuple::Array<float> m_SimHit_localz;       // Return the strip pos z
    NTuple::Array<float> m_SimHit_globalx;      // Return the strip pos x  global frame
    NTuple::Array<float> m_SimHit_globaly;      // Return the strip pos y
    NTuple::Array<float> m_SimHit_globalz;      // Return the strip pos z
    NTuple::Array<float> m_SimHit_trackNumber;  // Return the trackNumber of associated track
    NTuple::Array<float> m_SimHit_stepLen;      // length of the G4 step
    NTuple::Array<float> m_SimHit_energyDep;    // G4 energy loss along step
    NTuple::Array<float> m_SimHit_kinEnergy;    // initial kin energy of particle producing the hit
    NTuple::Array<int> m_SimHit_pdgId;          // pdgId of particle producing the hit

    NTuple::Item<int> m_nPads;              // number of pads in the container
    NTuple::Array<int> m_rpc_pad_id;        // pad id
    NTuple::Array<int> m_rpc_pad_bcid;      // pad bcid
    NTuple::Array<int> m_rpc_pad_lvl1id;    // pad lvl1 id
    NTuple::Array<int> m_rpc_pad_sectorid;  // pad sector id
    NTuple::Array<int> m_rpc_pad_error;     // pad error code
    NTuple::Array<int> m_rpc_pad_status;    // pad status

    NTuple::Item<int> m_nCMA;            // number of coincidence matrices
    NTuple::Item<int> m_nFiredChannels;  // number of fired channel
    NTuple::Array<int> m_hashId;         // hash id for pad == offline id
    NTuple::Array<int> m_sector;         // pad sector Id
    NTuple::Array<int> m_padId;          // pad Id
    NTuple::Array<int> m_status;         // status
    NTuple::Array<int> m_errorCode;      // error code
    NTuple::Array<int> m_cmaId;          // CMA Id
    NTuple::Array<int> m_fel1Id;         // fel1Id
    NTuple::Array<int> m_febcId;         // febcId
    NTuple::Array<int> m_crc;            // crc
    NTuple::Array<int> m_bcId;           // bc Id
    NTuple::Array<int> m_time;           // time
    NTuple::Array<int> m_ijk;            // ijk code
    NTuple::Array<int> m_channel;        // fired channel
    NTuple::Array<int> m_overlap;        // overlap
    NTuple::Array<int> m_threshold;      // threshold

    NTuple::Item<int> m_digit_nRpcColl;  // number of collection in the container
    NTuple::Item<int> m_digit_nRpc;      // number of digit in the collection

    NTuple::Array<float> m_digit_time;       // Return the time
    NTuple::Array<int> m_digit_station;      // Return the station
    NTuple::Array<int> m_digit_eta;          // Return the station eta
    NTuple::Array<int> m_digit_phi;          // Return the station phi
    NTuple::Array<int> m_digit_doubletR;     // Return doubletR
    NTuple::Array<int> m_digit_doubletZ;     // Return doubletZ
    NTuple::Array<int> m_digit_doubletPhi;   // Return doubletPhi
    NTuple::Array<int> m_digit_gasGap;       // Return gasGap
    NTuple::Array<int> m_digit_measuresPhi;  // Return measuresPhi
    NTuple::Array<int> m_digit_strip;        // Return the strip #
    NTuple::Array<float> m_digit_stripx;     // Return the strip pos x
    NTuple::Array<float> m_digit_stripy;     // Return the strip pos y
    NTuple::Array<float> m_digit_stripz;     // Return the strip pos z

    NTuple::Item<int> m_nRpcColl;  // number of collection in the container
    NTuple::Item<int> m_nRpcPrd;   // number of digit in the collection

    NTuple::Array<float> m_prd_time;         // Return the time
    NTuple::Array<int> m_prd_station;        // Return the station
    NTuple::Array<int> m_prd_eta;            // Return the station eta
    NTuple::Array<int> m_prd_phi;            // Return the station phi
    NTuple::Array<int> m_prd_doubletR;       // Return doubletR
    NTuple::Array<int> m_prd_doubletZ;       // Return doubletZ
    NTuple::Array<int> m_prd_doubletPhi;     // Return doubletPhi
    NTuple::Array<int> m_prd_gasGap;         // Return gasGap
    NTuple::Array<int> m_prd_measuresPhi;    // Return measuresPhi
    NTuple::Array<int> m_prd_strip;          // Return the strip #
    NTuple::Array<float> m_prd_stripx;       // Return the strip pos x
    NTuple::Array<float> m_prd_stripy;       // Return the strip pos y
    NTuple::Array<float> m_prd_stripz;       // Return the strip pos z
    NTuple::Array<float> m_prd_triggerInfo;  // Return trigger info
    NTuple::Array<float> m_prd_ambigFlag;    // Return ambiguity flag
};

RPC_SimHitToPrdCBNTAlgo::~RPC_SimHitToPrdCBNTAlgo() = default;
// Algorithm constructor
RPC_SimHitToPrdCBNTAlgo::RPC_SimHitToPrdCBNTAlgo(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {
    m_c = std::make_unique<Clockwork>();
}

// Initialize method

StatusCode RPC_SimHitToPrdCBNTAlgo::initialize() {
    ATH_MSG_DEBUG("in initialize()");

    ATH_CHECK(detStore()->retrieve(m_muonMgr, "Muon"));
    ATH_CHECK(m_idHelperSvc.retrieve());

    // book ntuple
    NTupleFilePtr file(ntupleSvc(), "/NTUPLES/FILE");

    if (!file) return StatusCode::FAILURE;

    NTuple::Directory* col = ntupleSvc()->createDirectory("/NTUPLES/FILE/RPCvalidNT");

    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE/RPCvalidNT/RPC");
    if (!nt)
        nt = ntupleSvc()->book(col, 1, CLID_ColumnWiseTuple, "RPC");
    else {
        ATH_MSG_ERROR("Cannot book the ntuple");
        return StatusCode::FAILURE;
    }

    // event info
    ATH_CHECK(nt->addItem("Run", m_c->run));
    ATH_CHECK(nt->addItem("Event", m_c->event));

    if (m_doMCtruth) {
        ATH_CHECK(m_mcEvtKey.initialize());
        ATH_CHECK(nt->addItem("NpartStored", m_c->m_npartStored, 0, MAX_PART));
        ATH_CHECK(nt->addIndexedItem("partVertX", m_c->m_npartStored, m_c->m_partVertX));
        ATH_CHECK(nt->addIndexedItem("partVertY", m_c->m_npartStored, m_c->m_partVertY));
        ATH_CHECK(nt->addIndexedItem("partVertZ", m_c->m_npartStored, m_c->m_partVertZ));
        ATH_CHECK(nt->addIndexedItem("partVertT", m_c->m_npartStored, m_c->m_partVertT));
        ATH_CHECK(nt->addIndexedItem("partPdgId", m_c->m_npartStored, m_c->m_partPdgId));
        ATH_CHECK(nt->addIndexedItem("partMomX", m_c->m_npartStored, m_c->m_partMomX));
        ATH_CHECK(nt->addIndexedItem("partMomY", m_c->m_npartStored, m_c->m_partMomY));
        ATH_CHECK(nt->addIndexedItem("partMomZ", m_c->m_npartStored, m_c->m_partMomZ));
        ATH_CHECK(nt->addIndexedItem("partStatus", m_c->m_npartStored, m_c->m_partStatus));
        ATH_CHECK(nt->addIndexedItem("partBarcode", m_c->m_npartStored, m_c->m_partBarcode));
        ATH_CHECK(nt->addIndexedItem("partMass", m_c->m_npartStored, m_c->m_partMass));
    }

    if (m_doRPCSimHit) {
        ATH_CHECK(m_HitCollKey.initialize());
        ATH_CHECK(nt->addItem("rpc_Nsimhit", m_c->m_SimHit_nRpc, 0, MAX_SIMHRPC));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_station", m_c->m_SimHit_nRpc, m_c->m_SimHit_station));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_eta", m_c->m_SimHit_nRpc, m_c->m_SimHit_eta));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_phi", m_c->m_SimHit_nRpc, m_c->m_SimHit_phi));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_doublr", m_c->m_SimHit_nRpc, m_c->m_SimHit_doubletR));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_doublz", m_c->m_SimHit_nRpc, m_c->m_SimHit_doubletZ));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_doublphi", m_c->m_SimHit_nRpc, m_c->m_SimHit_doubletPhi));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_gasgap", m_c->m_SimHit_nRpc, m_c->m_SimHit_gasGap));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_measphi", m_c->m_SimHit_nRpc, m_c->m_SimHit_measuresPhi));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_time", m_c->m_SimHit_nRpc, m_c->m_SimHit_time));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_localx", m_c->m_SimHit_nRpc, m_c->m_SimHit_localx));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_localy", m_c->m_SimHit_nRpc, m_c->m_SimHit_localy));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_localz", m_c->m_SimHit_nRpc, m_c->m_SimHit_localz));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_globalx", m_c->m_SimHit_nRpc, m_c->m_SimHit_globalx));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_globaly", m_c->m_SimHit_nRpc, m_c->m_SimHit_globaly));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_globalz", m_c->m_SimHit_nRpc, m_c->m_SimHit_globalz));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_trackNumber", m_c->m_SimHit_nRpc, m_c->m_SimHit_trackNumber));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_stepLen", m_c->m_SimHit_nRpc, m_c->m_SimHit_stepLen));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_energyDep", m_c->m_SimHit_nRpc, m_c->m_SimHit_energyDep));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_kinEnergy", m_c->m_SimHit_nRpc, m_c->m_SimHit_kinEnergy));
        ATH_CHECK(nt->addIndexedItem("rpc_simhit_pdgId", m_c->m_SimHit_nRpc, m_c->m_SimHit_pdgId));
    }
    // Digits block
    if (m_doRPCDigit) {
        ATH_CHECK(m_DigiCollKey.initialize());
        ATH_CHECK(nt->addItem("rpc_NdigitColl", m_c->m_digit_nRpcColl));
        ATH_CHECK(nt->addItem("rpc_Ndigit", m_c->m_digit_nRpc, 0, MAX_DIGITRPC));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_station", m_c->m_digit_nRpc, m_c->m_digit_station));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_eta", m_c->m_digit_nRpc, m_c->m_digit_eta));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_phi", m_c->m_digit_nRpc, m_c->m_digit_phi));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_doublr", m_c->m_digit_nRpc, m_c->m_digit_doubletR));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_doublz", m_c->m_digit_nRpc, m_c->m_digit_doubletZ));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_doublphi", m_c->m_digit_nRpc, m_c->m_digit_doubletPhi));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_gasgap", m_c->m_digit_nRpc, m_c->m_digit_gasGap));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_measphi", m_c->m_digit_nRpc, m_c->m_digit_measuresPhi));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_strip", m_c->m_digit_nRpc, m_c->m_digit_strip));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_time", m_c->m_digit_nRpc, m_c->m_digit_time));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_stripx", m_c->m_digit_nRpc, m_c->m_digit_stripx));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_stripy", m_c->m_digit_nRpc, m_c->m_digit_stripy));
        ATH_CHECK(nt->addIndexedItem("rpc_digit_stripz", m_c->m_digit_nRpc, m_c->m_digit_stripz));
    }

    // RDO block
    if (m_doRPCRDO) {
        ATH_CHECK(m_RDOKey.initialize());
        ATH_CHECK(nt->addItem("rpc_Npad", m_c->m_nPads, 0, MAX_PADRPC));
        ATH_CHECK(nt->addIndexedItem("rpc_pad_id", m_c->m_nPads, m_c->m_rpc_pad_id));
        ATH_CHECK(nt->addIndexedItem("rpc_pad_bcid", m_c->m_nPads, m_c->m_rpc_pad_bcid));
        ATH_CHECK(nt->addIndexedItem("rpc_pad_lvl1id", m_c->m_nPads, m_c->m_rpc_pad_lvl1id));
        ATH_CHECK(nt->addIndexedItem("rpc_pad_sectorid", m_c->m_nPads, m_c->m_rpc_pad_sectorid));
        ATH_CHECK(nt->addIndexedItem("rpc_pad_error", m_c->m_nPads, m_c->m_rpc_pad_error));
        ATH_CHECK(nt->addIndexedItem("rpc_pad_status", m_c->m_nPads, m_c->m_rpc_pad_status));

        ATH_CHECK(nt->addItem("rpc_Ncm", m_c->m_nCMA));
        ATH_CHECK(nt->addItem("rpc_Nrdo", m_c->m_nFiredChannels, 0, MAX_RDOCHRPC));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_sector", m_c->m_nFiredChannels, m_c->m_sector));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_padId", m_c->m_nFiredChannels, m_c->m_padId));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_status", m_c->m_nFiredChannels, m_c->m_status));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_errcode", m_c->m_nFiredChannels, m_c->m_errorCode));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_cmaId", m_c->m_nFiredChannels, m_c->m_cmaId));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_fel1Id", m_c->m_nFiredChannels, m_c->m_fel1Id));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_febcId", m_c->m_nFiredChannels, m_c->m_febcId));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_crc", m_c->m_nFiredChannels, m_c->m_crc));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_bcId", m_c->m_nFiredChannels, m_c->m_bcId));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_ticks", m_c->m_nFiredChannels, m_c->m_time));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_ijk", m_c->m_nFiredChannels, m_c->m_ijk));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_cmachan", m_c->m_nFiredChannels, m_c->m_channel));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_overlap", m_c->m_nFiredChannels, m_c->m_overlap));
        ATH_CHECK(nt->addIndexedItem("rpc_rdo_threshold", m_c->m_nFiredChannels, m_c->m_threshold));
    }

    // Prep block
    if (m_doRPCPrep) {
        ATH_CHECK(m_PrepKey.initialize());
        ATH_CHECK(nt->addItem("rpc_NprdColl", m_c->m_nRpcColl));
        ATH_CHECK(nt->addItem("rpc_Nprd", m_c->m_nRpcPrd, 0, MAX_PRDRPC));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_station", m_c->m_nRpcPrd, m_c->m_prd_station));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_eta", m_c->m_nRpcPrd, m_c->m_prd_eta));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_phi", m_c->m_nRpcPrd, m_c->m_prd_phi));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_doublr", m_c->m_nRpcPrd, m_c->m_prd_doubletR));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_doublz", m_c->m_nRpcPrd, m_c->m_prd_doubletZ));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_doublphi", m_c->m_nRpcPrd, m_c->m_prd_doubletPhi));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_gasgap", m_c->m_nRpcPrd, m_c->m_prd_gasGap));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_measphi", m_c->m_nRpcPrd, m_c->m_prd_measuresPhi));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_strip", m_c->m_nRpcPrd, m_c->m_prd_strip));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_time", m_c->m_nRpcPrd, m_c->m_prd_time));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_stripx", m_c->m_nRpcPrd, m_c->m_prd_stripx));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_stripy", m_c->m_nRpcPrd, m_c->m_prd_stripy));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_stripz", m_c->m_nRpcPrd, m_c->m_prd_stripz));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_triggerInfo", m_c->m_nRpcPrd, m_c->m_prd_triggerInfo));
        ATH_CHECK(nt->addIndexedItem("rpc_prd_ambigFlag", m_c->m_nRpcPrd, m_c->m_prd_ambigFlag));
    }
    m_c->nt = nt;

    return StatusCode::SUCCESS;
}

void RPC_SimHitToPrdCBNTAlgo::clearNTuple() {
    if (m_doMCtruth) {
        m_c->m_npartStored = 0;
        m_c->run = 0;
        m_c->event = 0;
    }

    if (m_doRPCSimHit) { m_c->m_SimHit_nRpc = 0; }

    if (m_doRPCDigit) {
        m_c->m_digit_nRpcColl = 0;
        m_c->m_digit_nRpc = 0;
    }

    if (m_doRPCRDO) {
        m_c->m_nPads = 0;

        m_c->m_nCMA = 0;
        m_c->m_nFiredChannels = 0;
    }

    if (m_doRPCPrep) {
        m_c->m_nRpcColl = 0;
        m_c->m_nRpcPrd = 0;
    }
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::execute() {
    ATH_MSG_DEBUG("in execute()");

    clearNTuple();

    const EventContext& ctx = Gaudi::Hive::currentContext();
    m_c->event = ctx.eventID().event_number();
    m_c->run = ctx.eventID().run_number();

    if (m_doMCtruth) {
        if (doMCtruth(ctx).isFailure()) {
            ATH_MSG_WARNING("MC truth fill failed");
            return StatusCode::RECOVERABLE;
        }
    }

    if (m_doRPCSimHit) { ATH_CHECK(doRPCSimHit(ctx)); }
    if (m_doRPCDigit) { ATH_CHECK(doRPCDigit(ctx)); }
    if (m_doRPCRDO) { ATH_CHECK(doRPCRDO(ctx)); }
    if (m_doRPCPrep) { ATH_CHECK(doRPCPrep(ctx)); }

    ATH_CHECK(ntupleSvc()->writeRecord(m_c->nt));
    return StatusCode::SUCCESS;
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doMCtruth(const EventContext& ctx) {
    ATH_MSG_DEBUG("in doMCtruth");
    ATH_MSG_DEBUG("RpcSimHitToPrdCBNT::doMCtruth begin.");

    // Retrieve test container of all hits in input to the digitization
    SG::ReadHandle<McEventCollection> mcEvent{m_mcEvtKey, ctx};
    if (!mcEvent.isValid()) {
        ATH_MSG_FATAL(" Cannot retrieve McEventCollection");
        return StatusCode::FAILURE;
    }

    long eventCounter = 0;
    Amg::Vector3D direction(0., 0., 0.);
    int ipart = 0;
    for (const HepMC::GenEvent* e : (*mcEvent)) {
        ++eventCounter;

        ATH_MSG_DEBUG("McEvent n.  " << eventCounter);
        int particleCounter = 0;
        for (auto p : (*e)) {
            ++particleCounter;
            ATH_MSG_DEBUG("McEvent n.  " << eventCounter << " particle # " << particleCounter);
            if (ipart >= MAX_PART) {
                ATH_MSG_WARNING(__FILE__ << ":" << __LINE__ << " Number of particles exceeded. Please consider to increase MAX_PART:"
                                         << MAX_PART << " at compile time");
                break;
            }
            if (!p) {
                ATH_MSG_DEBUG("Captain nullptr ahead");
                continue;
            }

            float xv{-FLT_MAX}, yv{-FLT_MAX}, zv{-FLT_MAX}, tv{-FLT_MAX};
            // the following lines give troubles  - protect them
            if (p->production_vertex()) {
                xv = p->production_vertex()->position().x();
                yv = p->production_vertex()->position().y();
                zv = p->production_vertex()->position().z();
                tv = p->production_vertex()->position().t();
            }
            float xd = p->momentum().px();
            float yd = p->momentum().py();
            float zd = p->momentum().pz();
            float mag = std::hypot(xd, yd, zd);
            if (std::abs(mag) > 0.001) {
                direction[0] = (xd / mag);
                direction[1] = (yd / mag);
                direction[2] = (zd / mag);
            }
            int status = p->status();
            int barcode = HepMC::barcode(p);
            float generatedMass = p->generated_mass();
            int pdgId = p->pdg_id();

            m_c->m_partVertX[ipart] = xv;
            m_c->m_partVertY[ipart] = yv;
            m_c->m_partVertZ[ipart] = zv;
            m_c->m_partVertT[ipart] = tv;
            m_c->m_partPdgId[ipart] = pdgId;
            m_c->m_partMomX[ipart] = xd;
            m_c->m_partMomY[ipart] = yd;
            m_c->m_partMomZ[ipart] = zd;
            m_c->m_partStatus[ipart] = status;
            m_c->m_partBarcode[ipart] = barcode;
            m_c->m_partMass[ipart] = generatedMass;
            ++ipart;
            ATH_MSG_DEBUG("Event # " << eventCounter << " vertex at " << xv << " " << yv << " " << zv << " produced at time = " << tv
                                     << " direction theta/eta/phi = " << direction.theta() << " " << direction.phi()
                                     << " p = " << sqrt(xd * xd + yd * yd + zd * zd) << " pdgId = " << pdgId << " Status " << status);
        }
    }
    m_c->m_npartStored = ipart;
    ATH_MSG_DEBUG("out of doMCtruth");
    return StatusCode::SUCCESS;
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCSimHit(const EventContext& ctx) {
    ATH_MSG_DEBUG("in doRPCSimHit");
    ATH_MSG_DEBUG("RpcSimHitToPrdCBNT::doRPCSimHit begin.");

    // Retrieve test container of all hits in input to the digitization
    SG::ReadHandle<RPCSimHitCollection> inputSimHitColl{m_HitCollKey, ctx};
    if (!inputSimHitColl.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve " << m_HitCollKey.fullKey());
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("RPCSimHitCollection->size() : " << inputSimHitColl->size());

    int myCounter = 0;
    for (RPCSimHitCollection::const_iterator rpcsimhit = inputSimHitColl.cptr()->begin(); rpcsimhit != inputSimHitColl.cptr()->end();
         ++rpcsimhit) {
        if (myCounter >= MAX_SIMHRPC) {
            ATH_MSG_WARNING(__FILE__ << ":" << __LINE__
                                     << "Maximum allowed number of simulated hits exceeded... Please consider to increase MAX_SIMHRPC="
                                     << MAX_SIMHRPC << " at compile time");
            break;
        }
        const int idHit = rpcsimhit->RPCid();

        int StationName = -1;

        if (!m_muonHelper) m_muonHelper = RpcHitIdHelper::GetHelper(m_idHelperSvc->rpcIdHelper().gasGapMax());
        std::string stationName_str = m_muonHelper->GetStationName(idHit);
        int stationEta = m_muonHelper->GetZSector(idHit);
        int stationPhi = m_muonHelper->GetPhiSector(idHit);
        int doubletR = m_muonHelper->GetDoubletR(idHit);
        int doubletZ = m_muonHelper->GetDoubletZ(idHit);
        int doubletPhi = m_muonHelper->GetDoubletPhi(idHit);
        int gasGap = m_muonHelper->GetGasGapLayer(idHit);
        int measphi = m_muonHelper->GetMeasuresPhi(idHit);

        if (stationName_str == "BIL") {
            StationName = 0;
        } else if (stationName_str == "BIS") {
            StationName = 1;
        } else if (stationName_str == "BML") {
            StationName = 2;
        } else if (stationName_str == "BMS") {
            StationName = 3;
        } else if (stationName_str == "BOL") {
            StationName = 4;
        } else if (stationName_str == "BOS") {
            StationName = 5;
        } else if (stationName_str == "BIR") {
            StationName = 7;
        } else if (stationName_str == "BMF") {
            StationName = 8;
        } else if (stationName_str == "BOF") {
            StationName = 9;
        } else if (stationName_str == "BOG") {
            StationName = 10;
        } else if (stationName_str == "BIM") {
            StationName = 52;
        } else if (stationName_str == "BME") {
            StationName = 53;
        } else if (stationName_str == "BOE") {
            StationName = 54;
        }

        // construct Atlas identifier from components
        ATH_MSG_DEBUG("creating id for hit in element:"
                      << " stationName_str " << stationName_str << " stationEta " << stationEta << " stationPhi " << stationPhi
                      << " doubletR " << doubletR << " doubletZ " << doubletZ << " doubletPhi " << doubletPhi << " gasGap " << gasGap
                      << " measphi " << measphi);  //

        // use GeoAdaptors to retrieve global positions of a simhit
        GeoRPCHit ghit(*rpcsimhit);
        Amg::Vector3D gPos = ghit.getGlobalPosition();

        m_c->m_SimHit_time[myCounter] = rpcsimhit->globalTime();
        m_c->m_SimHit_localx[myCounter] = rpcsimhit->localPosition().x();
        m_c->m_SimHit_localy[myCounter] = rpcsimhit->localPosition().y();
        m_c->m_SimHit_localz[myCounter] = rpcsimhit->localPosition().z();
        m_c->m_SimHit_globalx[myCounter] = gPos.x();
        m_c->m_SimHit_globaly[myCounter] = gPos.y();
        m_c->m_SimHit_globalz[myCounter] = gPos.z();

        m_c->m_SimHit_station[myCounter] = StationName;
        m_c->m_SimHit_eta[myCounter] = stationEta;
        m_c->m_SimHit_phi[myCounter] = stationPhi;
        m_c->m_SimHit_doubletR[myCounter] = doubletR;
        m_c->m_SimHit_doubletZ[myCounter] = doubletZ;
        m_c->m_SimHit_doubletPhi[myCounter] = doubletPhi;
        m_c->m_SimHit_gasGap[myCounter] = gasGap;
        m_c->m_SimHit_measuresPhi[myCounter] = measphi;

        m_c->m_SimHit_trackNumber[myCounter] = rpcsimhit->trackNumber();
        m_c->m_SimHit_stepLen[myCounter] = rpcsimhit->stepLength();
        m_c->m_SimHit_energyDep[myCounter] = rpcsimhit->energyDeposit();
        m_c->m_SimHit_kinEnergy[myCounter] = rpcsimhit->kineticEnergy();
        m_c->m_SimHit_pdgId[myCounter] = rpcsimhit->particleEncoding();

        ++myCounter;
    }

    m_c->m_SimHit_nRpc = myCounter;
    ATH_MSG_DEBUG("out of doRPCSimHit");
    return StatusCode::SUCCESS;
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCDigit(const EventContext& ctx) {
    ATH_MSG_DEBUG("in doRPCDigit");
    // Retrieve Digit container in StoreGate
    SG::ReadHandle<RpcDigitContainer> digitContainer{m_DigiCollKey, ctx};
    ATH_MSG_DEBUG("RpcDigitContainer size: " << digitContainer->size());
    if (!digitContainer.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve " << m_DigiCollKey.fullKey());
        return StatusCode::FAILURE;
    }
    int myCounter = 0;
    for (RpcDigitContainer::const_iterator containerit = digitContainer.cptr()->begin(); containerit != digitContainer.cptr()->end();
         ++containerit) {
        for (RpcDigitCollection::const_iterator rpcdigit = (*containerit)->begin(); rpcdigit != (*containerit)->end(); ++rpcdigit) {
            if (myCounter >= MAX_DIGITRPC) {
                ATH_MSG_WARNING(__FILE__ << ":" << __LINE__ << " Cannot store more digits. Please consider to increase MAX_DIGITRPC:"
                                         << MAX_DIGITRPC << " at compile time");
                break;
            }
            Identifier digit_id = (*rpcdigit)->identify();

            m_c->m_digit_time[myCounter] = (*rpcdigit)->time();

            m_c->m_digit_station[myCounter] = m_idHelperSvc->rpcIdHelper().stationName(digit_id);
            m_c->m_digit_eta[myCounter] = m_idHelperSvc->rpcIdHelper().stationEta(digit_id);
            m_c->m_digit_phi[myCounter] = m_idHelperSvc->rpcIdHelper().stationPhi(digit_id);
            m_c->m_digit_doubletR[myCounter] = m_idHelperSvc->rpcIdHelper().doubletR(digit_id);
            m_c->m_digit_doubletZ[myCounter] = m_idHelperSvc->rpcIdHelper().doubletZ(digit_id);
            m_c->m_digit_doubletPhi[myCounter] = m_idHelperSvc->rpcIdHelper().doubletPhi(digit_id);
            m_c->m_digit_gasGap[myCounter] = m_idHelperSvc->rpcIdHelper().gasGap(digit_id);
            m_c->m_digit_measuresPhi[myCounter] = m_idHelperSvc->rpcIdHelper().measuresPhi(digit_id);
            m_c->m_digit_strip[myCounter] = m_idHelperSvc->rpcIdHelper().strip(digit_id);

            // get the digit position
            const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(digit_id);

            const Amg::Vector3D stripPos = descriptor->stripPos(digit_id);

            m_c->m_digit_stripx[myCounter] = stripPos.x();
            m_c->m_digit_stripy[myCounter] = stripPos.y();
            m_c->m_digit_stripz[myCounter] = stripPos.z();

            ATH_MSG_DEBUG("DIG  Station/eta/phi/dbR/dbZ/dbP/measPhi/strip: "
                          << m_c->m_digit_station[myCounter] << " " << m_c->m_digit_eta[myCounter] << " " << m_c->m_digit_phi[myCounter]
                          << " " << m_c->m_digit_doubletR[myCounter] << " " << m_c->m_digit_doubletZ[myCounter] << " "
                          << m_c->m_digit_doubletPhi[myCounter] << " " << m_c->m_digit_gasGap[myCounter] << " "
                          << m_c->m_digit_measuresPhi[myCounter] << " " << m_c->m_digit_strip[myCounter]);
            ++myCounter;
        }  // end of rpcdigit container loop
    }      // end of container iteration loop

    m_c->m_digit_nRpcColl = digitContainer.cptr()->size();
    m_c->m_digit_nRpc = myCounter;
    ATH_MSG_DEBUG("Total n. of DIG / DIG collections  " << myCounter << " " << digitContainer.cptr()->size());
    ATH_MSG_DEBUG("out of doRPCDigit");

    return StatusCode::SUCCESS;
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCRDO(const EventContext& ctx) {
    ATH_MSG_DEBUG("in doRPCRDO");
    // Retrieve RDO container in StoreGate
    SG::ReadHandle<RpcPadContainer> rpcRDO{m_RDOKey, ctx};

    ATH_MSG_DEBUG("m_rpcRDO size: " << rpcRDO->size());

    // Access by Collection

    m_c->m_nPads = 0;
    m_c->m_nCMA = 0;
    m_c->m_nFiredChannels = 0;

    RpcPadContainer::const_iterator pad;

    int myCounter{0}, myPadCounter{0}, myCMACounter{0};
    for (const RpcPad* rdoColl : *rpcRDO) {
        // Now loop on the RDO
        if (myPadCounter >= MAX_PADRPC || myCounter >= MAX_RDOCHRPC) {
            ATH_MSG_WARNING(__FILE__ << ":" << __LINE__ << " Maximum pad is exceeded. Please consider to increase MAX_PADRPC=" << MAX_PADRPC
                                     << " or MAX_RDOCHRPC=" << MAX_RDOCHRPC);
            break;
        }
        if (rdoColl->size()) {
            ATH_MSG_DEBUG(" Number of CMA in this pad " << (rdoColl)->size());

            // pad ntuple variables
            m_c->m_rpc_pad_id[myPadCounter] = (int)(rdoColl)->onlineId();
            m_c->m_rpc_pad_sectorid[myPadCounter] = (int)(rdoColl)->sector();
            m_c->m_rpc_pad_error[myPadCounter] = (int)(rdoColl)->errorCode();
            m_c->m_rpc_pad_status[myPadCounter] = (int)(rdoColl)->status();

            m_c->m_rpc_pad_lvl1id[myPadCounter] = (int)(rdoColl)->lvl1Id();
            m_c->m_rpc_pad_bcid[myPadCounter] = (int)(rdoColl)->bcId();

            // for each pad, loop over cma
            RpcPad::const_iterator it3 = (rdoColl)->begin();
            RpcPad::const_iterator it4 = (rdoColl)->end();
            for (; it3 != it4; ++it3) {
                ATH_MSG_DEBUG(" Number of fired channels in this cma " << (*it3)->size());
                const RpcCoinMatrix* cma = (*it3);

                // for each cma for over fired channels
                RpcCoinMatrix::const_iterator it5 = (*it3)->begin();
                RpcCoinMatrix::const_iterator it6 = (*it3)->end();
                for (; it5 != it6; ++it5) {
                    if (myCounter < MAX_RDOCHRPC) {
                        const RpcFiredChannel* fChannel = (*it5);

                        m_c->m_sector[myCounter] = (rdoColl)->sector();
                        m_c->m_padId[myCounter] = (rdoColl)->onlineId();
                        m_c->m_status[myCounter] = (rdoColl)->status();
                        m_c->m_errorCode[myCounter] = (rdoColl)->errorCode();
                        m_c->m_cmaId[myCounter] = cma->onlineId();
                        m_c->m_fel1Id[myCounter] = cma->fel1Id();
                        m_c->m_febcId[myCounter] = cma->febcId();
                        m_c->m_crc[myCounter] = cma->crc();

                        m_c->m_bcId[myCounter] = fChannel->bcid();
                        m_c->m_time[myCounter] = fChannel->time();
                        m_c->m_ijk[myCounter] = fChannel->ijk();
                        m_c->m_channel[myCounter] = fChannel->channel();

                        ATH_MSG_DEBUG("RDO sector/pad/cma/ijk/channel " << m_c->m_sector[myCounter] << " " << m_c->m_padId[myCounter] << " "
                                                                        << m_c->m_cmaId[myCounter] << " " << m_c->m_ijk[myCounter] << " "
                                                                        << m_c->m_channel[myCounter]);
                        if (fChannel->ijk() == 7) {
                            m_c->m_overlap[myCounter] = fChannel->ovl();
                            m_c->m_threshold[myCounter] = fChannel->thr();
                        } else {
                            m_c->m_overlap[myCounter] = -1;
                            m_c->m_threshold[myCounter] = -1;
                        }
                        ++myCounter;
                    } else {
                        ATH_MSG_WARNING("More than " << MAX_RDOCHRPC << " channels ");
                        break;
                    }
                }  // end of loop over channels in a CM
                ++myCMACounter;
            }  // end of loop pver CMAs in a pad
            ++myPadCounter;
        }  // end of if non empty collection
    }      // end of loop over collections

    m_c->m_nFiredChannels = myCounter;
    m_c->m_nCMA = myCMACounter;
    m_c->m_nPads = myPadCounter;

    ATH_MSG_DEBUG("out of doRPCRDO");

    return StatusCode::SUCCESS;
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCPrep(const EventContext& ctx) {
    ATH_MSG_DEBUG("in doRPCPrep");
    // Retrieve RDO container in StoreGate
    SG::ReadHandle<Muon::RpcPrepDataContainer> rpc_container{m_PrepKey, ctx};
    if (!rpc_container.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve " << m_PrepKey.fullKey());
        return StatusCode::FAILURE;
    }
    int myCounter{0}, myCollCounter{0};
    for (Muon::RpcPrepDataContainer::const_iterator containerit = rpc_container.cptr()->begin(); containerit != rpc_container.cptr()->end();
         ++containerit) {
        for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = (*containerit)->begin(); rpcPrd != (*containerit)->end(); ++rpcPrd) {
            if (myCounter >= MAX_PRDRPC) {
                ATH_MSG_WARNING(__FILE__ << ":" << __LINE__ << " maximum prepdata exceeded. Please consider to retune MAX_PRDRPC="
                                         << MAX_PRDRPC << " at compile time");
                break;
            }
            Identifier prd_id = (*rpcPrd)->identify();

            m_c->m_prd_time[myCounter] = (*rpcPrd)->time();
            m_c->m_prd_station[myCounter] = m_idHelperSvc->rpcIdHelper().stationName(prd_id);
            m_c->m_prd_eta[myCounter] = m_idHelperSvc->rpcIdHelper().stationEta(prd_id);
            m_c->m_prd_phi[myCounter] = m_idHelperSvc->rpcIdHelper().stationPhi(prd_id);
            m_c->m_prd_doubletR[myCounter] = m_idHelperSvc->rpcIdHelper().doubletR(prd_id);
            m_c->m_prd_doubletZ[myCounter] = m_idHelperSvc->rpcIdHelper().doubletZ(prd_id);
            m_c->m_prd_doubletPhi[myCounter] = m_idHelperSvc->rpcIdHelper().doubletPhi(prd_id);
            m_c->m_prd_gasGap[myCounter] = m_idHelperSvc->rpcIdHelper().gasGap(prd_id);
            m_c->m_prd_measuresPhi[myCounter] = m_idHelperSvc->rpcIdHelper().measuresPhi(prd_id);
            m_c->m_prd_strip[myCounter] = m_idHelperSvc->rpcIdHelper().strip(prd_id);
            m_c->m_prd_triggerInfo[myCounter] = (*rpcPrd)->triggerInfo();
            m_c->m_prd_ambigFlag[myCounter] = (*rpcPrd)->ambiguityFlag();

            ATH_MSG_DEBUG("PRD  Station/eta/phi/dbR/dbZ/dbP/measPhi/strip: "
                          << m_c->m_prd_station[myCounter] << " " << m_c->m_prd_eta[myCounter] << " " << m_c->m_prd_phi[myCounter] << " "
                          << m_c->m_prd_doubletR[myCounter] << " " << m_c->m_prd_doubletZ[myCounter] << " "
                          << m_c->m_prd_doubletPhi[myCounter] << " " << m_c->m_prd_gasGap[myCounter] << " "
                          << m_c->m_prd_measuresPhi[myCounter] << " " << m_c->m_prd_strip[myCounter]);

            // get the digit position
            const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(prd_id);

            const Amg::Vector3D stripPos = descriptor->stripPos(prd_id);

            m_c->m_prd_stripx[myCounter] = stripPos.x();
            m_c->m_prd_stripy[myCounter] = stripPos.y();
            m_c->m_prd_stripz[myCounter] = stripPos.z();

            ++myCounter;
            // ATH_MSG_DEBUG ( " PRD number  " << myCounter );
        }  // end of rpcPrd container loop
        ++myCollCounter;
    }  // end of container iteration loop
    m_c->m_nRpcPrd = myCounter;
    m_c->m_nRpcColl = myCollCounter;
    ATH_MSG_DEBUG("Total n. of PRD / PRD collections  " << myCounter << " " << myCollCounter);
    ATH_MSG_DEBUG("out of doRPCPrep");
    return StatusCode::SUCCESS;
}
