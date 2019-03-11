/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>
#include <set>
#include <utility>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPBSCollectionV2.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include "CmxCpSubBlock.h"
#include "CmxSubBlock.h"
#include "CpmSubBlockV2.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"
#include "ModifySlices.h"

#include "CpByteStreamV2Tool.h"

namespace LVL1BS
{

// Interface ID

static const InterfaceID IID_ICpByteStreamV2Tool("CpByteStreamV2Tool", 1, 1);

const InterfaceID &CpByteStreamV2Tool::interfaceID()
{
    return IID_ICpByteStreamV2Tool;
}

// Constructor

CpByteStreamV2Tool::CpByteStreamV2Tool(const std::string &type,
                                       const std::string &name,
                                       const IInterface  *parent)
    : AthAlgTool(type, name, parent),
      m_cpmMaps("LVL1::CpmMappingTool/CpmMappingTool"),
      m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
      m_robDataProvider("ROBDataProviderSvc", name),
      m_channels(80), m_crates(4), m_modules(14), m_cmxs(2), m_maxTobs(5),
      m_chips(16), m_locs(4),
      m_coreOverlap(0), m_subDetector(eformat::TDAQ_CALO_CLUSTER_PROC_DAQ),
      m_srcIdMap(0), m_towerKey(0), m_cpmSubBlock(0), m_cmxCpSubBlock(0),
      m_rodStatus(0), m_fea(0)
{
    declareInterface<CpByteStreamV2Tool>(this);

    declareProperty("CpmMappingTool", m_cpmMaps,
                    "Crate/Module/Channel to Eta/Phi/Layer mapping tool");
    declareProperty("ErrorTool", m_errorTool,
                    "Tool to collect errors for monitoring");

    declareProperty("CrateOffsetHw",  m_crateOffsetHw  = 8,
                    "Offset of CP crate numbers in bytestream");
    declareProperty("CrateOffsetSw",  m_crateOffsetSw  = 0,
                    "Offset of CP crate numbers in RDOs");

    // Properties for reading bytestream only
    declareProperty("ROBSourceIDs",       m_sourceIDs,
                    "ROB fragment source identifiers");

    // Properties for writing bytestream only
    declareProperty("DataVersion",    m_version     = 2,                //  <<== CHECK
                    "Format version number in sub-block header");
    declareProperty("DataFormat",     m_dataFormat  = 1,
                    "Format identifier (0-1) in sub-block header");
    declareProperty("SlinksPerCrate", m_slinks      = 2,
                    "The number of S-Links per crate");
    declareProperty("SimulSlices",    m_dfltSlices  = 1,
                    "The number of slices in the simulation");
    declareProperty("ForceSlices",    m_forceSlices = 0,
                    "If >0, the number of slices in bytestream");
    declareProperty("CrateMin",       m_crateMin = 0,
                    "Minimum crate number, allows partial output");
    declareProperty("CrateMax",       m_crateMax = m_crates - 1,
                    "Maximum crate number, allows partial output");

}

// Destructor

CpByteStreamV2Tool::~CpByteStreamV2Tool()
{
}

// Initialize


StatusCode CpByteStreamV2Tool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version "
                   << PACKAGE_VERSION << endmsg;

    CHECK(m_cpmMaps.retrieve());
    CHECK(m_errorTool.retrieve());
    CHECK(m_robDataProvider.retrieve());
    
    m_srcIdMap      = new L1CaloSrcIdMap();
    m_towerKey      = new LVL1::TriggerTowerKey();
    m_cpmSubBlock   = new CpmSubBlockV2();
    m_cmxCpSubBlock = new CmxCpSubBlock();
    m_rodStatus     = new std::vector<uint32_t>(2);
    m_fea           = new FullEventAssembler<L1CaloSrcIdMap>();
    
    return StatusCode::SUCCESS;
}

// Finalize

StatusCode CpByteStreamV2Tool::finalize()
{
    delete m_fea;
    delete m_rodStatus;
    delete m_cmxCpSubBlock;
    delete m_cpmSubBlock;
    delete m_towerKey;
    delete m_srcIdMap;
    return StatusCode::SUCCESS;
}

// Conversion bytestream to CPM towers
StatusCode CpByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::CPMTower> *const ttCollection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "CpByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, ttCollection);
}

StatusCode CpByteStreamV2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG &robFrags,
    DataVector<LVL1::CPMTower> *const ttCollection)
{
    CpmTowerData data (ttCollection);
    return convertBs(robFrags, data);
}

// Conversion bytestream to CMX-CP TOBs
StatusCode CpByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::CMXCPTob> *const tobCollection)
{
  const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "CpByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, tobCollection);
}

StatusCode CpByteStreamV2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG &robFrags,
    DataVector<LVL1::CMXCPTob> *const tobCollection)
{
    CmxCpTobData data (tobCollection);
    return convertBs(robFrags, data);
}

// Conversion bytestream to CMX-CP hits
StatusCode CpByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::CMXCPHits> *const hitCollection)
{
  const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "CpByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, hitCollection);
}

StatusCode CpByteStreamV2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG &robFrags,
    DataVector<LVL1::CMXCPHits> *const hitCollection)
{
    CmxCpHitsData data (hitCollection);
    return convertBs(robFrags, data);
}

// Conversion of CP container to bytestream

