/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTTOOL_H
#define TILEBYTESTREAM_TILERAWCHANNELCONTRAWEVENTTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

#include "TileByteStream/TileHid2RESrcID.h"

class TileHWID;
class TileRawChannelContainer;
class TileFastRawChannel;
class TileCondToolEmscale;
class ITileBadChanTool;

#include <string>

/**
 * @class TileRawChannelContByteStreamTool
 * @brief AlgTool class to provide conversion from TileRawChannelContainer to ByteStream,
 * and fill it in RawEvent. <p>
 * @author Hong Ma
 * @version  Created, Sept 25, 2002 <p>
 *  requirements:   typedef for CONTAINER class method: <p> 
 *   StatusCode convert(CONTAINER* cont, RawEvent* re); 
 */

class TileRawChannelContByteStreamTool: public AthAlgTool {
  public:

    typedef TileRawChannelContainer CONTAINER;

    /** constructor
     */
    TileRawChannelContByteStreamTool(const std::string& type, const std::string& name,
        const IInterface* parent);

    /** destructor
     */
    virtual ~TileRawChannelContByteStreamTool();

    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Provides conversion from TileRawChannelContainer to BS
     */
    StatusCode convert(CONTAINER* cont, FullEventAssembler<TileHid2RESrcID> *fea);

  private:

    const TileHWID* m_tileHWID;

    const TileHid2RESrcID* m_hid2re;

    bool m_verbose;
    bool m_doFragType4;
    bool m_doFragType5;

    /** Handle to Tile calibration tool */
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;

    /** Handle to Tile bad channel tool */
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;

    /** internal array for all TileRawChannels */
    TileFastRawChannel* m_channels;

    /** maximum number of channels in a drawer */
    int m_maxChannels;
};

#endif
