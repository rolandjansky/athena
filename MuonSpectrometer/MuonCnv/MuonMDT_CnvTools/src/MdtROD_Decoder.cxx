/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtROD_Decoder.h"

#include <algorithm>

#include "eformat/Issue.h"

namespace {
    // elevator chambers are read out by 2 CSMs
    // they are split in the middle (for both multilayers)
    // the first tube read out by the 2nd CSM is (offline!) tube 43
    constexpr int BME_1st_tube_2nd_CSM = 43;

}  // namespace
using eformat::helper::SourceIdentifier;

static const InterfaceID IID_IMdtROD_Decoder("MdtROD_Decoder", 1, 0);

/** constructor
 */
MdtROD_Decoder::MdtROD_Decoder(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent) {
    declareInterface<MdtROD_Decoder>(this);

    declareProperty("SpecialROBNumber", m_specialROBNumber = -1);
}

const InterfaceID& MdtROD_Decoder::interfaceID() { return IID_IMdtROD_Decoder; }

StatusCode MdtROD_Decoder::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());

    // Here the mapping service has to be initialized
    m_hid2re = std::make_unique<MDT_Hid2RESrcID>();
    ATH_CHECK(m_hid2re->set(&m_idHelperSvc->mdtIdHelper()));

    // check if the layout includes elevator chambers
    m_BMEpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME") != -1;
    if (m_BMEpresent) {
        ATH_MSG_INFO("Processing configuration for layouts with BME chambers.");
        m_BMEid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME");
    }
    m_BMGpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG") != -1;
    if (m_BMGpresent) {
        ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
        m_BMGid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");
    }

    ATH_CHECK(m_readKey.initialize());

    return StatusCode::SUCCESS;
}

StatusCode MdtROD_Decoder::finalize() {
    if (m_nCache > 0 || m_nNotCache > 0) {
        const float cacheFraction = ((float)m_nCache) / ((float)(m_nCache + m_nNotCache));
        ATH_MSG_INFO("Fraction of fills that use the cache = " << cacheFraction);
    }

    return StatusCode::SUCCESS;
}