StatusCode CpByteStreamV2Tool::convert(const LVL1::CPBSCollectionV2 *const cp,
                                       RawEventWrite *const re)
{
    const bool debug = msgLvl(MSG::DEBUG);
    if (debug) msg(MSG::DEBUG);

    // Clear the event assembler

    m_fea->clear();
    const uint16_t minorVersion = m_srcIdMap->minorVersion();
    m_fea->setRodMinorVersion(minorVersion);
    m_rodStatusMap.clear();

    // Pointer to ROD data vector

    FullEventAssembler<L1CaloSrcIdMap>::RODDATA *theROD = 0;

    // Set up the container maps

    setupCpmTowerMap(cp->towers());
    setupCmxCpTobMap(cp->tobs());
    setupCmxCpHitsMap(cp->hits());

    // Loop over data

    const bool neutralFormat   = m_dataFormat == L1CaloSubBlock::NEUTRAL;
    const int  modulesPerSlink = m_modules / m_slinks;
    int timeslices    = 1;
    int trigCpm       = 0;
    int timeslicesNew = 1;
    int trigCpmNew    = 0;
    for (int crate = m_crateMin; crate <= m_crateMax; ++crate)
    {
        const int hwCrate = crate + m_crateOffsetHw;

        // CPM modules are numbered 1 to m_modules
        for (int module = 1; module <= m_modules; ++module)
        {
            const int mod = module - 1;

            // Pack required number of modules per slink

            if (mod % modulesPerSlink == 0)
            {
                const int daqOrRoi = 0;
                const int slink = (m_slinks == 2) ? 2 * (mod / modulesPerSlink)
                                  : mod / modulesPerSlink;
                if (debug)
                {
                    msg() << "Treating crate " << hwCrate
                          << " slink " << slink << endmsg;
                }
                // Get number of CPM slices and triggered slice offset
                // for this slink
                if ( ! slinkSlices(crate, module, modulesPerSlink,
                                   timeslices, trigCpm))
                {
                    msg(MSG::ERROR) << "Inconsistent number of slices or "
                                    << "triggered slice offsets in data for crate "
                                    << hwCrate << " slink " << slink << endmsg;
                    return StatusCode::FAILURE;
                }
                timeslicesNew = (m_forceSlices) ? m_forceSlices : timeslices;
                trigCpmNew    = ModifySlices::peak(trigCpm, timeslices, timeslicesNew);
                if (debug)
                {
                    msg() << "Data Version/Format: " << m_version
                          << " " << m_dataFormat << endmsg
                          << "Slices/offset: " << timeslices << " " << trigCpm;
                    if (timeslices != timeslicesNew)
                    {
                        msg() << " modified to " << timeslicesNew << " " << trigCpmNew;
                    }
                    msg() << endmsg;
                }
                L1CaloUserHeader userHeader;
                userHeader.setCpm(trigCpmNew);
                const uint32_t rodIdCpm = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                                          m_subDetector);
                theROD = m_fea->getRodData(rodIdCpm);
                theROD->push_back(userHeader.header());
                m_rodStatusMap.insert(make_pair(rodIdCpm, m_rodStatus));
            }
            if (debug) msg() << "Module " << module << endmsg;

            // Create a sub-block for each slice (except Neutral format)

            m_cpmBlocks.clear();
            for (int slice = 0; slice < timeslicesNew; ++slice)
            {
                CpmSubBlockV2 *const subBlock = new CpmSubBlockV2();
                subBlock->setCpmHeader(m_version, m_dataFormat, slice,
                                       hwCrate, module, timeslicesNew);
                m_cpmBlocks.push_back(subBlock);
                if (neutralFormat) break;
            }

            // Find CPM towers corresponding to each eta/phi pair and fill
            // sub-blocks

            for (int chan = 0; chan < m_channels; ++chan)
            {
                double eta = 0.;
                double phi = 0.;
                int layer = 0;
                if (m_cpmMaps->mapping(crate, module, chan, eta, phi, layer))
                {
                    const unsigned int key = m_towerKey->ttKey(phi, eta);
                    const LVL1::CPMTower *const tt = findCpmTower(key);
                    if (tt )
                    {
                        std::vector<int> emData;
                        std::vector<int> hadData;
                        std::vector<int> emError;
                        std::vector<int> hadError;
                        ModifySlices::data(tt->emEnergyVec(),  emData,   timeslicesNew);
                        ModifySlices::data(tt->hadEnergyVec(), hadData,  timeslicesNew);
                        ModifySlices::data(tt->emErrorVec(),   emError,  timeslicesNew);
                        ModifySlices::data(tt->hadErrorVec(),  hadError, timeslicesNew);
                        for (int slice = 0; slice < timeslicesNew; ++slice)
                        {
                            const LVL1::DataError emErrBits(emError[slice]);
                            const LVL1::DataError hadErrBits(hadError[slice]);
                            const int emErr  =
                                (emErrBits.get(LVL1::DataError::LinkDown) << 1) |
                                emErrBits.get(LVL1::DataError::Parity);
                            const int hadErr =
                                (hadErrBits.get(LVL1::DataError::LinkDown) << 1) |
                                hadErrBits.get(LVL1::DataError::Parity);
                            const int index  = ( neutralFormat ) ? 0 : slice;
                            CpmSubBlockV2 *const subBlock = m_cpmBlocks[index];
                            subBlock->fillTowerData(slice, chan, emData[slice],
                                                    hadData[slice], emErr, hadErr);
                            if ((emErrBits.error() >> LVL1::DataError::GLinkParity))
                            {
                                int gLinkParity   = emErrBits.get(LVL1::DataError::GLinkParity);
                                int gLinkProtocol = emErrBits.get(LVL1::DataError::GLinkProtocol);
                                int bCNMismatch   = emErrBits.get(LVL1::DataError::BCNMismatch);
                                int fIFOOverflow  = emErrBits.get(LVL1::DataError::FIFOOverflow);
                                int moduleError   = emErrBits.get(LVL1::DataError::ModuleError);
                                int gLinkDown     = emErrBits.get(LVL1::DataError::GLinkDown);
                                int gLinkTimeout  = emErrBits.get(LVL1::DataError::GLinkTimeout);
                                uint32_t failingBCN = emErrBits.get(LVL1::DataError::FailingBCN);
                                subBlock->setStatus(failingBCN, gLinkTimeout, gLinkDown,
                                                    moduleError, fIFOOverflow, bCNMismatch,
                                                    gLinkProtocol, gLinkParity);
                            }
                        }
                    }
                }
            }

            // Pack and write the sub-blocks

            DataVector<CpmSubBlockV2>::iterator pos;
            for (pos = m_cpmBlocks.begin(); pos != m_cpmBlocks.end(); ++pos)
            {
                CpmSubBlockV2 *const subBlock = *pos;
                if ( !subBlock->pack())
                {
                    msg(MSG::ERROR) << "CPM sub-block packing failed" << endmsg;
                    return StatusCode::FAILURE;
                }
                if (debug)
                {
                    msg() << "CPM sub-block data words: "
                          << subBlock->dataWords() << endmsg;
                }
                subBlock->write(theROD);
            }
        }

        // Append CMXs to last S-Link of the crate

        for (int cmx = 0; cmx < m_cmxs; ++cmx)
        {

            // Create a sub-block for each slice (except Neutral format)

            m_cmxBlocks.clear();
            const int summing = (crate == m_crates - 1) ? CmxSubBlock::SYSTEM
                                : CmxSubBlock::CRATE;
            for (int slice = 0; slice < timeslicesNew; ++slice)
            {
                CmxCpSubBlock *const block = new CmxCpSubBlock();
                block->setCmxHeader(m_version, m_dataFormat, slice, hwCrate,
                                    summing, CmxSubBlock::CMX_CP, cmx, timeslicesNew);
                m_cmxBlocks.push_back(block);
                if (neutralFormat) break;
            }

            // CMX-CP Tobs

            for (int cpm = 1; cpm <= m_modules; ++cpm)
            {
                for (int chip = 0; chip < m_chips; ++chip)
                {
                    for (int loc = 0; loc < m_locs; ++loc)
                    {
                        const int key = tobKey(crate, cmx, cpm, chip, loc);
                        const LVL1::CMXCPTob *const ct = findCmxCpTob(key);
                        if ( ct )
                        {
                            std::vector<int> energy;
                            std::vector<int> isolation;
                            std::vector<int> error;
                            std::vector<unsigned int> presence;
                            ModifySlices::data(ct->energyVec(),      energy,    timeslicesNew);
                            ModifySlices::data(ct->isolationVec(),   isolation, timeslicesNew);
                            ModifySlices::data(ct->errorVec(),       error,     timeslicesNew);
                            ModifySlices::data(ct->presenceMapVec(), presence,  timeslicesNew);
                            for (int slice = 0; slice < timeslicesNew; ++slice)
                            {
                                const LVL1::DataError errBits(error[slice]);
                                int err = errBits.get(LVL1::DataError::ParityMerge);
                                err |= (errBits.get(LVL1::DataError::ParityPhase0)) << 1;
                                err |= (errBits.get(LVL1::DataError::ParityPhase1)) << 2;
                                err |= (errBits.get(LVL1::DataError::ParityPhase2)) << 3;
                                err |= (errBits.get(LVL1::DataError::ParityPhase3)) << 4;
                                err |= (errBits.get(LVL1::DataError::Overflow)) << 5;
                                const int index = ( neutralFormat ) ? 0 : slice;
                                CmxCpSubBlock *const subBlock = m_cmxBlocks[index];
                                subBlock->setTob(slice, cpm, chip, loc, energy[slice],
                                                 isolation[slice], err);
                                subBlock->setPresenceMap(slice, cpm, presence[slice]);
                            }
                        }
                    }
                }
            }

            // CMX-CP Hits

            for (int source = 0; source < LVL1::CMXCPHits::MAXSOURCE; ++source)
            {
                const int key = hitsKey(crate, cmx, source);
                const LVL1::CMXCPHits *const ch = findCmxCpHits(key);
                if ( ch )
                {
                    std::vector<unsigned int> hits0;
                    std::vector<unsigned int> hits1;
                    std::vector<int> err0;
                    std::vector<int> err1;
                    ModifySlices::data(ch->hitsVec0(),  hits0, timeslicesNew);
                    ModifySlices::data(ch->hitsVec1(),  hits1, timeslicesNew);
                    ModifySlices::data(ch->errorVec0(), err0,  timeslicesNew);
                    ModifySlices::data(ch->errorVec1(), err1,  timeslicesNew);
                    for (int slice = 0; slice < timeslicesNew; ++slice)
                    {
                        const LVL1::DataError err0Bits(err0[slice]);
                        const LVL1::DataError err1Bits(err1[slice]);
                        const int index = ( neutralFormat ) ? 0 : slice;
                        CmxCpSubBlock *const subBlock = m_cmxBlocks[index];
                        subBlock->setHits(slice, source, 0, hits0[slice],                // Assuming CMXCPHits::source == CmxCpSubBlock::source
                                          err0Bits.get(LVL1::DataError::Parity));
                        subBlock->setHits(slice, source, 1, hits1[slice],
                                          err1Bits.get(LVL1::DataError::Parity));
                        if (neutralFormat)   // Neutral format wants RoI overflow bit
                        {
                            subBlock->setRoiOverflow(slice, source,
                                                     err0Bits.get(LVL1::DataError::Overflow));
                        }
                    }
                }
            }
            for (CmxCpSubBlock* subBlock : m_cmxBlocks)
            {
                if ( !subBlock->pack())
                {
                    msg(MSG::ERROR) << "CMX-Cp sub-block packing failed" << endmsg;
                    return StatusCode::FAILURE;
                }
                if (debug)
                {
                    msg() << "CMX-Cp sub-block data words: "
                          << subBlock->dataWords() << endmsg;
                }
                subBlock->write(theROD);
            }
        }
    }

    // Fill the raw event

    m_fea->fill(re, msg());

    // Set ROD status words

    //L1CaloRodStatus::setStatus(re, m_rodStatusMap, m_srcIdMap);

    return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t> &CpByteStreamV2Tool::sourceIDs(
    const std::string &sgKey)
{
    // Check if overlap tower channels wanted
    const std::string flag("Overlap");
    const std::string::size_type pos = sgKey.find(flag);
    m_coreOverlap =
        (pos == std::string::npos || pos != sgKey.length() - flag.length()) ? 0 : 1;

    if (m_sourceIDs.empty())
    {
        const int maxCrates = m_crates + m_crateOffsetHw;
        const int maxSlinks = m_srcIdMap->maxSlinks();
        for (int hwCrate = m_crateOffsetHw; hwCrate < maxCrates; ++hwCrate)
        {
            for (int slink = 0; slink < maxSlinks; ++slink)
            {
                const int daqOrRoi = 0;
                const uint32_t rodId = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                                       m_subDetector);
                const uint32_t robId = m_srcIdMap->getRobID(rodId);
                m_sourceIDs.push_back(robId);
            }
        }
    }
    return m_sourceIDs;
}

