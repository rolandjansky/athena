/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEROD_DECODER_H
#define TILEBYTESTREAM_TILEROD_DECODER_H

#include "AthenaKernel/MsgStreamMember.h"

#include "TileIdentifier/TileFragHash.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileEvent/TileRawDataOrdering.h" 

#include "TileByteStream/TileRawChannel2Bytes5.h" 
#include "TileByteStream/TileRawChannel2Bytes4.h" 
#include "TileByteStream/TileRawChannel2Bytes2.h" 
#include "TileByteStream/TileRawChannel2Bytes.h"
#include "TileByteStream/TileDigits2Bytes.h"

class TileFastRawChannel;
class TileDigits;
class TileL2;

#include <vector>
#include <stdint.h>

/** 
 * 
 * @class TileROD_Encoder
 * @brief Provides conversion from TileRawChannel and TileL2 to ROD format. 
 * @author A. Solodkov
 * @version  0-0-1 , Oct 17, 2002
 
 * Modified, Jan 20, 2003
 Split from TileROD_Decoder. 
 */

class TileROD_Encoder {

  public:

    /** constructor 
     */
    TileROD_Encoder()
        : m_tileHWID(0)
        , m_verbose(false)
        , m_type(0)
        , m_unitType(0)
        , m_msg("TileROD_Encoder")
    {
    }

    /** set all necessary parameters for the encoder
     */
    void setTileHWID(const TileHWID* tileHWID, bool verbose, unsigned int type = 4);

    /** set OF algorigtm type and amplitude units for a drawer
     */
    void setTypeAndUnit(TileFragHash::TYPE type, TileRawChannelUnit::UNIT unit);

    /** destructor 
     */
    ~TileROD_Encoder() { }

    /** add TileRawChannels to the current list 
     */
    void add(const TileFastRawChannel* rc) { m_vTileRC.push_back(rc); }

    /** add TileL2s to the current list 
     */
    void addL2(const TileL2* l2) { m_vTileL2.push_back(l2); }

    /** add TileDigits to the current list
     */
    void addDigi(const TileDigits* digi) { m_vTileDigi.push_back(digi); }

    /** convert all TileRawChannels in the current list to 
     a vector of 32bit words
     */

    void fillROD(std::vector<uint32_t>& v);
    void fillROD2(std::vector<uint32_t>& v);
    void fillROD3(std::vector<uint32_t>& v);
    void fillROD4(std::vector<uint32_t>& v);
    void fillROD5(std::vector<uint32_t>& v);

    /** convert all TileL2s in the current list to 
     a vector of 32bit words
     */

    void fillROD10(std::vector<uint32_t>& v);
    void fillROD12(std::vector<uint32_t>& v);
    void fillRODL2(std::vector<uint32_t>& v);

    /** convert all TileDigits in the current list to
     a vector of 32bit words
     */

    void fillROD1(std::vector<uint32_t>& v);
    void fillROD5D(std::vector<uint32_t>& v);

    /** dump contents of the ROD fragment
     */
    void dumpROD(const std::vector<uint32_t>& v);

    // Log a message using the Athena controlled logging system.
    MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl; }

    // Check whether the logging system is active at the provided verbosity level
    bool msgLvl(MSG::Level lvl) { return m_msg.get().level() <= lvl; }


  private:

    /** set the bitmap for a channel
     */
    void setBit(uint32_t* p, int chan);

    /** check the bitmap for a channel
     */
    bool checkBit(const uint32_t* p, int chan);

    std::vector<const TileFastRawChannel*> m_vTileRC;
    std::vector<const TileL2*> m_vTileL2;
    std::vector<const TileDigits*> m_vTileDigi;

    TileDigits2Bytes m_Digi2bytes;
    TileRawChannel2Bytes2 m_rc2bytes2;
    TileRawChannel2Bytes m_rc2bytes3;
    TileRawChannel2Bytes4 m_rc2bytes4;
    TileRawChannel2Bytes5 m_rc2bytes5;

    const TileHWID* m_tileHWID;

    // sort channels according to Channel HWID
    TileRawDataOrdering m_order;

    bool m_verbose;
    unsigned int m_type;
    unsigned int m_unitType;

    mutable Athena::MsgStreamMember m_msg;
};

#endif
