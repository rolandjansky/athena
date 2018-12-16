/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOL_H
#define MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
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

/** @class MDT_RawDataProviderTool       
    @author  Edward Moyse <Edward.Moyse@cern.ch>
*/  

class MDT_RawDataProviderTool : virtual public IMuonRawDataProviderTool, virtual public AthAlgTool
{
  public:    
  MDT_RawDataProviderTool(const std::string&,const std::string&,const IInterface*);

  /** default destructor */
  virtual ~MDT_RawDataProviderTool ();
    
  /** standard Athena-Algorithm method */
  virtual StatusCode initialize();
    
  /** standard Athena-Algorithm method */
  virtual StatusCode finalize  ();
  
  /** Convert method - declared in Muon::IMuonRdoToPrepDataTool*/
  virtual StatusCode convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs);
  virtual StatusCode convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, const std::vector<IdentifierHash>&);
  /** the new ones */
  virtual StatusCode convert(); //!< for the entire event 
  virtual StatusCode convert(const std::vector<IdentifierHash>& HashVec);
  virtual StatusCode convert(const std::vector<uint32_t>& robIds);//!< for a particular vector of ROBId's
  private:
 
  //MdtCsmContainer*                  m_rdoContainer; //!< MDT RDO container. Will be created by this tool.
  //bool                              m_debug; //!< If true, output debug info.
  std::set<uint32_t>                m_robIdSet;
  //unsigned int                      m_lastLvl1ID;
  ToolHandle<MdtROD_Decoder>        m_decoder; 
  SG::WriteHandleKey<MdtCsmContainer>   m_rdoContainerKey{
	this, "RdoLocation", "MDTCSM", "Name of the MDTCSM produced by RawDataProvider"};
  const MuonGM::MuonDetectorManager* m_muonMgr;    
  unsigned int m_maxhashtoUse;
  bool m_useContainer;
  // Rob Data Provider handle 
  ServiceHandle<IROBDataProviderSvc>          m_robDataProvider;

    SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

  /// This is the key for the cache for the CSM containers, can be empty
  SG::UpdateHandleKey<MdtCsm_Cache> m_rdoContainerCacheKey ;


};
}

#endif