// Convert bytestream to given container type

StatusCode CpByteStreamV2Tool::convertBs(
    const IROBDataProviderSvc::VROBFRAG &robFrags,
    CpByteStreamToolData& data)
{
    const bool debug = msgLvl(MSG::DEBUG);
    if (debug) msg(MSG::DEBUG);

    // Loop over ROB fragments

    int robCount = 0;
    std::set<uint32_t> dupCheck;
    ROBIterator rob    = robFrags.begin();
    ROBIterator robEnd = robFrags.end();
    for (; rob != robEnd; ++rob)
    {
        ++robCount;
        ATH_MSG_DEBUG("Treating ROB fragment " << robCount << " source_id = " << std::hex << (*rob)->rob_source_id() << std::dec);

        // Skip fragments with ROB status errors

        const uint32_t robid = (*rob)->source_id();
        if ((*rob)->nstatus() > 0)
        {
            ROBPointer robData;
            (*rob)->status(robData);
            if (*robData != 0)
            {
                m_errorTool->robError(robid, *robData);
                if (debug) msg() << "ROB status error - skipping fragment" << endmsg;
                continue;
            }
        }

        // Skip duplicate fragments

        if (!dupCheck.insert(robid).second)
        {
            m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_DUPLICATE_ROB);
            if (debug) msg() << "Skipping duplicate ROB fragment" << endmsg;
            continue;
        }

        // Unpack ROD data (slinks)

        RODPointer payloadBeg;
        RODPointer payload;
        RODPointer payloadEnd;
        (*rob)->rod_data(payloadBeg);
        payloadEnd = payloadBeg + (*rob)->rod_ndata();
        payload = payloadBeg;
        if (payload == payloadEnd)
        {
            if (debug) msg() << "ROB fragment empty" << endmsg;
            continue;
        }

        // Check identifier
        const uint32_t sourceID = (*rob)->rod_source_id();
        if (m_srcIdMap->getRobID(sourceID) != robid           ||
                m_srcIdMap->subDet(sourceID)   != m_subDetector   ||
                m_srcIdMap->daqOrRoi(sourceID) != 0               ||
                (m_srcIdMap->slink(sourceID) != 0 && m_srcIdMap->slink(sourceID) != 2) ||
                m_srcIdMap->crate(sourceID)    <  m_crateOffsetHw ||
                m_srcIdMap->crate(sourceID)    >= m_crateOffsetHw + m_crates)
        {
            m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_ID);
            if (debug)
            {
                msg() << "Wrong source identifier in data: ROD "
                      << MSG::hex << sourceID << "  ROB " << robid
                      << MSG::dec << endmsg;
            }
            continue;
        }

        // Check minor version
        const int minorVersion = (*rob)->rod_version() & 0xffff;

        if (minorVersion <= m_srcIdMap->minorVersionPreLS1())
        {
            if (debug) msg() << "Skipping pre-LS1 data" << endmsg;
            continue;
        }
        const int rodCrate = m_srcIdMap->crate(sourceID);
        if (debug)
        {
            msg() << "Treating crate " << rodCrate
                  << " slink " << m_srcIdMap->slink(sourceID) << endmsg;
        }

        // First word should be User Header
        if ( !L1CaloUserHeader::isValid(*payload) )
        {
            m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_USER_HEADER);
            if (debug) msg() << "Invalid or missing user header" << endmsg;
            continue;
        }
        L1CaloUserHeader userHeader(*payload);
        userHeader.setVersion(minorVersion);
        const int headerWords = userHeader.words();
        if (headerWords != 1)
        {
            m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_USER_HEADER);
            if (debug) msg() << "Unexpected number of user header words: "
                                 << headerWords << endmsg;
            continue;
        }
        for (int i = 0; i < headerWords; ++i) ++payload;
        // triggered slice offset
        const int trigCpm = userHeader.cpm();
        if (debug)
        {
            msg() << "Minor format version number: " << MSG::hex
                  << minorVersion << MSG::dec << endmsg
                  << "Triggered slice offset: "  << trigCpm << endmsg;
        }

        // Loop over sub-blocks

        m_rodErr = L1CaloSubBlock::ERROR_NONE;
        while (payload != payloadEnd)
        {
            if (L1CaloSubBlock::wordType(*payload) != L1CaloSubBlock::HEADER)
            {
                if (debug) msg() << "Unexpected data sequence" << endmsg;
                m_rodErr = L1CaloSubBlock::ERROR_MISSING_HEADER;
                break;
            }

            // TODO: (sasha) Comment this check since firmware does not ready
            // Select right tool by ROD version
            // if (L1CaloSubBlock::version(*payload) == 1) {
            //   if (debug) msg() << "Skipping pre-LS1 data" << endmsg;
            //   break;
            // }

            if (CmxSubBlock::cmxBlock(*payload))
            {
                // CMX
                if (CmxSubBlock::cmxType(*payload) == CmxSubBlock::CMX_CP)
                {
                    m_cmxCpSubBlock->clear();
                    payload = m_cmxCpSubBlock->read(payload, payloadEnd);
                    if (m_cmxCpSubBlock->crate() != rodCrate)
                    {
                        if (debug) msg() << "Inconsistent crate number in ROD source ID"
                                             << endmsg;
                        m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
                        break;
                    }

                    if (data.m_collection == CMX_CP_TOBS || data.m_collection == CMX_CP_HITS)
                    {
                        decodeCmxCp(m_cmxCpSubBlock, trigCpm, data);
                        if (m_rodErr != L1CaloSubBlock::ERROR_NONE)
                        {
                            if (debug) msg() << "decodeCmxCp failed" << endmsg;
                            break;
                        }
                    }
                }
                else
                {
                    if (debug) msg() << "Invalid CMX type in module field" << endmsg;
                    m_rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
                    break;
                }
            }
            else
            {
                // CPM
                m_cpmSubBlock->clear();
                payload = m_cpmSubBlock->read(payload, payloadEnd);
                if (m_cpmSubBlock->crate() != rodCrate)
                {
                    if (debug) msg() << "Inconsistent crate number in ROD source ID"
                                         << endmsg;
                    m_rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
                    break;
                }
                if (data.m_collection == CPM_TOWERS)
                {
                    decodeCpm(m_cpmSubBlock, trigCpm, static_cast<CpmTowerData&>(data));
                    if (m_rodErr != L1CaloSubBlock::ERROR_NONE)
                    {
                        if (debug) msg() << "decodeCpm failed" << endmsg;
                        break;
                    }
                }
            }
        }
        if (m_rodErr != L1CaloSubBlock::ERROR_NONE)
        {
            m_errorTool->rodError(robid, m_rodErr);
        }
    }
    return StatusCode::SUCCESS;
}

