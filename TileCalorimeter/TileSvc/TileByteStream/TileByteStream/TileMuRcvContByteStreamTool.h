/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuRcvContByteStreamTool.h
// Author   : Joao Gentil Saraiva (jmendes@cern.ch)
// Created  : October 2014
//
// DESCRIPTION
//    AlgTool class to provide conversion from TileMuRcvContainer to ByteStream
//    and fill it in RawEvent
//
// BUGS:
//
// History:
//
//****************************************************************************

#ifndef TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMTOOL_H
#define TILEBYTESTREAM_TILEMURCVCONTBYTESTREAMTOOL_H

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "TileEvent/TileMuonReceiverObj.h"
#include "TileEvent/TileMuonReceiverContainer.h"
#include "TileByteStream/TileHid2RESrcID.h"

class TileHWID;

#include <string>

/**
 * @class TileMuRcvContByteStreamTool
 * @brief This AlgTool class provides conversion from TileMuonReceiverContainer to ByteStream and fill it in RawEvent
 * @author Joao Gentil Saraiva
 **/

class TileMuRcvContByteStreamTool: public AthAlgTool {

 public:

  /** Constructor */
  TileMuRcvContByteStreamTool( const std::string& type, const std::string& name, const IInterface* parent );

  /** Destructor */
  virtual ~TileMuRcvContByteStreamTool();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID( );

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** Provides conversion from TileMuRcvContainer to bytestream */
  StatusCode convert(TileMuonReceiverContainer* cont, FullEventAssembler<TileHid2RESrcID> *fea);

 private:

  const TileHWID* m_tileHWID;

  const TileHid2RESrcID* m_hid2re;

  //bool m_verbose;
};

#endif
