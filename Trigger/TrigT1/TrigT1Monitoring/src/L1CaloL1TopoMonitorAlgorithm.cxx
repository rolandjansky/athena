/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

L1CaloL1TopoMonitorAlgorithm::L1CaloL1TopoMonitorAlgorithm( const std::string& name,
							    ISvcLocator* pSvcLocator )
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

  return AthMonitorAlgorithm::initialize();
}

StatusCode L1CaloL1TopoMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  
  ATH_MSG_DEBUG("L1CaloL1TopoMonitorAlgorithm::fillHistograms");
  ATH_MSG_DEBUG("m_ctpRdoKey=" << m_ctpRdoKey);
  ATH_MSG_DEBUG("m_cmxCpTobLocation=" << m_cmxCpTobLocation);
  ATH_MSG_DEBUG("m_cmxJetTobLocation=" << m_cmxJetTobLocation);
  ATH_MSG_DEBUG("m_l1TopoKey=" << m_l1TopoKey);

  typedef std::tuple<int,int,int,int,int,int> TobKey;
  std::set<TobKey> cmxKeys[TOB_TYPES],topoKeys[TOB_TYPES],
    keyDiff[TOB_TYPES],keyIntersect[TOB_TYPES];
  
  const int LUMI_BLOCK=GetEventInfo(ctx)->lumiBlock();
  const int NFPGA=4;
  const int MAXTOBS=20;
  
  // monitored variables
  std::vector<int> errors;
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;

  // 1D 
  auto run = Monitored::Scalar<int>("run",GetEventInfo(ctx)->runNumber());
  auto nJetTobs = Monitored::Scalar<int>("nJetTobs",0);
  auto nTauTobs = Monitored::Scalar<int>("nTauTobs",0);
  auto nEMTobs = Monitored::Scalar<int>("nEMTobs",0);
  auto nMuonTobs = Monitored::Scalar<int>("nMuonTobs",0);

  variables.push_back(run);
  variables.push_back(nJetTobs);
  variables.push_back(nTauTobs);
  variables.push_back(nEMTobs);
  variables.push_back(nMuonTobs);
  
  auto jetTobsEnergyL = Monitored::Scalar<int>("jetTobsEnergyL",0);
  auto wFPGA = Monitored::Scalar<float>("wFPGA",1.0/NFPGA);
  auto lbErrors = Monitored::Scalar<int>("lbErrors",0);
  auto sumErrors = Monitored::Collection("sumErrors",errors,[](const auto &_e){return _e;});

  // 2D Hitmaps
  auto etaJetSTobs_match = Monitored::Scalar<double>("etaJetSTobs_match", 0.);
  auto phiJetSTobs_match = Monitored::Scalar<double>("phiJetSTobs_match", 0.);
  auto etaJetLTobs_match = Monitored::Scalar<double>("etaJetLTobs_match", 0.);
  auto phiJetLTobs_match = Monitored::Scalar<double>("phiJetLTobs_match", 0.);
  auto etaTauTobs_match = Monitored::Scalar<double>("etaTauTobs_match", 0.);
  auto phiTauTobs_match = Monitored::Scalar<double>("phiTauTobs_match", 0.);
  auto etaEMTobs_match = Monitored::Scalar<double>("etaEMTobs_match", 0.);
  auto phiEMTobs_match = Monitored::Scalar<double>("phiEMTobs_match", 0.);
  auto xJetSTobs_match = Monitored::Scalar<double>("xJetSTobs_match", 0.);
  auto yJetSTobs_match = Monitored::Scalar<double>("yJetSTobs_match", 0.);
  auto xJetLTobs_match = Monitored::Scalar<double>("xJetLTobs_match", 0.);
  auto yJetLTobs_match = Monitored::Scalar<double>("yJetLTobs_match", 0.);
  auto xTauTobs_match = Monitored::Scalar<double>("xTauTobs_match", 0.);
  auto yTauTobs_match = Monitored::Scalar<double>("yTauTobs_match", 0.);
  auto xEMTobs_match = Monitored::Scalar<double>("xEMTobs_match", 0.);
  auto yEMTobs_match = Monitored::Scalar<double>("yEMTobs_match", 0.);

  auto etaJetSTobs_mismatch = Monitored::Scalar<double>("etaJetSTobs_mismatch", 0.);
  auto phiJetSTobs_mismatch = Monitored::Scalar<double>("phiJetSTobs_mismatch", 0.);
  auto etaJetLTobs_mismatch = Monitored::Scalar<double>("etaJetLTobs_mismatch", 0.);
  auto phiJetLTobs_mismatch = Monitored::Scalar<double>("phiJetLTobs_mismatch", 0.);
  auto etaTauTobs_mismatch = Monitored::Scalar<double>("etaTauTobs_mismatch", 0.);
  auto phiTauTobs_mismatch = Monitored::Scalar<double>("phiTauTobs_mismatch", 0.);
  auto etaEMTobs_mismatch = Monitored::Scalar<double>("etaEMTobs_mismatch", 0.);
  auto phiEMTobs_mismatch = Monitored::Scalar<double>("phiEMTobs_mismatch", 0.);
  auto xJetSTobs_mismatch = Monitored::Scalar<double>("xJetSTobs_mismatch", 0.);
  auto yJetSTobs_mismatch = Monitored::Scalar<double>("yJetSTobs_mismatch", 0.);
  auto xJetLTobs_mismatch = Monitored::Scalar<double>("xJetLTobs_mismatch", 0.);
  auto yJetLTobs_mismatch = Monitored::Scalar<double>("yJetLTobs_mismatch", 0.);
  auto xTauTobs_mismatch = Monitored::Scalar<double>("xTauTobs_mismatch", 0.);
  auto yTauTobs_mismatch = Monitored::Scalar<double>("yTauTobs_mismatch", 0.);
  auto xEMTobs_mismatch = Monitored::Scalar<double>("xEMTobs_mismatch", 0.);
  auto yEMTobs_mismatch = Monitored::Scalar<double>("yEMTobs_mismatch", 0.);

  // 2D Topo items vs BC
  auto item0 = Monitored::Scalar<int>("item0", 0.);
  auto item1 = Monitored::Scalar<int>("item1", 0.);
  auto item2 = Monitored::Scalar<int>("item2", 0.);
  auto item3 = Monitored::Scalar<int>("item3", 0.);
  auto bc0 = Monitored::Scalar<int>("bc0", 0.);
  auto bc1 = Monitored::Scalar<int>("bc1", 0.);
  auto bc2 = Monitored::Scalar<int>("bc2", 0.);
  auto bc3 = Monitored::Scalar<int>("bc3", 0.);

  // read all objects needed
  ATH_MSG_DEBUG("Begin to fetch store gate objects ..");
  SG::ReadHandle<CTP_RDO> const_ctpRdo(m_ctpRdoKey,ctx);
  ATH_CHECK(const_ctpRdo.isValid());
  SG::ReadHandle<xAOD::CMXCPTobContainer> cmxCpTob(m_cmxCpTobLocation,ctx);
  ATH_CHECK(cmxCpTob.isValid());
  SG::ReadHandle<xAOD::CMXJetTobContainer> cmxJetTob(m_cmxJetTobLocation,ctx);
  ATH_CHECK(cmxJetTob.isValid());
  SG::ReadHandle<xAOD::L1TopoRawDataContainer> l1TopoRawData{m_l1TopoKey,ctx};
  ATH_CHECK(l1TopoRawData.isValid());
  if (!l1TopoRawData.isValid()) {
    ATH_MSG_DEBUG ("L1Topo DAQ raw data collection not valid");
    errors.push_back(static_cast<int>(SummaryErrors::NO_DAQ));
    lbErrors=LUMI_BLOCK;
    fill(m_packageName,lbErrors,sumErrors);
    return StatusCode::SUCCESS;
  }
  if (l1TopoRawData->empty()) {
    ATH_MSG_DEBUG ("L1Topo DAQ raw data collection empty");
    errors.push_back(static_cast<int>(SummaryErrors::NO_DAQ));
    lbErrors=LUMI_BLOCK;
    fill(m_packageName,lbErrors,sumErrors);
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Done fetching from store gate.");

  // Make a writable copy and Set CTP version number to 4 when reading persistified data
  std::vector<uint32_t> ctp_data=const_ctpRdo->getDataWords();
  CTP_RDO ctpRdo(4,std::move(ctp_data));
  ctpRdo.setL1AcceptBunchPosition(const_ctpRdo->getL1AcceptBunchPosition());
  ctpRdo.setTurnCounter(const_ctpRdo->getTurnCounter());

  // Approximate timing test of L1Topo by comparing TIP bits to L1A
  if (ctpRdo.getCTPVersionNumber()==0) {
    ATH_MSG_DEBUG("CTP version number not set, skipping CTP test");
  }
  else {
    CTP_Decoder ctp;
    ctp.setRDO(&ctpRdo);
    const uint16_t l1aPos = ctpRdo.getL1AcceptBunchPosition();
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
	      if (item/32==0) {item0=item; bc0=bc; fill(m_packageName,item0,bc0);}
	      else if (item/32==1) {item1=item; bc1=bc; fill(m_packageName,item1,bc1);}
	      else if (item/32==2) {item2=item; bc2=bc; fill(m_packageName,item2,bc2);}
	      else if (item/32==3) {item3=item; bc3=bc; fill(m_packageName,item3,bc3);}
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
  }
  
  // Retrieve CMX jet tobs
  if (cmxJetTob->empty()) {
    ATH_MSG_DEBUG ("No CMX jet TOBs found");
  }   
  else {
    ATH_MSG_DEBUG( "Found CMXJetTobCollection, looping on TOBs ..." );
    for (const xAOD::CMXJetTob* t : *cmxJetTob) {
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
      errors.push_back(static_cast<int>(SummaryErrors::DAQ_CONV));
    }
    if (rawdata->dataWords().empty()) {
      ATH_MSG_INFO("L1Topo raw data DAQ payload is empty");
      errors.push_back(static_cast<int>(SummaryErrors::NO_DAQ));
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
	      errors.push_back(static_cast<int>(SummaryErrors::PAYL_CRC));
	    }
	    i_fpga=(((rawdata->sourceID())>>3)&2)+header.fpga();
	    break;
	  }
	case L1Topo::BlockTypes::FIBRE:
	  {
	    auto fibreBlock = L1Topo::Fibre(word);
	    break;
	  }
	case L1Topo::BlockTypes::STATUS:
	  {
	    auto status = L1Topo::Status(word);
	    ATH_MSG_WARNING( "fibre overflow: " << status.overflow()
			     << " fibre crc: " << status.crc() );
	    if (status.crc()) {
	      errors.push_back(static_cast<int>(SummaryErrors::F_CRC));
	    }
	    break;
	  }
	case L1Topo::BlockTypes::L1TOPO_TOB:
	  {
	    if (header.bcn_offset()==0){
	      auto tob = L1Topo::L1TopoTOB(word);
	      ATH_MSG_DEBUG(tob);
	      daqTobs.push_back(tob);
	      // not reimplemented yet
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
		++daqJetTobs;
		jetTobsEnergyL=energyL;
		fill(m_packageName,jetTobsEnergyL,wFPGA);
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
  bool mismatch=false;
  for (int t=0; t<TOB_TYPES; ++t) { 
    // create set of mismatches
    set_symmetric_difference(cmxKeys[t].begin(),cmxKeys[t].end(),
			     topoKeys[t].begin(),topoKeys[t].end(),
			     inserter(keyDiff[t],keyDiff[t].begin()));
    if (keyDiff[t].size()>0) {
      ATH_MSG_DEBUG("Error: CMX <-> L1Topo TOB mismatches = "
		    << keyDiff[t].size());
      mismatch=true;
    }
    for (auto& tob : keyDiff[t]) {
      int x,y;
      double eta,phi;
      if (t==JETS_TOB || t==JETL_TOB) {
	jem2Coord(std::get<0>(tob),std::get<1>(tob),
		  std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
        if (t==JETS_TOB) {
          xJetSTobs_mismatch=x; yJetSTobs_mismatch=y;
          etaJetSTobs_mismatch=eta; phiJetSTobs_mismatch=phi;
          fill(m_packageName, xJetSTobs_mismatch, yJetSTobs_mismatch,
	       etaJetSTobs_mismatch, phiJetSTobs_mismatch);
        }
	else { // JETL_TOB
          xJetLTobs_mismatch=x; yJetLTobs_mismatch=y;
          etaJetLTobs_mismatch=eta; phiJetLTobs_mismatch=phi;
          fill(m_packageName, xJetLTobs_mismatch, yJetLTobs_mismatch,
	       etaJetLTobs_mismatch, phiJetLTobs_mismatch);
	}
      }
      else {
	cpm2Coord(std::get<0>(tob),std::get<1>(tob),
		  std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
        if (t==TAU_TOB) {
          xTauTobs_mismatch=x; yTauTobs_mismatch=y;
          etaTauTobs_mismatch=eta; phiTauTobs_mismatch=phi;
          fill(m_packageName, xTauTobs_mismatch, yTauTobs_mismatch,
	       etaTauTobs_mismatch, phiTauTobs_mismatch);
	}
	else { // EM_TOB
          xEMTobs_mismatch=x; yEMTobs_mismatch=y;
          etaEMTobs_mismatch=eta; phiEMTobs_mismatch=phi;
          fill(m_packageName, xEMTobs_mismatch, yEMTobs_mismatch,
	       etaEMTobs_mismatch, phiEMTobs_mismatch);
	}
      }
    }
    // create set of matches
    set_intersection(cmxKeys[t].begin(),cmxKeys[t].end(),
		     topoKeys[t].begin(),topoKeys[t].end(),
		     inserter(keyIntersect[t],keyIntersect[t].begin()));
    for (auto& tob : keyIntersect[t]) {
      int x,y;
      double eta,phi;
      if (t==JETS_TOB || t==JETL_TOB) {
        jem2Coord(std::get<0>(tob),std::get<1>(tob),
		      std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
        if (t==JETS_TOB) {
          xJetSTobs_match=x; yJetSTobs_match=y;
          etaJetSTobs_match=eta; phiJetSTobs_match=phi;
          fill(m_packageName, xJetSTobs_match, yJetSTobs_match,
	       etaJetSTobs_match, phiJetSTobs_match);
        }
	else { // JETL_TOB
          xJetLTobs_match=x; yJetLTobs_match=y;
          etaJetLTobs_match=eta; phiJetLTobs_match=phi;
          fill(m_packageName, xJetLTobs_match, yJetLTobs_match,
	       etaJetLTobs_match, phiJetLTobs_match);
	}
      }
      else {
	cpm2Coord(std::get<0>(tob),std::get<1>(tob),
		  std::get<2>(tob),std::get<3>(tob),x,y,eta,phi);
        if (t==TAU_TOB) {
          xTauTobs_match=x; yTauTobs_match=y;
          etaTauTobs_match=eta; phiTauTobs_match=phi;
          fill(m_packageName, xTauTobs_match, yTauTobs_match,
	       etaTauTobs_match, phiTauTobs_match);
	}
	else { // EM_TOB
          xEMTobs_match=x; yEMTobs_match=y;
          etaEMTobs_match=eta; phiEMTobs_match=phi;
          fill(m_packageName, xEMTobs_match, yEMTobs_match,
	       etaEMTobs_match, phiEMTobs_match);
	}
      }
    }
  }

  if (mismatch) {
    errors.push_back(static_cast<int>(SummaryErrors::CMX_MATCH));
  }
  if (!errors.empty()) {
    lbErrors=LUMI_BLOCK;
    fill(m_packageName,lbErrors,sumErrors);
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
