/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 ----------------------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/MuonDetectorElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "GaudiKernel/MsgStream.h"

namespace MuonGM {

MuonDetectorElement::MuonDetectorElement(GeoVFullPhysVol* pv, 
                                         MuonDetectorManager* mgr,
                                         Identifier id,
                                         IdentifierHash idHash)
  : TrkDetElementBase(pv), m_debug(0), m_verbose(0)
{
  m_muon_mgr = mgr;
  m_id = id;
  m_idhash = idHash;
  m_nREinDetectorElement = 0;
}


MuonDetectorElement::~MuonDetectorElement()
{
    //    if (m_MsgStream) delete m_MsgStream;
    //  m_MsgStream=0;  
}
    
}