// Unpack CMX-CP sub-block

void CpByteStreamV2Tool::decodeCmxCp(CmxCpSubBlock *subBlock, int trigCpm,
                                     CpByteStreamToolData& data)
{
    const bool debug = msgLvl(MSG::DEBUG);
    if (debug) msg(MSG::DEBUG);

    const int hwCrate    = subBlock->crate();
    const int cmx        = subBlock->cmxPosition();
    const int firmware   = subBlock->cmxFirmware();
    const int summing    = subBlock->cmxSumming();
    const int timeslices = subBlock->timeslices();
    const int sliceNum   = subBlock->slice();
    if (debug)
    {
        msg() << "CMX-CP: Crate "  << hwCrate
              << "  Position "     << cmx
              << "  Firmware "     << firmware
              << "  Summing "      << summing
              << "  Total slices " << timeslices
              << "  Slice "        << sliceNum
              << endmsg;
    }
    if (timeslices <= trigCpm)
    {
        if (debug) msg() << "Triggered slice from header "
                             << "inconsistent with number of slices: "
                             << trigCpm << ", " << timeslices << endmsg;
        m_rodErr = L1CaloSubBlock::ERROR_SLICES;
        return;
    }
    if (timeslices <= sliceNum)
    {
        if (debug) msg() << "Total slices inconsistent with slice number: "
                             << timeslices << ", " << sliceNum << endmsg;
        m_rodErr = L1CaloSubBlock::ERROR_SLICES;
        return;
    }
    // Unpack sub-block
    if (subBlock->dataWords() && !subBlock->unpack())
    {
        if (debug)
        {
            std::string errMsg(subBlock->unpackErrorMsg());
            msg() << "CMX-CP sub-block unpacking failed: " << errMsg << endmsg;
        }
        m_rodErr = subBlock->unpackErrorCode();
        return;
    }

    // Retrieve required data

    const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
    LVL1::DataError dErr;
    dErr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
    const int subStatus = dErr.error();
    const int crate    = hwCrate - m_crateOffsetHw;
    const int swCrate  = crate   + m_crateOffsetSw;
    const int maxSid   = CmxCpSubBlock::MAX_SOURCE_ID;
    const int sliceBeg = ( neutralFormat ) ? 0          : sliceNum;
    const int sliceEnd = ( neutralFormat ) ? timeslices : sliceNum + 1;
    for (int slice = sliceBeg; slice < sliceEnd; ++slice)
    {

        if (data.m_collection == CMX_CP_TOBS)
        {
            CmxCpTobData& tdata = static_cast<CmxCpTobData&> (data);

            // TOBs

            for (int cpm = 1; cpm <= m_modules; ++cpm)
            {
                const unsigned int presenceMap = subBlock->presenceMap(slice, cpm);
                for (int tob = 0; tob < m_maxTobs; ++tob)
                {
                    const int energy = subBlock->energy(slice, cpm, tob);
                    const int isolation = subBlock->isolation(slice, cpm, tob);
                    int error = subBlock->tobError(slice, cpm, tob);
                    if (energy == 0 && isolation == 0 && error == 0) break;
                    const int loc = subBlock->localCoord(slice, cpm, tob);
                    const int chip = subBlock->chip(slice, cpm, tob);
                    LVL1::DataError errBits(subStatus);
                    if (error)
                    {
                        errBits.set(LVL1::DataError::ParityPhase0, error);
                        errBits.set(LVL1::DataError::ParityPhase1, (error >> 1));
                        errBits.set(LVL1::DataError::ParityPhase2, (error >> 2));
                        errBits.set(LVL1::DataError::ParityPhase3, (error >> 3));
                        errBits.set(LVL1::DataError::Overflow,     (error >> 4));
                        errBits.set(LVL1::DataError::ParityMerge, (error >> 5));
                        errBits.set(LVL1::DataError::Parity, (error & 0x2f) ? 1 : 0);
                    }
                    error = errBits.error();
                    const int key = tobKey(crate, cmx, cpm, chip, loc);
                    LVL1::CMXCPTob *tb = findCmxCpTob(tdata, key);
                    if ( ! tb )   // create new CMX TOB
                    {
                        m_energyVec.assign(timeslices, 0);
                        m_isolVec.assign(timeslices, 0);
                        m_errorVec.assign(timeslices, 0);
                        m_presenceMapVec.assign(timeslices, 0);
                        m_energyVec[slice] = energy;
                        m_isolVec[slice]   = isolation;
                        m_errorVec[slice]  = error;
                        m_presenceMapVec[slice] = presenceMap;
                        auto tbp =
                          std::make_unique<LVL1::CMXCPTob>(swCrate, cmx, cpm, chip, loc,
                                                           m_energyVec, m_isolVec, m_errorVec,
                                                           m_presenceMapVec, trigCpm);
                        tdata.m_tobMap.insert(std::make_pair(key, tbp.get()));
                        tdata.m_tobCollection->push_back(std::move(tbp));
                    }
                    else
                    {
                        m_energyVec = tb->energyVec();
                        m_isolVec   = tb->isolationVec();
                        m_errorVec  = tb->errorVec();
                        m_presenceMapVec = tb->presenceMapVec();
                        const int nsl = m_energyVec.size();
                        if (timeslices != nsl)
                        {
                            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                                                 << endmsg;
                            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
                            return;
                        }
                        if (m_energyVec[slice] != 0 || m_isolVec[slice] != 0 ||
                                m_errorVec[slice]  != 0)
                        {
                            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
                            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
                            return;
                        }
                        m_energyVec[slice] = energy;
                        m_isolVec[slice]   = isolation;
                        m_errorVec[slice]  = error;
                        m_presenceMapVec[slice] = presenceMap;
                        tb->addTob(m_energyVec, m_isolVec, m_errorVec, m_presenceMapVec);
                    }
                }
            }

        }
        else if (data.m_collection == CMX_CP_HITS)
        {
            CmxCpHitsData& hdata = static_cast<CmxCpHitsData&> (data);

            // Hit/Topo counts

            for (int source = 0; source < maxSid; ++source)
            {
                if (summing == CmxSubBlock::CRATE &&
                        (source == CmxCpSubBlock::REMOTE_0 ||
                         source == CmxCpSubBlock::REMOTE_1 ||
                         source == CmxCpSubBlock::REMOTE_2 ||
                         source == CmxCpSubBlock::TOTAL)) continue;
                const unsigned int hits0 = subBlock->hits(slice, source, 0); //low
                const unsigned int hits1 = subBlock->hits(slice, source, 1); //high
                int err0 = subBlock->hitsError(slice, source, 0);
                int err1 = subBlock->hitsError(slice, source, 1);
                int overflow = subBlock->roiOverflow(slice, source);
                LVL1::DataError err0Bits(subStatus);
                err0Bits.set(LVL1::DataError::Parity, err0);
                err0Bits.set(LVL1::DataError::Overflow, overflow);
                err0 = err0Bits.error();
                LVL1::DataError err1Bits(subStatus);
                err1Bits.set(LVL1::DataError::Parity, err1);
                err1Bits.set(LVL1::DataError::Overflow, overflow);
                err1 = err1Bits.error();
                if (hits0 || hits1 || err0 || err1)
                {
                    const int key = hitsKey(crate, cmx, source);
                    LVL1::CMXCPHits *ch = findCmxCpHits(hdata, key);
                    if ( ! ch )     // create new CMX hits
                    {
                        m_hitsVec0.assign(timeslices, 0);
                        m_hitsVec1.assign(timeslices, 0);
                        m_errVec0.assign(timeslices, 0);
                        m_errVec1.assign(timeslices, 0);
                        m_hitsVec0[slice] = hits0;
                        m_hitsVec1[slice] = hits1;
                        m_errVec0[slice]  = err0;
                        m_errVec1[slice]  = err1;
                        auto chp =
                          std::make_unique<LVL1::CMXCPHits>(swCrate, cmx, source,
                                                            m_hitsVec0, m_hitsVec1,
                                                            m_errVec0, m_errVec1, trigCpm);
                        hdata.m_hitsMap.insert(std::make_pair(key, chp.get()));
                        hdata.m_hitCollection->push_back(std::move(chp));
                    }
                    else
                    {
                        m_hitsVec0 = ch->hitsVec0();
                        m_hitsVec1 = ch->hitsVec1();
                        m_errVec0  = ch->errorVec0();
                        m_errVec1  = ch->errorVec1();
                        const int nsl = m_hitsVec0.size();
                        if (timeslices != nsl)
                        {
                            if (debug) msg() << "Inconsistent number of slices in sub-blocks"
                                                 << endmsg;
                            m_rodErr = L1CaloSubBlock::ERROR_SLICES;
                            return;
                        }
                        if (m_hitsVec0[slice] != 0 || m_hitsVec1[slice] != 0 ||
                                m_errVec0[slice]  != 0 || m_errVec1[slice]  != 0)
                        {
                            if (debug) msg() << "Duplicate data for slice " << slice << endmsg;
                            m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
                            return;
                        }
                        m_hitsVec0[slice] = hits0;
                        m_hitsVec1[slice] = hits1;
                        m_errVec0[slice]  = err0;
                        m_errVec1[slice]  = err1;
                        ch->addHits(m_hitsVec0, m_hitsVec1, m_errVec0, m_errVec1);
                    }
                }
            }
        }
    }

    return;
}

