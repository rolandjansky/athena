/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscRdoContByteStreamTool.h"
#include "CscROD_Encoder.h"

#include "MuonRDO/CscRawDataContainer.h"

#include "AthenaKernel/CLASS_DEF.h"

#include <map> 

// contructor
Muon::CscRdoContByteStreamTool::CscRdoContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type,name,parent) 
{
  declareInterface<Muon::ICSC_RDOtoByteStreamTool>(this);
  declareProperty("EventType", m_eventType=0x0);
  declareProperty("RODVersion", m_rodVersion=0x0200);
  declareProperty("IsCosmicData", m_isCosmic=false);
  declareProperty("IsOldCosmicData", m_isOldCosmic=false);
}

// initialize 
StatusCode Muon::CscRdoContByteStreamTool::initialize()
{
  ATH_CHECK(AthAlgTool::initialize()); 
  ATH_MSG_DEBUG ("Muon::CscRdoContByteStreamTool::initialize()");  

  // get the cabling service
  ATH_CHECK(serviceLocator()->service("CSCcablingSvc", m_cabling));
  ATH_CHECK(m_idHelperSvc.retrieve());
  
  // create CSC RDO ID to source ID mapper
  m_hid2re.set(m_cabling, &m_idHelperSvc->cscIdHelper());
  if ( m_isCosmic ) { 
    m_hid2re.set_isCosmic();
    if ( m_isOldCosmic ) m_hid2re.set_isOldCosmic();
  }
  return StatusCode::SUCCESS;
}

// convert CSC RDO to ByteStream
StatusCode Muon::CscRdoContByteStreamTool::convert(const CONTAINER* cont, RawEventWrite* re, 
					     MsgStream& log)
{
  m_fea.clear();
  m_fea.idMap().set(m_cabling, &m_idHelperSvc->cscIdHelper());
  if(m_cabling->nROD()==16) m_fea.setRodMinorVersion(0x400);
  else m_fea.setRodMinorVersion(m_rodVersion);

  StatusCode sc = StatusCode::SUCCESS;

  FullEventAssembler<CSC_Hid2RESrcID>::RODDATA * theROD;
 
  // a map for ROD ID onto Encoder
  std::map<uint32_t, CscROD_Encoder> mapEncoder; 

  /** set the iterator over the RDO */
  CscRawDataContainer::const_iterator it_col     = cont->begin(); 
  CscRawDataContainer::const_iterator it_col_end = cont->end(); 

  bool first = true;
  for(; it_col != it_col_end; ++it_col)
    {

      /** set the detector event type word */
      if ( first && (*it_col)->size() > 0 ) {
         first = false;
         m_eventType = (*it_col)->eventType();
         m_fea.setDetEvtType(m_eventType);
      }

      // get ROD ID
      const uint32_t rodId = m_hid2re.getRodID(*it_col); 

      // map the RDO onto Encoder
      mapEncoder[rodId].setRdo(*it_col);
      mapEncoder[rodId].setIdHelper(&m_idHelperSvc->cscIdHelper());
    } 

  // loop over map and fill all ROD Data Blocks
  std::map<uint32_t,CscROD_Encoder>::iterator it_map     = mapEncoder.begin(); 
  std::map<uint32_t,CscROD_Encoder>::iterator it_map_end = mapEncoder.end();
  for (; it_map != it_map_end; ++it_map)
    { 
      // get ROD data address
      theROD = m_fea.getRodData((*it_map).first); 

      // fill ROD data
      sc = ((*it_map).second).fillROD( *theROD, log );
      if ( sc.isFailure() ) return StatusCode::RECOVERABLE; 
    } 

  // Finnally, fill full event
  m_fea.fill(re,log); 

  return sc; 
}






