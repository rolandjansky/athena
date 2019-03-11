/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>
#include <set>
#include <utility>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/SegMemSvc.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

#include "CmmSubBlock.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloSubBlock.h"
#include "L1CaloUserHeader.h"
#include "ModifySlices.h"
#include "PpmSubBlockV1.h"

#include "PpmByteStreamV1Tool.h"

namespace LVL1BS
{

const int PpmByteStreamV1Tool::s_crates;
const int PpmByteStreamV1Tool::s_modules;
const int PpmByteStreamV1Tool::s_channels;
const int PpmByteStreamV1Tool::s_dataSize;

// Interface ID

static const InterfaceID IID_IPpmByteStreamV1Tool("PpmByteStreamV1Tool", 1, 1);

const InterfaceID &PpmByteStreamV1Tool::interfaceID()
{
    return IID_IPpmByteStreamV1Tool;
}

// Constructor

PpmByteStreamV1Tool::PpmByteStreamV1Tool(const std::string &type,
        const std::string &name,
        const IInterface  *parent)
    : AthAlgTool(type, name, parent),
      m_ppmMaps("LVL1::PpmMappingTool/PpmMappingTool"),
      m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
      m_sms("SegMemSvc/SegMemSvc", name),
      m_version(1), m_compVers(4),
      m_dataChannels(true), m_spareChannels(false), m_muonChannels(false),
      m_subDetector(eformat::TDAQ_CALO_PREPROC),
      m_srcIdMap(0), m_towerKey(0), m_errorBlock(0), m_rodStatus(0), m_fea(0)
{
    declareInterface<PpmByteStreamV1Tool>(this);

    declareProperty("PpmMappingTool", m_ppmMaps,
                    "Crate/Module/Channel to Eta/Phi/Layer mapping tool");
    declareProperty("ErrorTool", m_errorTool,
                    "Tool to collect errors for monitoring");

    declareProperty("PrintCompStats",     m_printCompStats  = 0,
                    "Print compressed format statistics");
    declareProperty("SlinksPerCrate",     m_slinks          = 4,
                    "The number of S-Links per crate");

    // Properties for reading bytestream only
    declareProperty("ZeroSuppress",       m_zeroSuppress    = 0,
                    "Only make trigger towers with non-zero EM or Had energy");
    declareProperty("ROBSourceIDs",       m_sourceIDs,
                    "ROB fragment source identifiers");
    declareProperty("PedestalValue",      m_pedestal        = 10,
                    "Pedestal value - needed for compressed formats 0,1 only");

    // Properties for writing bytestream only
    declareProperty("DataFormat",         m_dataFormat      = 1,
                    "Format identifier (0-3) in sub-block header");
    declareProperty("FADCBaseline",       m_fadcBaseline    = 0,
                    "FADC baseline lower bound for compressed formats");
    declareProperty("FADCThreshold",      m_fadcThreshold   = 1,
                    "FADC threshold for super-compressed format");
    declareProperty("SimulSlicesLUT",     m_dfltSlicesLut   = 1,
                    "The number of LUT slices in the simulation");
    declareProperty("SimulSlicesFADC",    m_dfltSlicesFadc  = 7,
                    "The number of FADC slices in the simulation");
    declareProperty("ForceSlicesLUT",     m_forceSlicesLut  = 0,
                    "If >0, the number of LUT slices in bytestream");
    declareProperty("ForceSlicesFADC",    m_forceSlicesFadc = 0,
                    "If >0, the number of FADC slices in bytestream");
    declareProperty("CrateMin",       m_crateMin = 0,
                    "Minimum crate number, allows partial output");
    declareProperty("CrateMax",       m_crateMax = s_crates - 1,
                    "Maximum crate number, allows partial output");

}

// Destructor

PpmByteStreamV1Tool::~PpmByteStreamV1Tool()
{
}

// Initialize


StatusCode PpmByteStreamV1Tool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version "
                   << PACKAGE_VERSION << endmsg;

    StatusCode sc = m_ppmMaps.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Failed to retrieve tool " << m_ppmMaps << endmsg;
        return sc;
    }
    else msg(MSG::INFO) << "Retrieved tool " << m_ppmMaps << endmsg;

    sc = m_errorTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Failed to retrieve tool " << m_errorTool << endmsg;
        return sc;
    }
    else msg(MSG::INFO) << "Retrieved tool " << m_errorTool << endmsg;

    sc = m_sms.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Failed to retrieve service " << m_sms << endmsg;
        return sc;
    }
    else msg(MSG::INFO) << "Retrieved service " << m_sms << endmsg;

    m_srcIdMap  = new L1CaloSrcIdMap();
    m_towerKey  = new LVL1::TriggerTowerKey();
    m_rodStatus = new std::vector<uint32_t>(2);
    m_fea       = new FullEventAssembler<L1CaloSrcIdMap>();

    return StatusCode::SUCCESS;
}

// Finalize

StatusCode PpmByteStreamV1Tool::finalize()
{
    if (m_printCompStats && msgLvl(MSG::INFO))
    {
        msg(MSG::INFO);
        printCompStats();
    }
    delete m_fea;
    delete m_rodStatus;
    delete m_errorBlock;
    delete m_towerKey;
    delete m_srcIdMap;
    return StatusCode::SUCCESS;
}

// Conversion bytestream to trigger towers