// Unpack CPM sub-block

void CpByteStreamV2Tool::decodeCpm(CpmSubBlockV2 *subBlock, int trigCpm,
                                   CpmTowerData& data)
{
    const bool debug   = msgLvl(MSG::DEBUG);
    const bool verbose = msgLvl(MSG::VERBOSE);
    if (debug) msg(MSG::DEBUG);

    const int hwCrate    = subBlock->crate();
    const int module     = subBlock->module();
    const int timeslices = subBlock->timeslices();
    const int sliceNum   = subBlock->slice();
    if (debug)
    {
        msg() << "CPM: Crate "     << hwCrate
              << "  Module "       << module
              << "  Total slices " << timeslices
              << "  Slice "        << sliceNum    << endmsg;
    }
    if (module < 1 || module > m_modules)
    {
        if (debug) msg() << "Unexpected module number: " << module << endmsg;
        m_rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
        return;
    }
    if (timeslices <= trigCpm)
    {
        if (debug) msg() << "Triggered slice from header "
                             << "inconsistent with number of slices: "
                             << trigCpm << ", " << timeslices << endmsg;
        m_rodErr = L1CaloSubBlock::ERROR_SLICES;
        return;
    }
    if (timeslices <= sliceNum)
    {
        if (debug) msg() << "Total slices inconsistent with slice number: "
                             << timeslices << ", " << sliceNum << endmsg;
        m_rodErr = L1CaloSubBlock::ERROR_SLICES;
        return;
    }
    // Unpack sub-block
    if (subBlock->dataWords() && !subBlock->unpack())
    {
        if (debug)
        {
            std::string errMsg(subBlock->unpackErrorMsg());
            msg() << "CPM sub-block unpacking failed: " << errMsg << endmsg;
        }
        m_rodErr = subBlock->unpackErrorCode();
        return;
    }

    // Retrieve required data
    const bool neutralFormat = subBlock->format() == L1CaloSubBlock::NEUTRAL;
    LVL1::DataError dErr;
    dErr.set(LVL1::DataError::SubStatusWord, subBlock->subStatus());
    const int subStatus = dErr.error();
    const int crate     = hwCrate - m_crateOffsetHw;
    const int sliceBeg  = ( neutralFormat ) ? 0          : sliceNum;
    const int sliceEnd  = ( neutralFormat ) ? timeslices : sliceNum + 1;
    for (int slice = sliceBeg; slice < sliceEnd; ++slice)
    {

        // Loop over tower channels and fill CPM towers

        for (int chan = 0; chan < m_channels; ++chan)
        {
            if (!subStatus && !subBlock->anyTowerData(chan)) continue;
            const int em     = subBlock->emData(slice, chan);
            const int had    = subBlock->hadData(slice, chan);
            const int emErr  = subBlock->emError(slice, chan);
            const int hadErr = subBlock->hadError(slice, chan);
            int emErr1 = subStatus;
            if (emErr)
            {
                LVL1::DataError emErrBits(emErr1);
                emErrBits.set(LVL1::DataError::Parity, emErr & 0x1);
                emErrBits.set(LVL1::DataError::LinkDown, (emErr >> 1) & 0x1);
                emErr1 = emErrBits.error();
            }
            int hadErr1 = subStatus;
            if (hadErr)
            {
                LVL1::DataError hadErrBits(hadErr1);
                hadErrBits.set(LVL1::DataError::Parity, hadErr & 0x1);
                hadErrBits.set(LVL1::DataError::LinkDown, (hadErr >> 1) & 0x1);
                hadErr1 = hadErrBits.error();
            }
            if (em || had || emErr1 || hadErr1)
            {
                double eta = 0.;
                double phi = 0.;
                int layer = 0;
                if (m_cpmMaps->mapping(crate, module, chan, eta, phi, layer))
                {
                    if (layer == m_coreOverlap)
                    {
                        const unsigned int key = m_towerKey->ttKey(phi, eta);
                        LVL1::CPMTower *tt = findCpmTower(data, key);
                        if ( ! tt )     // create new CPM tower
                        {
                            m_emVec.assign(timeslices, 0);
                            m_hadVec.assign(timeslices, 0);
                            m_emErrVec.assign(timeslices, 0);
                            m_hadErrVec.assign(timeslices, 0);
                            m_emVec[slice]     = em;
                            m_hadVec[slice]    = had;
                            m_emErrVec[slice]  = emErr1;
                            m_hadErrVec[slice] = hadErr1;
                            auto ttp =
                              std::make_unique<LVL1::CPMTower>(phi, eta, m_emVec, m_emErrVec,
                                                               m_hadVec, m_hadErrVec, trigCpm);
                            data.m_ttMap.insert(std::make_pair(key, ttp.get()));
                            data.m_ttCollection->push_back(std::move(ttp));
                        }
                        else
                        {
                            m_emVec     = tt->emEnergyVec();
                            m_hadVec    = tt->hadEnergyVec();
                            m_emErrVec  = tt->emErrorVec();
                            m_hadErrVec = tt->hadErrorVec();
                            const int nsl = m_emVec.size();
                            if (timeslices != nsl)
                            {
                                if (debug)
                                {
                                    msg() << "Inconsistent number of slices in sub-blocks"
                                          << endmsg;
                                }
                                m_rodErr = L1CaloSubBlock::ERROR_SLICES;
                                return;
                            }
                            if (m_emVec[slice]    != 0 || m_hadVec[slice]    != 0 ||
                                    m_emErrVec[slice] != 0 || m_hadErrVec[slice] != 0)
                            {
                                if (debug) msg() << "Duplicate data for slice "
                                                     << slice << endmsg;
                                m_rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
                                return;
                            }
                            m_emVec[slice]     = em;
                            m_hadVec[slice]    = had;
                            m_emErrVec[slice]  = emErr1;
                            m_hadErrVec[slice] = hadErr1;
                            tt->fill(m_emVec, m_emErrVec, m_hadVec, m_hadErrVec, trigCpm);
                        }
                    }
                }
                else if (verbose && (em || had || emErr || hadErr))
                {
                    msg(MSG::VERBOSE) << "Non-zero data but no channel mapping for channel "
                                      << chan << endmsg;
                    msg(MSG::DEBUG);
                }
            }
            else if (verbose)
            {
                msg(MSG::VERBOSE) << "No CPM tower data for channel "
                                  << chan << " slice " << slice << endmsg;
                msg(MSG::DEBUG);
            }
        }
    }
    return;
}

