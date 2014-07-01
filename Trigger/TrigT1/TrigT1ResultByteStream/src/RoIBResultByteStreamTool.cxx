/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ServiceHandle.h"

// eformat include(s):
#include "eformat/SourceIdentifier.h"

// Trigger include(s):
#include "TrigT1Result/RoIBResult.h"

// Local include(s):
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.h"

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IRoIBResultByteStreamTool( "RoIBResultByteStreamTool", 2, 0 );

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID& RoIBResultByteStreamTool::interfaceID() {
  return IID_IRoIBResultByteStreamTool;
}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
RoIBResultByteStreamTool::RoIBResultByteStreamTool( const std::string& type, const std::string& name,
                                                    const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface< RoIBResultByteStreamTool >( this );

  // set defailt RoI ROB module IDs
  m_ctpModuleID = 1;

  m_muCTPIModuleID = 1;

  m_jetModuleID.reserve(2);
  m_jetModuleID.push_back(0xac);
  m_jetModuleID.push_back(0xad);

  m_emModuleID.reserve(4);
  m_emModuleID.push_back(0xa8) ;
  m_emModuleID.push_back(0xa9) ;
  m_emModuleID.push_back(0xaa) ;
  m_emModuleID.push_back(0xab) ;
}

/**
 * The destructor doesn't do anything.
 */
RoIBResultByteStreamTool::~RoIBResultByteStreamTool() {

}

/**
 * The function initialises the base class.
 */
StatusCode RoIBResultByteStreamTool::initialize() {

  //
  // Initialise the base class:
  //
  StatusCode sc = AlgTool::initialize();
  if( sc.isFailure() ) {
    return sc;
  }

  MsgStream log( msgSvc(), name() );

  //
  // Get ByteStreamAddressProviderSvc:
  //
  ServiceHandle< IProperty > p_ByteStreamAddressProviderSvc( "ByteStreamAddressProviderSvc", name() );
  sc = p_ByteStreamAddressProviderSvc.retrieve();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Can't get ByteStreamAddressProviderSvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to ByteStreamAddressProviderSvc" << endreq;

    UnsignedIntegerProperty ctpModuleID;
    ctpModuleID.setName("CTPModuleID");
    if (ctpModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("CTPModuleID"))) {
      m_ctpModuleID = ctpModuleID.value() ;
      log << MSG::DEBUG << " ---> getProperty('CTPModuleID')             = " << ctpModuleID << endreq;
    } else {
      log << MSG::WARNING << " RoIBResultByteStreamTool::getProperty('CTPModuleID') failed." << endreq;
    }

    UnsignedIntegerProperty muCTPIModuleID;
    muCTPIModuleID.setName("MuCTPIModuleID");
    if (muCTPIModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("MuCTPIModuleID"))) {
      m_muCTPIModuleID = muCTPIModuleID.value() ;
      log << MSG::DEBUG << " ---> getProperty('MuCTPIModuleID')          = " << muCTPIModuleID << endreq;
    } else {
      log << MSG::WARNING << " RoIBResultByteStreamTool::getProperty('MuCTPIModuleID') failed." << endreq;
    }

    UnsignedIntegerArrayProperty jetProcModuleID;
    jetProcModuleID.setName("JetProcModuleID");
    if (jetProcModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("JetProcModuleID"))) {
      m_jetModuleID = jetProcModuleID.value() ;
      log << MSG::DEBUG << " ---> getProperty('JetProcModuleID')         = " << jetProcModuleID << endreq;
    } else {
      log << MSG::WARNING << " RoIBResultByteStreamTool::getProperty('JetProcModuleID') failed." << endreq;
    }

    UnsignedIntegerArrayProperty caloClusterProcModuleID;
    caloClusterProcModuleID.setName("CaloClusterProcModuleID");
    if (caloClusterProcModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("CaloClusterProcModuleID"))) {
      m_emModuleID = caloClusterProcModuleID.value() ;
      log << MSG::DEBUG << " ---> getProperty('CaloClusterProcModuleID') = " << caloClusterProcModuleID << endreq;
    } else {
      log << MSG::WARNING << " RoIBResultByteStreamTool::getProperty('CaloClusterProcModuleID') failed." << endreq;
    }
  }

  log<<MSG::DEBUG << " -- Module IDs for: "<<endreq;
  log<<MSG::DEBUG << "    CTP                                  = 0x" <<MSG::hex<< m_ctpModuleID <<MSG::dec<<endreq;
  log<<MSG::DEBUG << "    muCTPi                               = 0x" <<MSG::hex<< m_muCTPIModuleID <<MSG::dec<<endreq;
  log<<MSG::DEBUG << "    Calorimeter Cluster Processor RoI    = 0x" <<MSG::hex<< m_emModuleID[0]
     << ", 0x" << m_emModuleID[1] << ", 0x" << m_emModuleID[2] << ", 0x" << m_emModuleID[3] <<MSG::dec<<endreq;
  log<<MSG::DEBUG << "    Calorimeter Jet/Energy Processor RoI = 0x" <<MSG::hex<< m_jetModuleID[0]
     << ", 0x" << m_jetModuleID[1] <<MSG::dec<<endreq;

  return StatusCode::SUCCESS;
}

