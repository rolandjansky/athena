/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <utility>
#include <set>
#include <tuple>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>

#include "L1CaloL1TopoMonitorAlgorithm.h"

#include "L1TopoRDO/BlockTypes.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/Fibre.h"
#include "L1TopoRDO/Status.h"
#include "TrigT1Result/CTP_Decoder.h"

#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

L1CaloL1TopoMonitorAlgorithm::L1CaloL1TopoMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode L1CaloL1TopoMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("L1CaloL1TopoMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);

  // we initialise all the containers that we need
  ATH_CHECK(m_ctpRdoKey.initialize());
  ATH_CHECK(m_cmxJetTobLocation.initialize());
  ATH_CHECK(m_cmxCpTobLocation.initialize());
  ATH_CHECK(m_l1TopoKey.initialize());
  //ATH_CHECK(m_RoIBResultKey.initialize());
  //ATH_CHECK(m_simTopoCTPKey.initialize());
  //ATH_CHECK(m_simTopoOverflowCTPKey.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode L1CaloL1TopoMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  
  ATH_MSG_DEBUG("L1CaloL1TopoMonitorAlgorithm::fillHistograms");
  ATH_MSG_DEBUG("m_ctpRdoKey=" << m_ctpRdoKey);
  ATH_MSG_DEBUG("m_cmxCpTobLocation=" << m_cmxCpTobLocation);
  ATH_MSG_DEBUG("m_cmxJetTobLocation=" << m_cmxJetTobLocation);
  ATH_MSG_DEBUG("m_l1TopoKey=" << m_l1TopoKey);
  //ATH_MSG_DEBUG("m_RoIBResultKey=" << m_RoIBResultKey);
  //ATH_MSG_DEBUG("m_simTopoCTPKey=" << m_simTopoCTPKey);
  //ATH_MSG_DEBUG("m_simTopoOverflowCTPKey=" << m_simTopoOverflowCTPKey);

  typedef std::tuple<int,int,int,int,int,int> TobKey;
  std::set<TobKey> cmxKeys[TOB_TYPES],topoKeys[TOB_TYPES],
    keyDiff[TOB_TYPES],keyIntersect[TOB_TYPES];
  
  const int LUMI_BLOCK=GetEventInfo(ctx)->lumiBlock();
  const int NFPGA=4;
  const int MAXTOBS=20;
  
  // monitored variables
  std::vector<uint8_t> errors;
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;

  auto run = Monitored::Scalar<int>("run",GetEventInfo(ctx)->runNumber());
  auto lbErrors = Monitored::Scalar<int>("lbErrors",0);
  auto sumErrors = Monitored::Collection("sumErrors", errors);
  auto jetTobsEnergyL = Monitored::Scalar<int>("jetTobsEnergyL",0);
  auto wFPGA = Monitored::Scalar<float>("wFPGA",1.0/NFPGA);
  auto nJetTobs = Monitored::Scalar<int>("nJetTobs",0);
  auto nTauTobs = Monitored::Scalar<int>("nTauTobs",0);
  auto nEMTobs = Monitored::Scalar<int>("nEMTobs",0);
  auto nMuonTobs = Monitored::Scalar<int>("nMuonTobs",0);

  variables.push_back(run);
  variables.push_back(lbErrors);
  variables.push_back(sumErrors);
  variables.push_back(nJetTobs);
  variables.push_back(nTauTobs);
  variables.push_back(nEMTobs);
  variables.push_back(nMuonTobs);

  // read all objects needed
  ATH_MSG_DEBUG("Begin to fetch store gate objects ..");
  SG::ReadHandle<CTP_RDO> ctpRdo(m_ctpRdoKey,ctx);
  ATH_CHECK(ctpRdo.isValid());
  SG::ReadHandle<xAOD::CMXCPTobContainer> cmxCpTob(m_cmxCpTobLocation,ctx);
  ATH_CHECK(cmxCpTob.isValid());
  SG::ReadHandle<xAOD::CMXJetTobContainer> cmxJetTob(m_cmxJetTobLocation,ctx);
  ATH_CHECK(cmxJetTob.isValid());
  SG::ReadHandle<xAOD::L1TopoRawDataContainer> l1TopoRawData{m_l1TopoKey,ctx};
  ATH_CHECK(l1TopoRawData.isValid());
  if (!l1TopoRawData.isValid()) {
    ATH_MSG_DEBUG ("L1Topo DAQ raw data collection not valid");
    errors.push_back(static_cast<uint8_t>(SummaryErrors::NO_DAQ));
    lbErrors=LUMI_BLOCK;
    fill(m_packageName,variables);
    return StatusCode::SUCCESS;
  }
  if (l1TopoRawData->empty()) {
    ATH_MSG_DEBUG ("L1Topo DAQ raw data collection empty");
    errors.push_back(static_cast<uint8_t>(SummaryErrors::NO_DAQ));
    lbErrors=LUMI_BLOCK;
    fill(m_packageName,variables);
    return StatusCode::SUCCESS;
  }
  //SG::ReadHandle<ROIB::RoIBResult> roibResult{m_RoIBResultKey,ctx};
  //ATH_CHECK(roibResult.isValid());
  ATH_MSG_DEBUG("Done fetching from store gate.");

  // Approximate timing test of L1Topo by comparing TIP bits to L1A
  if (ctpRdo->getCTPVersionNumber()==0) {
    ATH_MSG_DEBUG("CTP version number not set, skipping CTP test");
  }
  else {
    CTP_Decoder ctp;
    ctp.setRDO(ctpRdo.cptr());
    const uint16_t l1aPos = ctpRdo->getL1AcceptBunchPosition();
    const uint32_t bx=ctp.getBunchCrossings().size();
    const int dbx=bx/2;
    if (l1aPos >= bx) {
      ATH_MSG_DEBUG( "CTP_RDO gave invalid l1aPos." );
    }
    else {
      ATH_MSG_DEBUG( "CTP l1aPos, size : " << l1aPos << ", " << bx );
      if ( bx>=3 && l1aPos>0) {
	for (int bc=-dbx; bc<=dbx; ++bc) {
	  const CTP_BC& ctp_bc = ctp.getBunchCrossings().at(l1aPos+bc);
	  std::bitset<512> tip = ctp_bc.getTIP();
	  ATH_MSG_DEBUG( "got CTP TIP bits: " << tip.to_string() );
	  const unsigned int topoTipStart(384);
	  const unsigned int nTopoCTPOutputs(128);
	  for (unsigned int item=0; item<nTopoCTPOutputs;++item) {
	    // Check if algorithms fired
	    if (tip.test(item+topoTipStart)) {
	      //m_h_l1topo_2d_ItemsBC[item/32]->Fill(item,bc);
	      ATH_MSG_DEBUG("  Fired (item, bc) =" << item << ", " << bc);
	    }
	  }
	}
      }
    }
  }

  // Retrieve CMX CP tobs
  if (cmxCpTob->empty()) {
    ATH_MSG_DEBUG ("No CMX CP TOBs found");
    //errors.push_back(static_cast<uint8_t>(SummaryErrors::NO_CMX));
  }   
  else {
    ATH_MSG_DEBUG( "Found CMXCPTobCollection, looping on TOBs ..." );
    for (const xAOD::CMXCPTob* t : *cmxCpTob) {
      if (t->energy()) {
	for (int clone=0;clone<4;++clone) {
	  if (t->cmx()==0)
	    cmxKeys[TAU_TOB].insert(std::make_tuple(t->crate(),t->cpm(),
						    t->chip(),t->location(),
						    t->energy(),clone));
	  else
	    cmxKeys[EM_TOB].insert(std::make_tuple(t->crate(),t->cpm(),
						    t->chip(),t->location(),
						    t->energy(),clone));
	}
      }
    }
    //m_h_l1topo_1d_CMXCPTobs->Fill(std::min((int)cmxtobs.size(),MAXTOBS-1));
  }
  
  // Retrieve CMX jet tobs
  if (cmxJetTob->empty()) {
    ATH_MSG_DEBUG ("No CMX jet TOBs found");
    //errors.push_back(static_cast<uint8_t>(SummaryErrors::NO_CMX));
  }   
  else {
    ATH_MSG_DEBUG( "Found CMXJetTobCollection, looping on TOBs ..." );
    for (const xAOD::CMXJetTob* t : *cmxJetTob) {
      //if (t->energyLarge()) cmxtobs.push_back(t);
      for (int clone=0;clone<4;++clone) {
	if (t->energyLarge())
	  cmxKeys[JETL_TOB].insert(std::make_tuple(t->crate(),t->jem(),
						   t->frame(),t->location(),
						   t->energyLarge(),clone));
	if (t->energySmall())
	  cmxKeys[JETS_TOB].insert(std::make_tuple(t->crate(),t->jem(),
						   t->frame(),t->location(),
						   t->energySmall(),clone));
      }
    }
    //m_h_l1topo_1d_CMXTobs->Fill(std::min((int)cmxtobs.size(),MAXTOBS-1));
  }

  // analyse and register the L1Topo DAQ TOBs
  int daqJetTobs=0;
  int daqTauTobs=0;
  int daqEMTobs=0;
  int daqMuonTobs=0;
  for (const xAOD::L1TopoRawData* rawdata : *l1TopoRawData) {
    ATH_MSG_DEBUG("Found L1Topo raw DAQ data with source ID "
		  << L1Topo::formatHex8(rawdata->sourceID()));
    //rdo.setStatusWords(rawdata->statusWords());
    if (rawdata->errorWord()) {
      ATH_MSG_INFO("DAQ Converter errors reported: " << rawdata->errorWord());
      errors.push_back(static_cast<uint8_t>(SummaryErrors::DAQ_CONV));
    }
    if (rawdata->dataWords().empty()) {
      ATH_MSG_INFO("L1Topo raw data DAQ payload is empty");
      errors.push_back(static_cast<uint8_t>(SummaryErrors::NO_DAQ));
    }
    else {
      std::vector<L1Topo::L1TopoTOB> daqTobs;

      // initialise header: beware, this can make a valid-looking header
      //   set version 15, BCN -7, which is unlikely:
      L1Topo::Header header(0xf, 0, 0, 0, 0, 1, 0x7);
      int i_fpga=-1;
      for (auto word : rawdata->dataWords()) {
	switch (L1Topo::blockType(word)) {

	case L1Topo::BlockTypes::HEADER:
	  {
	    header = L1Topo::Header(word);
	    if (header.payload_crc()!=0) {
	      errors.push_back(static_cast<uint8_t>(SummaryErrors::PAYL_CRC));
	    }
	    i_fpga=(((rawdata->sourceID())>>3)&2)+header.fpga();
	    break;
	  }
	case L1Topo::BlockTypes::FIBRE:
	  {
	    auto fibreBlock = L1Topo::Fibre(word);
	    //for (auto fsize: fibreBlock.count()){
	      //vFibreSizes.push_back(fsize);
	    //}
	    //for (auto fstatus: fibreBlock.status()){
	      //vFibreStatus.push_back(fstatus);
	    //}
	    break;
	  }
	case L1Topo::BlockTypes::STATUS:
	  {
	    auto status = L1Topo::Status(word);
	    ATH_MSG_WARNING( "fibre overflow: " << status.overflow()
			     << " fibre crc: " << status.crc() );
	    //if (status.overflow()) m_h_l1topo_1d_Overflows->Fill(0.5);
	    if (status.crc()) {
	      errors.push_back(static_cast<uint8_t>(SummaryErrors::F_CRC));
	    }
	    break;
	  }
	case L1Topo::BlockTypes::L1TOPO_TOB:
	  {
	    if (header.bcn_offset()==0){
	      auto tob = L1Topo::L1TopoTOB(word);
	      ATH_MSG_DEBUG(tob);
	      daqTobs.push_back(tob);
	      //auto index = L1Topo::triggerBitIndex(rawdata->sourceID(),tob);
	      for (unsigned int i=0; i<8; ++i){
		if ((tob.trigger_bits() >>i)&1) {
		  //m_h_l1topo_1d_DAQTriggerBits->Fill(index+i);
		}
		if ((tob.overflow_bits()>>i)&1) {
		  //m_h_l1topo_1d_DAQOverflowBits->Fill(index+i);
		}
	      }
	    }
	    break;
	  }
	case L1Topo::BlockTypes::JET1_TOB:
	case L1Topo::BlockTypes::JET2_TOB:
	  {
	    if (header.bcn_offset()==0) {
	      const int crate    = (word >> 28) & 0x1;
	      const int jem      = (word >> 24) & 0xF;
	      const int frame    = (word >> 21) & 0x7;
	      const int location = (word >> 19) & 0x3;
	      const int energyS  = (word >> 10) & 0x1FF;
	      const int energyL  = (word & 0x3FF);	      
	      if (energyL) {
		topoKeys[JETL_TOB].
		  insert(std::make_tuple(crate,jem,frame,
					 location,energyL,i_fpga));
		//auto tob = L1Topo::JetTOB(word);
		++daqJetTobs;
		jetTobsEnergyL=energyL;
		fill(m_packageName,jetTobsEnergyL,wFPGA);
		//m_h_l1topo_1d_JetTobs_EnergyLg->Fill(energyL,1./NFPGA);
	      }
	      if (energyS) {
		topoKeys[JETS_TOB].
		  insert(std::make_tuple(crate,jem,frame,
					 location,energyS,i_fpga));
	      }
	    }
	    break;
	  }
	case L1Topo::BlockTypes::TAU_TOB:
	  {
	    if (header.bcn_offset()==0) {
	      ++daqTauTobs;
	      const int crate    = (word >> 26) & 0x3;
	      const int cpm      = (word >> 20) & 0xF;
	      const int chip     = (word >> 15) & 0xF;
	      const int location = (word >> 13) & 0x3;
	      const int energy   = (word & 0xFF);	      
	      if (energy)
		topoKeys[TAU_TOB].
		  insert(std::make_tuple(crate,cpm,chip,
					 location,energy,i_fpga));
	    }
	    break;
	  }	  
	case L1Topo::BlockTypes::EM_TOB:
	  {
	    if (header.bcn_offset()==0) {
	      ++daqEMTobs;
	      const int crate    = (word >> 26) & 0x3;
	      const int cpm      = (word >> 20) & 0xF;
	      const int chip     = (word >> 15) & 0xF;
	      const int location = (word >> 13) & 0x3;
	      const int energy   = (word & 0xFF);
	      if (energy)
		topoKeys[EM_TOB].
		  insert(std::make_tuple(crate,cpm,chip,
					 location,energy,i_fpga));
	    }
	    break;
	  }
	case L1Topo::BlockTypes::MUON_TOB:
	  {
	    if (header.bcn_offset()==0){
	      ++daqMuonTobs;
	    }
	    break;
	  }
	case L1Topo::BlockTypes::ENERGY_TOB:
	  {
	    if (header.bcn_offset()==0){
	    }
	    break;
	  }
	default:
	  {
	    break;
	  }

	}
      } // for word
    } // L1Topo raw data
  } // L1 Topo collection


  // look in both directions for CMX and Topo TOBs (mis)matches
  int tests=0;
  for (int t=0; t<TOB_TYPES; ++t) { 
    tests+=topoKeys[t].size();
    set_symmetric_difference(cmxKeys[t].begin(),cmxKeys[t].end(),
			     topoKeys[t].begin(),topoKeys[t].end(),
			     inserter(keyDiff[t],keyDiff[t].begin()));
    if (keyDiff[t].size()>0) {
      ATH_MSG_DEBUG("Error: CMX <-> L1Topo TOB mismatches = "
		    << keyDiff[t].size());
      errors.push_back(static_cast<uint8_t>(SummaryErrors::CMX_MATCH));
    }
    else {
      ATH_MSG_DEBUG("Good: All CMX <-> L1Topo TOBs match, TOBs tested = "
		    << tests);
    }
    for (auto& tob : keyDiff[t]) {
      int x,y;
      double eta,phi;
      if (t==JETS_TOB || t==JETL_TOB) {
	jem2Coord(std::get<0>(tob),std::get<1>(tob),
		  std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
	//m_histTool->fillJEMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_mismatch[t],
	//eta,phi);
      }
      else {
	cpm2Coord(std::get<0>(tob),std::get<1>(tob),
		  std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
	//m_histTool->fillCPMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_mismatch[t],
	//eta,phi);
      }
      //m_h_l1topo_2d_Tobs_Hitmap_mismatch[t]->Fill(x,y);
    }
    set_intersection(cmxKeys[t].begin(),cmxKeys[t].end(),
		     topoKeys[t].begin(),topoKeys[t].end(),
		     inserter(keyIntersect[t],keyIntersect[t].begin()));
    for (auto& tob : keyIntersect[t]) {
      int x,y;
      double eta,phi;
      if (t==JETS_TOB || t==JETL_TOB) {
	jem2Coord(std::get<0>(tob),std::get<1>(tob),
		  std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
	//m_histTool->fillJEMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_match[t],
	//eta,phi);
      }
      else {
	cpm2Coord(std::get<0>(tob),std::get<1>(tob),
		  std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
	//m_histTool->fillCPMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_match[t],
	//eta,phi);
      }
      //m_h_l1topo_2d_Tobs_Hitmap_match[t]->Fill(x,y);
    }
  }

  if (!errors.empty()) {
    lbErrors=LUMI_BLOCK;
  }

  nJetTobs=std::min(daqJetTobs/NFPGA,MAXTOBS-1);
  nTauTobs=std::min(daqTauTobs/NFPGA,MAXTOBS-1);
  nEMTobs=std::min(daqEMTobs/NFPGA,MAXTOBS-1);
  nMuonTobs=std::min(daqMuonTobs/NFPGA,MAXTOBS-1);

  fill(m_packageName,variables);
  return StatusCode::SUCCESS;
}

void L1CaloL1TopoMonitorAlgorithm::jem2Coord(const int crate, const int jem,
	       const int frame, const int location,
	       int &x, int &y, double &eta, double &phi) const
{
  x = crate*16 + jem;
  y = frame*4 + location;
  const uint32_t roiWord = // formula from JEPSimMon
    ((((((crate << 4) + jem) << 3) + frame) << 2) + location) << 19;
  LVL1::JEPRoIDecoder decoder;
  const LVL1::CoordinateRange coord(decoder.coordinate(roiWord));
  eta = coord.eta();
  phi = coord.phi();
}

void L1CaloL1TopoMonitorAlgorithm::cpm2Coord(const int crate, const int cpm,
	       const int chip, const int location,
	       int &x, int &y, double &eta, double &phi) const
{
  x = crate*14 + cpm - 1;
  y = chip*4 + location;
  const uint32_t roiWord = // formula from CPSimMon
    ((((((crate << 4) + cpm) << 4) + chip) << 2)
     + location) << 18;
  LVL1::CPRoIDecoder decoder;
    const LVL1::CoordinateRange coord(decoder.coordinate(roiWord));
    eta = coord.eta();
    phi = coord.phi();
}