// Find a CPM tower for given key

const
LVL1::CPMTower *CpByteStreamV2Tool::findCpmTower(const unsigned int key) const
{
    ConstCpmTowerMap::const_iterator mapIter = m_ttMap.find(key);
    if (mapIter != m_ttMap.end()) return mapIter->second;
    return nullptr;
}

LVL1::CPMTower *CpByteStreamV2Tool::findCpmTower(const CpmTowerData& data,
                                                 const unsigned int key) const
{
    CpmTowerMap::const_iterator mapIter = data.m_ttMap.find(key);
    if (mapIter != data.m_ttMap.end()) return mapIter->second;
    return nullptr;
}

// Find CMX-CP TOB for given key

const
LVL1::CMXCPTob *CpByteStreamV2Tool::findCmxCpTob(const int key) const
{
    ConstCmxCpTobMap::const_iterator mapIter = m_tobMap.find(key);
    if (mapIter != m_tobMap.end()) return mapIter->second;
    return nullptr;
}

LVL1::CMXCPTob *CpByteStreamV2Tool::findCmxCpTob(const CmxCpTobData& data,
                                                 const int key) const
{
    CmxCpTobMap::const_iterator mapIter = data.m_tobMap.find(key);
    if (mapIter != data.m_tobMap.end()) return mapIter->second;
    return nullptr;
}