StatusCode PpmByteStreamV1Tool::convert(
    const IROBDataProviderSvc::VROBFRAG &robFrags,
    DataVector<LVL1::TriggerTower> *const ttCollection)
{
    const bool debug   = msgLvl(MSG::DEBUG);
    const bool verbose = msgLvl(MSG::VERBOSE);
    if (debug) msg(MSG::DEBUG);

    // Set up TriggerTower pool and mappings
    // (NB. This assumes mappings won't change during the course of a job)
    const int maxChannels = s_crates * s_modules * s_channels;
    const int chanBitVecSize = maxChannels / 32;
    if (m_ttData.empty())
    {
        const int spareSize = maxChannels - 2 * s_dataSize;
        const int muonSize = 2 * s_channels;
        const int modBitVecSize = (s_crates * s_modules) / 32;
        m_ttData.reserve(s_dataSize);
        m_ttSpare.reserve(spareSize);
        m_ttMuon.reserve(muonSize);
        m_ttPos.resize(maxChannels);
        m_chanLayer.resize(chanBitVecSize);
        m_dataChan.resize(chanBitVecSize);
        m_spareChan.resize(chanBitVecSize);
        m_muonChan.resize(chanBitVecSize);
        m_dataMod.resize(modBitVecSize);
        m_spareMod.resize(modBitVecSize);
        m_muonMod.resize(modBitVecSize);
        TriggerTowerMap ttMap;
        TriggerTowerMap::iterator itt;
        std::vector<int> dummyS;
        std::vector<int> dummyL(1);
        std::vector<int> dummyF(5);
        int dataCount  = 0;
        int spareCount = 0;
        for (int crate = 0; crate < s_crates; ++crate)
        {
            for (int module = 0; module < s_modules; ++module)
            {
                const int index2 = (crate << 4) + module;
                const int word2  = index2 / 32;
                const int bit2   = index2 % 32;
                for (int channel = 0; channel < s_channels; ++channel)
                {
                    const int index = (crate << 10) + (module << 6) + channel;
                    const int word  = index / 32;
                    const int bit   = index % 32;
                    double eta = 0.;
                    double phi = 0.;
                    int layer = 0;
                    unsigned int key = 0;
                    if (m_ppmMaps->mapping(crate, module, channel, eta, phi, layer))
                    {
                        // Data channels
                        key = m_towerKey->ttKey(phi, eta);
                        itt = ttMap.find(key);
                        if (itt == ttMap.end())
                        {
                            LVL1::TriggerTower *tt =
                                new (m_sms->allocate<LVL1::TriggerTower>(SegMemSvc::JOB))
                            LVL1::TriggerTower(phi, eta, key,
                                               dummyF, dummyL, dummyF, dummyL, 0, 0, 0,
                                               dummyF, dummyL, dummyF, dummyL, 0, 0, 0);
                            m_ttData.push_back(tt);
                            const int count = dataCount++;
                            m_ttPos[index] = count;
                            ttMap.insert(std::make_pair(key, count));
                        }
                        else
                        {
                            m_ttPos[index] = itt->second;
                        }
                        m_chanLayer[word] |= (layer << bit);
                        m_dataChan[word]  |= (1 << bit);
                        m_dataMod[word2]  |= (1 << bit2);
                    }
                    else
                    {
                        // Spare channels
                        const int pin  = channel % 16;
                        const int asic = channel / 16;
                        eta = 16 * crate + module;
                        phi = 4 * pin + asic;
                        layer = 0;
                        const int type = 1;
                        key = (crate << 24) | (type << 20) | (module << 16) | (pin << 8) | asic; // CoolID
                        LVL1::TriggerTower *tt =
                            new (m_sms->allocate<LVL1::TriggerTower>(SegMemSvc::JOB))
                        LVL1::TriggerTower(phi, eta, key,
                                           dummyF, dummyL, dummyF, dummyL, 0, 0, 0,
                                           dummyS, dummyS, dummyS, dummyS, 0, 0, 0);
                        m_ttSpare.push_back(tt);
                        m_ttPos[index] = spareCount++;
                        m_chanLayer[word] |= (layer << bit);
                        m_spareChan[word] |= (1 << bit);
                        m_spareMod[word2] |= (1 << bit2);
                        if ((crate == 2 || crate == 3) && (module == 0))
                        {
                            m_ttMuon.push_back(tt);
                            m_muonChan[word] |= (1 << bit);
                            m_muonMod[word2] |= (1 << bit2);
                        }
                    }
                }
            }
        }
    }

    // Set up according to the collection we want

    TriggerTowerVector &ttCol((m_dataChannels) ? m_ttData
                              : (m_spareChannels)
                              ? m_ttSpare
                              : m_ttMuon);
    TriggerTowerVector &ttColRef((m_dataChannels) ? m_ttData
                                 : m_ttSpare);
    ChannelBitVector &colChan((m_dataChannels) ? m_dataChan
                              : (m_spareChannels)
                              ? m_spareChan
                              : m_muonChan);
    ChannelBitVector &colMod((m_dataChannels) ? m_dataMod
                             : (m_spareChannels)
                             ? m_spareMod
                             : m_muonMod);
    const int colSize = (m_dataChannels) ? 2 * ttCol.size()
                        : ttCol.size();
    m_foundChan.assign(chanBitVecSize, 0);
    int ttCount = 0;

    // Vectors to unpack into
    std::vector<int> lut;
    std::vector<int> fadc;
    std::vector<int> bcidLut;
    std::vector<int> bcidFadc;

    // Loop over ROB fragments

    int robCount = 0;
    std::set<uint32_t> dupCheck;
    ROBIterator rob    = robFrags.begin();
    ROBIterator robEnd = robFrags.end();
    for (; rob != robEnd; ++rob)
    {

        if (debug)
        {
            ++robCount;
            msg() << "Treating ROB fragment " << robCount << endmsg;
        }

        // Skip fragments with ROB status errors

        uint32_t robid = (*rob)->source_id();
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
        if (m_srcIdMap->getRobID(sourceID) != robid         ||
                m_srcIdMap->subDet(sourceID)   != m_subDetector ||
                m_srcIdMap->daqOrRoi(sourceID) != 0             ||
                m_srcIdMap->slink(sourceID)    >= m_slinks      ||
                m_srcIdMap->crate(sourceID)    >= s_crates)
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
        if (minorVersion > m_srcIdMap->minorVersionPreLS1())
        {
            if (debug) msg() << "Skipping post-LS1 data" << endmsg;
            continue;
        }
        const int rodCrate = m_srcIdMap->crate(sourceID);
        if (debug)
        {
            msg() << "Treating crate " << rodCrate
                  << " slink "         << m_srcIdMap->slink(sourceID) << endmsg;
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
            if (debug)
            {
                msg() << "Unexpected number of user header words: "
                      << headerWords << endmsg;
            }
            continue;
        }
        for (int i = 0; i < headerWords; ++i) ++payload;
        // triggered slice offsets
        const int trigLut  = userHeader.ppmLut();
        const int trigFadc = userHeader.ppmFadc();
        // FADC baseline lower bound
        m_fadcBaseline = userHeader.lowerBound();
        if (debug)
        {
            msg() << "Minor format version number: "
                  << MSG::hex << minorVersion << MSG::dec              << endmsg
                  << "LUT triggered slice offset:  " << trigLut        << endmsg
                  << "FADC triggered slice offset: " << trigFadc       << endmsg
                  << "FADC baseline lower bound:   " << m_fadcBaseline << endmsg;
        }
        const int runNumber = (*rob)->rod_run_no() & 0xffffff;

        // Find the number of channels per sub-block

        int chanPerSubBlock = 0;
        bool firstBlock = false;
        uint32_t firstWord = 0;
        RODPointer payloadFirst;
        if (payload != payloadEnd)
        {
            if (L1CaloSubBlock::wordType(*payload) != L1CaloSubBlock::HEADER
                    || CmmSubBlock::cmmBlock(*payload))
            {
                m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_MISSING_HEADER);
                if (debug) msg() << "Missing Sub-block header" << endmsg;
                continue;
            }
            firstBlock = true;
            firstWord = *payload;
            if (m_ppmBlocks.empty())
            {
                m_ppmBlocks.push_back(new PpmSubBlockV1());
            }
            PpmSubBlockV1 *const subBlock = m_ppmBlocks[0];
            subBlock->clear();
            payloadFirst = subBlock->read(payload, payloadEnd);
            chanPerSubBlock = subBlock->channelsPerSubBlock();
            if (chanPerSubBlock == 0)
            {
                m_errorTool->rodError(robid, subBlock->unpackErrorCode());
                if (debug) msg() << "Unsupported version/data format: "
                                     << subBlock->version() << "/"
                                     << subBlock->format()  << endmsg;
                continue;
            }
            if (debug) msg() << "Channels per sub-block: "
                                 << chanPerSubBlock << endmsg;
        }
        else
        {
            if (debug) msg() << "ROB fragment contains user header only" << endmsg;
            continue;
        }
        const int numSubBlocks = s_channels / chanPerSubBlock;
        const int size = m_ppmBlocks.size();
        if (numSubBlocks > size)
        {
            for (int i = size; i < numSubBlocks; ++i)
            {
                m_ppmBlocks.push_back(new PpmSubBlockV1());
            }
        }

        // Loop over PPMs

        payload = payloadBeg;
        for (int i = 0; i < headerWords; ++i) ++payload;
        unsigned int rodErr = L1CaloSubBlock::ERROR_NONE;
        while (payload != payloadEnd)
        {

            // Get all sub-blocks for one PPM (first already read in above)

            int crate = 0;
            int module = 0;
            int nPpmBlocks = 0;
            for (int block = 0; block < numSubBlocks; ++block)
            {
                const uint32_t word = (firstBlock) ? firstWord : *payload;
                if (L1CaloSubBlock::wordType(word) != L1CaloSubBlock::HEADER
                        || CmmSubBlock::cmmBlock(word)
                        || PpmSubBlockV1::errorBlock(word))
                {
                    if (debug) msg() << "Unexpected data sequence" << endmsg;
                    rodErr = L1CaloSubBlock::ERROR_MISSING_HEADER;
                    break;
                }
                if (chanPerSubBlock != s_channels &&
                        L1CaloSubBlock::seqno(word) != block * chanPerSubBlock)
                {
                    if (debug)
                    {
                        msg() << "Unexpected channel sequence number: "
                              << L1CaloSubBlock::seqno(word) << " expected "
                              << block *chanPerSubBlock << endmsg;
                    }
                    rodErr = L1CaloSubBlock::ERROR_MISSING_SUBBLOCK;
                    break;
                }
                PpmSubBlockV1 *const subBlock = m_ppmBlocks[block];
                nPpmBlocks++;
                if (firstBlock)
                {
                    payload = payloadFirst;
                    firstBlock = false;
                }
                else
                {
                    subBlock->clear();
                    payload = subBlock->read(payload, payloadEnd);
                }
                if (block == 0)
                {
                    crate = subBlock->crate();
                    module = subBlock->module();
                    if (debug)
                    {
                        msg() << "Crate " << crate << "  Module " << module << endmsg;
                    }
                    if (crate != rodCrate)
                    {
                        if (debug)
                        {
                            msg() << "Inconsistent crate number in ROD source ID" << endmsg;
                        }
                        rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
                        break;
                    }
                }
                else
                {
                    if (subBlock->crate() != crate)
                    {
                        if (debug) msg() << "Inconsistent crate number in sub-blocks"
                                             << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
                        break;
                    }
                    if (subBlock->module() != module)
                    {
                        if (debug) msg() << "Inconsistent module number in sub-blocks"
                                             << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
                        break;
                    }
                }
                if (payload == payloadEnd && block != numSubBlocks - 1)
                {
                    if (debug) msg() << "Premature end of data" << endmsg;
                    rodErr = L1CaloSubBlock::ERROR_MISSING_SUBBLOCK;
                    break;
                }
            }
            if (rodErr != L1CaloSubBlock::ERROR_NONE) break;

            // Is there an error block?

            bool isErrBlock = false;
            if (payload != payloadEnd)
            {
                if (L1CaloSubBlock::wordType(*payload) == L1CaloSubBlock::HEADER
                        && !CmmSubBlock::cmmBlock(*payload)
                        && PpmSubBlockV1::errorBlock(*payload))
                {
                    if (debug) msg() << "Error block found" << endmsg;
                    if (!m_errorBlock) m_errorBlock = new PpmSubBlockV1();
                    else m_errorBlock->clear();
                    isErrBlock = true;
                    payload = m_errorBlock->read(payload, payloadEnd);
                    if (m_errorBlock->crate() != crate)
                    {
                        if (debug) msg() << "Inconsistent crate number in error block"
                                             << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
                        break;
                    }
                    if (m_errorBlock->module() != module)
                    {
                        if (debug) msg() << "Inconsistent module number in error block"
                                             << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
                        break;
                    }
                    if (m_errorBlock->dataWords() && !m_errorBlock->unpack())
                    {
                        if (debug)
                        {
                            std::string errMsg(m_errorBlock->unpackErrorMsg());
                            msg() << "Unpacking error block failed: " << errMsg << endmsg;
                        }
                        rodErr = m_errorBlock->unpackErrorCode();
                        break;
                    }
                }
            }

            // Don't bother unpacking modules that aren't used for required collection

            const int index2 = (crate << 4) + module;
            const int word2  = index2 / 32;
            const int bit2   = index2 % 32;
            if (!((colMod[word2] >> bit2) & 1)) continue;

            // Loop over sub-blocks and fill trigger towers

            for (int block = 0; block < nPpmBlocks; ++block)
            {
                PpmSubBlockV1 *const subBlock = m_ppmBlocks[block];
                subBlock->setLutOffset(trigLut);
                subBlock->setFadcOffset(trigFadc);
                subBlock->setPedestal(m_pedestal);
                subBlock->setFadcBaseline(m_fadcBaseline);
                subBlock->setRunNumber(runNumber);
                if (debug)
                {
                    msg() << "Unpacking sub-block version/format/seqno: "
                          << subBlock->version() << "/" << subBlock->format() << "/"
                          << subBlock->seqno() << endmsg;
                }
                if (subBlock->dataWords() && !subBlock->unpack())
                {
                    if (debug)
                    {
                        std::string errMsg(subBlock->unpackErrorMsg());
                        msg() << "Unpacking PPM sub-block failed: " << errMsg << endmsg;
                    }
                    rodErr = subBlock->unpackErrorCode();
                    break;
                }
                if (m_printCompStats) addCompStats(subBlock->compStats());
                for (int chan = 0; chan < chanPerSubBlock; ++chan)
                {
                    const int channel = block * chanPerSubBlock + chan;
                    const int index = (crate << 10) + (module << 6) + channel;
                    const int word  = index / 32;
                    const int bit   = index % 32;
                    if (!((colChan[word] >> bit) & 1)) continue; // skip unwanted channels
                    if (((m_foundChan[word] >> bit) & 1))
                    {
                        if (debug) msg() << "Duplicate data for crate/module/channel: "
                                             << crate << "/" << module << "/" << channel
                                             << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
                        break;
                    }
                    lut.clear();
                    fadc.clear();
                    bcidLut.clear();
                    bcidFadc.clear();
                    subBlock->ppmData(channel, lut, fadc, bcidLut, bcidFadc);
                    if (lut.size() < size_t(trigLut + 1))
                    {
                        if (debug)
                        {
                            msg() << "Triggered LUT slice from header "
                                  << "inconsistent with number of slices: "
                                  << trigLut << ", " << lut.size() << endmsg;
                        }
                        rodErr = L1CaloSubBlock::ERROR_SLICES;
                        break;
                    }
                    if (fadc.size() < size_t(trigFadc + 1))
                    {
                        if (debug)
                        {
                            msg() << "Triggered FADC slice from header "
                                  << "inconsistent with number of slices: "
                                  << trigFadc << ", " << fadc.size() << endmsg;
                        }
                        rodErr = L1CaloSubBlock::ERROR_SLICES;
                        break;
                    }
                    LVL1::DataError errorBits(0);
                    if (isErrBlock)
                    {
                        errorBits.set(LVL1::DataError::PPMErrorWord,
                                      m_errorBlock->ppmError(channel));
                        errorBits.set(LVL1::DataError::SubStatusWord,
                                      m_errorBlock->subStatus());
                    }
                    else
                    {
                        errorBits.set(LVL1::DataError::PPMErrorWord,
                                      subBlock->ppmError(channel));
                        const PpmSubBlockV1 *const lastBlock =
                            m_ppmBlocks[nPpmBlocks - 1];
                        errorBits.set(LVL1::DataError::SubStatusWord,
                                      lastBlock->subStatus());
                    }
                    // Wrong bit set for compressed formats 1.01 to 1.03
                    if (subBlock->format() > 1 && subBlock->seqno() < 4)
                    {
                        errorBits.set(LVL1::DataError::ModuleError,
                                      (errorBits.error() >> (LVL1::DataError::ModuleError + 1)) & 0x1);
                    }
                    const int error = errorBits.error();

                    // Save to TriggerTower

                    if (verbose)
                    {
                        msg(MSG::VERBOSE) << "channel/LUT/FADC/bcidLUT/bcidFADC/error: "
                                          << channel << "/";
                        printVec(lut);
                        printVec(fadc);
                        printVec(bcidLut);
                        printVec(bcidFadc);
                        msg() << MSG::hex << error << MSG::dec << "/";
                    }
                    m_foundChan[word] |= (1 << bit);
                    ++ttCount;
                    LVL1::TriggerTower *tt = ttColRef[m_ttPos[index]];
                    const int layer = ((m_chanLayer[word] >> bit) & 1);
                    if (layer == 0)    // EM
                    {
                        tt->addEM(fadc, lut, bcidFadc, bcidLut, error, trigLut, trigFadc);
                    }
                    else               // Had
                    {
                        tt->addHad(fadc, lut, bcidFadc, bcidLut, error, trigLut, trigFadc);
                    }
                }
                if (rodErr != L1CaloSubBlock::ERROR_NONE) break;
            }
            if (rodErr != L1CaloSubBlock::ERROR_NONE) break;
        }
        if (rodErr != L1CaloSubBlock::ERROR_NONE)
            m_errorTool->rodError(robid, rodErr);
    }

    // Reset any missing channels (should be rare)

    if (ttCount != colSize)
    {
        if (debug)
        {
            msg() << "Found " << ttCount << " channels, expected " << colSize << endmsg;
        }
        std::vector<int> dummy(1);
        for (int word = 0; word < chanBitVecSize; ++word)
        {
            if (m_foundChan[word] != colChan[word])
            {
                for (int bit = 0; bit < 32; ++bit)
                {
                    if (((m_foundChan[word] >> bit) & 1) != ((colChan[word] >> bit) & 1))
                    {
                        const int index = word * 32 + bit;
                        LVL1::TriggerTower *tt = ttColRef[m_ttPos[index]];
                        const int layer = ((m_chanLayer[word] >> bit) & 1);
                        if (layer == 0)              // EM
                        {
                            tt->addEM(dummy, dummy, dummy, dummy, 0, 0, 0);
                        }
                        else if (m_dataChannels)     // Had
                        {
                            tt->addHad(dummy, dummy, dummy, dummy, 0, 0, 0);
                        }
                    }
                }
            }
        }
    }

    // And copy into output collection

    if (m_zeroSuppress)
    {
        TriggerTowerVector::iterator itr  = ttCol.begin();
        TriggerTowerVector::iterator itrE = ttCol.end();
        for (; itr != itrE; ++itr)
        {
            if ((*itr)->emEnergy() || (m_dataChannels && (*itr)->hadEnergy()))
            {
                ttCollection->push_back(*itr);
            }
        }
    }
    else
    {
        ttCollection->assign(ttCol.begin(), ttCol.end());
    }

    return StatusCode::SUCCESS;
}

