/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file src/util.cxx
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Implements a few of the utilities in util.h  
 */

#include "util.h"
#include "hltinterface/EventId.h"
#include "eformat/index.h"
#include "eformat/eformat.h"
#include "eformat/write/eformat.h"
#include "owl/time.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGate.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "CTPfragment/CTPdataformatVersion.h"
#include "Event.h" // datacollector implementation
#include "L1_ROBs.h" // L1 RoIB ROB identifiers
#include <Python.h>
#include <cstdlib>
#include <set>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cctype>
#include <execinfo.h>
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;

namespace
{
  class ProcessProxy
  {
  public:
    ProcessProxy(hltinterface::HLTInterface& interface,
                 const eformat::read::FullEventFragment& e,
                 HLTTestApps::TimeoutGuard& watchdog,
                 uint32_t max_result_size);
                 
    /**
     * Call operator. Processes the event
     */
    eformat::helper::u32list operator()();
    
  private:
    hltinterface::HLTInterface& m_interface;
    const eformat::read::FullEventFragment& m_event;
    hltinterface::HLTResult m_hltr;
    HLTTestApps::TimeoutGuard& m_watchdog;
    uint32_t m_max_result_size;
  };

  /**
   * Programatically obtain and print a stack trace.
   */
  void print_stack_trace ()
  {
    static const int TRACE_DEPTH=128;
    void * array[TRACE_DEPTH];
    int nSize = backtrace(array, TRACE_DEPTH);
    char ** symbols = backtrace_symbols(array, nSize);
    std::cerr << "Found " << nSize << " backtrace symbols." << std::endl;
    for (int i = 0; i < nSize; i++) std::cerr << symbols[i] << std::endl;
    free(symbols);
  }

  /*
   * A PEB specification type consisting of a pair with a set of robs and a set
   * of sub-detectors
   */
  using PebSpec = std::pair<std::set<uint32_t>, std::set<eformat::SubDetector>>;

  /*
   * If all tags specify PEB lists, get a pair with a list of ROB IDs and a list
   * of SubDetector IDs, corresponding to the union of the PEB lists specified
   * in all tags. Otherwise (at least one tag specifies FEB), get a pair with
   * two empty sets.
   */
  PebSpec get_peb_list(const std::vector<eformat::helper::StreamTag>& tags)
  {
    std::pair<std::set<uint32_t>, std::set<eformat::SubDetector>> ret;
    for(const auto& t : tags)
    {
      if(!t.robs.empty() || !t.dets.empty())
      {
        copy(begin(t.robs), end(t.robs), inserter(ret.first,
                                                  begin(ret.first)));
        copy(begin(t.dets), end(t.dets), inserter(ret.second,
                                                  begin(ret.second)));
      }
      else // at least one tag requires full event building
      {
        // we clear everything and we're done (empty return signals no PEB)
        ret.first.clear();
        ret.second.clear();
        break;
      }
    }

    return ret;
  }

  /*
   * select robs from src_robs following peb_spec, and put them into selection,
   * filtering result robs or not according to filter_hltrs
   */
  void choose_robs(std::vector<const uint32_t*>& selection,
                   const std::vector<eformat::read::ROBFragment>& src_robs,
                   const PebSpec& peb_spec,
                   bool filter_hltrs)
  {
    // for each rob...
    for(const auto& rob : src_robs)
    {
      auto sid = rob.source_id();
      auto sdid = eformat::helper::SourceIdentifier{sid}.subdetector_id();
      if(!filter_hltrs || // ... if we don't need to filter...
         (sdid != eformat::TDAQ_HLT &&  // ... or if we don't have to filter
          sdid != eformat::TDAQ_LVL2))  // this particular rob...
      {
        auto end_peb_robs = end(peb_spec.first);
        auto end_peb_dets = end(peb_spec.second);
        bool peb_empty = peb_spec.first.empty() && peb_spec.second.empty();
        if(peb_empty || // ... if we need FEB, or if the rob is covered by PEB
           find(begin(peb_spec.first), end_peb_robs, sid) != end_peb_robs ||
           find(begin(peb_spec.second), end_peb_dets, sdid) != end_peb_dets)
        {
          selection.push_back(rob.start()); // then we select it
        }
      }
    }
  }