// Find CMX-CP hits for given key

const
LVL1::CMXCPHits *CpByteStreamV2Tool::findCmxCpHits(const int key) const
{
    ConstCmxCpHitsMap::const_iterator mapIter = m_hitsMap.find(key);
    if (mapIter != m_hitsMap.end()) return mapIter->second;
    return nullptr;
}

LVL1::CMXCPHits *CpByteStreamV2Tool::findCmxCpHits(const CmxCpHitsData& data,
                                                   const int key) const
{
    CmxCpHitsMap::const_iterator mapIter = data.m_hitsMap.find(key);
    if (mapIter != data.m_hitsMap.end()) return mapIter->second;
    return nullptr;
}

// Set up CPM tower map

void CpByteStreamV2Tool::setupCpmTowerMap(const CpmTowerCollection *
        const ttCollection)
{
    m_ttMap.clear();
    if (ttCollection)
    {
        CpmTowerCollection::const_iterator pos  = ttCollection->begin();
        CpmTowerCollection::const_iterator pose = ttCollection->end();
        for (; pos != pose; ++pos)
        {
            const LVL1::CPMTower *const tt = *pos;
            const unsigned int key = m_towerKey->ttKey(tt->phi(), tt->eta());
            m_ttMap.insert(std::make_pair(key, tt));
        }
    }
}

// Set up CMX-CP TOB map

void CpByteStreamV2Tool::setupCmxCpTobMap(const CmxCpTobCollection *
        const tobCollection)
{
    m_tobMap.clear();
    if (tobCollection)
    {
        CmxCpTobCollection::const_iterator pos  = tobCollection->begin();
        CmxCpTobCollection::const_iterator pose = tobCollection->end();
        for (; pos != pose; ++pos)
        {
            const LVL1::CMXCPTob *const tob = *pos;
            const int crate = tob->crate() - m_crateOffsetSw;
            const int cmx = tob->cmx();
            const int cpm = tob->cpm();
            const int chip = tob->chip();
            const int loc = tob->location();
            const int key = tobKey(crate, cmx, cpm, chip, loc);
            m_tobMap.insert(std::make_pair(key, tob));
        }
    }
}

// Set up CMX-CP hits map