StatusCode MdtROD_Decoder::fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, MdtCsmContainer& rdoIDC) const {
    //  m_debug = (m_log.level() <= MSG::DEBUG); // use to control if output debug info.
    //  decoding classes
    MdtCsmReadOut csmReadOut;
    MdtAmtReadOut amtReadOut;
    MdtHptdcReadOut hptdcReadOut;

    ATH_MSG_DEBUG("fillCollection: starting");
    uint32_t wordPos = 0;

    // Check the ROB and ROD fragment for lenght and version consistency. E.P.

    try {
        robFrag.check();
    }

    catch (eformat::Issue& ex) {
        ATH_MSG_DEBUG(ex.what());
        return StatusCode::FAILURE;  // error in fragment - we search for no collection
    }

    // Check the ROB status word for truncation or corruption E.P.

    uint32_t nstat = robFrag.nstatus();

    if (nstat) {
        const uint32_t* it;
        robFrag.status(it);

        if (*it) {
            ATH_MSG_DEBUG("Error in ROB status word: 0x" << std::hex << *it << std::dec);

            // the status word analysis in case of eformat version > 3.0
            // if ( (robFrag.version() >> 16) > 0x0300 ) {

            // various checks to be defined

            //}
            return StatusCode::FAILURE;
        }
    }

    // here go the checks on the ROD header

    const unsigned int size = robFrag.rod_ndata();  // Think this is right - it was size() before. Ed.  // Yes it is... Enrico

    // FIXME - Removed debug output for the moment. EJWM
    //   if (m_debug) log << MSG::DEBUG
    //       << "Collection identifier : " << csmOfflineId.getString() << endmsg;
    //
    //   if (m_debug) log << MSG::DEBUG
    //               << "**********Decoder dumping the words******** " << endmsg;
    //   if (size > 0) {
    //     log << MSG::DEBUG << "The size of this ROD-read is " << size << endmsg;
    //     for (unsigned int i=0; i < size; i++)
    //       if (m_debug) log << MSG::DEBUG << "word " << i
    //                           << " = " << MSG::hex << robFrag[i] << MSG::dec << endmsg;
    //   }
    //   else {
    //       if (m_debug) log << MSG::DEBUG << " Buffer size 0 ! "
    //                                           << endmsg;
    //       return;
    //   }

    leading_amt_map leadingHitMap = leading_amt_map();

    // Decode the SourceId from the ROD Header
    SourceIdentifier sid(robFrag.rod_source_id());
    
    MuonMDT_CablingMap::CablingData cabling_data{};
    cabling_data.subdetectorId = sid.subdetector_id();
    cabling_data.mrod = sid.module_id();
        
  
    // Get the first word of a buffer:
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
    robFrag.rod_data(vint);

    ATH_MSG_DEBUG("**********Decoder dumping the words******** ");

    if (size > 0) {
        ATH_MSG_DEBUG("The size of this ROD-read is ");
        for (unsigned int i = 0; i < size; i++) ATH_MSG_DEBUG("word " << i << " = " << MSG::hex << vint[i] << MSG::dec);
    } else {
        ATH_MSG_DEBUG(" Buffer size 0 ! ");
        return StatusCode::FAILURE;
    }

    csmReadOut.decodeWord(vint[wordPos]);

    if (csmReadOut.is_BOB()) {
        ATH_MSG_DEBUG("Found the beginning of buffer ");
        // Check that Lvl1d matches the one from the ROD header
        ATH_MSG_DEBUG("Level 1 Id : " << csmReadOut.lvl1Id());
    } else {
        ATH_MSG_ERROR(" Beginning of block not found ! ");
        ATH_MSG_ERROR(" Subdetector, ROD ID: 0x" << MSG::hex << cabling_data.subdetectorId << MSG::dec << ", 0x" << cabling_data.mrod << MSG::dec);
    }

    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
      const MuonMDT_CablingMap* readCdo{*readHandle};
      if (!readCdo) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    auto &msg = msgStream();
    while (!csmReadOut.is_EOB()) {
        while ((!csmReadOut.is_BOL()) && (!csmReadOut.is_EOB())) {
            wordPos += 1;
            if (wordPos >= size) {
                ATH_MSG_DEBUG("Error: data corrupted");
                return StatusCode::FAILURE;
            }
            csmReadOut.decodeWord(vint[wordPos]);
        }

        if (csmReadOut.is_BOL()) {
            ATH_MSG_DEBUG("Found the Beginnning of Link ");
        } else if (csmReadOut.is_EOB()) {
            ATH_MSG_DEBUG("Error: collection not found ");
            return StatusCode::FAILURE;
        }
        
        // uint16_t subdetId = 0x61;
        // uint16_t mrodId = csmReadOut.mrodId();

        cabling_data.csm = csmReadOut.csmId();
       

        // Get the offline identifier from the MDT cabling service
        // TDC and Tube identifier are dummy, only the chamber map is needed here
      

        ATH_MSG_DEBUG("subdetId : " << cabling_data.subdetectorId << " "
                                    << "mrodId : " << cabling_data.mrod << " "
                                    << "csmId : " << cabling_data.csm);

        cabling_data.tdcId = 0xff;
        cabling_data.channelId = 0xff;
        Identifier moduleId;

        bool cab = readCdo->getOfflineId(cabling_data, msg);

        ATH_MSG_DEBUG("getOfflineIdfromOnlineID result: ");
        ATH_MSG_DEBUG("Name : " << cabling_data.stationIndex);
        ATH_MSG_DEBUG("Eta  : " << cabling_data.eta);
        ATH_MSG_DEBUG("Phi  : " << cabling_data.phi);

        if (m_BMEpresent) {
            // for layouts with BMEs (read out by 2 CSMs) the RDOs have to be registered with the detectorElement hash
            // registration of common chambers is always done with detectorElement hash of 1st multilayer
            // boundary in BME when 2nd CSM starts is (offline!) tube 43, 1st CMS is registered with ML1 hash, 2nd CSM is ML2 hash
            if (cabling_data.stationIndex == m_BMEid && cabling_data.tube >= BME_1st_tube_2nd_CSM)
                moduleId = m_idHelperSvc->mdtIdHelper().channelID(cabling_data.stationIndex, cabling_data.eta, cabling_data.phi, 2, 1, 1);
            else
                moduleId = m_idHelperSvc->mdtIdHelper().channelID(cabling_data.stationIndex, cabling_data.eta, cabling_data.phi, 1, 1, 1);
        } else
            // for layouts with no BME the module hash keeps being used for registration
            moduleId = m_idHelperSvc->mdtIdHelper().elementID(cabling_data.stationIndex, cabling_data.eta, cabling_data.phi);

        if (!cab) {
            ATH_MSG_DEBUG("Cabling not understood");
            ATH_MSG_DEBUG("Skip decoding of CSM link, subdetId : " << MSG::hex << std::showbase << cabling_data.subdetectorId << " "
                                                                   << "mrodId : " << cabling_data.mrod << " " << MSG::dec << "csmId : " << cabling_data.csm);
            // assert(false);
            // continue;  // Go to next link instead of crashing (here or later) E.P.

            // In case of failure this produce an infinite loop because
            // you do not decode the next word before doing "continue".
            // I commented it out.

            // The previous implementation was ok, the bad BOL was simply
            // skipped. This means that you skip decoding this chamber.

            // I'm not sure how you can recover this error. I think you
            // simple have to store the error condition in the
            // StatusCode word returned to the client.
            //    } else if (!isValid) {
            // ATH_MSG_DEBUG("Offline identifier returned by MdtIdHelper is not valid");
            // ATH_MSG_DEBUG("Skip decoding of CSM link, subdetId : "
            //		    << MSG::hex << std::showbase << subdetId << " "
            //		    << "mrodId : " << mrodId << " " << MSG::dec
            //		    << "csmId : " << csmId);

        } else {
            // EJWM Removed: if ( moduleId == csmOfflineId)
            // Can't see how to keep that in here now, and also not really sure what the point of it is
            // Might be wrong though - job for experts to look into.
            // FIXME

            // get IdentifierHash for this module ID
            auto idHash = getHash(moduleId);

            // Create MdtCsm and try to get it from the cache via the IDC_WriteHandle
            std::unique_ptr<MdtCsm> collection(nullptr);
            MdtCsmContainer::IDC_WriteHandle lock = rdoIDC.getWriteHandle(idHash.first);
            if (lock.alreadyPresent()) {
                ATH_MSG_DEBUG("collections already found, do not convert");
                ++m_nCache;
            } else {
                ATH_MSG_DEBUG(" Collection ID = " << idHash.second.getString() << " does not exist, create it ");
                collection = std::make_unique<MdtCsm>(idHash.second, idHash.first);
                ++m_nNotCache;
            }

            // Set values (keep Identifier and IdentifierHash the same though)
            if (collection) collection->set_values(collection->identify(), collection->identifyHash(), cabling_data.subdetectorId, cabling_data.mrod, cabling_data.csm);

            wordPos += 1;
            if (wordPos >= size) {
                ATH_MSG_DEBUG("Error: data corrupted");
                return StatusCode::FAILURE;
            }
            csmReadOut.decodeWord(vint[wordPos]);
            if (!csmReadOut.is_TLP()) { ATH_MSG_DEBUG("Error: TDC Link Present not found "); }

            // Loop on the TDCs blocks, if present
            wordPos += 1;
            if (wordPos >= size) {
                ATH_MSG_DEBUG("Error: data corrupted");
                return StatusCode::FAILURE;
            }
            cabling_data.stationIndex == m_BMGid ? hptdcReadOut.decodeWord(vint[wordPos]) : amtReadOut.decodeWord(vint[wordPos]);
            csmReadOut.decodeWord(vint[wordPos]);
            while (!csmReadOut.is_TWC()) {
                uint16_t tdcNum = cabling_data.stationIndex == m_BMGid ? hptdcReadOut.tdcId() : amtReadOut.tdcId();

                ATH_MSG_DEBUG(" Decoding data from TDC number : " << tdcNum);

                // increase word count by one
                wordPos += 1;
                if (wordPos >= size) {
                    ATH_MSG_DEBUG("Error: data corrupted");
                    return StatusCode::FAILURE;
                }

                cabling_data.stationIndex == m_BMGid ? hptdcReadOut.decodeWord(vint[wordPos]) : amtReadOut.decodeWord(vint[wordPos]);

                MdtAmtHit* amtHit;
                // Loop on the TDC data words and create the corresponding
                // RDO's
                while (!((cabling_data.stationIndex == m_BMGid ? hptdcReadOut.is_EOT() : amtReadOut.is_EOT()) ||
                         (cabling_data.stationIndex == m_BMGid ? hptdcReadOut.is_BOT() : amtReadOut.is_BOT()) ||
                         (cabling_data.stationIndex == m_BMGid ? csmReadOut.is_TWC() : amtReadOut.is_TWC()))) {
                    cabling_data.stationIndex == m_BMGid ? hptdcReadOut.decodeWord(vint[wordPos]) : amtReadOut.decodeWord(vint[wordPos]);
                    int tdcCounts;
                    uint16_t chanNum;
                    // decode the tdc channel number

                    // Check whether this channel has already been
                    // created for this CSM

                    if ((cabling_data.stationIndex == m_BMGid ? hptdcReadOut.is_TSM() : amtReadOut.is_TSM()) &&
                        (cabling_data.stationIndex == m_BMGid ? hptdcReadOut.isLeading() : amtReadOut.isLeading()) && collection) {
                        chanNum = cabling_data.stationIndex == m_BMGid ? hptdcReadOut.channel() : amtReadOut.channel();

                        amtHit = new MdtAmtHit(tdcNum, chanNum);
                        amtHit->setValues((cabling_data.stationIndex == m_BMGid ? hptdcReadOut.coarse() : amtReadOut.coarse()),
                                          (cabling_data.stationIndex == m_BMGid ? hptdcReadOut.fine() : amtReadOut.fine()), 0);
                        amtHit->addData(vint[wordPos]);
                        std::pair<leading_amt_map::iterator, bool> ins = leadingHitMap.insert(leading_amt_map::value_type(chanNum, amtHit));
                        if (!ins.second) {
                            ATH_MSG_DEBUG("not including leading " << *amtHit);
                            ATH_MSG_DEBUG("on the same channel is already decoded the " << *amtHit);
                            delete amtHit;
                        }
                        // leadingHitMap[chanNum] = amtHit;
                    } else if ((cabling_data.stationIndex == m_BMGid ? hptdcReadOut.is_TSM() : amtReadOut.is_TSM()) &&
                               !(cabling_data.stationIndex == m_BMGid ? hptdcReadOut.isLeading() : amtReadOut.isLeading()) && collection) {
                        chanNum = cabling_data.stationIndex == m_BMGid ? hptdcReadOut.channel() : amtReadOut.channel();
                        leading_amt_map::iterator chanPosition = leadingHitMap.find(chanNum);

                        if (chanPosition != leadingHitMap.end()) {
                            // get the amt hit corresponding to the first data word
                            amtHit = leadingHitMap[chanNum];
                            uint16_t coarse = amtHit->coarse();
                            uint16_t fine = amtHit->fine();
                            int tdcCountsFirst = coarse * 32 + fine;

                            // get the tdc counts of the current data word
                            tdcCounts = cabling_data.stationIndex == m_BMGid ? hptdcReadOut.coarse() * 32 + hptdcReadOut.fine()
                                                               : amtReadOut.coarse() * 32 + amtReadOut.fine();
                            int width = tdcCounts - tdcCountsFirst;

                            amtHit->setValues(coarse, fine, width);
                            amtHit->addData(vint[wordPos]);
                            collection->push_back(amtHit);

                            // delete the channel from the map
                            leadingHitMap.erase(chanPosition);
                        } else {
                            ATH_MSG_DEBUG("Error: corresponding leading edge not found for the trailing edge tdc: " << tdcNum << " chan: "
                                                                                                                    << chanNum);
                        }

                    } else if ((cabling_data.stationIndex == m_BMGid ? hptdcReadOut.is_TCM() : amtReadOut.is_TCM()) && collection) {
                        uint16_t chanNum = cabling_data.stationIndex == m_BMGid ? hptdcReadOut.channel() : amtReadOut.channel();

                        amtHit = new MdtAmtHit(tdcNum, chanNum);
                        amtHit->setValues((cabling_data.stationIndex == m_BMGid ? hptdcReadOut.coarse() : amtReadOut.coarse()),
                                          (cabling_data.stationIndex == m_BMGid ? hptdcReadOut.fine() : amtReadOut.fine()),
                                          (cabling_data.stationIndex == m_BMGid ? hptdcReadOut.width() : amtReadOut.width()));
                        amtHit->addData(vint[wordPos]);
                        collection->push_back(amtHit);
                    }

                    // increase word count by one
                    wordPos += 1;
                    if (wordPos >= size) {
                        ATH_MSG_DEBUG("Error: data corrupted");
                        return StatusCode::FAILURE;
                    }

                    cabling_data.stationIndex == m_BMGid ? hptdcReadOut.decodeWord(vint[wordPos]) : amtReadOut.decodeWord(vint[wordPos]);

                    if (cabling_data.stationIndex == m_BMGid) csmReadOut.decodeWord(vint[wordPos]);

                }  // End of loop on AMTs

                while (leadingHitMap.begin() != leadingHitMap.end()) {
                    leading_amt_map::iterator itHit = leadingHitMap.begin();
                    // for ( ; itHit != leadingHitMap.end() ; ) {
                    if (collection)
                        collection->push_back((*itHit).second);
                    else
                        delete (*itHit).second;
                    leadingHitMap.erase(itHit);
                }

                // increase the decoding position only if it's end of TDC
                // i.e. not operating in TDC trailer suppression mode
                if ((cabling_data.stationIndex == m_BMGid ? hptdcReadOut.is_EOT() : amtReadOut.is_EOT())) {
                    wordPos += 1;
                    if (wordPos >= size) {
                        ATH_MSG_ERROR("Error: data corrupted");
                        return StatusCode::FAILURE;
                    }
                }
                csmReadOut.decodeWord(vint[wordPos]);
                cabling_data.stationIndex == m_BMGid ? hptdcReadOut.decodeWord(vint[wordPos]) : amtReadOut.decodeWord(vint[wordPos]);
            }  // End of loop on TDCs
            if (collection) ATH_CHECK(lock.addOrDelete(std::move(collection)));
            // Collection has been found, go out
            // return;
        }  // Check for the chamber offline id = collection offline id
        wordPos += 1;
        if (wordPos >= size) {
            ATH_MSG_ERROR("Data corrupted");
            return StatusCode::FAILURE;
        }
        csmReadOut.decodeWord(vint[wordPos]);
    }

    // clear the renmant trailing edge from the map
    // leading_amt_map::iterator itHit;
    while (leadingHitMap.begin() != leadingHitMap.end()) {
        leading_amt_map::iterator itHit = leadingHitMap.begin();
        // for ( ; itHit != leadingHitMap.end() ;) {
        delete (*itHit).second;
        leadingHitMap.erase(itHit);
    }

    return StatusCode::SUCCESS;
}

std::pair<IdentifierHash, Identifier> MdtROD_Decoder::getHash(Identifier ident) const {
    // get hash from identifier.
    IdentifierHash idHash;
    Identifier regid;
    if (m_BMEpresent) {
        regid = m_idHelperSvc->mdtIdHelper().channelID(
            m_idHelperSvc->mdtIdHelper().stationName(ident), m_idHelperSvc->mdtIdHelper().stationEta(ident),
            m_idHelperSvc->mdtIdHelper().stationPhi(ident), m_idHelperSvc->mdtIdHelper().multilayer(ident), 1, 1);
        m_idHelperSvc->mdtIdHelper().get_detectorElement_hash(regid, idHash);
    } else {
        regid = ident;
        m_idHelperSvc->mdtIdHelper().get_module_hash(regid, idHash);
    }
    return std::make_pair(idHash, regid);
}
