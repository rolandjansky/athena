/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "L1TopoRDO/Helpers.h"

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
  : AthAlgTool( type, name, parent ) {

  declareInterface< RoIBResultByteStreamTool >( this );

  // set default RoI ROB module IDs
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

  // default for L1Topo is an empty list
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

  //
  // Get ByteStreamAddressProviderSvc:
  //
  ServiceHandle< IProperty > p_ByteStreamAddressProviderSvc( "ByteStreamAddressProviderSvc", name() );
  sc = p_ByteStreamAddressProviderSvc.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get ByteStreamAddressProviderSvc");
    return sc;
  } else {
    ATH_MSG_DEBUG("Connected to " << p_ByteStreamAddressProviderSvc.name());

    UnsignedIntegerProperty ctpModuleID;
    ctpModuleID.setName("CTPModuleID");
    if (ctpModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("CTPModuleID"))) {
      m_ctpModuleID = ctpModuleID.value() ;
      ATH_MSG_DEBUG(" ---> getProperty('CTPModuleID')             = " << ctpModuleID);
    } else {
      ATH_MSG_WARNING(p_ByteStreamAddressProviderSvc.name() << "::getProperty('CTPModuleID') failed.");
    }

    UnsignedIntegerProperty muCTPIModuleID;
    muCTPIModuleID.setName("MuCTPIModuleID");
    if (muCTPIModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("MuCTPIModuleID"))) {
      m_muCTPIModuleID = muCTPIModuleID.value() ;
      ATH_MSG_DEBUG(" ---> getProperty('MuCTPIModuleID')          = " << muCTPIModuleID);
    } else {
      ATH_MSG_WARNING(p_ByteStreamAddressProviderSvc.name() << "::getProperty('MuCTPIModuleID') failed.");
    }

    UnsignedIntegerArrayProperty jetProcModuleID;
    jetProcModuleID.setName("JetProcModuleID");
    if (jetProcModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("JetProcModuleID"))) {
      m_jetModuleID = jetProcModuleID.value() ;
      ATH_MSG_DEBUG(" ---> getProperty('JetProcModuleID')         = " << jetProcModuleID);
    } else {
      ATH_MSG_WARNING(p_ByteStreamAddressProviderSvc.name() << "::getProperty('JetProcModuleID') failed.");
    }

    UnsignedIntegerArrayProperty caloClusterProcModuleID;
    caloClusterProcModuleID.setName("CaloClusterProcModuleID");
    if (caloClusterProcModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("CaloClusterProcModuleID"))) {
      m_emModuleID = caloClusterProcModuleID.value() ;
      ATH_MSG_DEBUG(" ---> getProperty('CaloClusterProcModuleID') = " << caloClusterProcModuleID);
    } else {
      ATH_MSG_WARNING(p_ByteStreamAddressProviderSvc.name() << "::getProperty('CaloClusterProcModuleID') failed.");
    }

    UnsignedIntegerArrayProperty l1TopoModuleID;
    l1TopoModuleID.setName("TopoProcModuleID");
    if (l1TopoModuleID.assign(p_ByteStreamAddressProviderSvc->getProperty("TopoProcModuleID"))) {
      m_l1TopoModuleID = l1TopoModuleID.value();
      ATH_MSG_DEBUG(" ---> getProperty('TopoProcModuleID') = " << l1TopoModuleID);
    } else {
      ATH_MSG_WARNING(p_ByteStreamAddressProviderSvc.name() << "::getProperty('TopoProcModuleID') failed.");
    }
  }

  ATH_MSG_DEBUG(" -- Module IDs for: ");
  ATH_MSG_DEBUG("    CTP                                  = 0x" <<MSG::hex<< m_ctpModuleID <<MSG::dec);
  ATH_MSG_DEBUG("    muCTPi                               = 0x" <<MSG::hex<< m_muCTPIModuleID <<MSG::dec);
  ATH_MSG_DEBUG("    Calorimeter Cluster Processor RoI    = 0x" <<MSG::hex<< m_emModuleID[0]
     << ", 0x" << m_emModuleID[1] << ", 0x" << m_emModuleID[2] << ", 0x" << m_emModuleID[3] <<MSG::dec);
  ATH_MSG_DEBUG("    Calorimeter Jet/Energy Processor RoI = 0x" <<MSG::hex<< m_jetModuleID[0]
     << ", 0x" << m_jetModuleID[1] <<MSG::dec);

  //  ATH_MSG_DEBUG("    L1Topo                               = 0x" <<MSG::hex<< m_l1TopoModuleID[0]     << ", 0x" <<  m_l1TopoModuleID[1] <<MSG::dec);
  std::ostringstream topoModulesString;
  for (unsigned int mid: m_l1TopoModuleID){ 
    topoModulesString << L1Topo::formatHex4(mid) << " ";
  }
  ATH_MSG_DEBUG( "    L1Topo                               = " << topoModulesString.str());

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

  // Clear the event assembler:
  m_fea.clear();
  // Reset lumi-block number to 1
  m_fea.setDetEvtType( 1 );

  ATH_MSG_DEBUG("executing convert() from RDO to ROBFragments");

  // I use only this one pointer now to fill the RODs:
  FullEventAssembler< L1SrcIdMap >::RODDATA* theROD;

  /** CTP ROD */
  ATH_MSG_VERBOSE("   Dumping CTP RoI words:");
  eformat::helper::SourceIdentifier helpCTPID( eformat::TDAQ_CTP, m_ctpModuleID );
  uint32_t rodIdCTP = helpCTPID.code();
  theROD = m_fea.getRodData( rodIdCTP );
  const std::vector< ROIB::CTPRoI >& CTPRoIVec = result->cTPResult().roIVec();
  for( std::vector< ROIB::CTPRoI >::const_iterator j = CTPRoIVec.begin(); j != CTPRoIVec.end(); ++j ) {
    ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
    theROD->push_back( j->roIWord() );
  }

  /** Muon ROD */
  ATH_MSG_VERBOSE( "   Dumping MuCTPI RoI words:" );
  eformat::helper::SourceIdentifier helpMuonID( eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID );
  uint32_t rodIdMuon = helpMuonID.code();
  theROD = m_fea.getRodData( rodIdMuon );
  const std::vector< ROIB::MuCTPIRoI >& MuCTPIRoIVec = result->muCTPIResult().roIVec();
  for( std::vector< ROIB::MuCTPIRoI >::const_iterator j = MuCTPIRoIVec.begin();
       j != MuCTPIRoIVec.end(); ++j ) {
    ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
    theROD->push_back( j->roIWord() );
  }

  /** Jet/Energy ROD */
  const std::vector< ROIB::JetEnergyResult >& jetEnergyResultVector = result->jetEnergyResult();
  for( unsigned int slink = 0; slink < jetEnergyResultVector.size(); ++slink ) {
    ATH_MSG_VERBOSE( "   Dumping Jet/Energy " << slink << " RoI words:" );
    eformat::helper::SourceIdentifier helpJetID( eformat::TDAQ_CALO_JET_PROC_ROI, m_jetModuleID[slink] );
    theROD = m_fea.getRodData( helpJetID.code() );
    const std::vector< ROIB::JetEnergyRoI >& JetEnergyRoIVec = jetEnergyResultVector[ slink ].roIVec();
    for( std::vector< ROIB::JetEnergyRoI >::const_iterator j = JetEnergyRoIVec.begin();
         j != JetEnergyRoIVec.end(); ++j ) {
      ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
      theROD->push_back( j->roIWord() );
    }
  }

  /* EMTau RODs */
  const std::vector< ROIB::EMTauResult >& eMTauRDOResultVector = result->eMTauResult();
  for( unsigned int slink = 0; slink < eMTauRDOResultVector.size(); ++slink ) {
    ATH_MSG_VERBOSE( "   Dumping EM/Tau " << slink << " RoI words:" );
    eformat::helper::SourceIdentifier helpEMTauID( eformat::TDAQ_CALO_CLUSTER_PROC_ROI, m_emModuleID[slink]);
    theROD = m_fea.getRodData( helpEMTauID.code() );
    const std::vector< ROIB::EMTauRoI >& EMTauRoIVec = eMTauRDOResultVector[ slink ].roIVec();
    for( std::vector<ROIB::EMTauRoI>::const_iterator j = EMTauRoIVec.begin();
         j != EMTauRoIVec.end(); ++j ) {
      ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
      theROD->push_back( j->roIWord() );
    }
  }

  /** L1Topo ROD */
  ATH_MSG_VERBOSE( "   Dumping L1Topo RoI words" );
  const std::vector< ROIB::L1TopoResult >& l1TopoResultVector = result->l1TopoResult();
  ATH_MSG_VERBOSE( "   Dumping L1Topo RoI words for " << l1TopoResultVector.size() << " RODs:" );
  unsigned int slink(0);
  for (auto & l1tr : l1TopoResultVector){
    uint32_t sourceID = l1tr.rdo().getSourceID(); // needs to be a 16-bit module ID
    ATH_MSG_VERBOSE( "  for L1Topo source ID from RDO "  << L1Topo::formatHex8(sourceID) );
    if (sourceID == 0 && slink < m_l1TopoModuleID.size()){
      sourceID = eformat::helper::SourceIdentifier( eformat::TDAQ_CALO_TOPO_PROC, m_l1TopoModuleID.at(slink) ).code();
      ATH_MSG_DEBUG("  (source ID in L1TopoRDO was zero so using Property for slink " << slink << ": " << L1Topo::formatHex8(sourceID) << ")");
    } 
    else if (sourceID == 0){
      sourceID=eformat::helper::SourceIdentifier( eformat::TDAQ_CALO_TOPO_PROC, 0 ).code();
      ATH_MSG_WARNING("  (source ID in L1TopoRDO was zero, no properties available for slink counter " << slink << ", so as a fall back, constructed module 0 with source ID " << L1Topo::formatHex8(sourceID) << ")");
    }
    theROD = m_fea.getRodData( sourceID );
    for ( auto & word : l1tr.rdo().getDataWords()){
      ATH_MSG_VERBOSE("     " << MSG::hex << std::setw(8) << std::showbase << word << std::noshowbase << std::dec);
      theROD->push_back(word);
    }
    ++slink;
  }
  /** now fill them into the RawEvent re */
  ATH_MSG_DEBUG("Now filling the event with the RoI Builder fragments");
  MsgStream log( msgSvc(), name() );
  m_fea.fill( re, log );

  return StatusCode::SUCCESS;
}
