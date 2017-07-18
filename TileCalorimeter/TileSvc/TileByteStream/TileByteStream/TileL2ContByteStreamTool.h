/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileL2ContByteStreamTool.h
// Author   : Aranzazu Ruiz
// Created  : March 2007
//
// DESCRIPTION
//    AlgTool class to provide conversion from TileL2Container to ByteStream
//    and fill it in RawEvent
//
// BUGS:
//
// History:
//
//****************************************************************************

#ifndef TILEBYTESTREAM_TILEL2CONTBYTESTREAMTOOL_H
#define TILEBYTESTREAM_TILEL2CONTBYTESTREAMTOOL_H

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "TileEvent/TileContainer.h"
#include "TileByteStream/TileHid2RESrcID.h"

class TileHWID;

#include <string>


/**
 * @class TileL2ContByteStreamTool
 * @brief This AlgTool class provides conversion from TileL2Container to ByteStream and fill it in RawEvent
 * @author Aranzazu Ruiz
 *
 * This class provides methods to convert the TileL2 objects into bytestream data.
 * It fills ROD-by-ROD the frag type 0x10 in RawEvent.
 */

class TileL2ContByteStreamTool: public AthAlgTool {

 public:

  /** Constructor */
  TileL2ContByteStreamTool( const std::string& type, const std::string& name, 
                            const IInterface* parent );

  /** Destructor */
  virtual ~TileL2ContByteStreamTool();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID( );

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** Provides conversion from TileL2Container to BS */
  StatusCode convert(TileL2Container* cont, FullEventAssembler<TileHid2RESrcID> *fea);

 private:

  const TileHWID* m_tileHWID;
  const TileHid2RESrcID* m_hid2re;
  bool m_verbose;
  bool m_doFragTypeMu;
  bool m_doFragTypeEt;
};

#endif