// Conversion of trigger towers to bytestream

StatusCode PpmByteStreamV1Tool::convert(
    const DataVector<LVL1::TriggerTower> *const ttCollection,
    RawEventWrite *const re)
{
    const bool debug = msgLvl(MSG::DEBUG);
    if (debug) msg(MSG::DEBUG);

    // Clear the event assembler

    m_fea->clear();
    const uint16_t minorVersion = m_srcIdMap->minorVersionPreLS1();
    m_fea->setRodMinorVersion(minorVersion);
    m_rodStatusMap.clear();

    // Pointer to ROD data vector

    FullEventAssembler<L1CaloSrcIdMap>::RODDATA *theROD = 0;

    // Set up trigger tower maps

    setupTTMaps(ttCollection);

    // Create the sub-blocks to do the packing

    PpmSubBlockV1 subBlock;
    const int chanPerSubBlock = subBlock.channelsPerSubBlock(m_version,
                                m_dataFormat);
    if (chanPerSubBlock == 0)
    {
        msg(MSG::ERROR) << "Unsupported version/data format: "
                        << m_version << "/" << m_dataFormat << endmsg;
        return StatusCode::FAILURE;
    }
    PpmSubBlockV1 errorBlock;

    int slicesLut  = 1;
    int slicesFadc = 1;
    int trigLut    = 0;
    int trigFadc   = 0;
    int slicesLutNew  = 1;
    int slicesFadcNew = 1;
    int trigLutNew    = 0;
    int trigFadcNew   = 0;
    const int modulesPerSlink = s_modules / m_slinks;
    for (int crate = m_crateMin; crate <= m_crateMax; ++crate)
    {
        for (int module = 0; module < s_modules; ++module)
        {

            // Pack required number of modules per slink

            if (module % modulesPerSlink == 0)
            {
                const int daqOrRoi = 0;
                const int slink = module / modulesPerSlink;
                if (debug)
                {
                    msg() << "Treating crate " << crate << " slink " << slink << endmsg;
                }
                // Get number of slices and triggered slice offsets
                // for this slink
                if ( ! slinkSlices(crate, module, modulesPerSlink,
                                   slicesLut, slicesFadc, trigLut, trigFadc))
                {
                    msg(MSG::ERROR) << "Inconsistent number of slices or "
                                    << "triggered slice offsets in data for crate "
                                    << crate << " slink " << slink << endmsg;
                    return StatusCode::FAILURE;
                }
                slicesLutNew  = (m_forceSlicesLut)  ? m_forceSlicesLut  : slicesLut;
                slicesFadcNew = (m_forceSlicesFadc) ? m_forceSlicesFadc : slicesFadc;
                trigLutNew    = ModifySlices::peak(trigLut,  slicesLut,  slicesLutNew);
                trigFadcNew   = ModifySlices::peak(trigFadc, slicesFadc, slicesFadcNew);
                if (debug)
                {
                    msg() << "Data Version/Format: " << m_version
                          << " " << m_dataFormat << endmsg
                          << "LUT slices/offset: " << slicesLut << " " << trigLut;
                    if (slicesLut != slicesLutNew)
                    {
                        msg() << " modified to " << slicesLutNew << " " << trigLutNew;
                    }
                    msg() << endmsg
                          << "FADC slices/offset: " << slicesFadc << " " << trigFadc;
                    if (slicesFadc != slicesFadcNew)
                    {
                        msg() << " modified to " << slicesFadcNew << " " << trigFadcNew;
                    }
                    msg() << endmsg;
                }
                L1CaloUserHeader userHeader;
                userHeader.setPpmLut(trigLutNew);
                userHeader.setPpmFadc(trigFadcNew);
                userHeader.setLowerBound(m_fadcBaseline);
                const uint32_t rodIdPpm = m_srcIdMap->getRodID(crate, slink, daqOrRoi,
                                          m_subDetector);
                theROD = m_fea->getRodData(rodIdPpm);
                theROD->push_back(userHeader.header());
                m_rodStatusMap.insert(make_pair(rodIdPpm, m_rodStatus));
            }
            if (debug) msg() << "Module " << module << endmsg;

            // Find trigger towers corresponding to each eta/phi pair and fill
            // sub-blocks

            bool upstreamError = false;
            for (int channel = 0; channel < s_channels; ++channel)
            {
                const int chan = channel % chanPerSubBlock;
                if (channel == 0 && m_dataFormat == L1CaloSubBlock::UNCOMPRESSED)
                {
                    errorBlock.clear();
                    errorBlock.setPpmErrorHeader(m_version, m_dataFormat, crate,
                                                 module, slicesFadcNew, slicesLutNew);
                }
                if (chan == 0)
                {
                    subBlock.clear();
                    if (m_dataFormat >= L1CaloSubBlock::COMPRESSED)
                    {
                        subBlock.setPpmHeader(m_version, m_dataFormat, m_compVers, crate,
                                              module, slicesFadcNew, slicesLutNew);
                    }
                    else
                    {
                        subBlock.setPpmHeader(m_version, m_dataFormat, channel, crate,
                                              module, slicesFadcNew, slicesLutNew);
                    }
                    subBlock.setLutOffset(trigLutNew);
                    subBlock.setFadcOffset(trigFadcNew);
                    subBlock.setFadcBaseline(m_fadcBaseline);
                    subBlock.setFadcThreshold(m_fadcThreshold);
                }
                const LVL1::TriggerTower *tt = 0;
                double eta = 0.;
                double phi = 0.;
                int layer = 0;
                if (m_ppmMaps->mapping(crate, module, channel, eta, phi, layer))
                {
                    tt = findLayerTriggerTower(eta, phi, layer);
                }
                if (tt )
                {
                    int err = 0;
                    std::vector<int> lut;
                    std::vector<int> fadc;
                    std::vector<int> bcidLut;
                    std::vector<int> bcidFadc;
                    if (layer == 0)    // em
                    {
                        ModifySlices::data(tt->emLUT(),     lut,      slicesLutNew);
                        ModifySlices::data(tt->emADC(),     fadc,     slicesFadcNew);
                        ModifySlices::data(tt->emBCIDvec(), bcidLut,  slicesLutNew);
                        ModifySlices::data(tt->emBCIDext(), bcidFadc, slicesFadcNew);
                        err = tt->emError();
                    }
                    else               // had
                    {
                        ModifySlices::data(tt->hadLUT(),     lut,      slicesLutNew);
                        ModifySlices::data(tt->hadADC(),     fadc,     slicesFadcNew);
                        ModifySlices::data(tt->hadBCIDvec(), bcidLut,  slicesLutNew);
                        ModifySlices::data(tt->hadBCIDext(), bcidFadc, slicesFadcNew);
                        err = tt->hadError();
                    }
                    subBlock.fillPpmData(channel, lut, fadc, bcidLut, bcidFadc);
                    if (err)
                    {
                        const LVL1::DataError errorBits(err);
                        const int errpp = errorBits.get(LVL1::DataError::PPMErrorWord);
                        if (m_dataFormat == L1CaloSubBlock::UNCOMPRESSED)
                        {
                            errorBlock.fillPpmError(channel, errpp);
                        }
                        else subBlock.fillPpmError(channel, errpp);
                        if (errpp >> 2) upstreamError = true;
                    }
                }
                if (chan == chanPerSubBlock - 1)
                {
                    // output the packed sub-block
                    if ( !subBlock.pack())
                    {
                        msg(MSG::ERROR) << "PPM sub-block packing failed" << endmsg;
                        return StatusCode::FAILURE;
                    }
                    if (m_printCompStats) addCompStats(subBlock.compStats());
                    if (channel != s_channels - 1)
                    {
                        // Only put errors in last sub-block
                        subBlock.setStatus(0, false, false, false, false,
                                           false, false, false);
                        if (debug)
                        {
                            msg() << "PPM sub-block data words: "
                                  << subBlock.dataWords() << endmsg;
                        }
                        subBlock.write(theROD);
                    }
                    else
                    {
                        // Last sub-block - write error block
                        bool glinkTimeout = false;
                        bool daqOverflow  = false;
                        bool bcnMismatch  = false;
                        bool glinkParity  = false;
                        if (m_dataFormat == L1CaloSubBlock::UNCOMPRESSED)
                        {
                            glinkTimeout = errorBlock.mcmAbsent() ||
                                           errorBlock.timeout();
                            daqOverflow  = errorBlock.asicFull() ||
                                           errorBlock.fpgaCorrupt();
                            bcnMismatch  = errorBlock.eventMismatch() ||
                                           errorBlock.bunchMismatch();
                            glinkParity  = errorBlock.glinkPinParity();
                        }
                        else
                        {
                            glinkTimeout = subBlock.mcmAbsent() ||
                                           subBlock.timeout();
                            daqOverflow  = subBlock.asicFull() ||
                                           subBlock.fpgaCorrupt();
                            bcnMismatch  = subBlock.eventMismatch() ||
                                           subBlock.bunchMismatch();
                            glinkParity  = subBlock.glinkPinParity();
                        }
                        subBlock.setStatus(0, glinkTimeout, false, upstreamError,
                                           daqOverflow, bcnMismatch, false, glinkParity);
                        if (debug)
                        {
                            msg() << "PPM sub-block data words: "
                                  << subBlock.dataWords() << endmsg;
                        }
                        subBlock.write(theROD);
                        // Only uncompressed format has a separate error block
                        if (m_dataFormat == L1CaloSubBlock::UNCOMPRESSED)
                        {
                            if ( ! errorBlock.pack())
                            {
                                msg(MSG::ERROR) << "PPM error block packing failed" << endmsg;
                                return StatusCode::FAILURE;
                            }
                            errorBlock.setStatus(0, glinkTimeout, false, upstreamError,
                                                 daqOverflow, bcnMismatch, false, glinkParity);
                            errorBlock.write(theROD);
                            if (debug)
                            {
                                msg() << "PPM error block data words: "
                                      << errorBlock.dataWords() << endmsg;
                            }
                        }
                    }
                }
            }
        }
    }

    // Fill the raw event

    m_fea->fill(re, msg());

    // Set ROD status words

    //L1CaloRodStatus::setStatus(re, m_rodStatusMap, m_srcIdMap);

    return StatusCode::SUCCESS;
}