  /*
   * Choose which ROBs, from both src and hltr_robs, to put into the output
   * event. If FEB is required, all robs from hltr_robs are included in the
   * output, as well as all robs from src, with the exception of HLT results.
   * If PEB is required, only the robs covered by it are included. HLTR robs
   * from src are still not included in this case.
   */
  std::vector<const uint32_t*>
  choose_event_payload(const eformat::read::FullEventFragment& src,
                       const std::vector<eformat::read::ROBFragment>& hltr_robs,
                       const std::vector<eformat::helper::StreamTag>& tags)
  {
    std::vector<const uint32_t*> ret;
    std::vector<eformat::read::ROBFragment> src_robs;
    src.robs(src_robs);
    ret.reserve(src_robs.size() + hltr_robs.size());

    auto peb_spec = get_peb_list(tags);
    choose_robs(ret, src_robs, peb_spec, true);
    choose_robs(ret, hltr_robs, peb_spec, false);

    return ret;
  }

  /*
   * check if L1 simulation was run and get the new L1 trigger info words and the
   * updated L1 ROBs
   */
  bool check_rerun_L1(const eformat::read::FullEventFragment& src_event, 
		      std::vector<uint32_t>& l1_info,
		      std::map<uint32_t,uint32_t* >& l1_robs)
  {
    bool ret(0);
    // get the l1 trigger info from the original event
    unsigned int number_of_words_tav = src_event.nlvl1_trigger_info()/3;
    const uint32_t* buffer;
    src_event.lvl1_trigger_info(buffer);
    std::vector<uint32_t> original_l1_info(buffer, buffer+src_event.nlvl1_trigger_info());
    std::vector<uint32_t> original_l1_info_TBP(buffer,buffer+number_of_words_tav);
    std::vector<uint32_t> original_l1_info_TAV(buffer+2*number_of_words_tav, buffer+src_event.nlvl1_trigger_info());

    // get the l1 trigger info from the RoIB result (remade if L1 is rerun)

    // use only the RoIB object in the transient store, do not try to remake it from persistent store and
    // avoid any data collector call 
    if ( !StoreGate::instance().transientContains<ROIB::RoIBResult>("RoIBResult") ) {
      return ret; // if there is no RoIB object in SG there is nothing to do and rerunLVL1 can not be checked
    }

    // a transient object is available
    const DataHandle<ROIB::RoIBResult> dobj;
    StoreGate::instance().retrieve(dobj,"RoIBResult");
    if (!dobj.isValid()) { 
      return ret; // if there is no RoIB object there is nothing to do
    }

    // check if the CTPResult from RoIB is different from event header (use only the TAV words)
    if (number_of_words_tav != dobj->cTPResult().TAV().size()) { // e.g. run 1 data are used to simulate run 2
      ret = 1;
    } else { // input data and simulation are for the same run period
      for (unsigned int index = 0; index < dobj->cTPResult().TAV().size(); ++index) {
        if (dobj->cTPResult().TBP()[index].roIWord() != original_l1_info_TBP[index]) {ret = 1;}
        if (dobj->cTPResult().TAV()[index].roIWord() != original_l1_info_TAV[index]) {ret = 1;}
      }
    } 

    // the L1 was not re-made, return immediately
    if (!ret) return ret;

    //-------------------------+
    //  L1 decision was remade |
    //-------------------------+

    // get all original L1 ROBs
    std::map<uint32_t, std::vector<const uint32_t*> > original_RoIB_Robs, original_DAQ_Robs; 
    std::vector<eformat::read::ROBFragment> src_robs;
    src_event.robs(src_robs);
    for(const auto& rob : src_robs) {
      auto sid = rob.source_id();
      auto sdid = eformat::helper::SourceIdentifier{sid}.subdetector_id();
      switch (sdid) {
      case eformat::TDAQ_CALO_CLUSTER_PROC_DAQ:  // = 0x72
	original_DAQ_Robs[sdid].push_back(rob.start());
	break;
      case eformat::TDAQ_CALO_CLUSTER_PROC_ROI:  // = 0x73,
	if (find(begin(L1R_ROBS),end(L1R_ROBS),sid) != end(L1R_ROBS))
	  original_RoIB_Robs[sdid].push_back(rob.start());
	break;
      case eformat::TDAQ_CALO_JET_PROC_DAQ: // = 0x74,
	original_DAQ_Robs[sdid].push_back(rob.start());
	break;
      case eformat::TDAQ_CALO_JET_PROC_ROI: // = 0x75,
	if (find(begin(L1R_ROBS),end(L1R_ROBS),sid) != end(L1R_ROBS))
	  original_RoIB_Robs[sdid].push_back(rob.start());
	break;
      case eformat::TDAQ_MUON_CTP_INTERFACE: // = 0x76,
	if (find(begin(L1R_ROBS),end(L1R_ROBS),sid) != end(L1R_ROBS))
	  original_RoIB_Robs[sdid].push_back(rob.start());
	else
	  original_DAQ_Robs[sdid].push_back(rob.start());
	break;
      case eformat::TDAQ_CTP: // = 0x77
	if (find(begin(L1R_ROBS),end(L1R_ROBS),sid) != end(L1R_ROBS))
	  original_RoIB_Robs[sdid].push_back(rob.start());
	else
	  original_DAQ_Robs[sdid].push_back(rob.start());
	break;
      case eformat::TDAQ_CALO_TOPO_PROC: // = 0x91,
	if (find(begin(L1R_ROBS),end(L1R_ROBS),sid) != end(L1R_ROBS))
	  original_RoIB_Robs[sdid].push_back(rob.start());
	else
	  original_DAQ_Robs[sdid].push_back(rob.start());
	break;

      default:
	break;
      }
    }

    // Remake the new L1 trigger info words for the event header
    l1_info.resize(3*dobj->cTPResult().TAV().size(),0);
    for (unsigned i = 0; i < dobj->cTPResult().TAV().size(); ++i) {
      if ( i < dobj->cTPResult().TBP().size() ) l1_info[i] = dobj->cTPResult().TBP()[i].roIWord() ;
      if ( i < dobj->cTPResult().TAP().size() ) l1_info[i+dobj->cTPResult().TAV().size()] = dobj->cTPResult().TAP()[i].roIWord() ;
      if ( i < dobj->cTPResult().TAV().size() ) l1_info[i+2*dobj->cTPResult().TAV().size()] = dobj->cTPResult().TAV()[i].roIWord() ;
    }

    // remake the L1 ROB payload data from the RoIB result
    /** CTP ROD */
    // Default CTP minor version word
    uint16_t minorVersion = 0x0004; // default minor CTP version
    bool minorVersionFromRDO(0);
    if (dobj->cTPResult().TAV().size() == 8) { minorVersion = 0x0003; } // CTP version for Run 1
    // DAQ
    // get the l1 CTP_RDO which was remade
    const DataHandle<CTP_RDO> dobj_ctp_rdo;
    StoreGate::instance().retrieve(dobj_ctp_rdo,"CTP_RDO_Rerun");
    if (dobj_ctp_rdo.isValid()) {
      // calculate CTP minor version word
      CTPdataformatVersion ctpVersion(dobj_ctp_rdo->getCTPVersionNumber());
      // Set L1Apos to center of readout window
      uint16_t l1a = ( dobj_ctp_rdo->getNumberOfBunches() - 1u ) / 2u;
      l1a <<= ctpVersion.getL1APositionShift();
      uint16_t addWords = dobj_ctp_rdo->getNumberOfAdditionalWords();
      addWords <<= ctpVersion.getProgrammableExtraWordsShift();
      uint16_t ctpVer = dobj_ctp_rdo->getCTPVersionNumber();
      ctpVer <<= ctpVersion.getCTPFormatVersionShift();
      minorVersion = addWords + l1a + ctpVer;
      minorVersionFromRDO = 1;
      // payload data
      std::vector<uint32_t> ctpDAQRod;
      ctpDAQRod.reserve(dobj_ctp_rdo->getDataWords().size());
      for(const auto& j: dobj_ctp_rdo->getDataWords()) ctpDAQRod.push_back( j );

      if ( (original_DAQ_Robs.find(eformat::TDAQ_CTP) != original_DAQ_Robs.end()) &&
	   (original_DAQ_Robs[eformat::TDAQ_CTP].size() == 1)) {
	  eformat::write::ROBFragment ctpDAQRob(original_DAQ_Robs[eformat::TDAQ_CTP][0]);
	  ctpDAQRob.rod_minor_version( minorVersion );
	  ctpDAQRob.rod_data(ctpDAQRod.size(),ctpDAQRod.data());
	  l1_robs[ ctpDAQRob.source_id() ] = new uint32_t[ctpDAQRob.size_word()];
	  auto copied = eformat::write::copy(*ctpDAQRob.bind(), l1_robs[ ctpDAQRob.source_id() ], ctpDAQRob.size_word());
	  if(copied == 0 || copied != ctpDAQRob.size_word()) {
	    boost::format msg("Copy failed for DAQ CTP Rob: words copied: %s words expected %s");
	    msg % copied, ctpDAQRob.size_word();
	    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	  }
      }
    }

    // RoIB
    std::vector<uint32_t> ctpRod;
    ctpRod.reserve(dobj->cTPResult().roIVec().size());
    for(const auto& j: dobj->cTPResult().roIVec()) ctpRod.push_back( j.roIWord() );

    if ( (original_RoIB_Robs.find(eformat::TDAQ_CTP) != original_RoIB_Robs.end()) &&
	 (original_RoIB_Robs[eformat::TDAQ_CTP].size() == 1)) {
	eformat::write::ROBFragment ctpRob(original_RoIB_Robs[eformat::TDAQ_CTP][0]);
	ctpRob.rod_data(ctpRod.size(),ctpRod.data());
	if (minorVersionFromRDO) ctpRob.rod_minor_version(minorVersion); // reuse minor version of DAQ CTP ROB
	l1_robs[ ctpRob.source_id() ] = new uint32_t[ctpRob.size_word()];
	auto copied = eformat::write::copy(*ctpRob.bind(), l1_robs[ ctpRob.source_id() ], ctpRob.size_word());
	if(copied == 0 || copied != ctpRob.size_word()) {
	  boost::format msg("Copy failed for RoIB CTP Rob: words copied: %s words expected %s");
	  msg % copied, ctpRob.size_word() ;
	  throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	}
    }

    /** Muon ROD */
    // DAQ
    // get the l1 MuCTP_RDO which was remade
    const DataHandle<MuCTPI_RDO> dobj_muctpi_rdo;
    StoreGate::instance().retrieve(dobj_muctpi_rdo,"MUCTPI_RDO+");
    if (dobj_muctpi_rdo.isValid()) {
      // payload data
      std::vector<uint32_t> muCTPIDAQRod;
      muCTPIDAQRod.reserve(dobj_muctpi_rdo->getAllCandidateMultiplicities().size() + dobj_muctpi_rdo->dataWord().size());
      for(const auto& j: dobj_muctpi_rdo->getAllCandidateMultiplicities()) muCTPIDAQRod.push_back( j );
      for(const auto& j: dobj_muctpi_rdo->dataWord()) muCTPIDAQRod.push_back( j );

      if ( (original_DAQ_Robs.find(eformat::TDAQ_MUON_CTP_INTERFACE) != original_DAQ_Robs.end()) &&
	   (original_DAQ_Robs[eformat::TDAQ_MUON_CTP_INTERFACE].size() == 1)) {
	  eformat::write::ROBFragment muCTPIDAQRob(original_DAQ_Robs[eformat::TDAQ_MUON_CTP_INTERFACE][0]);
	  muCTPIDAQRob.rod_data(muCTPIDAQRod.size(),muCTPIDAQRod.data());
	  l1_robs[ muCTPIDAQRob.source_id() ] = new uint32_t[muCTPIDAQRob.size_word()];
	  auto copied = eformat::write::copy(*muCTPIDAQRob.bind(), l1_robs[ muCTPIDAQRob.source_id() ], muCTPIDAQRob.size_word());
	  if(copied == 0 || copied != muCTPIDAQRob.size_word()) {
	    boost::format msg("Copy failed for DAQ muCTPI Rob: words copied: %s words expected %s");
	    msg % copied, muCTPIDAQRob.size_word();
	    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	  }
      }
    } 
    // RoIB
    std::vector<uint32_t> muCTPIRod;
    muCTPIRod.reserve(dobj->muCTPIResult().roIVec().size());
    for(const auto& j: dobj->muCTPIResult().roIVec()) muCTPIRod.push_back( j.roIWord() );

    if ( (original_RoIB_Robs.find(eformat::TDAQ_MUON_CTP_INTERFACE) != original_RoIB_Robs.end()) &&
	 (original_RoIB_Robs[eformat::TDAQ_MUON_CTP_INTERFACE].size() == 1)) {
	eformat::write::ROBFragment muCTPIRob(original_RoIB_Robs[eformat::TDAQ_MUON_CTP_INTERFACE][0]);
	muCTPIRob.rod_data(muCTPIRod.size(),muCTPIRod.data());
	l1_robs[ muCTPIRob.source_id() ] = new uint32_t[muCTPIRob.size_word()];
	auto copied = eformat::write::copy(*muCTPIRob.bind(), l1_robs[ muCTPIRob.source_id() ], muCTPIRob.size_word());
	if(copied == 0 || copied != muCTPIRob.size_word()) {
	  boost::format msg("Copy failed for RoIB muCTPI Rob: words copied: %s words expected %s");
	  msg % copied, muCTPIRob.size_word() ;
	  throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	}
    }

    /** Jet/Energy ROD */
    // RoIB
    for( unsigned int slink = 0; slink < dobj->jetEnergyResult().size(); ++slink ) {
      std::vector<uint32_t> jetEnergyRod;
      jetEnergyRod.reserve(dobj->jetEnergyResult()[slink].roIVec().size());
      for(const auto& j: dobj->jetEnergyResult()[slink].roIVec()) jetEnergyRod.push_back( j.roIWord() );

      if ( (original_RoIB_Robs.find(eformat::TDAQ_CALO_JET_PROC_ROI) != original_RoIB_Robs.end()) &&
	   (original_RoIB_Robs[eformat::TDAQ_CALO_JET_PROC_ROI].size() == dobj->jetEnergyResult().size())) {
	eformat::write::ROBFragment jetEnergyRob(original_RoIB_Robs[eformat::TDAQ_CALO_JET_PROC_ROI][slink]);
	jetEnergyRob.rod_data(jetEnergyRod.size(),jetEnergyRod.data());
	l1_robs[ jetEnergyRob.source_id() ] = new uint32_t[jetEnergyRob.size_word()];
	auto copied = eformat::write::copy(*jetEnergyRob.bind(), l1_robs[ jetEnergyRob.source_id() ], jetEnergyRob.size_word());
	if(copied == 0 || copied != jetEnergyRob.size_word()) {
	  boost::format msg("Copy failed for RoIB JET/Energy Rob: words copied: %s words expected %s");
	  msg % copied, jetEnergyRob.size_word() ;
	  throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	}
      }
    }

    /* EMTau RODs */
    // RoIB
    for( unsigned int slink = 0; slink < dobj->eMTauResult().size(); ++slink ) {
      std::vector<uint32_t> eMTauRod;
      eMTauRod.reserve(dobj->eMTauResult()[slink].roIVec().size());
      for(const auto& j: dobj->eMTauResult()[slink].roIVec()) eMTauRod.push_back( j.roIWord() );

      if ( (original_RoIB_Robs.find(eformat::TDAQ_CALO_CLUSTER_PROC_ROI) != original_RoIB_Robs.end()) &&
	   (original_RoIB_Robs[eformat::TDAQ_CALO_CLUSTER_PROC_ROI].size() == dobj->eMTauResult().size())) {
	eformat::write::ROBFragment eMTauRob(original_RoIB_Robs[eformat::TDAQ_CALO_CLUSTER_PROC_ROI][slink]);
	eMTauRob.rod_data(eMTauRod.size(),eMTauRod.data());
	l1_robs[ eMTauRob.source_id() ] = new uint32_t[eMTauRob.size_word()];
	auto copied = eformat::write::copy(*eMTauRob.bind(), l1_robs[ eMTauRob.source_id() ], eMTauRob.size_word());
	if(copied == 0 || copied != eMTauRob.size_word()) {
	  boost::format msg("Copy failed for RoIB EM/Tau Rob: words copied: %s words expected %s");
	  msg % copied, eMTauRob.size_word() ;
	  throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	}
      }
    }

    /** L1Topo ROD */
    // RoIB
    for( unsigned int slink = 0; slink < dobj->l1TopoResult().size(); ++slink ) {
      std::vector<uint32_t> l1TopoRod;
      l1TopoRod.reserve(dobj->l1TopoResult()[slink].rdo().getDataWords().size());
      for(const auto& j: dobj->l1TopoResult()[slink].rdo().getDataWords()) l1TopoRod.push_back( j );

      if ( (original_RoIB_Robs.find(eformat::TDAQ_CALO_TOPO_PROC) != original_RoIB_Robs.end()) &&
	   (original_RoIB_Robs[eformat::TDAQ_CALO_TOPO_PROC].size() == dobj->l1TopoResult().size())) {
	eformat::write::ROBFragment l1TopoRob(original_RoIB_Robs[eformat::TDAQ_CALO_TOPO_PROC][slink]);
	l1TopoRob.rod_data(l1TopoRod.size(),l1TopoRod.data());
	l1_robs[ l1TopoRob.source_id() ] = new uint32_t[l1TopoRob.size_word()];
	auto copied = eformat::write::copy(*l1TopoRob.bind(), l1_robs[ l1TopoRob.source_id() ], l1TopoRob.size_word());
	if(copied == 0 || copied != l1TopoRob.size_word()) {
	  boost::format msg("Copy failed for RoIB L1 Topo Rob: words copied: %s words expected %s");
	  msg % copied, l1TopoRob.size_word() ;
	  throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	}
      }
    }
    // in case the result size = 0 produce empty ROBs
    if ((dobj->l1TopoResult().size() == 0) && (original_RoIB_Robs.find(eformat::TDAQ_CALO_TOPO_PROC) != original_RoIB_Robs.end())) {
      for( unsigned int slink = 0; slink < original_RoIB_Robs[eformat::TDAQ_CALO_TOPO_PROC].size(); ++slink ) {
	std::vector<uint32_t> l1TopoRod;
	eformat::write::ROBFragment l1TopoRob(original_RoIB_Robs[eformat::TDAQ_CALO_TOPO_PROC][slink]);
	l1TopoRob.rod_data(l1TopoRod.size(),l1TopoRod.data());
	l1_robs[ l1TopoRob.source_id() ] = new uint32_t[l1TopoRob.size_word()];
	auto copied = eformat::write::copy(*l1TopoRob.bind(), l1_robs[ l1TopoRob.source_id() ], l1TopoRob.size_word());
	if(copied == 0 || copied != l1TopoRob.size_word()) {
	  boost::format msg("Copy failed for RoIB (empty) L1 Topo Rob: words copied: %s words expected %s");
	  msg % copied, l1TopoRob.size_word() ;
	  throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
	}
      }
    }

    return ret;
  }
}

