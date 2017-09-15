/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//author Renato Febbraro
//renato.febbraro@cern.ch
//date February 2008

#ifndef TILELASEROBJ_BYTESTREAMTOOL_H
#define TILELASEROBJ_BYTESTREAMTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "TileByteStream/TileHid2RESrcID.h"

class TileHWID;
class TileLaserObject;

#include <string>

class TileLaserObjByteStreamTool: public AthAlgTool {

  public:

    /** constructor
     */
    TileLaserObjByteStreamTool(const std::string& type, const std::string& name,
        const IInterface* parent);

    /** destructor
     */
    virtual ~TileLaserObjByteStreamTool();

    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Provides conversion from TileLaserObject to BS
     */
    StatusCode convertLaser(TileLaserObject* cont, FullEventAssembler<TileHid2RESrcID> *fea);

  private:

    const TileHWID* m_tileHWID;
    const TileHid2RESrcID* m_hid2re;
    bool m_verbose;
};

#endif