// Add compression stats to totals

void PpmByteStreamV1Tool::addCompStats(const std::vector<uint32_t> &stats)
{
    if (stats.empty()) return;
    const int n = stats.size();
    if (m_compStats.empty()) m_compStats.resize(n);
    for (int i = 0; i < n; ++i) m_compStats[i] += stats[i];
}

// Print compression stats

void PpmByteStreamV1Tool::printCompStats() const
{
    msg() << "Compression stats format/count: ";
    const int n = m_compStats.size();
    for (int i = 0; i < n; ++i)
    {
        msg() << " " << i << "/" << m_compStats[i];
    }
    msg() << endmsg;
}

// Find a trigger tower using separate layer maps

const LVL1::TriggerTower *PpmByteStreamV1Tool::findLayerTriggerTower(
    const double eta, const double phi, const int layer)
{
    const LVL1::TriggerTower *tt = 0;
    const unsigned int key = m_towerKey->ttKey(phi, eta);
    TriggerTowerMapConst::const_iterator mapIter;
    if (layer == 0)
    {
        mapIter = m_ttEmMap.find(key);
        if (mapIter != m_ttEmMap.end()) tt = mapIter->second;
    }
    else
    {
        mapIter = m_ttHadMap.find(key);
        if (mapIter != m_ttHadMap.end()) tt = mapIter->second;
    }
    return tt;
}

