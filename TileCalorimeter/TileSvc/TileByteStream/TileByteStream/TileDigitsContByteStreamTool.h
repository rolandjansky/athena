/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEDIGITS_BYTESTREAMTOOL_H
#define TILEBYTESTREAM_TILEDIGITS_BYTESTREAMTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

#include "TileByteStream/TileHid2RESrcID.h"

class TileHWID;
class TileDigitsContainer;

#include <string>

/**
 * @class TileDigitsContByteStreamTool
 * @brief AlgTool class to provide conversion from TileDigitsContainer
 * to ByteStream, and fill it in RawEvent. <p>
 * @author Hong Ma
 * @version  Created, Sept 25, 2002 <p>
 *  requirements:   typedef for DIGITS class method: <p> 
 *   StatusCode convert(DIGITS* cont, RawEvent* re); 
 */

class TileDigitsContByteStreamTool: public AthAlgTool {
  public:

    typedef TileDigitsContainer DIGITS;

    /** constructor
     */
    TileDigitsContByteStreamTool(const std::string& type, const std::string& name,
        const IInterface* parent);

    /** destructor
     */
    virtual ~TileDigitsContByteStreamTool();

    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Provides conversion from TileDigitsContainer to BS
     */
    StatusCode convert(DIGITS* cont, FullEventAssembler<TileHid2RESrcID> *fea);

  private:

    const TileHWID* m_tileHWID;

    const TileHid2RESrcID* m_hid2re;

    bool m_verbose;
    bool m_doFragType1;
    bool m_doFragType5;
};

#endif
