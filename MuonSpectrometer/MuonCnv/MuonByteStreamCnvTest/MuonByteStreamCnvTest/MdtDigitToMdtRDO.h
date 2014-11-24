/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTDIGITTOMDTRDO_H
#define MDTDIGITTOMDTRDO_H

#include <vector>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/DataHandle.h"
#include "Identifier/Identifier.h"

#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/MdtCsm.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

class ActiveStoreSvc;
class MsgStream;
class ITagInfoMgr;

/////////////////////////////////////////////////////////////////////////////

class MdtDigitToMdtRDO : public Algorithm {

 public:

  MdtDigitToMdtRDO (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  StatusCode fill_MDTdata() const;

  StatusCode fillTagInfo() const;

 protected:

  ActiveStoreSvc*      m_activeStore;
  ServiceHandle<StoreGateSvc>  m_EvtStore;

  MuonMDT_CablingSvc*       m_cabling;

  MdtCsmContainer *    m_csmContainer;
  
  const MdtIdHelper*   m_mdtIdHelper;

  ITagInfoMgr          * m_tagInfoMgr;   // Tag Info Manager

  MsgStream* m_log;
  bool m_debug;
  bool m_verbose;
  bool m_BMEpresent;

};

#endif

