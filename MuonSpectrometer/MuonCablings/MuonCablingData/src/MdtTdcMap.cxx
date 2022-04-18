/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtTdcMap.h"

#include "MuonCablingData/MdtMezzanineType.h"
#include "MuonIdHelpers/MdtIdHelper.h"

// constructor
MdtTdcMap::MdtTdcMap(const MdtMezzanineType* mezType, const MdtCablingData& cabling_data, const MdtIdHelper* helper, MsgStream& ext_log) :
    m_statId{cabling_data}, m_mezType(mezType->type()), m_mdtIdHelper(helper) {
    bool debug = (ext_log.level() <= MSG::VERBOSE);

    m_chanToLayer.fill(std::make_pair(NOTSET, NOTSET));
    if (debug) { ext_log << MSG::VERBOSE << "Adding Tdc number: " << (int)moduleId() << endmsg; }

    // call the function that initializes the actual map
    if (initMap(mezType, cabling_data.channelId, cabling_data.layer, cabling_data.tube, ext_log)) {
        if (debug) {
            ext_log << MSG::VERBOSE << "Map initialized successfully for tdc number: " << MSG::hex << (int)moduleId() << MSG::dec << endmsg;
        }
    } else {
        if (debug) { ext_log << MSG::VERBOSE << "Could not initialize completely the TDC map" << endmsg; }
    }
}

/** initialize the channel-to-tube map */
/** channel, layer, tube, contain the channel and position of the first tube */
bool MdtTdcMap::initMap(const MdtMezzanineType* mezType, uint8_t chanZero, int layerZero, int tubeZero, MsgStream& log) {
    bool successful = true;
    bool debug = (log.level() <= MSG::VERBOSE);
    if (debug) {
        log << MSG::VERBOSE << "Initializing the map, the mezzanine type is " << (int)m_mezType << " with "
            << (int)mezType->nOfTubesInLayer() << " tubes per layer and " << (int)mezType->nOfLayers() << " layers" << endmsg;
    }
    for (uint8_t chan = 0; chan < 24; ++chan) {
        const uint8_t tube_chan = ((chan - chan % 4) / 4);
        int layer = 0;
        int tube = 0;

        // special case of the BME of end 2013 and BMG 2017
        /*
         * cases 50 and 60 follow the same rules. hedgehog card 50 is the mirror image of card 60,
         * but the rules concerning how to decode the tube mapping are the same.
         * channel 0 of a mezzanine card for case 50 is either top right or bottom left,
         * while for case 60 it is top left or bottom right.
         * Another thing to keep in mind for BMG is, that tube counting in some cases is along |z|,
         * while in some other cases it is opposite.
         *
         *  E.g: Numbering from top to bottom -> Tube counting along z
         *
         *       Layer |         Tube number
         *       4     |  0     4      8    12     16   20
         *       3     |  1     5      9    13     17   21
         *       2     |  2     6     10    14     18   22
         *       1     |  3     7     11    15     19   23
         */
        if (m_mezType == 50 || m_mezType == 60 || m_mezType == 61) {
            /// Tube numbering is from bottom to top
            if (layerZero == 1) {
                layer = chan % 4 + 1;
            }
            // Tube numbering is from top to bottom
            else {
                layer = 4 - chan % 4;
            }
            if ((tubeZero - 1) % 6 == 0) {
                tube = tubeZero + tube_chan;
            } else {
                tube = tubeZero - tube_chan;
            }
        }
        /*  Few mezzanine cards in the BIS78 chambers are collateral vicitms of the famous
         *  regional customs happening in the Bavarian autumun. The tube staggering is kind
         *  of discontinous where the upper two layers are swapped with the bottom two
         *    1     5      9     13     17      21
         *    0     4      8     12     16      20
         *    3     7     11     15     19      23
         *    2     6     10     14     18      22
         */
        else if (m_mezType == 71) {
            layer = (chan % 4);
            layer += 2 * (layer > 1 ? -1 : +1) + 1;
            tube = tubeZero - tube_chan;
        } else {
            // calculate the layer
            if (layerZero == 1) {
                layer = chan / (mezType->nOfTubesInLayer()) + 1;
            } else {
                layer = layerZero - chan / (mezType->nOfTubesInLayer());
            }

            MdtLayer chanOrder = mezType->getLayer(layer);

            //    if (m_debug) {
            //      *m_log << MSG::DEBUG << "Channel order : ";
            //      for (int i=0 ; i< chanOrder.size() ; ++i) {
            //	*m_log << MSG::DEBUG << (int) chanOrder[i];
            //      }
            //      *m_log << MSG::DEBUG << endmsg;
            //    }

            // calculate the tube
            uint8_t localchan = chan % (mezType->nOfTubesInLayer());

            // special case of the BIR with 3 tubes overlapping to another mezzanine
            if (chanZero == 5 && stationName() == 7 && std::abs(stationEta()) == 3 &&
                ((mezType->type() > 40 && localchan % 2 == 0) || (mezType->type() < 40 && localchan < 3)))
                continue;

            //    *m_log << "the zero channel is: " << (int) chanZero
            //	   << " and the local channel is: " << (int) localchan << endmsg;
            tube = chanOrder[localchan] - chanOrder[chanZero] + tubeZero;
        }

        bool setChan = setChannel(chan, layer, tube, log);

        if (!setChan) {
            log << MSG::ERROR << "Could not set the channel " << (int)chan << endmsg;
            successful = false;
        }
    }

    return successful;
}