/**
 * The function finalises the base class.
 */
StatusCode RoIBResultByteStreamTool::finalize() {

  return AlgTool::finalize();
}

/**
 * Conversion from RDO to eformat::ROBFragment.
 * This is called from the RoIBResultByteStreamCnv::createRep method.
 */
StatusCode RoIBResultByteStreamTool::convert( ROIB::RoIBResult* result, RawEventWrite* re ) {

  MsgStream log( msgSvc(), name() );

  // Clear the event assembler:
  m_fea.clear();
  // Reset lumi-block number to 1
  m_fea.setDetEvtType( 1 );

  log << MSG::DEBUG << "executing convert() from RDO to ROBFragments" << endreq;

  // I use only this one pointer now to fill the RODs:
  FullEventAssembler< L1SrcIdMap >::RODDATA* theROD;

  /** CTP ROD */
  log << MSG::VERBOSE << "   Dumping CTP RoI words:" << endreq;
  eformat::helper::SourceIdentifier helpCTPID( eformat::TDAQ_CTP, m_ctpModuleID );
  uint32_t rodIdCTP = helpCTPID.code();
  theROD = m_fea.getRodData( rodIdCTP );
  const std::vector< ROIB::CTPRoI >& CTPRoIVec = result->cTPResult().roIVec();
  for( std::vector< ROIB::CTPRoI >::const_iterator j = CTPRoIVec.begin(); j != CTPRoIVec.end(); ++j ) {
    log << MSG::VERBOSE << "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() << endreq;
    theROD->push_back( j->roIWord() );
  }

  /** Muon ROD */
  log << MSG::VERBOSE << "   Dumping MuCTPI RoI words:" << endreq;
  eformat::helper::SourceIdentifier helpMuonID( eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID );
  uint32_t rodIdMuon = helpMuonID.code();
  theROD = m_fea.getRodData( rodIdMuon );
  const std::vector< ROIB::MuCTPIRoI >& MuCTPIRoIVec = result->muCTPIResult().roIVec();
  for( std::vector< ROIB::MuCTPIRoI >::const_iterator j = MuCTPIRoIVec.begin();
       j != MuCTPIRoIVec.end(); ++j ) {
    log << MSG::VERBOSE << "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() << endreq;
    theROD->push_back( j->roIWord() );
  }

  /** Jet/Energy ROD */
  const std::vector< ROIB::JetEnergyResult >& jetEnergyResultVector = result->jetEnergyResult();
  for( unsigned int slink = 0; slink < jetEnergyResultVector.size(); ++slink ) {
    log << MSG::VERBOSE << "   Dumping Jet/Energy " << slink << " RoI words:" << endreq;
    eformat::helper::SourceIdentifier helpJetID( eformat::TDAQ_CALO_JET_PROC_ROI, m_jetModuleID[slink] );
    theROD = m_fea.getRodData( helpJetID.code() );
    const std::vector< ROIB::JetEnergyRoI >& JetEnergyRoIVec = jetEnergyResultVector[ slink ].roIVec();
    for( std::vector< ROIB::JetEnergyRoI >::const_iterator j = JetEnergyRoIVec.begin();
         j != JetEnergyRoIVec.end(); ++j ) {
      log << MSG::VERBOSE << "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() << endreq;
      theROD->push_back( j->roIWord() );
    }
  }

  /* EMTau RODs */
  const std::vector< ROIB::EMTauResult >& eMTauRDOResultVector = result->eMTauResult();
  for( unsigned int slink = 0; slink < eMTauRDOResultVector.size(); ++slink ) {
    log << MSG::VERBOSE << "   Dumping EM/Tau " << slink << " RoI words:" << endreq;
    eformat::helper::SourceIdentifier helpEMTauID( eformat::TDAQ_CALO_CLUSTER_PROC_ROI, m_emModuleID[slink]);
    theROD = m_fea.getRodData( helpEMTauID.code() );
    const std::vector< ROIB::EMTauRoI >& EMTauRoIVec = eMTauRDOResultVector[ slink ].roIVec();
    for( std::vector<ROIB::EMTauRoI>::const_iterator j = EMTauRoIVec.begin();
         j != EMTauRoIVec.end(); ++j ) {
      log << MSG::VERBOSE << "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() << endreq;
      theROD->push_back( j->roIWord() );
    }
  }

  /** now fill them into the RawEvent re */
  log << MSG::DEBUG << "Now filling the event with the RoI Builder fragments" << endreq;
  m_fea.fill( re, log );

  return StatusCode::SUCCESS;
}
