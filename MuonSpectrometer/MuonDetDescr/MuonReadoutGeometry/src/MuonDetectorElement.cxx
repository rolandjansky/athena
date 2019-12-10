/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 ----------------------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/MuonDetectorElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

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
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", m_msgSvc);
  if (sc.isFailure()) std::cout << "Fail to locate Message Service" << std::endl;
  m_Log = std::make_unique<MsgStream>(m_msgSvc, "MuonDetectorElement");
}


MuonDetectorElement::~MuonDetectorElement()
{
}
    
}