ProcessProxy::ProcessProxy(hltinterface::HLTInterface& interface,
                           const eformat::read::FullEventFragment& e,
                           HLTTestApps::TimeoutGuard& watchdog,
                           uint32_t max_result_size)
  : m_interface(interface)
  , m_event(e)
  , m_watchdog(watchdog)
  , m_max_result_size(max_result_size)
{
  m_hltr.max_result_size = m_max_result_size;
  m_hltr.fragment_pointer = new uint32_t[m_max_result_size];
  m_hltr.fragment_pointer[0] = 0; //makes WW happier
}

eformat::helper::u32list ProcessProxy::operator()()
{
  try
  {
    HLTTestApps::Event proxy(m_event); //our DataCollector interface
    proxy.register_datacollector();
    auto eid = hltinterface::EventId{m_event.global_id(),
                                     m_event.lvl1_id(),
                                     m_event.lumi_block()};

    m_watchdog.start();
    if(!m_interface.process(proxy.l1r(), m_hltr, eid))
      ers::warning(HLTTESTAPPS_UNCLASSIFIED("HLT framework has not returned OK "
                                            "for process() call"));
    m_watchdog.reset();

    // accumulate ROS statistics      
    proxy.accumulateStatistics(m_hltr.stream_tag.size());
  }
  catch (ers::Issue& e) 
  {
    m_watchdog.reset();
    print_stack_trace();
    boost::format msg("Uncaught ers::Issue left HLT framework: %s");
    msg % e.what();
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  catch (std::exception& e) 
  {
    m_watchdog.reset();
    print_stack_trace();
    boost::format msg("Uncaught std::exception left HLT framework: %s");
    msg % e.what();
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  catch (...) 
  {
    m_watchdog.reset();
    print_stack_trace();
    boost::format msg("Unknown left HLT framework");
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
      
  //do we need to do continue?
  if (m_hltr.stream_tag.size() == 0) 
  {
    delete[] m_hltr.fragment_pointer;
    return eformat::helper::u32list(0, 0, 0);
  }
      
  //check the quality of data we get back
  try 
  {
    eformat::ROBFragment<const uint32_t*>(m_hltr.fragment_pointer).check();
  }
  catch (ers::Issue& e) 
  {
    boost::format msg("HLT result ROB does not validate: %s");
    msg % e.what();
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  
  eformat::write::FullEventFragment retval;
  retval.copy_header(m_event.start());
  retval.lvl2_trigger_info(0, nullptr); // drop l2 trigger bits
  
  //set the L1 trigger info if needed
  uint32_t* new_l1_info = 0;
  std::vector<uint32_t> remade_l1_info;
  std::map<uint32_t, uint32_t* > remade_l1_robs;
  if(check_rerun_L1(m_event,remade_l1_info,remade_l1_robs)) {
    new_l1_info = new uint32_t[remade_l1_info.size()];
    for(unsigned i = 0; i < remade_l1_info.size(); ++i) new_l1_info[i] = remade_l1_info[i];
    retval.lvl1_trigger_info(remade_l1_info.size(), new_l1_info); 
  }

  //set the HLT trigger info if needed
  uint32_t* hlt_info = 0;
  if(m_hltr.trigger_info.size()) 
  {
    hlt_info = new uint32_t[m_hltr.trigger_info.size()];
    for(unsigned i = 0; i < m_hltr.trigger_info.size(); ++i) 
      hlt_info[i] = m_hltr.trigger_info[i];
    retval.hlt_info(m_hltr.trigger_info.size(), hlt_info);
  }
  
  //set the stream tags
  uint32_t* stream_tag = 0;
  if (m_hltr.stream_tag.size()) {
    uint32_t size = eformat::helper::size_word(m_hltr.stream_tag);
    stream_tag = new uint32_t[size];
    eformat::helper::encode(m_hltr.stream_tag, size, stream_tag);
    retval.stream_tag(size, stream_tag);
  }
  
  //choose the stuff to put on the final event output
  auto to_pack = choose_event_payload(m_event,
                                      m_hltr.hltResult_robs,
                                      m_hltr.stream_tag);

  //copy the selected ROB fragments to the output event
  std::vector<eformat::write::ROBFragment> rob;
  for(std::vector<const uint32_t*>::const_iterator it = to_pack.begin(); it != to_pack.end(); ++it)
    try
    {
      eformat::read::ROBFragment old(*it) ;
      if (remade_l1_robs.find(old.source_id()) == remade_l1_robs.end()) { // no remade l1 rob available
	rob.push_back(*it);
      } else { // take the remade L1 ROB
	rob.push_back(remade_l1_robs[old.source_id()]);
      }
    }
    catch(const eformat::Issue&)
    {
      ers::warning(HLTTESTAPPS_UNCLASSIFIED("Got an invalid ROBFragment that will be skipped"));
    }

  for(unsigned i = 0; i < rob.size(); ++i) 
    retval.append(&rob[i]);
    

  // this signals the event is finished (no more robs will be added)
  // If activated, compression kicks in now
  const eformat::write::node_t* top = retval.bind();
  
    //finally, we serialize the new event and return that
  uint32_t final_size = retval.size_word();
  uint32_t* final_event = new uint32_t[final_size];
  uint32_t result = eformat::write::copy(*top, final_event, final_size);
  if (final_size != result) {
    boost::format msg("Event serialization failed. Serialized %lu words out of %lu");
    msg % result % final_size;
    delete[] m_hltr.fragment_pointer;
    delete[] hlt_info;
    delete[] stream_tag;
    delete[] new_l1_info;
    for (const auto& rob: remade_l1_robs) delete[] rob.second;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  
  //cleanup
  delete[] m_hltr.fragment_pointer;
  delete[] hlt_info;
  delete[] stream_tag;
  delete[] new_l1_info;  
  for (const auto& rob: remade_l1_robs) delete[] rob.second;

  return eformat::helper::u32list(final_event, 0, final_size);
}

eformat::helper::u32list HLTTestApps::process
  (boost::shared_ptr<hltinterface::HLTInterface> interface,
   const eformat::read::FullEventFragment& e,
   HLTTestApps::TimeoutGuard& watchdog,
   uint32_t max_result_size)
{
  ProcessProxy pp(*interface, e, watchdog, max_result_size);
  return pp();
}

void HLTTestApps::python_prompt(void) {
  PyRun_InteractiveLoop(stdin, const_cast< char* >( "\0" ));
}

void HLTTestApps::python_exec(const std::string& command) {
  int result = PyRun_SimpleString(command.c_str());
  if (result != 0) {
    boost::format msg("Error detected while executing '%s'.");
    msg % command;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
}

void HLTTestApps::python_execfile(const std::string& filename) {
  FILE *fp;
  if((fp = fopen(filename.c_str(), "rb")) == NULL) {
    boost::format msg("Cannot open file '%s'.");
    msg % filename;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
  int result = PyRun_SimpleFileEx(fp, filename.c_str(), 1); //close "fp"
  if (result != 0) {
    boost::format msg("Error detected while executing file '%s'.");
    msg % filename;
    throw(HLTTESTAPPS_UNCLASSIFIED(msg.str()));
  }
}

std::string HLTTestApps::tdaq_time_str_from_microsec(time_t sec, time_t microsec)
{
  auto * cstr = OWLTime{sec, microsec}.c_str();
  auto ret = std::string{cstr};
  delete[] cstr;
  return ret;
}

void HLTTestApps::ers_debug_level(int level) {
  ers::Configuration::instance().debug_level(level);
}

int HLTTestApps::get_ers_debug_level() {
  return ers::Configuration::instance().debug_level();
}

void HLTTestApps::ers_debug(const std::string& msg)
{
    ers::debug(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_info(const std::string& msg)
{
    ers::info(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_warning(const std::string& msg)
{
    ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_error(const std::string& msg)
{
    ers::error(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ers_fatal(const std::string& msg)
{
    ers::fatal(HLTTESTAPPS_UNCLASSIFIED(msg));
}

void HLTTestApps::ipc_init()
{
  try
  {
    IPCCore::init({});
  }
  catch(const daq::ipc::AlreadyInitialized& e)
  {
    ers::log(e);
  }
}

using namespace boost::property_tree;
namespace
{
#if BOOST_VERSION >= 105600
  using T = ptree::key_type;
#else
  using T = char;  
#endif

  auto wsettings = xml_parser::xml_writer_make_settings<T>(' ', 2);
}

string HLTTestApps::to_string(const ptree& p)
{
  std::ostringstream oss;
  xml_parser::write_xml(oss, p, wsettings);
  return oss.str();
}

void HLTTestApps::print_ptree(const ptree& p)
{
  xml_parser::write_xml(std::cout, p, wsettings);
}

