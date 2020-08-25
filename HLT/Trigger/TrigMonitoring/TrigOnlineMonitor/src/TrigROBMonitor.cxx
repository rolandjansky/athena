/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOnlineMonitor/TrigROBMonitor.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/Timeout.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "AthenaMonitoringKernel/OHLockedHist.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "eformat/eformat.h"
#include "eformat/index.h"
#include <string>
#include <cassert>
#include <bitset>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>

/////////////////////////////////////////////////////////////////////////////

TrigROBMonitor::TrigROBMonitor(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),
  m_hist_failedChecksumForROB(0),
  m_histProp_failedChecksumForROB(Gaudi::Histo1DDef("FailedChecksumForROB" ,0,1,1)),
  m_hist_failedChecksumForSD(0),
  m_histProp_failedChecksumForSD(Gaudi::Histo1DDef("FailedChecksumForSubDetector" ,0,60,60)),
  m_hist_genericStatusForROB(0),
  m_hist_specificStatusForROB(0),
  m_hist_totalDataVolumeROB(0),
  m_histProp_totalDataVolumeROB(Gaudi::Histo1DDef("totalDataVolumeForROBs" ,0.,5000.,50)),
  m_hist_dataVolumeFractionForSD(0)
{
  // Declare the properties
  declareProperty("SetDebugStream",           m_setDebugStream=false);
  declareProperty("DebugStreamName",          m_debugStreamName="RobErrorStream");
  declareProperty("TestROBChecksum",          m_doROBChecksum=true);
  declareProperty("HistFailedChecksumForROB", m_histProp_failedChecksumForROB,"ROBs with inconsistent checksum");
  declareProperty("HistFailedChecksumForSD",  m_histProp_failedChecksumForSD, "SDs with inconsistent checksum");
  declareProperty("TestROBStatus",            m_doROBStatus=true);
  declareProperty("PlotROBDataVolume",        m_doROBDataVolume=true);
  declareProperty("HistTotalDataVolumeROB",   m_histProp_totalDataVolumeROB,  "Total data volume in ROBs");

  // fill map with generic status codes
  m_map_GenericStatus[eformat::UNCLASSIFIED]      = "UNCLASSIFIED";
  m_map_GenericStatus[eformat::BCID_CHECK_FAIL]   = "BCID_CHECK_FAIL";
  m_map_GenericStatus[eformat::LVL1ID_CHECK_FAIL] = "LVL1ID_CHECK_FAIL";
  m_map_GenericStatus[eformat::TIMEOUT]           = "TIMEOUT";
  m_map_GenericStatus[eformat::DATA_CORRUPTION]   = "DATA_CORRUPTION";
  m_map_GenericStatus[eformat::INTERNAL_OVERFLOW] = "INTERNAL_OVERFLOW";

  // fill vector with specific status codes
  m_vec_SpecificStatus.reserve(16);
  m_vec_SpecificStatus.push_back("TRIGGER_TYPE_SYNC_ERROR");
  m_vec_SpecificStatus.push_back("FRAGMENT_SIZE_ERROR");
  m_vec_SpecificStatus.push_back("DATABLOCK_ERROR");
  m_vec_SpecificStatus.push_back("CTRL_WORD_ERROR");
  m_vec_SpecificStatus.push_back("MISSING_BOF");
  m_vec_SpecificStatus.push_back("MISSING_EOF");
  m_vec_SpecificStatus.push_back("INVALID_HEADER_MARKER");
  m_vec_SpecificStatus.push_back("FORMAT_ERROR");
  m_vec_SpecificStatus.push_back("DUPLICATE_EVENT");
  m_vec_SpecificStatus.push_back("SEQUENCE_ERROR");
  m_vec_SpecificStatus.push_back("TRANSMISSION_ERROR");
  m_vec_SpecificStatus.push_back("TRUNCATION");
  m_vec_SpecificStatus.push_back("SHORT_FRAGMENT");
  m_vec_SpecificStatus.push_back("FRAGMENT_LOST");
  m_vec_SpecificStatus.push_back("FRAGMENT_PENDING");
  m_vec_SpecificStatus.push_back("ROL_DISABLED");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigROBMonitor::initialize(){

  // Get the messaging service
  ATH_MSG_INFO( "initialize()" );

  // Print out the property values
  ATH_MSG_INFO( " Put events with ROB errors on DEBUG stream = " << m_setDebugStream );
  ATH_MSG_INFO( "         Name of used DEBUG stream          = " << m_debugStreamName );
  ATH_MSG_INFO( " Do ROB checksum test                       = " << m_doROBChecksum );
  ATH_MSG_INFO( "        Hist:FailedChecksumForROB           = " << m_histProp_failedChecksumForROB );
  ATH_MSG_INFO( "        Hist:FailedChecksumForSD            = " << m_histProp_failedChecksumForSD );
  ATH_MSG_INFO( " Do ROB status test                         = " << m_doROBStatus );
  ATH_MSG_INFO( " Plot ROB data volumes                      = " << m_doROBDataVolume );
  ATH_MSG_INFO( "        Hist:TotalDataVolumeROB             = " << m_histProp_totalDataVolumeROB );

  // Locate the ROBDataProviderSvc
  ATH_CHECK( m_robDataProviderSvc.retrieve() );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigROBMonitor::execute() {

  ATH_MSG_DEBUG( "execute()" );

  //--------------------------------------------------------------------------
  // check that there is still time left
  //--------------------------------------------------------------------------
  if (Athena::Timeout::instance().reached()) {
    ATH_MSG_INFO( " Time out reached in entry to execute." );
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  // Loop over all ROB fragments held in the ROBDataProviderSvc and do the checks
  //--------------------------------------------------------------------------
  bool event_with_checksum_failure(false);
  int  total_ROB_Data_Volume(0);
  std::map< eformat::SubDetector, uint32_t > rob_Data_Volume_SD; 

  if (m_robDataProviderSvc->getEvent() != 0) {
    // get total fragment size
    total_ROB_Data_Volume = (m_robDataProviderSvc->getEvent())->fragment_size_word();
    rob_Data_Volume_SD[ eformat::helper::SourceIdentifier((m_robDataProviderSvc->getEvent())->source_id()).subdetector_id() ] 
      = (m_robDataProviderSvc->getEvent())->fragment_size_word();

    // build an index of all ROB fragments in the event 
    std::map<uint32_t, const uint32_t*> fullEventIndex;
    eformat::helper::build_toc( *(m_robDataProviderSvc->getEvent()), fullEventIndex);
    ATH_MSG_DEBUG( " ===> Standard ROBDataProviderSvc is used : # ROB fragments = " 
						 << fullEventIndex.size() 
						 );
    for (std::map<uint32_t, const uint32_t*>::iterator it = fullEventIndex.begin();
	 it != fullEventIndex.end(); ++it) {
      // check for time out
      if (Athena::Timeout::instance().reached()) {
        ATH_MSG_INFO( " Time out reached in loop over ROB fragments." );
	return StatusCode::SUCCESS;
      }
      // verify checksum
      if (verifyROBChecksum((*it).second )) event_with_checksum_failure=true ;

      // verify status bits
      verifyROBStatusBits((*it).second );

      // get ROB fragment sizes
      OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment tmpROB( (*it).second );
      rob_Data_Volume_SD[ eformat::helper::SourceIdentifier( (*it).first ).subdetector_id() ] += tmpROB.fragment_size_word();
    }
  } else {
      ATH_MSG_WARNING(
	" No check on ROB fragments was performed, since neither the ROB cache in L2 could be accessed nor the full event could be found." 
	);
  } 

  // if the event shows errors, set the DEBUG stream tag when requested
  if ((m_setDebugStream.value()) && (event_with_checksum_failure)) {
    // get EventInfo
    const EventInfo* p_EventInfo(0);
    StatusCode sc = evtStore()->retrieve(p_EventInfo);
    if(sc.isFailure()){
      ATH_MSG_ERROR( "Can't get EventInfo object for updating the StreamTag" );
      return sc;
    }

    // set the stream tag
    typedef std::vector< TriggerInfo::StreamTag > StreamTagVector_t;
    if (p_EventInfo) {
      StreamTagVector_t vecStreamTags = p_EventInfo->trigger_info()->streamTags();
      vecStreamTags.push_back( TriggerInfo::StreamTag(m_debugStreamName,"debug",false) );
      // FIXME: const_cast
      const_cast<TriggerInfo*>(p_EventInfo->trigger_info())->setStreamTags(vecStreamTags);
    }
  }

  // fill data volume plots
  if (m_hist_totalDataVolumeROB) {
    m_hist_totalDataVolumeROB->Fill(total_ROB_Data_Volume/256,1); // convert #words to kByte
  }

  if (m_hist_dataVolumeFractionForSD) {
    for (std::map< eformat::SubDetector, uint32_t >::const_iterator it = rob_Data_Volume_SD.begin(); it != rob_Data_Volume_SD.end(); ++it) {
      if(total_ROB_Data_Volume)
      {
        float ratio = ((float)(*it).second)/((float)total_ROB_Data_Volume) ; 
        m_hist_dataVolumeFractionForSD->Fill(eformat::helper::SourceIdentifier( (*it).first,0 ).human_detector().c_str(), ratio ,1.);
      }
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigROBMonitor::finalize() {

  // Get the messaging service
  ATH_MSG_INFO( "finalize()" );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigROBMonitor::start() {

  // Define histograms only when checks are requested
  if ((not m_doROBChecksum.value()) && (not m_doROBStatus.value()) &&
      (not m_doROBDataVolume.value())) return StatusCode::SUCCESS;

  // find histogramming service
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
  if ((rootHistSvc.retrieve()).isFailure()) {
    ATH_MSG_ERROR( "Unable to locate THistSvc" );
    rootHistSvc.release().ignore();
    return StatusCode::FAILURE;
  }

  // *-- booking path
  std::string path = std::string("/EXPERT/");

  // *-- number of bins for sub detector plots (55 SubDet max.)
  uint32_t n_bins_partEBSubDet = 55 ;

  if ( m_doROBChecksum.value() ) {
    // *-- ROBs with failed checksum
    m_hist_failedChecksumForROB = new TH1F (m_histProp_failedChecksumForROB.value().title().c_str(),
					    (m_histProp_failedChecksumForROB.value().title()+";ROB id").c_str(),
					    m_histProp_failedChecksumForROB.value().bins(),
					    m_histProp_failedChecksumForROB.value().lowEdge(),
					    m_histProp_failedChecksumForROB.value().highEdge());
    if (m_hist_failedChecksumForROB) {
      m_hist_failedChecksumForROB->SetCanExtend(TH1::kAllAxes);
      if( rootHistSvc->regHist(path + m_hist_failedChecksumForROB->GetName(), m_hist_failedChecksumForROB).isFailure() ) {
	ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_failedChecksumForROB->GetName() );
      }
    }

    // *-- Sub detectors with ROBs with failed checksum
    m_hist_failedChecksumForSD = new TH1F (m_histProp_failedChecksumForSD.value().title().c_str(),
					    (m_histProp_failedChecksumForSD.value().title()+";SD id").c_str(),
					    m_histProp_failedChecksumForSD.value().bins(),
					    m_histProp_failedChecksumForSD.value().lowEdge(),
					    m_histProp_failedChecksumForSD.value().highEdge());
    if (m_hist_failedChecksumForSD) {
      uint32_t n_tmp_bin = 1;
      for (uint16_t i=0; i<256; i++) {
	eformat::helper::SourceIdentifier tmpsrc( (eformat::SubDetector) i,0);
	if ((tmpsrc.human_detector() != "UNKNOWN") && (n_tmp_bin <= (uint32_t) m_histProp_failedChecksumForSD.value().bins())) {
	  m_hist_failedChecksumForSD->GetXaxis()->SetBinLabel( n_tmp_bin, tmpsrc.human_detector().c_str() );
	  n_tmp_bin++;
	}
      }

      if( rootHistSvc->regHist(path + m_hist_failedChecksumForSD->GetName(), m_hist_failedChecksumForSD).isFailure() ) {
	ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_failedChecksumForSD->GetName() );
      }
    }
  }

  if ( m_doROBStatus.value() ) {
    // *-- Generic Status for ROBs per sub detector
    m_hist_genericStatusForROB = new TH2F ("GenericStatusForROBsFromSubDetectors",
					   "GenericStatusForROBsFromSubDetectors;;",
					   n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,
					   m_map_GenericStatus.size(),0., (float) m_map_GenericStatus.size());
    if (m_hist_genericStatusForROB) {
      uint32_t n_tmp_bin = 1;
      for (uint16_t i=0; i<256; i++) {
	eformat::helper::SourceIdentifier tmpsrc( (eformat::SubDetector) i,0);
	if ((tmpsrc.human_detector() != "UNKNOWN") && (n_tmp_bin <= n_bins_partEBSubDet)) {
	  m_hist_genericStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, tmpsrc.human_detector().c_str() );
	  n_tmp_bin++;
	}
      }

      n_tmp_bin = 1;
      for (std::map<eformat::GenericStatus, std::string>::const_iterator it = m_map_GenericStatus.begin();it != m_map_GenericStatus.end();++it) {
	m_hist_genericStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).second.c_str() );
	n_tmp_bin++;
      }

      if( rootHistSvc->regHist(path + m_hist_genericStatusForROB->GetName(), m_hist_genericStatusForROB).isFailure() ) {
	ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_genericStatusForROB->GetName() );
      }
    }

    // *-- Specific Status Bits for ROBs per sub detector
    m_hist_specificStatusForROB = new TH2F ("SpecificStatusBitsForROBsFromSubDetectors",
					    "SpecificStatusBitsForROBsFromSubDetectors;;",
					    n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,
					    m_vec_SpecificStatus.size(),0., (float) m_vec_SpecificStatus.size());
    if (m_hist_specificStatusForROB) {
      uint32_t n_tmp_bin = 1;
      for (uint16_t i=0; i<256; i++) {
	eformat::helper::SourceIdentifier tmpsrc( (eformat::SubDetector) i,0);
	if ((tmpsrc.human_detector() != "UNKNOWN") && (n_tmp_bin <= n_bins_partEBSubDet)) {
	  m_hist_specificStatusForROB->GetXaxis()->SetBinLabel( n_tmp_bin, tmpsrc.human_detector().c_str() );
	  n_tmp_bin++;
	}
      }

      n_tmp_bin = 1;
      for (std::vector<std::string>::const_iterator it = m_vec_SpecificStatus.begin();it != m_vec_SpecificStatus.end();++it) {
	m_hist_specificStatusForROB->GetYaxis()->SetBinLabel( n_tmp_bin, (*it).c_str() );
	n_tmp_bin++;
      }

      if( rootHistSvc->regHist(path + m_hist_specificStatusForROB->GetName(), m_hist_specificStatusForROB).isFailure() ) {
	ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_specificStatusForROB->GetName() );
      }
    }
  }

  if ( m_doROBDataVolume.value() ) {
    // *-- Total data volume retrieved in ROBs
    m_hist_totalDataVolumeROB = new TH1F (m_histProp_totalDataVolumeROB.value().title().c_str(),
					  (m_histProp_totalDataVolumeROB.value().title()+";kBytes").c_str(),
					  m_histProp_totalDataVolumeROB.value().bins(),
					  m_histProp_totalDataVolumeROB.value().lowEdge(),
					  m_histProp_totalDataVolumeROB.value().highEdge());
    if (m_hist_totalDataVolumeROB) {
      m_hist_totalDataVolumeROB->SetCanExtend(TH1::kAllAxes);
      if( rootHistSvc->regHist(path + m_hist_totalDataVolumeROB->GetName(), m_hist_totalDataVolumeROB).isFailure() ) {
	ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_totalDataVolumeROB->GetName() );
      }
    }

    // *-- Data volume fraction per sub detector
    m_hist_dataVolumeFractionForSD = new TProfile("DataVolumeFractionForSD",
						  "Fraction of data volume per SD;;Fraction of total Volume",
						  n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet,0.,1.);
    if (m_hist_dataVolumeFractionForSD) {
      uint32_t n_tmp_bin = 1;
      for (uint16_t i=0; i<256; i++) {
	eformat::helper::SourceIdentifier tmpsrc( (eformat::SubDetector) i,0);
	if ((tmpsrc.human_detector() != "UNKNOWN") && (n_tmp_bin <= n_bins_partEBSubDet)) {
	  m_hist_dataVolumeFractionForSD->GetXaxis()->SetBinLabel( n_tmp_bin, tmpsrc.human_detector().c_str() );
	  n_tmp_bin++;
	}
      }

      if( rootHistSvc->regHist(path + m_hist_dataVolumeFractionForSD->GetName(), m_hist_dataVolumeFractionForSD).isFailure() ) {
	ATH_MSG_WARNING( "Can not register monitoring histogram: " << m_hist_dataVolumeFractionForSD->GetName() );
      }
    }
  } // end m_doROBDataVolume.value()

  // release histogramming service
  rootHistSvc.release().ignore();

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
bool TrigROBMonitor::verifyROBChecksum(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {

  bool failed_checksum(false);
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType it(0); 
  uint32_t current_value(0);

  // print check for received ROB
  if (msg().level() <= MSG::VERBOSE) {
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    ATH_MSG_VERBOSE( 
           " ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum: type = "       << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = " << std::setw(12) << current_value
	<< " check = "                << std::setw(2)  << robFrag.checksum()
	);
  }

  // checksum test failed
  if ( not robFrag.checksum() ) {
    failed_checksum = true;

    // recalculate checksum value
    robFrag.payload(it);
    current_value = eformat::helper::checksum(robFrag.checksum_type(), it, robFrag.payload_size_word());

    // print warning
    ATH_MSG_WARNING( 
      " ROB checksum verification failed." 
	<< " ROB id = 0x"             << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec 
	<< " checksum type = "        << std::setw(2)  << robFrag.checksum_type()
	<< " value = "                << std::setw(12) << robFrag.checksum_value()
	<< " value (recalculated) = " << std::setw(12) << current_value
	<< " check = "                << std::setw(2)  << robFrag.checksum()
	);

    // fill the histograms
    std::ostringstream ost;
    ost << "0x" << std::hex << robFrag.source_id();
    if (m_hist_failedChecksumForROB) {
      oh_scoped_lock_histogram lock;
      m_hist_failedChecksumForROB->Fill((ost.str()).c_str(), 1.);
      m_hist_failedChecksumForROB->LabelsDeflate("X");
    }

    if (m_hist_failedChecksumForSD) m_hist_failedChecksumForSD->Fill(eformat::helper::SourceIdentifier(robFrag.source_id()).human_detector().c_str(),1.);
  }

  return failed_checksum;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void TrigROBMonitor::verifyROBStatusBits(OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robFrag) {

  // print check for received ROB
  ATH_MSG_VERBOSE( " verifyROBStatusBits: ROB id = 0x" << std::setw(6)  << MSG::hex << robFrag.source_id() << MSG::dec );

  // fill monitoring histogram for ROB generic status
  if ( ( m_hist_genericStatusForROB ) && ( robFrag.nstatus() != 0 ) ) {
    const uint32_t* it_status;
    robFrag.status(it_status);
    if ((*it_status) != 0) m_hist_genericStatusForROB->Fill(eformat::helper::SourceIdentifier(robFrag.source_id()).human_detector().c_str(),
							    m_map_GenericStatus[eformat::helper::Status(*it_status).generic()].c_str(),1.);
  }

  // fill monitoring histogram for ROB specific status
  if ( ( m_hist_specificStatusForROB ) && ( robFrag.nstatus() != 0 ) ) {
    const uint32_t* it_status;
    robFrag.status(it_status);
    if ((*it_status) != 0) {
      std::bitset<16> specificBits(eformat::helper::Status(*it_status).specific());
      for (unsigned int index=0; index < 16; ++index) {
	if (specificBits[index]) m_hist_specificStatusForROB->Fill(eformat::helper::SourceIdentifier(robFrag.source_id()).human_detector().c_str(),
								   m_vec_SpecificStatus[index].c_str(),1.);
      }
    }
  }
}