// Set up trigger tower maps

void PpmByteStreamV1Tool::setupTTMaps(const TriggerTowerCollection *
                                      const ttCollection)
{
    using std::accumulate;

    m_ttEmMap.clear();
    m_ttHadMap.clear();
    TriggerTowerCollection::const_iterator pos  = ttCollection->begin();
    TriggerTowerCollection::const_iterator pose = ttCollection->end();
    for (; pos != pose; ++pos)
    {
        const LVL1::TriggerTower *tt = *pos;
        const unsigned int key = m_towerKey->ttKey(tt->phi(), tt->eta());
        // Ignore any with zero data
        // EM
        if (accumulate((tt->emLUT()).begin(),     (tt->emLUT()).end(),     0) ||
                accumulate((tt->emADC()).begin(),     (tt->emADC()).end(),     0) ||
                accumulate((tt->emBCIDvec()).begin(), (tt->emBCIDvec()).end(), 0) ||
                accumulate((tt->emBCIDext()).begin(), (tt->emBCIDext()).end(), 0) ||
                tt->emError())
        {
            m_ttEmMap.insert(std::make_pair(key, tt));
        }
        // Had
        if (accumulate((tt->hadLUT()).begin(),     (tt->hadLUT()).end(),     0) ||
                accumulate((tt->hadADC()).begin(),     (tt->hadADC()).end(),     0) ||
                accumulate((tt->hadBCIDvec()).begin(), (tt->hadBCIDvec()).end(), 0) ||
                accumulate((tt->hadBCIDext()).begin(), (tt->hadBCIDext()).end(), 0) ||
                tt->hadError())
        {
            m_ttHadMap.insert(std::make_pair(key, tt));
        }
    }
}