void CpByteStreamV2Tool::setupCmxCpHitsMap(const CmxCpHitsCollection *
        const hitCollection)
{
    m_hitsMap.clear();
    if (hitCollection)
    {
        CmxCpHitsCollection::const_iterator pos  = hitCollection->begin();
        CmxCpHitsCollection::const_iterator pose = hitCollection->end();
        for (; pos != pose; ++pos)
        {
            const LVL1::CMXCPHits *const hits = *pos;
            const int crate = hits->crate() - m_crateOffsetSw;
            const int cmx = hits->cmx();
            const int source = hits->source();
            const int key = hitsKey(crate, cmx, source);
            m_hitsMap.insert(std::make_pair(key, hits));
        }
    }
}

// Key for TOBs

int CpByteStreamV2Tool::tobKey(const int crate, const int cmx, const int cpm,
                               const int chip, const int loc) const
{
    return (((((((crate << 1) | cmx) << 4) | cpm) << 4) | chip) << 2) | loc;
}

// Key for Hits

int CpByteStreamV2Tool::hitsKey(const int crate, const int cmx,
                                const int source) const
{
    return (((crate << 1) | cmx) << 3) | source;
}

// Get number of slices and triggered slice offset for next slink

bool CpByteStreamV2Tool::slinkSlices(const int crate, const int module,
                                     const int modulesPerSlink, int &timeslices, int &trigCpm)
{
    int slices = -1;
    int trigC  = m_dfltSlices / 2;
    for (int mod = module; mod < module + modulesPerSlink; ++mod)
    {
        for (int chan = 0; chan < m_channels; ++chan)
        {
            double eta = 0.;
            double phi = 0.;
            int layer = 0;
            if ( !m_cpmMaps->mapping(crate, mod, chan, eta, phi, layer)) continue;
            const unsigned int key = m_towerKey->ttKey(phi, eta);
            const LVL1::CPMTower *const tt = findCpmTower(key);
            if ( !tt ) continue;
            const int numdat = 4;
            std::vector<int> sums(numdat);
            std::vector<int> sizes(numdat);
            sums[0] = std::accumulate((tt->emEnergyVec()).begin(),
                                      (tt->emEnergyVec()).end(), 0);
            sums[1] = std::accumulate((tt->hadEnergyVec()).begin(),
                                      (tt->hadEnergyVec()).end(), 0);
            sums[2] = std::accumulate((tt->emErrorVec()).begin(),
                                      (tt->emErrorVec()).end(), 0);
            sums[3] = std::accumulate((tt->hadErrorVec()).begin(),
                                      (tt->hadErrorVec()).end(), 0);
            sizes[0] = (tt->emEnergyVec()).size();
            sizes[1] = (tt->hadEnergyVec()).size();
            sizes[2] = (tt->emErrorVec()).size();
            sizes[3] = (tt->hadErrorVec()).size();
            const int peak = tt->peak();
            for (int i = 0; i < numdat; ++i)
            {
                if (sums[i] == 0) continue;
                if (slices < 0)
                {
                    slices = sizes[i];
                    trigC  = peak;
                }
                else if (slices != sizes[i] || trigC != peak) return false;
            }
        }
    }
    // CMXs last slink of crate
    if (module / modulesPerSlink == m_slinks - 1)
    {
        for (int cmx = 0; cmx < m_cmxs; ++cmx)
        {
            for (int cpm = 1; cpm <= m_modules; ++ cpm)
            {
                for (int chip = 0; chip < m_chips; ++chip)
                {
                    for (int loc = 0; loc < m_locs; ++loc)
                    {
                        const int key = tobKey(crate, cmx, cpm, chip, loc);
                        const LVL1::CMXCPTob *const tob = findCmxCpTob(key);
                        if (tob)
                        {
                            const int numdat = 3;
                            std::vector<int> sums(numdat);
                            std::vector<int> sizes(numdat);
                            sums[0] = std::accumulate((tob->energyVec()).begin(),
                                                      (tob->energyVec()).end(), 0);
                            sums[1] = std::accumulate((tob->isolationVec()).begin(),
                                                      (tob->isolationVec()).end(), 0);
                            sums[2] = std::accumulate((tob->errorVec()).begin(),
                                                      (tob->errorVec()).end(), 0);
                            sizes[0] = (tob->energyVec()).size();
                            sizes[1] = (tob->isolationVec()).size();
                            sizes[2] = (tob->errorVec()).size();
                            const int peak = tob->peak();
                            for (int i = 0; i < numdat; ++i)
                            {
                                if (sums[i] == 0) continue;
                                if (slices < 0)
                                {
                                    slices = sizes[i];
                                    trigC  = peak;
                                }
                                else if (slices != sizes[i] || trigC != peak) return false;
                            }
                        }
                    }
                }
            }
            for (int source = 0; source < LVL1::CMXCPHits::MAXSOURCE; ++source)
            {
                const int key = hitsKey(crate, cmx, source);
                const LVL1::CMXCPHits *const hits = findCmxCpHits(key);
                if (hits)
                {
                    const int numdat = 4;
                    std::vector<unsigned int> sums(numdat);
                    std::vector<int> sizes(numdat);
                    sums[0] = std::accumulate((hits->hitsVec0()).begin(),
                                              (hits->hitsVec0()).end(), 0);
                    sums[1] = std::accumulate((hits->hitsVec1()).begin(),
                                              (hits->hitsVec1()).end(), 0);
                    sums[2] = std::accumulate((hits->errorVec0()).begin(),
                                              (hits->errorVec0()).end(), 0);
                    sums[3] = std::accumulate((hits->errorVec1()).begin(),
                                              (hits->errorVec1()).end(), 0);
                    sizes[0] = (hits->hitsVec0()).size();
                    sizes[1] = (hits->hitsVec1()).size();
                    sizes[2] = (hits->errorVec0()).size();
                    sizes[3] = (hits->errorVec1()).size();
                    const int peak = hits->peak();
                    for (int i = 0; i < numdat; ++i)
                    {
                        if (sums[i] == 0) continue;
                        if (slices < 0)
                        {
                            slices = sizes[i];
                            trigC  = peak;
                        }
                        else if (slices != sizes[i] || trigC != peak) return false;
                    }
                }
            }
        }
    }
    if (slices < 0) slices = m_dfltSlices;
    timeslices = slices;
    trigCpm    = trigC;
    return true;
}

} // end namespace