/** setting the single channel in the map */
bool MdtTdcMap::setChannel(uint8_t channel, int n_layer, int n_tube, MsgStream& log) {
    bool channelSet = false;

    /** set the channel if not already set */
    if (layer(channel) == NOTSET && tube(channel) == NOTSET) {
        /** Comment momentarily the output channel by channel (to be uncommented for debugging...) */
        //
        if (log.level() < MSG::VERBOSE) {
            log << MSG::VERBOSE << "Adding channel: " << (int)channel << " connected to tube: " << (int)n_tube << " layer: " << (int)n_layer
                << endmsg;
        }

        // do the validity check only for the BIR (some mezzanines are exceeding the tube range)
        //
        if (stationName() == 7) {
            Identifier id = m_mdtIdHelper->channelID(stationName(), stationEta(), stationPhi(), multiLayer(), n_layer, n_tube);

            if (n_tube > m_mdtIdHelper->tubeMax(id) || n_tube < m_mdtIdHelper->tubeMin(id)) {
                bool debug = (log.level() <= MSG::VERBOSE);
                if (debug) { log << MSG::VERBOSE << "The tube is invalid (as expected) ! not adding it..." << endmsg; }

                return true;
            }
        }
        m_minTube = std::min(m_minTube, n_tube);
        m_maxTube = std::max(m_maxTube, n_tube);

        m_minLayer = std::min(m_minLayer, n_layer);
        m_maxLayer = std::max(m_maxLayer, n_layer);

        m_LayTubeToChan[std::make_pair(n_layer, n_tube)] = channel;
        m_chanToLayer[channel] = std::make_pair(n_layer, n_tube);

        channelSet = true;
    }
    /** otherwise return an error */
    else {
        log << MSG::ERROR << "Error in TDC number " << int(moduleId()) << endmsg;
        log << MSG::ERROR << "The channel: " << int(channel) << " has been already assigned to tube: " << int(tube(channel))
            << " layer: " << int(layer(channel)) << endmsg;

        channelSet = false;
    }

    return channelSet;
}

// get the offlineId
bool MdtTdcMap::offlineId(MdtCablingData& cabling_map, MsgStream& log) const {
    if (cabling_map.channelId >= CHANMAX) {
        log << MSG::WARNING << "Error in tdc " << MSG::hex << (int)moduleId() << MSG::dec << " channel: " << MSG::hex
            << (int)cabling_map.channelId << MSG::dec << " not defined!!" << endmsg;
        return false;
    }

    const std::pair<int, int>& layerTube = m_chanToLayer[cabling_map.channelId];

    if (layerTube.first == NOTSET || layerTube.second == NOTSET) {
        if (log.level() <= MSG::VERBOSE) {
            log << MSG::WARNING << offId() << " error in tdc " << MSG::hex << int(moduleId()) << MSG::dec << ". channel: " << MSG::hex
                << int(cabling_map.channelId) << MSG::dec << " not found" << endmsg;
        }
        return false;
    }
    cabling_map.MdtCablingOffData::operator=(offId());
    cabling_map.layer = layerTube.first;
    cabling_map.tube = layerTube.second;
    return true;
}
bool MdtTdcMap::onlineId(MdtCablingData& cabling_data, MsgStream& log) const {
    if (offId() != cabling_data) {
        log << MSG::WARNING << "The cabling data " << cabling_data << " does not share the same offline id as the Tdc " << offId() << ". "
            << endmsg;
        return false;
    }
    const bool debug = log.level() <= MSG::VERBOSE;
    if (debug) {
        log << MSG::VERBOSE << "Try to match " << cabling_data << " with Tdc tubeZero: " << int(tubeZero())
            << " layerZero: " << int(layerZero()) << " minTube:  " << int(minTube()) << " maxtube: " << int(maxTube())
            << " maxLayer: " << int(maxLayer()) << " minLayer: " << int(minLayer()) << endmsg;
    }
    if (cabling_data.tube > maxTube() || cabling_data.tube < minTube()) {
        if (debug) { log << MSG::VERBOSE << "The requested tube is out of range of " << minTube() << " -- " << maxTube() << endmsg; }
        return false;
    }
    if (cabling_data.layer > maxLayer() || cabling_data.layer < minLayer()) {
        if (debug) { log << MSG::VERBOSE << "The requested layer is out of range of " << minLayer() << " -- " << maxLayer() << endmsg; }
        return false;
    }
    auto chan_itr = m_LayTubeToChan.find(std::make_pair(cabling_data.layer, cabling_data.tube));
    if (chan_itr != m_LayTubeToChan.end()) {
        cabling_data.channelId = chan_itr->second;
        cabling_data.MdtCablingOnData::operator=(m_statId);
        cabling_data.tdcId = moduleId();
        return true;
    }
    if (debug) { log << MSG::VERBOSE << "The layer & tube are unknown" << endmsg; }
    return false;
}