// Get number of slices and triggered slice offsets for next slink

bool PpmByteStreamV1Tool::slinkSlices(const int crate, const int module,
                                      const int modulesPerSlink, int &slicesLut, int &slicesFadc,
                                      int &trigLut, int &trigFadc)
{
    int sliceL = -1;
    int sliceF =  m_dfltSlicesFadc;
    int trigL  =  m_dfltSlicesLut / 2;
    int trigF  =  m_dfltSlicesFadc / 2;
    for (int mod = module; mod < module + modulesPerSlink; ++mod)
    {
        for (int chan = 0; chan < s_channels; ++chan)
        {
            double eta = 0.;
            double phi = 0.;
            int layer = 0;
            if (!m_ppmMaps->mapping(crate, mod, chan,
                                    eta, phi, layer)) continue;
            const LVL1::TriggerTower *const tt = findLayerTriggerTower(
                    eta, phi, layer);
            if ( !tt ) continue;
            if (layer == 0)
            {
                if (sliceL < 0)   // initialise
                {
                    sliceL = (tt->emLUT()).size();
                    sliceF = (tt->emADC()).size();
                    trigL  = tt->emPeak();
                    trigF  = tt->emADCPeak();
                }
                else     // check consistent
                {
                    if ((tt->emLUT()).size() != size_t(sliceL) ||
                            (tt->emADC()).size() != size_t(sliceF) ||
                            tt->emPeak() != trigL || tt->emADCPeak() != trigF)
                    {
                        return false;
                    }
                }
            }
            else
            {
                if (sliceL < 0)
                {
                    sliceL = (tt->hadLUT()).size();
                    sliceF = (tt->hadADC()).size();
                    trigL  = tt->hadPeak();
                    trigF  = tt->hadADCPeak();
                }
                else
                {
                    if ((tt->hadLUT()).size() != size_t(sliceL) ||
                            (tt->hadADC()).size() != size_t(sliceF) ||
                            tt->hadPeak() != trigL || tt->hadADCPeak() != trigF)
                    {
                        return false;
                    }
                }
            }
        }
    }
    if (sliceL < 0) sliceL = m_dfltSlicesLut;
    slicesLut  = sliceL;
    slicesFadc = sliceF;
    trigLut    = trigL;
    trigFadc   = trigF;
    return true;
}

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t> &PpmByteStreamV1Tool::sourceIDs(
    const std::string &sgKey)
{
    // Check if spare channels wanted
    const std::string flag("Spare");
    const std::string::size_type pos = sgKey.find(flag);
    m_spareChannels =
        (pos != std::string::npos && pos == (sgKey.length() - flag.length()));
    // Check if Tile Muon channels wanted
    const std::string flag2("Muon");
    const std::string::size_type pos2 = sgKey.find(flag2);
    m_muonChannels =
        (pos2 != std::string::npos && pos2 == (sgKey.length() - flag2.length()));
    m_dataChannels = (!m_spareChannels && !m_muonChannels);
    if (m_sourceIDs.empty())
    {
        const int maxlinks = m_srcIdMap->maxSlinks();
        for (int crate = 0; crate < s_crates; ++crate)
        {
            for (int slink = 0; slink < maxlinks; ++slink)
            {
                const int daqOrRoi = 0;
                const uint32_t rodId = m_srcIdMap->getRodID(crate, slink, daqOrRoi,
                                       m_subDetector);
                const uint32_t robId = m_srcIdMap->getRobID(rodId);
                m_sourceIDs.push_back(robId);
                if (crate > 1 && crate < 6)
                {
                    m_sourceIDsSpare.push_back(robId);
                    if (crate < 4 && slink == 0) m_sourceIDsMuon.push_back(robId);
                }
            }
        }
    }
    if (m_spareChannels) return m_sourceIDsSpare;
    if (m_muonChannels)  return m_sourceIDsMuon;
    return m_sourceIDs;
}

// Print a vector

void PpmByteStreamV1Tool::printVec(const std::vector<int> &vec) const
{
    std::vector<int>::const_iterator pos;
    for (pos = vec.begin(); pos != vec.end(); ++pos)
    {
        if (pos != vec.begin()) msg() << ",";
        msg() << *pos;
    }
    msg() << "/";
}


} // end namespace
