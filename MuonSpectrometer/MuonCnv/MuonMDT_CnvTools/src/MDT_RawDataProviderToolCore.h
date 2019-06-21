/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOLCORE_H
#define MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOLCORE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MdtROD_Decoder.h"
#include <vector>
#include <set>
#include <string>

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonRDO/MdtCsm_Cache.h"


class MdtCsmContainer;
class StoreGateSvc;
class IROBDataProviderSvc;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon 
{

/** @class MDT_RawDataProviderToolCore       
    @author  Edward Moyse <Edward.Moyse@cern.ch>
    @author  Mark Owen <markowen@cern.ch>
*/  

class MDT_RawDataProviderToolCore : public AthAlgTool
{
  public:    
  MDT_RawDataProviderToolCore(const std::string&,const std::string&,const IInterface*);

  /** default destructor */
  virtual ~MDT_RawDataProviderToolCore ();
    
  /** standard Athena-Algorithm method */
  virtual StatusCode initialize();
    
  /** standard Athena-Algorithm method */
  virtual StatusCode finalize  ();
  
  /** Convert method */
    virtual StatusCode convertIntoContainer( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, MdtCsmContainer& mdtContainer);

  protected:

  ToolHandle<MdtROD_Decoder>        m_decoder; 
  SG::WriteHandleKey<MdtCsmContainer>   m_rdoContainerKey{
	this, "RdoLocation", "MDTCSM", "Name of the MDTCSM produced by RawDataProvider"};
  const MuonGM::MuonDetectorManager* m_muonMgr;    
  unsigned int m_maxhashtoUse;

  // Rob Data Provider handle 
  ServiceHandle<IROBDataProviderSvc>          m_robDataProvider;

  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};


};
}

#endif
