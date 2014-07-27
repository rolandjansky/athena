/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//author Renato Febbraro
//renato.febbraro@cern.ch
//date February 2008

// Gaudi includes
#include "GaudiKernel/ToolFactory.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileByteStream/TileLaserObjByteStreamTool.h"
#include "TileByteStream/TileROD_Encoder.h"
#include "TileEvent/TileLaserObject.h"
#include "TileIdentifier/TileHWID.h"

#include "CLIDSvc/CLASS_DEF.h"



static const InterfaceID IID_ITileLaserObjByteStreamTool("TileLaserObjByteStreamTool", 1, 0);

const InterfaceID& TileLaserObjByteStreamTool::interfaceID() {
  return IID_ITileLaserObjByteStreamTool;
}


// default constructor
 TileLaserObjByteStreamTool::TileLaserObjByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent)
  , m_verbose(false)
{
  declareInterface< TileLaserObjByteStreamTool  >( this );
}



// destructor 
TileLaserObjByteStreamTool::~TileLaserObjByteStreamTool() {
}



StatusCode TileLaserObjByteStreamTool::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  m_hid2re.setTileHWID(m_tileHWID);
  m_fea.idMap().setTileHWID(m_tileHWID);
  return StatusCode::SUCCESS;
}

StatusCode TileLaserObjByteStreamTool::finalize() {

  return StatusCode::SUCCESS;
}



